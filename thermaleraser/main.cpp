//
//  main.cpp
//  thermaleraser
//
//  Created by Mark Larus on 9/8/12.
//  Copyright (c) 2012 Kenyon College. All rights reserved.
//  

#include <iostream>
#include <gsl/gsl_randist.h>
#include <gsl/gsl_sf.h>
#include <time.h>
#include <semaphore.h>
#include <algorithm>
#include <math.h>
#include <sqlite3.h>
#include <assert.h>

#include "States.h"
#include "System.h"
#include "Utilities.h"
#include <mathlink/mathlink.h>

struct Result {
    double average;
    double epsilon;
    double delta;
};


int count_notebooks(MLINK link) {
    MLPutFunction(link, "EvaluatePacket", 1);
    MLPutFunction(link, "ToExpression",1);
    MLPutString(link, "Length[Notebooks[]]");
    int next = MLGetNext(link);
    const char *name;
    int count;
    while (next!=MLTKEND) {
        switch (next) {
            case MLTKFUNC:
                MLGetFunction(link, &name, &count);
                break;
            case MLTKSTR:
                MLGetString(link, &name);
                break;
            case MLTKINT:
                MLGetInteger(link, &count);
                break;
            case MLTKSYM:
                MLGetSymbol(link, &name);
                break;
            default:
                assert(0);
                break;
        }
        next=MLGetNext(link);
        if(next==MLTKINT) {
            break;
        }
    }
    return count;
}


Result simulate_and_print(Constants constants, int iterations);

int main(int argc, char * argv[])
{
    MLENV env;
    MLINK link;
    int error;
    
    env = MLInitialize(0);
    if(env==0) {
        assert("Unable to initialize MathLink");
    }
    link = MLOpenArgcArgv(env, argc, argv, &error);
    MLActivate(link);

    /*! This call sets up our state machine for the wheel. Each state (i.e. "A0", "C1") is
     represented by an object with pointers to the next states and the bit-flip states. */
    setupStates();
    
    int iterations = 1<<23;
    
    Constants constants;
    
    /*! The delta used here is NOT, at this point, the delta used in the paper. This is the 
     ratio of ones to zeroes in the bit stream. Probably worth changing the name, but
     calculating this at runtime is just an invitation for bugs. */
    constants.delta = .5;
    constants.epsilon = .7;
    constants.tau = 1;

    Result results[5*5];
    
    while (!count_notebooks(link)) {
        sleep(1);
    }
    
    const char *name;
    int count;
    
    MLPutFunction(link, "EvaluatePacket", 1);
    MLPutFunction(link, "Set", 2);
    MLPutSymbol(link, "Data");
    MLPutFunction(link, "List", 10);
    for(int k=0; k<10; k++) {
        MLPutFunction(link, "List", 2);
        MLPutInteger(link, k);
        MLPutDouble(link, 1);
    }
    
    MLPutFunction(link, "EvaluatePacket", 1);
    MLPutFunction(link, "NotebookWrite", 3);
    MLPutFunction(link, "First", 1);
    MLPutFunction(link, "Notebooks", 0);
    MLPutString(link, "ListPlot[Data]");
    MLPutSymbol(link, "All");
    
    MLPutFunction(link, "EvaluatePacket", 1);
    MLPutFunction(link, "SelectionEvaluateCreateCell", 1);
    MLPutFunction(link, "First", 1);
    MLPutFunction(link, "Notebooks", 0);
    
    int next = MLGetNext(link);

    double real;
    while (next!=MLTKEND) {
        switch (next) {
            case MLTKFUNC:
                MLGetFunction(link, &name, &count);
                printf("Function::%s,%d\n",name,count);
                break;
            case MLTKSTR:
                MLGetString(link, &name);
                printf("String::%s\n",name);
                break;
            case MLTKINT:
                MLGetInteger(link, &count);
                printf("Integer::%d",count);
                break;
            case MLTKSYM:
                MLGetSymbol(link, &name);
                printf("Symbol::%s\n",name);
                break;
            case MLTKREAL:
                MLGetReal(link, &real);
                printf("Real::%lf\n",real);
                break;
            case MLTKERROR:
                printf("%s",MLErrorMessage(link));
            
            default:
                assert(0);
                break;
        }
        next=MLGetNext(link);
        if(next==MLTKINT) {
            break;
        }
    }

        sleep(1000);
}

Result simulate_and_print(Constants constants, int iterations) {
    
    /*! Use this to change the length of the tape. */
    const int BIT_STREAM_LENGTH = 16;
    
    int first_pass_iterations = iterations;
    
    System *systems = new System[first_pass_iterations]();
    //time_t start_time = time(NULL);
    #pragma omp parallel default(shared)
    {
        //TODO: Move this into a semaphore in the utility function
        gsl_rng *localRNG = GSLRandomNumberGenerator();
        
        #pragma omp for
        for (int k=0; k<first_pass_iterations; ++k) {
            System *currentSystem = systems+k;
            
            currentSystem->constants = constants;
            currentSystem->nbits = BIT_STREAM_LENGTH;
            
            evolveSystem(currentSystem, localRNG);
        }
        gsl_rng_free(localRNG);
    }//End parallel
    
    //time_t stop_time = time(NULL);
    
    //std::clog<<"Time elapsed: "<<(stop_time-start_time)<<std::endl;
    
    long double *p = new long double[1<<BIT_STREAM_LENGTH];
    long double *p_prime = new long double[1<<BIT_STREAM_LENGTH];
    
    int *histogram = new int[1<<BIT_STREAM_LENGTH];
    std::fill_n(histogram, 1<<BIT_STREAM_LENGTH, 0);
    
    for(int k=0; k<first_pass_iterations; k++) {
        System *currentSystem = systems+k;
        ++histogram[currentSystem->endingBitString];
    }
    
    for(int k=0; k<1<<BIT_STREAM_LENGTH; k++) {
        int setBits = bitCount(k,BIT_STREAM_LENGTH);
        p[k] = gsl_ran_binomial_pdf(setBits, constants.delta, BIT_STREAM_LENGTH)/gsl_sf_choose(BIT_STREAM_LENGTH,setBits);
        p_prime[k]=static_cast<long double>(histogram[k])/(first_pass_iterations);
    }
    
    delete [] histogram;
    delete [] systems;
    
    long double sum = 0;
    
    const long double beta = log((1+constants.epsilon)/(1-constants.epsilon));
    long double max_surprise = 0;
    long double min_surprise = LONG_MAX;
    
    #pragma omp parallel
    {
        
        //Make sure we don't accidentally share a seed.
        gsl_rng *localRNG = GSLRandomNumberGenerator();
        
        #pragma omp for reduction(+ : sum)
        for(int k=0; k<iterations; k++) {
            System *currentSystem = new System();
            currentSystem->constants = constants;
            currentSystem->nbits = BIT_STREAM_LENGTH;
            
            evolveSystem(currentSystem, localRNG);
            
            long double surprise = exp(beta*currentSystem->mass)*p_prime[currentSystem->endingBitString]/p[currentSystem->startingBitString];
            max_surprise = surprise > max_surprise ? surprise : max_surprise;
            min_surprise = surprise && surprise < min_surprise ? surprise : min_surprise;
            sum = sum + surprise;
            delete currentSystem;
        }
        gsl_rng_free(localRNG);
    }
    

//    static int once = 0;
//    if (!once) {
//        std::cout<<"delta,epsilon,avg,max_surprise\n";
//        once=1;
//    }
//    
//    std::cout<< constants.delta << "," << constants.epsilon << "," << sum/iterations << "," << max_surprise << std::endl;
    #define print(x) std::cout<<#x <<": " <<x<<std::endl;
    
    
    print(beta);
    print(constants.delta);
    print(constants.epsilon);   
    print(sum/iterations);
    print(max_surprise);
    print(sum);
    std::cout<<std::endl;

    Result result;
    result.epsilon = constants.epsilon;
    result.average = sum/iterations;
    result.delta = constants.delta;
    
    delete [] p_prime;
    delete [] p;
}
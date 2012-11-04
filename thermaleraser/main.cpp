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

#include "States.h"
#include "System.h"
#include "Utilities.h"

int main(int argc, const char * argv[])
{
    /*! This call sets up our state machine for the wheel. Each state (i.e. "A0", "C1") is
     represented by an object with pointers to the next states and the bit-flip states. */
    setupStates();
    
    Constants constants;
    
    /*! The delta used here is NOT, at this point, the delta used in the paper. This is the 
     ratio of ones to zeroes in the bit stream. Probably worth changing the name, but
     calculating this at runtime is just an invitation for bugs. */
    constants.delta = .5;
    constants.epsilon = .5;
    constants.tau = 1;
    
    /*! Use this to change the number of times the tape is simulated */
    int iterations = 1<<22;
    
    /*! Use this to change the length of the tape. */
    #define BIT_STREAM_LENGTH 16
    
    System *systems = new System[iterations]();
    time_t start_time = time(NULL);
    
    #pragma omp parallel default(shared)
    {
        //TODO: Move this into a semaphore in the utility function
        gsl_rng *localRNG = GSLRandomNumberGenerator();

        #pragma omp for
        for (int k=0; k<iterations; ++k) {
            System *currentSystem = systems+k;
            
            currentSystem->constants = constants;
            currentSystem->nbits = BIT_STREAM_LENGTH;
            
            evolveSystem(currentSystem, localRNG);
        }
        gsl_rng_free(localRNG);
    }//End parallel
    
    time_t stop_time = time(NULL);
    
    std::clog<<"Time elapsed: "<<(stop_time-start_time)<<std::endl;
    
    long double *p = new long double[1<<BIT_STREAM_LENGTH];
    long double *p_prime = new long double[1<<BIT_STREAM_LENGTH];
    
    int *histogram = new int[1<<BIT_STREAM_LENGTH];
    
    for(int k=0; k<iterations; k++) {
        System *currentSystem = systems+k;
        ++histogram[currentSystem->endingBitString];
    }
    
    for(int k=0; k<1<<BIT_STREAM_LENGTH; k++) {
        int setBits = bitCount(k,BIT_STREAM_LENGTH);
        p[k] = gsl_ran_binomial_pdf(setBits, constants.delta, BIT_STREAM_LENGTH)/gsl_sf_choose(BIT_STREAM_LENGTH,setBits);
        p_prime[k]=static_cast<long double>(histogram[k])/iterations;
    }
    
    delete histogram;
    delete systems;
    
    long double sum = 0;
    
    const long double beta = log((1+constants.epsilon)/(1-constants.epsilon));
    long double max_surprise = 0;
    
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
            
            long double surprise = expl(-beta*currentSystem->mass)*p_prime[currentSystem->endingBitString]/p[currentSystem->startingBitString];
            max_surprise = surprise > max_surprise ? surprise : max_surprise;
            sum = sum + surprise;
            delete currentSystem;
        }
    }
    
    std::cout<<"Average: "<<sum/iterations<<std::endl;
    std::cout<<"max_J/sum: "<<max_surprise/sum<<std::endl;
}

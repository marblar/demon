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
#include <set>
#include <boost/program_options.hpp>
#include <boost/timer/timer.hpp>


#include "States.h"
#include "System.h"
#include "Utilities.h"

#define print(x) std::cout<<#x <<": " <<x<<std::endl;

namespace opt = boost::program_options;

enum OutputType {
    CommaSeparated,
    PrettyPrint,
    Mathematica,
    NoOutput
};

void simulate_and_print(Constants constants, int iterations, OutputType type);

int main(int argc, char * argv[]) {
  /*! Initialize options list.
   */

    bool verbose = false;
    const int default_iterations = 1<<16;
    
    opt::options_description desc("Allowed options");
    desc.add_options()
        ("iterations,n",opt::value<int>(), "Number of iterations")
        ("help","Show help message")
        ("verbose,v", "Show extensive debugging info") 
    ;
    
    opt::variables_map vmap;
    opt::store(opt::parse_command_line(argc,argv,desc),vmap);
    opt::notify(vmap);
    
    if (vmap.count("help")) {
        std::cout << desc << "\n";
        return 1;
    }

    verbose = vmap.count("verbose");
    
    int iterations = vmap.count("iterations,n") ? vmap["iterations,n"].as<int>() : default_iterations;
    
    if (verbose) {
      print(iterations);
    }

    /*! This call sets up our state machine for the wheel. Each state (i.e. "A0", "C1") is
     represented by an object with pointers to the next states and the bit-flip states. */
    setupStates();
        
    Constants constants;
    
    /*! The delta used here is NOT, at this point, the delta used in the paper. This is the 
     ratio of ones to zeroes in the bit stream. Probably worth changing the name, but
     calculating this at runtime is just an invitation for bugs. */
    constants.delta = .5;
    constants.epsilon = .7;
    constants.tau = 1;
    int dimension = 50;
    for (int k=dimension*dimension; k>=0; k--) {
        constants.epsilon = (k % dimension)/(double)(dimension);
        constants.delta = .5 + .5*(k / dimension)/(double)(dimension);
        simulate_and_print(constants, iterations, CommaSeparated);
    }

}

void simulate_and_print(Constants constants, int iterations, OutputType type) {
    boost::timer::auto_cpu_timer t;
    
    /*! Use this to change the length of the tape. */
    const int BIT_STREAM_LENGTH = 8;
    
    int first_pass_iterations = iterations;
    
    System *systems = new System[first_pass_iterations]();
    
    int *histogram = new int[1<<BIT_STREAM_LENGTH];
    std::fill_n(histogram, 1<<BIT_STREAM_LENGTH, 0);

    #pragma omp parallel default(shared)
    {
        //TODO: Move this into a semaphore in the utility function
        gsl_rng *localRNG = GSLRandomNumberGenerator();
        
        int *localHistogram = new int[1<<BIT_STREAM_LENGTH];
        std::fill_n(histogram, 1<<BIT_STREAM_LENGTH, 0);
        
        #pragma omp for
        for (int k=0; k<first_pass_iterations; ++k) {
            System *currentSystem = systems+k;
            
            currentSystem->constants = constants;
            currentSystem->nbits = BIT_STREAM_LENGTH;
            
            evolveSystem(currentSystem, localRNG);
            ++localHistogram[currentSystem->endingBitString];
        
        }
        

        delete [] localHistogram;
        
        gsl_rng_free(localRNG);
    }//End parallel
    
    for(int k=0; k<first_pass_iterations; k++) {
        histogram[systems[k].endingBitString]++;
    }
    
    
    delete [] systems;
    
    //time_t stop_time = time(NULL);
    
    //std::clog<<"Time elapsed: "<<(stop_time-start_time)<<std::endl;
    
    long double *p = new long double[1<<BIT_STREAM_LENGTH];
    long double *p_prime = new long double[1<<BIT_STREAM_LENGTH];

    for(int k=0; k<1<<BIT_STREAM_LENGTH; k++) {
        int setBits = bitCount(k,BIT_STREAM_LENGTH);
        p[k] = gsl_ran_binomial_pdf(setBits, constants.delta, BIT_STREAM_LENGTH)/gsl_sf_choose(BIT_STREAM_LENGTH,setBits);
        p_prime[k]=static_cast<long double>(histogram[k])/(first_pass_iterations);
    }
    
    delete [] histogram;
    
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
    
    delete [] p_prime;
    delete [] p;
    
    if(type==CommaSeparated) {
        static int once = 0;
        if (!once) {
            std::cout<<"delta,epsilon,avg,max_surprise\n";
            once=1;
        }
        std::cout<< constants.delta << "," << constants.epsilon << "," << sum/iterations << "," << max_surprise << std::endl;
    }

    
    if(type==PrettyPrint) {
        print(beta);
        print(constants.delta);
        print(constants.epsilon);
        print(sum/iterations);
        print(max_surprise);
        print(sum);
        std::cout<<std::endl;
    }
    
    if (type==Mathematica) {
        assert(0);
    }
}

//
//  main.cpp
//  thermaleraser
//
//  Created by Mark Larus on 9/8/12.
//  Copyright (c) 2012 Kenyon College. All rights reserved.
//

#include <iostream>
#include <algorithm>
#include <math.h>
#include <gsl/gsl_randist.h>
#include <gsl/gsl_sf.h>
#include <boost/program_options.hpp>

#ifdef HAS_BOOST_TIMER
#include <boost/timer/timer.hpp>
/*! Fix a namespace issue with boost library.
  This lets us use both cpu_timer and progress
*/
#define timer timer_class
#include <boost/progress.hpp>
#undef timer
#endif

#include "clangomp.h"
#include "Stochastic.h"
#include "Ising.h"
#include "System.h"
#include "Utilities.h"
#include "ReservoirFactory.h"

#define print(x) std::cout<<#x <<": " <<x<<std::endl;

namespace opt = boost::program_options;

enum OutputType {
    CommaSeparated,
    PrettyPrint,
    Mathematica,
    NoOutput
};

enum ReservoirType {
    Ising,
    Stoch
};

void simulate_and_print(Constants constants, int iterations,
                        OutputType type, ReservoirFactory *factory, bool verbose = false);

int main(int argc, char * argv[]) {
    /*! Initialize options list.
     */
    
    bool verbose = false;
    const int default_iterations = 1<<16;
    
    opt::options_description desc("Allowed options");
    desc.add_options()
    ("iterations,n",opt::value<int>(), "Number of iterations.")
    ("help","Show help message")
    ("verbose,v", "Show extensive debugging info")
#ifdef HAS_BOOST_TIMER
    ("benchmark", "Test evaluation speed")
#endif
    ("ising", "Use Ising reservoir. Requires -d. Overrides --stoch")
    ("stoch", "Use Stochastic reservoir. This is set by default, and overridden"
            " by --ising.")
    ("dimension,d", opt::value<int>()->default_value(100),
        "Set the dimension of the Ising reservoir")
    ("tau,t", opt::value<double>()->default_value(5))
    ;
    
    opt::variables_map vmap;
    opt::store(opt::parse_command_line(argc,argv,desc),vmap);
    opt::notify(vmap);
    
    if (vmap.count("help")) {
        std::cout << desc << "\n";
        return 1;
    }
    
    verbose = vmap.count("verbose");
    
    int iterations = vmap.count("iterations") ? vmap["iterations"].as<int>() : default_iterations;
    
    if (verbose) {
        print(iterations);
        #pragma omp parallel
        #pragma omp single
        print(omp_get_num_threads());
    }
    
    OutputType output_style = vmap.count("output") ? (OutputType)vmap["output"].as<int>() : CommaSeparated;
    
    /*! This call sets up our state machine for the wheel. Each state (i.e. "A0", "C1") is
     represented by an object with pointers to the next states and the bit-flip states. */
    setupStates();
    
    Constants constants;
#ifdef HAS_BOOST_TIMER
    if(vmap.count("benchmark")) {
        std::cout<<"Benchmarking speed.\n";
        int benchmark_size = 1000;
        boost::timer::cpu_timer timer;
        boost::progress_display display(benchmark_size);
        timer.start();
        #pragma omp parallel for private(constants)
        for (int k=0; k<benchmark_size; k++) {
            simulate_and_print(constants,iterations,NoOutput,false);
            ++display;
        }
        timer.stop();
        double time_elapsed = timer.elapsed().wall;
        print(benchmark_size);
        print(timer.format(3,"%ws"));
        print(benchmark_size/time_elapsed);
        exit(0);
    }
#endif
    ReservoirFactory *rFactory = NULL;
    if ( vmap.count("ising") )  {
        if (!vmap.count("dimension")) {
            std::clog << "Option --ising requires -d\n";
            exit(1);
        }
        
        int dim = vmap["dimension"].as<int>();
        rFactory = new IsingReservoir::IsingFactory(dim);
    } else {
        //Assume stochastic
        rFactory = new DefaultArgsReservoirFactory<StochasticReservoir>;
    }
    
    assert(rFactory);

    int dimension = 50;
    const double tau = vmap["tau"].as<double>();

    if ( output_style == CommaSeparated ) {
        printf("delta,epsilon,avg,max_surprise\n");
    }
    
    #pragma omp parallel for private(constants)
    for (int k=dimension*dimension; k>=0; k--) {
        constants.epsilon = (k % dimension)/(double)(dimension);
        constants.delta = .5 + .5*(k / dimension)/(double)(dimension);
        constants.tau = tau;
        simulate_and_print(constants, iterations, output_style, rFactory, verbose);
    }
    
    delete rFactory;
}

void simulate_and_print(Constants constants, int iterations, OutputType type, \
                        ReservoirFactory *factory, bool verbose) {
    if (constants.tau<.1) {
        std::clog<<"Warning: small tau.\n";
    }
    
    /*! Use this to change the length of the tape. */
    const int BIT_STREAM_LENGTH = 8;
    
    int first_pass_iterations = iterations;
    
    int *histogram = new int[1<<BIT_STREAM_LENGTH];
    std::fill_n(histogram, 1<<BIT_STREAM_LENGTH, 0);
    
    long double *p = new long double[1<<BIT_STREAM_LENGTH];
    long double *p_prime = new long double[1<<BIT_STREAM_LENGTH];
    long double sum = 0;
    
    const long double beta = log((1+constants.epsilon)/(1-constants.epsilon));
    long double max_surprise = 0;
    long double min_surprise = LONG_MAX;
    
    gsl_rng *localRNG = GSLRandomNumberGenerator();
        
    for (int k=0; k<first_pass_iterations; ++k) {
        System *currentSystem = new System(localRNG, constants,BIT_STREAM_LENGTH);
        Reservoir *reservoir = factory->create(localRNG, constants);
        
        currentSystem->evolveWithReservoir(reservoir);
        histogram[currentSystem->endingBitString]++;
        delete currentSystem;
        delete reservoir;
    }
    
    for(int k=0; k<1<<BIT_STREAM_LENGTH; k++) {
        int setBits = bitCount(k,BIT_STREAM_LENGTH);
        p[k] = gsl_ran_binomial_pdf(setBits, constants.delta, BIT_STREAM_LENGTH)/gsl_sf_choose(BIT_STREAM_LENGTH,setBits);
        p_prime[k]=static_cast<long double>(histogram[k])/(first_pass_iterations);
    }
    
    
    delete [] histogram;
    
    for(int k=0; k<iterations; k++) {
        System *currentSystem = new System(localRNG, constants, BIT_STREAM_LENGTH);
        Reservoir *reservoir = factory->create(localRNG,constants);
        
        currentSystem->evolveWithReservoir(reservoir);
        
        long double surprise = exp(beta*currentSystem->mass)*p_prime[currentSystem->endingBitString]/p[currentSystem->startingBitString];
        max_surprise = surprise > max_surprise ? surprise : max_surprise;
        min_surprise = surprise && surprise < min_surprise ? surprise : min_surprise;
        sum = sum + surprise;
        
        delete currentSystem;
        delete reservoir;
    }
    
    delete [] p_prime;
    delete [] p;

//    #pragma omp critical
    {
        if(type==CommaSeparated) {
            printf("%llf,%llf,%llf,%llf\n",constants.delta,constants.epsilon,sum/iterations,max_surprise);
        }
        if(type==PrettyPrint) {
            #pragma omp critical
            {
                print(beta);
                print(constants.delta);
                print(constants.epsilon);
                print(sum/iterations);
                print(max_surprise);
                print(sum);
                std::cout<<std::endl;
            }
            
        }
        
        if (type==Mathematica) {
            exit(1);
        }
    }
    
    gsl_rng_free(localRNG);
}

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
#include "Measurement.h"
#include "InstrumentFactories.h"
#include "Ising.h"
#include "Stochastic.h"

#define print(x) std::cout<<#x <<": " <<x<<std::endl;

namespace opt = boost::program_options;

int main(int argc, char * argv[]) {
    /*! Initialize options list.
     */
    
    for(int k=0; k<1000000; k++) {
        
    }
    
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
    ("dimension,d", opt::value<int>()->default_value(10),
        "Set the dimension of the Ising reservoir")
    ("tau,t", opt::value<double>()->default_value(5))
    ("distribution","Output the energy distribution of the ising reservoir.")
    ("clusters,c",opt::value<int>()->default_value(10),
        "The number of cluster steps used by the ising model.")
    ;
    
    opt::variables_map vmap;
    opt::store(opt::parse_command_line(argc,argv,desc),vmap);
    opt::notify(vmap);
    
    if (vmap.count("help")) {
        std::cout << desc << "\n";
        return 1;
    }
    
    if(vmap.count("distribution")) {
        isingEnergyDistribution(vmap["dimension"].as<int>(),
                                    vmap["clusters"].as<int>());
        exit(0);
    }
    
    verbose = vmap.count("verbose");
    
    int iterations = vmap.count("iterations") ? vmap["iterations"].as<int>() : default_iterations;
    
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
        int clst = vmap["clusters"].as<int>();
        rFactory = new IsingReservoir::Factory(dim,clst);
    } else {
        //Assume stochastic
        rFactory = new DefaultArgsReservoirFactory<Stochastic::Reservoir>;
    }
    
    SystemFactory *sFactory = new BinomialSystemFactory;
    
    assert(rFactory);
    assert(sFactory);
    
    int dimension = 50;
    const double tau = vmap["tau"].as<double>();

    printf("delta,epsilon,avg,max_surprise\n");
    
    #pragma omp parallel for private(constants) schedule(guided) 
    for (int k=dimension*dimension; k>=0; k--) {
        constants.setEpsilon((k % dimension)/(double)(dimension));
        constants.setDelta(.5 + .5*(k / dimension)/(double)(dimension));
        constants.setTau(tau);
        constants.setNbits(10);
        Measurement measurement(constants,iterations,rFactory,sFactory);
        MeasurementResult &result = measurement.getResult();
        printf("%s\n",outputString(result).c_str());
    }
    
    delete rFactory;
}

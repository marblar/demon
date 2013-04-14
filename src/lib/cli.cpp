//
//  cli.cpp
//  jdemon
//
//  Created by Mark Larus on 3/8/13.
//  Copyright (c) 2013 Kenyon College. All rights reserved.
//

#include <boost/program_options.hpp>
#include "cli.h"

namespace opt = boost::program_options;

opt::options_description optionsDescription() {
    opt::options_description desc("Allowed options");
    desc.add_options()
    ("iterations,n",opt::value<size_t>()->default_value(1<<16),
     "Number of iterations.")
    ("help","Show help message")
    ("verbose,v", "Show extensive debugging info")
    ("ising", "Use Ising reservoir. Requires -d.")
    ("stoch", "Use Stochastic reservoir. This is set by default, and overridden"
     " by --ising.")
    ("dimension,d", opt::value<int>()->default_value(10),
     "Set the dimension of the Ising reservoir")
    ("tau,t", opt::value<double>()->default_value(5))
    ("distribution","Output the energy distribution of the ising reservoir.")
    ("clusters,c",opt::value<int>()->default_value(10),
     "The number of cluster steps used by the ising model.")
    ("tmgas", "Use TMGas reservoir. Requires -d.")
    ;
    return desc;
}
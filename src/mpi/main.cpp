#include <boost/mpi.hpp>
#include <iostream>
#include <string>
#include <set>
#include <boost/serialization/string.hpp>
#include <boost/program_options.hpp>
#include "InstrumentFactories.h"
#include "Stochastic.h"
#include "Ising.h"
#include "TMGas.h"
#include "Measurement.h"
#include "cli.h"

#ifdef HAVE_CONFIG_H
#include "config.h"
#else
#define PACKAGE_STRING "UNKNOWN"
#define PACKAGE_VERSION "UNKNOWN"
#endif

namespace mpi = boost::mpi;
namespace opt = boost::program_options;

int main(int argc, char* argv[]) {
    bool verbose = false;
    opt::options_description desc = optionsDescription();
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
    
    size_t iterations = vmap["iterations"].as<size_t>();
    
    DemonBase::ReservoirFactory *rFactory = NULL;
    if ( vmap.count("ising") )  {
        if (!vmap.count("dimension")) {
            std::clog << "Option --ising requires -d\n";
            exit(1);
        }
        
        int dim = vmap["dimension"].as<int>();
        int clst = vmap["clusters"].as<int>();
        rFactory = new Ising::Reservoir::Factory(dim,clst);
    } else if ( vmap.count("tmgas") ){
        if (!vmap.count("dimension")) {
            std::clog << "Option --tmgas requires -d\n";
            exit(1);
        }
        
        int dim = vmap["dimension"].as<int>();
        rFactory = new TMGas::Reservoir::Factory(dim);
    } else {
        //Assume stochastic
        rFactory = new DemonBase::DefaultArgsReservoirFactory<Stochastic::Reservoir>;
    }
    
    DemonBase::SystemFactory *sFactory = new DemonBase::BinomialSystemFactory;
    
    assert(rFactory);
    assert(sFactory);
    
    int dimension = 50;
    const double tau = vmap["tau"].as<double>();
    
    mpi::environment env(argc, argv);
    mpi::communicator world;
    char hostname[256];
    int requester;
    
    if (world.rank()==0) {
        std::string args;
        for (int k=1; k!=argc; ++k) {
            args += " ";
            args += argv[k];
        }
        printf("%s%s\n",PACKAGE_STRING,args.c_str());
        std::clog << "Running v" << PACKAGE_VERSION << " on " << world.size()
                    << " nodes." << std::endl;
        std::clog << (double)(dimension*dimension*iterations)/world.size()
                    << " simulations per node." << std::endl;
        printf("%s\n",DemonBase::outputHeader().c_str());
    }
    
    world.barrier();
    
    gethostname(hostname,255);
    fprintf(stderr, "Hello world from host %s (rank %d)\n",hostname,world.rank());
    
    DemonBase::Experiment::range work;
    DemonBase::Experiment experiment;
    experiment.iterations = iterations;
    experiment.dimension = dimension;
    experiment.sfactory = sFactory;
    experiment.rfactory = rFactory;
    
    for (int k=world.rank(); k<dimension*dimension; k+=world.size()) {
        DemonBase::MeasurementResult result = experiment.performIteration(k);
        printf("%s\n",outputString(result).c_str());
    }
    
    std::clog<<"Rank "<< world.rank()<<" finished.\n";
    return 0;
}

#include <boost/mpi.hpp>
#include <iostream>
#include <string>
#include <set>
#include <boost/serialization/string.hpp>
#include "InstrumentFactories.h"
#include "Stochastic.h"
#include "Ising.h"
#include "Measurement.h"

namespace mpi = boost::mpi;


int main(int argc, char* argv[])
{
    mpi::environment env(argc, argv);
    mpi::communicator world;
    char hostname[256];
    int requester;
    const int dimension = 50;
    const int iterations = 1000;
    
    gethostname(hostname,255);
    std::clog << "Hello world from host " << hostname << std::endl;

    ReservoirFactory *rFactory = new DefaultArgsReservoirFactory<StochasticReservoir>;
    SystemFactory *sFactory = new BinomialSystemFactory;
    Experiment::range work;
    Experiment experiment;
    experiment.iterations = iterations;
    experiment.dimension = dimension;
    experiment.sfactory=sFactory;
    experiment.rfactory=rFactory;
    
    for (int k=world.rank(); k<iterations; k+=world.size()) {
        MeasurementResult result = experiment.performIteration(k);
        printf("%s\n",outputString(result).c_str());
    }
    
    std::clog<<"Node "<<world.rank()<<" finished.\n";
    return 0;
}

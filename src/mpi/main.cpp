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
    
    int requester;
    const int dimension = 50;
    const int iterations = 1<<23;
    
    if (world.rank() == 0) {
        printf("Hello world of size %d.\n",world.size());
        mpi::request reqs[1];
        int currentPosition = 0;
        while (currentPosition<dimension*dimension) {
            world.recv(mpi::any_source, 1, requester);
            if (requester) {
                // TODO: This too would make an awesome iterator
                Experiment::range work;
                int remaining = dimension*dimension - currentPosition;
                int chunkSize = remaining/world.size() + 1;
                work.first = currentPosition;
                work.second = currentPosition + chunkSize;
                currentPosition += chunkSize + 1;
                world.send(requester, 1, work);
            }
        }
        std::set<int> notFinished;
        for(int k=1; k<world.size(); k++) {
            notFinished.insert(k);
        }
        while (!notFinished.empty()) {
            world.recv(mpi::any_source, 1, requester);
            if (requester) {
                Experiment::range work(-1,0);
                world.send(requester,1,work);
                notFinished.erase(requester);
            }
        }
        std::clog<<"Master finished.\n";
    } else {
        ReservoirFactory *rFactory = new DefaultArgsReservoirFactory<StochasticReservoir>;
        SystemFactory *sFactory = new BinomialSystemFactory;
        Experiment::range work;
        Experiment experiment;
        experiment.iterations = iterations;
        experiment.dimension = dimension;
        experiment.sfactory=sFactory;
        experiment.rfactory=rFactory;
        
        world.send(0, 1, world.rank());
        world.recv(0, 1, work);
        do {
            experiment.performWork(work);
            world.send(0, 1, world.rank());
            world.recv(0, 1, work);
        } while(work.first!=-1);
        std::clog<<"Node "<<world.rank()<<" finished.\n";
    }
    return 0;
}

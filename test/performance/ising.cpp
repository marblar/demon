#include <boost/test/unit_test.hpp>

#include "ReservoirBenchmark.h"
#include "Ising.h"

BOOST_AUTO_TEST_SUITE(IsingBenchmark);

BOOST_AUTO_TEST_CASE( performMeasurement ) {
    int iterations = 100;
    ReservoirFactory *rFactory = new IsingReservoir::IsingFactory(20,20);
    reservoirBenchmark(rFactory,iterations);
}

BOOST_AUTO_TEST_SUITE_END()
#include <boost/test/unit_test.hpp>

#include "ReservoirBenchmark.h"
#include "Stochastic.h"

BOOST_AUTO_TEST_SUITE(IsingBenchmark);

BOOST_AUTO_TEST_CASE( performMeasurement ) {
    int iterations = 10000;
    ReservoirFactory *rFactory = new DefaultArgsReservoirFactory<StochasticReservoir>;
    reservoirBenchmark(rFactory,iterations);
}

BOOST_AUTO_TEST_SUITE_END()

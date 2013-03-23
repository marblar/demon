#define BOOST_TEST_ALTERNATIVE_INIT_API
#include <boost/test/unit_test.hpp>

#include "ReservoirBenchmark.h"
#include "Ising.h"
using namespace boost::unit_test;

void performMeasurement() {
    int iterations = 100;
    ReservoirFactory *rFactory = new Ising::Reservoir::Factory(20,20);
    reservoirBenchmark(rFactory,iterations);
}

bool init_function() {
    for (int k=0; k<15; ++k) {
        framework::master_test_suite().add(BOOST_TEST_CASE(&performMeasurement),0,60);
    }
    framework::master_test_suite().p_name.value = "IsingBenchmark";
    return true;
}

int main( int argc, char* argv[] )
{
    return ::boost::unit_test::unit_test_main( init_function, argc, argv );
}
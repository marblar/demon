#include <boost/test/unit_test.hpp>
#include <gsl/gsl_rng.h>
#include "InstrumentFactories.h"
#include "Measurement.h"
#include "Utilities.h"
using namespace DemonBase;

void reservoirBenchmark(ReservoirFactory *rFactory,int iterations) {
    gsl_rng *rng = GSLRandomNumberGenerator();
    gsl_rng_set(rng,0);
    SystemFactory *sFactory = new BinomialSystemFactory;
    Constants constants;
    int dimension = 10;
    double tau = 1;
    BOOST_REQUIRE(rFactory);
    BOOST_REQUIRE(sFactory);
    for (int k=dimension*dimension; k>=0; k--) {
        constants.setEpsilon((k % dimension)/(double)(dimension));
        constants.setDelta(.5 + .5*(k / dimension)/(double)(dimension));
        constants.setTau(tau);
        constants.setNbits(8);
        Measurement measurement(constants,iterations,rFactory,sFactory,rng);
        MeasurementResult &result = measurement.getResult();
        BOOST_REQUIRE(measurement.isComplete());
    }
    delete sFactory;
    gsl_rng_free(rng);
}

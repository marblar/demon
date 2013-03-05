//
//  IsingPerformance.cpp
//  jdemon
//
//  Created by Mark Larus on 3/3/13.
//  Copyright (c) 2013 Kenyon College. All rights reserved.
//

#include "Utilities.h"
#include "ReservoirBenchmark.h"
#include "Measurement.h"

void ReservoirBenchmark::setUp() {
    rng = GSLRandomNumberGenerator();
    gsl_rng_set(rng, 0);
}

void ReservoirBenchmark::tearDown() {
//    gsl_rng_free(rng);
}

void ReservoirBenchmark::performMeasurement() {
    ReservoirFactory *rFactory = createReservoirFactory();   
    SystemFactory *sFactory = new BinomialSystemFactory;
    Constants constants;
    int dimension = 10;
    double tau = 1;
    
    CPPUNIT_ASSERT(rFactory);
    CPPUNIT_ASSERT(sFactory);

    for (int k=dimension*dimension; k>=0; k--) {
        constants.setEpsilon((k % dimension)/(double)(dimension));
        constants.setDelta(.5 + .5*(k / dimension)/(double)(dimension));
        constants.setTau(tau);
        constants.setNbits(8);
        Measurement measurement(constants,iterations(),rFactory,sFactory,rng);
        MeasurementResult &result = measurement.getResult();
        CPPUNIT_ASSERT(measurement.isComplete());
    }

    delete rFactory;
    delete sFactory;
}

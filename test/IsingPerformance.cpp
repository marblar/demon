//
//  IsingPerformance.cpp
//  jdemon
//
//  Created by Mark Larus on 3/3/13.
//  Copyright (c) 2013 Kenyon College. All rights reserved.
//

#include "IsingPerformance.h"
#include "SystemTest.h"
#include "Ising.h"
#include "Utilities.h"
#include "Reservoir.h"

CPPUNIT_TEST_SUITE_REGISTRATION(IsingPerformance);

void IsingPerformance::setUp() {
    rng = GSLRandomNumberGenerator();
    gsl_rng_set(rng, 0);
}

void IsingPerformance::tearDown() {
    gsl_rng_free(rng);
}

void IsingPerformance::testColdMetropolis() {
    metropolis(.9);
}

void IsingPerformance::metropolis(double e) {
    Constants c;
    c.setDelta(.5);
    c.setEpsilon(e);
    c.setNbits(8);
    c.setTau(1);
    IsingReservoir reservoir(rng,c,20,20);
    for (int k = 0; k<10000; ++k) {
        reservoir.reset();
    }
}

void IsingPerformance::testLukewarmMetropolis() {
    metropolis(.45);
}

void IsingPerformance::testHotMetropolis() {
    metropolis(.1);
}

void IsingPerformance::testIsingStep() {
    Constants c;
    c.setDelta(.5);
    c.setEpsilon(.2);
    c.setNbits(8);
    c.setTau(1);
    IsingReservoir reservoir(rng,c,20,20);
    Reservoir::InteractionResult result;
    for (int k = 0; k<1000000; ++k) {
        reservoir.isingStep(result);
    }
}
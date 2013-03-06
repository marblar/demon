//
//  StochTest.cpp
//  jdemon
//
//  Created by Mark Larus on 3/5/13.
//  Copyright (c) 2013 Kenyon College. All rights reserved.
//

#include "StochTest.h"
#include "Utilities.h"
#include "Stochastic.h"

CPPUNIT_TEST_SUITE_REGISTRATION(StochReservoirTest);

static Constants defaultConstants() {
    Constants c;
    c.setDelta(.5);
    c.setEpsilon(0);
    c.setTau(1);
    c.setNbits(8);
    return c;
}

void StochReservoirTest::setUp() {
    rng = GSLRandomNumberGenerator();
    gsl_rng_set(rng, 0);
}

void StochReservoirTest::tearDown() {
    gsl_rng_free(rng);
}

void StochReservoirTest::testInteractBit() {
    // This test is volatile with changes to
    // the random number generator.
    Constants c = defaultConstants();
    c.setEpsilon(1);
    StochasticReservoir res(rng, c);
    Reservoir::InteractionResult result = res.interactWithBit(1);
    CPPUNIT_ASSERT(!result.bit);
}

void StochReservoirTest::testInvalidParameters() {
    
}

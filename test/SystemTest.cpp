//
//  SystemTest.cpp
//  jdemon
//
//  Created by Mark Larus on 3/1/13.
//  Copyright (c) 2013 Kenyon College. All rights reserved.
//

#include <gsl/gsl_randist.h>

#include "SystemTest.h"
#include "Reservoir.h"
#include "Utilities.h"
#include <algorithm>

CPPUNIT_TEST_SUITE_REGISTRATION(SystemTest);
CPPUNIT_TEST_SUITE_REGISTRATION(ConstantsTest);


Constants defaultConstants() {
    Constants c(.7,.1,.7);
    return c;
}


class MockReservoirFlipDown : public Reservoir {
public:
    InteractionResult interactWithBit(int bit) {
        InteractionResult result;
        result.work = 1;
        result.bit = 0;
        ++bitsProvided;
        return result;
    };
    void reset() { /* Do nothing */ }
    int bitsProvided;
    MockReservoirFlipDown(Constants constants) : bitsProvided(0), Reservoir(constants) {}
};

class SpecificEndStringReservoir : public Reservoir {
public:
    InteractionResult interactWithBit(int bit) {
        InteractionResult result;
        result.work = 1;
        result.bit = (end>>bitPos)&1;
        bitPos++;
        return result;
    };
    void reset() { /* Do nothing */ }
    int end;
    int bitPos;
    SpecificEndStringReservoir(Constants constants, int end_) :
    Reservoir(constants), end(end_), bitPos(0) {}
};

void SystemTest::setUp() {
    rng = GSLRandomNumberGenerator();
    // Always set the seed to 0. If anything dependent on this worked once
    // it will work again on a different box.
    gsl_rng_set(rng, 0);
}

void SystemTest::tearDown() {
    gsl_rng_free(rng);
}

void SystemTest::testStartingString() {
    int nbits = 8;
    int resultSize = nbits + 1;
    int results[resultSize];
    int iterations = 10000;
    
    std::fill(results, results + resultSize, 0);
    
    for (int k = 0; k<iterations; k++) {
        System *system = new System(rng,defaultConstants());
        ++results[bitCount(system->startingBitString, 8)];
        delete system;
    }
    
    for (int k = 0; k<resultSize; k++) {
        double expected = gsl_ran_binomial_pdf(k, defaultConstants().getDelta(), 8);
        double actual = ((double)results[k])/iterations;
        CPPUNIT_ASSERT_DOUBLES_EQUAL(expected, actual, .1);
    }
};

void SystemTest::testConstructor() {
    int nbits = 8;
    System system(rng,defaultConstants(),nbits);
    
    CPPUNIT_ASSERT_EQUAL(system.nbits, nbits);
    CPPUNIT_ASSERT_EQUAL(defaultConstants().getEpsilon(), system.constants.getEpsilon());
    CPPUNIT_ASSERT_EQUAL(defaultConstants().getDelta(), system.constants.getDelta());
    CPPUNIT_ASSERT_EQUAL(defaultConstants().getTau(), system.constants.getTau());
    CPPUNIT_ASSERT_EQUAL(system.bitPosition, 0);
    CPPUNIT_ASSERT_EQUAL(system.mass, 0);
    
}

void SystemTest::testNbits() {
    int nbits = 8;
    System system(rng,defaultConstants(),nbits);
    MockReservoirFlipDown res(defaultConstants());
    system.evolveWithReservoir(&res);
    
    CPPUNIT_ASSERT_EQUAL(nbits, res.bitsProvided);
    
    nbits = 10;
    MockReservoirFlipDown res2(defaultConstants());
    System otherSystem(rng,defaultConstants(),nbits);
    otherSystem.evolveWithReservoir(&res2);
    
    CPPUNIT_ASSERT_EQUAL(nbits, res2.bitsProvided);
}

void SystemTest::testEndingString() {
    int nbits = 8;
    unsigned int endString = 6;
    
    gsl_rng *rng = GSLRandomNumberGenerator();
    System system(rng,defaultConstants(),nbits);
    SpecificEndStringReservoir res(defaultConstants(),endString);
    system.evolveWithReservoir(&res);
    
    CPPUNIT_ASSERT_EQUAL(system.endingBitString, endString);
}

void ConstantsTest::testHighTemperature() {
    double delta = .5;
    double epsilon = 0;
    double tau = 1;
    Constants c(delta,epsilon,tau);
    CPPUNIT_ASSERT(c.getBeta()==0);
}

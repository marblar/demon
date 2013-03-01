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
    Constants c;
    c.delta = .7;
    c.tau = 1;
    c.epsilon = .7;
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
}

void SystemTest::tearDown() {
    gsl_rng_free(rng);
}

void SystemTest::testStartingString() {
    /*! This is a probabilistic test, which makes it basically satan
        as far as I'm concerned. 
        
        TODO: Make sure the fail rate is less than
        one in a billion. Require failure of several string lengths, combined
        with 3 sigmas for the delta value?
     */
    int nbits = 8;
    int results[9];
    int iterations = 10000;
    
    std::fill(results, results+9, 0);
    
    for (int k = 0; k<iterations; k++) {
        System *system = new System(rng,defaultConstants());
        ++results[bitCount(system->startingBitString, 8)];
        delete system;
    }
    
    for (int k = 0; k<9; k++) {
        double expected = gsl_ran_binomial_pdf(k, defaultConstants().delta, 8);
        double actual = ((double)results[k])/iterations;
        CPPUNIT_ASSERT_DOUBLES_EQUAL(expected, actual, .1);
    }
};

void SystemTest::testConstructor() {
    int nbits = 8;
    System system(rng,defaultConstants(),nbits);
    
    CPPUNIT_ASSERT_EQUAL(system.nbits, nbits);
    CPPUNIT_ASSERT_EQUAL(defaultConstants().epsilon, system.constants.epsilon);
    CPPUNIT_ASSERT_EQUAL(defaultConstants().delta, system.constants.delta);
    CPPUNIT_ASSERT_EQUAL(defaultConstants().tau, system.constants.tau);
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
    Constants c;
    c.delta = .5;
    c.epsilon = 0;
    c.tau = 1;
    
    CPPUNIT_ASSERT(c.beta()==0);
}

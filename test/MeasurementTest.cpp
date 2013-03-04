//
//  MeasurementTest.cpp
//  jdemon
//
//  Created by Mark Larus on 3/3/13.
//  Copyright (c) 2013 Kenyon College. All rights reserved.
//

#include "MeasurementTest.h"
#include "Measurement.h"
#include "Reservoir.h"
#include "InstrumentFactories.h"
#include "Utilities.h"
#include <map>

CPPUNIT_TEST_SUITE_REGISTRATION(MeasurementTest);

#define DEMON_ASSERT_DOUBLES_EQUAL(exp,actual,d) \
            CPPUNIT_ASSERT_DOUBLES_EQUAL_MESSAGE(#exp"!="#actual,exp,actual,d)
class TrivialReservoir : public Reservoir {
public:
    InteractionResult interactWithBit(int bit) {
        InteractionResult result;
        result.work = 0;
        result.bit = bit;
        return result;
    };
    void reset() { /* Do nothing */ }
    TrivialReservoir(gsl_rng *rng, Constants) : Reservoir(constants) {}
};

static Constants defaultConstants() {
    Constants c;
    c.setDelta(.5);
    c.setEpsilon(0);
    c.setTau(1);
    c.setNbits(8);
    return c;
}

void MeasurementTest::testWithTrivialReservoir() {
    Constants c;
    c.setDelta(.5);
    c.setEpsilon(0);
    c.setTau(1);
    c.setNbits(8);
    ReservoirFactory *rfactory = new DefaultArgsReservoirFactory<TrivialReservoir>;
    SystemFactory *sfactory = new BinomialSystemFactory;
    Measurement measurement(c,1000,rfactory,sfactory,rng);
    MeasurementResult result = measurement.getResult();
    CPPUNIT_ASSERT_DOUBLES_EQUAL_MESSAGE("averageJ",\
                                         1, result.averageJ, .02);
    // Too lazy to work this out analytically, but this result is consistent,
    // and so this test will lock this functionality in.
    CPPUNIT_ASSERT_DOUBLES_EQUAL_MESSAGE("maxJ",2.56,
                                         result.maxJ, .01);
    delete rfactory;
    delete sfactory;
}

void MeasurementTest::testTrivialCalculateJ() {
    Constants c = defaultConstants();
    c.setNbits(2);
    int starting_string = 1;
    System system(c,starting_string);
    long double *p = new long double[1<<2];
    long double *p_prime = new long double[1<<2];
    std::fill(p, p+(1<<2), 1.0/(1<<2));
    std::fill(p_prime, p_prime+(1<<2), 1.0/(1<<2));
    system.startingBitString = 1;
    system.endingBitString = 1;
    
    delete p;
    delete p_prime;
    
    double J = calculateJ(system, p, p_prime);
    CPPUNIT_ASSERT_DOUBLES_EQUAL_MESSAGE("Trivial J", 1, J, .001);
}

void MeasurementTest::testResultsConstants() {
    Constants c = defaultConstants();
    ReservoirFactory *rfactory = new DefaultArgsReservoirFactory<TrivialReservoir>;
    SystemFactory *sfactory = new BinomialSystemFactory;
    Measurement measurement(c,1,rfactory,sfactory);
    MeasurementResult result = measurement.getResult();
    DEMON_ASSERT_DOUBLES_EQUAL(c.getBeta(), result.constants.getBeta(), 0);
    DEMON_ASSERT_DOUBLES_EQUAL(c.getDelta(), result.constants.getDelta(), 0);
    DEMON_ASSERT_DOUBLES_EQUAL(c.getEpsilon(), result.constants.getEpsilon(), 0);
    DEMON_ASSERT_DOUBLES_EQUAL(c.getNbits(), result.constants.getNbits(), 0);
    delete rfactory;
    delete sfactory;
}

void MeasurementTest::setUp() {
    rng = GSLRandomNumberGenerator();
    gsl_rng_set(rng, 0);
}

void MeasurementTest::tearDown() {
    gsl_rng_free(rng);
}

void MeasurementTest::testOutput() {
    Constants c;
    c.setDelta(.7);
    c.setEpsilon(0);
    
    MeasurementResult result;
    result.constants = c;
    result.maxJ = 1000.93;
    result.averageJ = 1;
    
    std::string actualResultString = outputString(result);
    std::string expectedResultString = "0.7,0,1,1000.93";
    CPPUNIT_ASSERT_EQUAL(actualResultString, expectedResultString);
}


void MeasurementTest::testIncompleteDestructor() {
    // Regression test for a memory error.
    {
        Constants c;
        ReservoirFactory *rf = new DefaultArgsReservoirFactory<TrivialReservoir>;
        SystemFactory *sf = new BinomialSystemFactory;
        Measurement m(c, 1, rf, sf);
        //If the measurement is not complete, that means that the destructor
        // will be called without initializing the internal arrays. 
        CPPUNIT_ASSERT(!m.isComplete());
    }
}

void MeasurementTest::testCompleteFlag() {
    Constants c;
    c.setTau(1);
    c.setDelta(1);
    c.setEpsilon(0);
    c.setNbits(1);
    ReservoirFactory *rf = new DefaultArgsReservoirFactory<TrivialReservoir>;
    SystemFactory *sf = new BinomialSystemFactory;
    Measurement m(c, 1, rf, sf);
    CPPUNIT_ASSERT(!m.isComplete());
    MeasurementResult &result = m.getResult();
    CPPUNIT_ASSERT(m.isComplete());
}

void MeasurementTest::testZeroBits() {
    Constants c;
    c.setTau(1);
    c.setDelta(1);
    c.setEpsilon(0);
    c.setNbits(0);
    ReservoirFactory *rf = new DefaultArgsReservoirFactory<TrivialReservoir>;
    SystemFactory *sf = new BinomialSystemFactory;
    Measurement m(c, 1, rf, sf);
    MeasurementResult result;
    CPPUNIT_ASSERT_THROW(result=m.getResult(), InvalidNbitsError);
}

void MeasurementTest::testNegativeBits() {
    Constants c;
    c.setTau(1);
    c.setDelta(1);
    c.setEpsilon(0);
    c.setNbits(-1000);
    ReservoirFactory *rf = new DefaultArgsReservoirFactory<TrivialReservoir>;
    SystemFactory *sf = new BinomialSystemFactory;
    Measurement m(c, 1, rf, sf);
    MeasurementResult result;
    CPPUNIT_ASSERT_THROW(result=m.getResult(), InvalidNbitsError);
}


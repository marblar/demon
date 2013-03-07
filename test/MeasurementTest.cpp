//
//  MeasurementTest.cpp
//  jdemon
//
//  Created by Mark Larus on 3/3/13.
//  Copyright (c) 2013 Kenyon College. All rights reserved.
//

#include <boost/test/unit_test.hpp>
#include <map>

#include "TestFixtures.h"
#include "Measurement.h"
#include "Reservoir.h"
#include "InstrumentFactories.h"
#include "Utilities.h"
#include "MockObjects.h"

struct MeasurementTestFixture : \
    public ConstantsTestFixture, \
    public RandomNumberTestFixture \
{};

BOOST_FIXTURE_TEST_SUITE(MeasurementTest, MeasurementTestFixture);

BOOST_AUTO_TEST_CASE( testWithTrivialReservoir ) {
    ReservoirFactory *rfactory = new DefaultArgsReservoirFactory<TrivialReservoir>;
    SystemFactory *sfactory = new BinomialSystemFactory;
    Measurement measurement(c,1000,rfactory,sfactory,rng);
    MeasurementResult result = measurement.getResult();
    
    BOOST_CHECK_CLOSE(1, result.averageJ, 1);
    // Too lazy to work this out analytically, but this result is consistent,
    // and so this test will lock this functionality in.

    BOOST_CHECK_CLOSE(2.56, result.maxJ, .5);

    delete rfactory;
    delete sfactory;
}

BOOST_AUTO_TEST_CASE (testTrivialCalculateJ ) {
    c.setNbits(2);
    int starting_string = 1;
    System system(c,starting_string);
    long double *p = new long double[1<<2];
    long double *p_prime = new long double[1<<2];
    std::fill(p, p+(1<<2), 1.0/(1<<2));
    std::fill(p_prime, p_prime+(1<<2), 1.0/(1<<2));
    system.startingBitString = 1;
    system.endingBitString = 1;
    
    delete [] p;
    delete [] p_prime;
    
    double J = calculateJ(system, p, p_prime);
    BOOST_CHECK_CLOSE(1, J, .001);
}

BOOST_AUTO_TEST_CASE( testResultsConstants ) {
    ReservoirFactory *rfactory = new DefaultArgsReservoirFactory<TrivialReservoir>;
    SystemFactory *sfactory = new BinomialSystemFactory;
    Measurement measurement(c,1,rfactory,sfactory);
    MeasurementResult result = measurement.getResult();
    BOOST_CHECK_EQUAL(c.getBeta(), result.constants.getBeta());
    BOOST_CHECK_EQUAL(c.getDelta(), result.constants.getDelta());
    BOOST_CHECK_EQUAL(c.getEpsilon(), result.constants.getEpsilon());
    BOOST_CHECK_EQUAL(c.getNbits(), result.constants.getNbits());
    delete rfactory;
    delete sfactory;
}

BOOST_AUTO_TEST_CASE( testOutput ) {
    MeasurementResult result;
    result.constants = c;
    result.maxJ = 1000.93;
    result.averageJ = 1;
    
    std::string actualResultString = outputString(result);
    std::string expectedResultString = "0.5,0,1,1000.93";
    BOOST_CHECK_EQUAL(actualResultString, expectedResultString);
}


BOOST_AUTO_TEST_CASE( testIncompleteDestructor ) {
    // Regression test for a memory error.
    {
        ReservoirFactory *rf = new DefaultArgsReservoirFactory<TrivialReservoir>;
        SystemFactory *sf = new BinomialSystemFactory;
        Measurement m(c, 1, rf, sf);
        //If the measurement is not complete, that means that the destructor
        // will be called without initializing the internal arrays. 
        BOOST_REQUIRE(!m.isComplete());
    }
}

BOOST_AUTO_TEST_CASE( testCompleteFlag ) {
    ReservoirFactory *rf = new DefaultArgsReservoirFactory<TrivialReservoir>;
    SystemFactory *sf = new BinomialSystemFactory;
    Measurement m(c, 1, rf, sf);
    BOOST_REQUIRE(!m.isComplete());
    MeasurementResult &result = m.getResult();
    BOOST_REQUIRE(m.isComplete());
}

BOOST_AUTO_TEST_CASE( testZeroBits ) {
    Constants c;
    c.setTau(1);
    c.setDelta(1);
    c.setEpsilon(0);
    c.setNbits(0);
    ReservoirFactory *rf = new DefaultArgsReservoirFactory<TrivialReservoir>;
    SystemFactory *sf = new BinomialSystemFactory;
    Measurement m(c, 1, rf, sf);
    MeasurementResult result;
    BOOST_REQUIRE_THROW(result=m.getResult(), InvalidNbitsError);
}

BOOST_AUTO_TEST_CASE( testNegativeBits ) {
    c.setNbits(-1000);
    ReservoirFactory *rf = new DefaultArgsReservoirFactory<TrivialReservoir>;
    SystemFactory *sf = new BinomialSystemFactory;
    Measurement m(c, 1, rf, sf);
    MeasurementResult result;
    BOOST_REQUIRE_THROW(result=m.getResult(), InvalidNbitsError);
}

BOOST_AUTO_TEST_SUITE_END()

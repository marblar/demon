//
//  SystemTest.cpp
//  jdemon
//
//  Created by Mark Larus on 3/1/13.
//  Copyright (c) 2013 Kenyon College. All rights reserved.
//

#include <gsl/gsl_randist.h>
#include <algorithm>
#include <boost/test/included/unit_test.hpp>

#include "TestFixtures.h"
#include "MockObjects.h"

#include "Reservoir.h"
#include "Utilities.h"
#include "InstrumentFactories.h"

static int defaultStartingString() {
    return 2;
}

struct SystemTestFixture : \
    public ConstantsTestFixture, \
    public RandomNumberTestFixture \
{};

BOOST_FIXTURE_TEST_SUITE(SystemTest,SystemTestFixture);

BOOST_AUTO_TEST_CASE( testSystemFactory ) {
    BinomialSystemFactory sfactory;
    
    int resultSize = c.getNbits()+1;
    int results[resultSize];
    int iterations = 10000;
    
    std::fill(results, results + resultSize, 0);
    
    for (int k = 0; k<iterations; k++) {
        System *system = sfactory.create(rng, c);
        ++results[bitCount(system->startingBitString, c.getNbits())];
        delete system;
    }
    
    for (int k = 0; k<resultSize; k++) {
        double expected = gsl_ran_binomial_pdf(k, c.getDelta(), 8);
        double actual = ((double)results[k])/iterations;
        BOOST_REQUIRE_CLOSE(expected, actual, .1);
    }
};

BOOST_AUTO_TEST_CASE( testConstructor ) {
    System system(c,defaultStartingString());
    
    BOOST_CHECK_EQUAL(c.getEpsilon(), system.constants.getEpsilon());
    BOOST_CHECK_EQUAL(c.getDelta(), system.constants.getDelta());
    BOOST_CHECK_EQUAL(c.getTau(), system.constants.getTau());
    BOOST_CHECK_EQUAL(system.bitPosition, 0);
    BOOST_CHECK_EQUAL(system.mass, 0);
}

BOOST_AUTO_TEST_CASE( testNbits )  {
    System system(c,defaultStartingString());
    MockReservoirFlipDown res(c);
    system.evolveWithReservoir(&res);
    
    BOOST_CHECK_EQUAL(c.getNbits(), res.bitsProvided);
    
    c.setNbits(10);
    MockReservoirFlipDown res2(c);
    System otherSystem(c,defaultStartingString());
    otherSystem.evolveWithReservoir(&res2);
    
    BOOST_CHECK_EQUAL(c.getNbits(), res2.bitsProvided);
}

BOOST_AUTO_TEST_CASE( testEndingString ) {
    unsigned int endString = 6;
    System system(c,defaultStartingString());
    SpecificEndStringReservoir res(c,endString);
    system.evolveWithReservoir(&res);
    
    BOOST_REQUIRE_EQUAL(system.endingBitString, endString);
}

BOOST_AUTO_TEST_CASE ( testReuseSystem ) {
    System system(c,defaultStartingString());
    Reservoir *res = new MockReservoirFlipDown(c);
    system.bitPosition = 1;
    
    BOOST_REQUIRE_THROW(system.evolveWithReservoir(res),InconsistentSystemState);
}

BOOST_AUTO_TEST_CASE ( testHighTemperature ) {
    double delta = .5;
    double epsilon = 0;
    double tau = 1;
    Constants c(delta,epsilon,tau);
    BOOST_CHECK_EQUAL(c.getBeta(),0);
}

BOOST_AUTO_TEST_SUITE_END()
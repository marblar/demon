//
//  StochTest.cpp
//  jdemon
//
//  Created by Mark Larus on 3/5/13.
//  Copyright (c) 2013 Kenyon College. All rights reserved.
//

#include <boost/test/unit_test.hpp>

#include "Utilities.h"
#include "Stochastic.h"

#include "TestFixtures.h"

struct StochTestFixture : \
    public ConstantsTestFixture, \
    public RandomNumberTestFixture \
{};

BOOST_FIXTURE_TEST_SUITE(StochReservoirTest, StochTestFixture);

BOOST_AUTO_TEST_CASE( testInteractBit ) {
    // This test is volatile with changes to
    // the random number generator.
    c.setEpsilon(1);
    StochasticReservoir res(rng, c);
    Reservoir::InteractionResult result = res.interactWithBit(1);
    BOOST_REQUIRE(!result.bit);
}

BOOST_AUTO_TEST_CASE ( testInvalidEpsilon ) {
    c.setEpsilon(1.01);
    StochasticReservoir *res;
    BOOST_CHECK_THROW(res = new StochasticReservoir(rng,c), InvalidEpsilonError);
    delete res;
    c.setEpsilon(-0.01);
    BOOST_CHECK_THROW(res = new StochasticReservoir(rng,c), InvalidEpsilonError);
    delete res;
}

BOOST_AUTO_TEST_CASE ( testInvalidTau ) {
    c.setTau(-1);
    StochasticReservoir *res;
    BOOST_CHECK_THROW(res = new StochasticReservoir(rng,c), InvalidTauError);
    delete res;
    c.setTau(0);
    BOOST_CHECK_THROW(res = new StochasticReservoir(rng,c), InvalidTauError);
    delete res;
}

BOOST_AUTO_TEST_CASE( testInvalidDelta ) {
    c.setDelta(.49);
    StochasticReservoir *res;
    BOOST_CHECK_THROW(res = new StochasticReservoir(rng,c), InvalidDeltaError);
    delete res;
    c.setDelta(1.01);
    BOOST_CHECK_THROW(res = new StochasticReservoir(rng,c), InvalidDeltaError);
    delete res;
}

BOOST_AUTO_TEST_SUITE_END()

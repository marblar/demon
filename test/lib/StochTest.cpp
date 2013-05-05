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
using namespace DemonBase;

struct StochTestFixture : \
    public ConstantsTestFixture, \
    public RandomNumberTestFixture \
{};

BOOST_FIXTURE_TEST_SUITE(StochReservoirTest, StochTestFixture);

// This test isn't portable at all. Let's skip it for now
BOOST_AUTO_TEST_CASE( testInteractBit ) {
//    c.setEpsilon(1);
//    Stochastic::Reservoir res(rng, c);
//    DemonBase::Reservoir::InteractionResult result = res.interactWithBit(1);
//    BOOST_REQUIRE(!result.bit);
}

BOOST_AUTO_TEST_CASE ( testInvalidEpsilon ) {
    c.setEpsilon(1.05);
    Stochastic::Reservoir res(rng,c);
    BOOST_CHECK_THROW(res.interactWithBit(1), InvalidEpsilonError);
    
    c.setEpsilon(-0.05);
    Stochastic::Reservoir res2(rng,c);
    BOOST_CHECK_THROW(res2.interactWithBit(1), InvalidEpsilonError);
}

BOOST_AUTO_TEST_CASE ( testInvalidTau ) {
    c.setTau(-1);
    Stochastic::Reservoir res(rng,c);
    BOOST_CHECK_THROW(res.interactWithBit(1), InvalidTauError);
    c.setTau(0);
    Stochastic::Reservoir res2(rng,c);
    BOOST_CHECK_THROW(res2.interactWithBit(1), InvalidTauError);
}

BOOST_AUTO_TEST_CASE( testInvalidDelta ) {
    c.setEpsilon(.5);
    c.setDelta(-.01);
    Stochastic::Reservoir res(rng,c);
    BOOST_CHECK_THROW(res.interactWithBit(1), InvalidDeltaError);
    
    c.setDelta(1.01);
    Stochastic::Reservoir res2(rng,c);
    BOOST_CHECK_THROW(res2.interactWithBit(1), InvalidDeltaError);
}

BOOST_AUTO_TEST_CASE ( validEpsilon ) {
    // Regression: Make sure e=0 is still valid.
    c.setEpsilon(0);
    Stochastic::Reservoir res(rng,c);
    BOOST_CHECK_NO_THROW(res.interactWithBit(1));
}

BOOST_AUTO_TEST_SUITE_END()

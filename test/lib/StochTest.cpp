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

BOOST_AUTO_TEST_CASE ( testInvalidParameters ) {
    
}

BOOST_AUTO_TEST_SUITE_END()

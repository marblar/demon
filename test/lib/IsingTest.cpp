//
//  IsingTest.cpp
//  jdemon
//
//  Created by Mark Larus on 3/4/13.
//  Copyright (c) 2013 Kenyon College. All rights reserved.
//

#include <boost/test/unit_test.hpp>
#include <set>
#include <algorithm>

#include "Ising.h"
#include "Utilities.h"

#include "TestFixtures.h"
#include "MockObjects.h"

class TransitionRuleTestFixture : \
    public DefaultTransitionRuleTestFixture, \
    public ValidStatesTestFixture \
{};

BOOST_FIXTURE_TEST_SUITE(TransitionRuleTest, TransitionRuleTestFixture);

BOOST_AUTO_TEST_CASE ( testTableSize ) {
    for (TransitionRule::iterator it = defaultRule.begin(); it!=defaultRule.end(); ++it) {
        BOOST_REQUIRE(it->second.size() == 8 && "Transition table size should be 8");
    }
}

BOOST_AUTO_TEST_CASE ( testTableDeadEnds ) {
    typedef TransitionRule::iterator RuleIterator;
    typedef TransitionTable::iterator TableIterator;
    
    for (RuleIterator currentRuleIterator = defaultRule.begin(); \
         currentRuleIterator!=defaultRule.end(); ++currentRuleIterator) {
        
        TransitionTable currentRule = currentRuleIterator->second;
        
        for (TableIterator currentTableIterator = currentRule.begin();\
             currentTableIterator != currentRule.end(); ++currentTableIterator) {
            BOOST_CHECK(currentTableIterator->second);
        }
    }
}

BOOST_AUTO_TEST_CASE ( testValidTargetStates ) {
    typedef TransitionRule::iterator RuleIterator;
    typedef TransitionTable::iterator TableIterator;
    
    for (RuleIterator currentRuleIterator = defaultRule.begin(); \
         currentRuleIterator!=defaultRule.end(); ++currentRuleIterator) {
        
        TransitionTable currentRule = currentRuleIterator->second;
        
        for (TableIterator currentTableIterator = currentRule.begin();\
             currentTableIterator != currentRule.end(); ++currentTableIterator) {
            SystemState *targetState = currentTableIterator->second;
            BOOST_CHECK(validStates.count(targetState));
        }
    }
}

BOOST_AUTO_TEST_SUITE_END()

class IsingReservoirTestFixture : \
    public RandomNumberTestFixture,
    public ConstantsTestFixture,
    public ValidStatesTestFixture
{};

BOOST_FIXTURE_TEST_SUITE(IsingReservoirTest, IsingReservoirTestFixture);

BOOST_AUTO_TEST_CASE ( testWheelStep ) {
    IsingReservoir reservoir(rng,c,5,5);
    Reservoir::InteractionResult result;
    BOOST_REQUIRE_NO_THROW(reservoir.wheelStep(result));
}

BOOST_AUTO_TEST_CASE ( testEmptyTransitionRule ) {
    TransitionRule emptyRule;
    IsingReservoir reservoir(rng,c,5,5,emptyRule);
    Reservoir::InteractionResult result;
    BOOST_REQUIRE_THROW(reservoir.wheelStep(result), EmptyTransitionRuleError);
}

BOOST_AUTO_TEST_CASE ( testDeadEndTransitionRule ) {
    TransitionRule deadEndRule;
    TransitionTable emptyTable;
    for (char k=0; k<8; k++) {
        emptyTable[k] = NULL;
    }
    
    Reservoir::InteractionResult result;
    
    for (StateSet::iterator it = validStates.begin(); it!=validStates.end(); ++it) {
        deadEndRule[*it] = emptyTable;
    }
    IsingReservoir reservoir(rng,c,5,5,deadEndRule);
    BOOST_REQUIRE_THROW(reservoir.wheelStep(result),TransitionDeadEndError);
}

BOOST_AUTO_TEST_CASE ( testTooSmallTransitionTable ) {
    TransitionRule invalidRule;
    TransitionTable tooSmallTable;
    
    for (char k=0; k<3; k++) {
        tooSmallTable[k] = NULL;
    }
    
    for (StateSet::iterator it = validStates.begin(); it!=validStates.end(); ++it) {
        invalidRule[*it] = tooSmallTable;
    }
    
    IsingReservoir reservoir(rng,c,5,5,invalidRule);
    Reservoir::InteractionResult result;
    BOOST_REQUIRE_THROW(reservoir.wheelStep(result),InvalidTableSizeError);
}

BOOST_AUTO_TEST_SUITE_END()

BOOST_AUTO_TEST_SUITE(IsingUtilityTest)

BOOST_AUTO_TEST_CASE( testNonbinaryParity ) {
    int inputState = ising::s(0,0,157);
    BOOST_REQUIRE(inputState == 1);
}

void requireNeighbor(Coordinate center,int dimension,Coordinate neighbor) {
    IsingReservoir::Neighbors neighbors = getNeighbors(center, dimension);
    bool found = false;
    
    for (int k=0; k<4; ++k) {
        found = (neighbors.coordinates[k] == neighbor);
        if (found) {
            break;
        }
    }
    BOOST_CHECK(found);
}

BOOST_AUTO_TEST_CASE ( testGetNeighbors ) {
    Coordinate c(10,10);
    
    requireNeighbor(c,
                    20,
                    Coordinate(10,11)
                    );
    
    requireNeighbor(c,
                    20,
                    Coordinate(11,10)
                    );
    
    requireNeighbor(c,
                    20,
                    Coordinate(9,10)
                    );
    
    requireNeighbor(c,
                    20,
                    Coordinate(10,9)
                    );
}

BOOST_AUTO_TEST_CASE( testTopEdgeNeighbor ) {
    requireNeighbor(Coordinate(4,0),
                    20,
                    Coordinate(4,19)
                    );
}

BOOST_AUTO_TEST_CASE( testBottomEdgeNeighbor ) {
    requireNeighbor(Coordinate(4,12),
                    13,
                    Coordinate(4,0)
                    );
}

BOOST_AUTO_TEST_CASE( testLeftEdgeNeighbor ) {
    requireNeighbor(Coordinate(0,10),
                    20,
                    Coordinate(19,10)
                    );
}

BOOST_AUTO_TEST_CASE( testRightEdgeNeighbor ) {
    requireNeighbor(Coordinate(39,5),
                    40,
                    Coordinate(0,5)
                    );
}

BOOST_AUTO_TEST_CASE( testCornerCase ) {
    Coordinate center(0,0);
    requireNeighbor(center,
                    20,
                    Coordinate(19,0)
                    );
    requireNeighbor(center,
                    20,
                    Coordinate(0,19)
                    );
}

BOOST_AUTO_TEST_SUITE_END()
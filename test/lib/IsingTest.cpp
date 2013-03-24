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
#include <stack>

#include "Ising.h"
#include "Utilities.h"

#include "TestFixtures.h"
#include "MockObjects.h"

using namespace DemonBase;

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
    Ising::Reservoir reservoir(rng,c,6,6);
    Reservoir::InteractionResult result;
    BOOST_REQUIRE_NO_THROW(reservoir.wheelStep(result));
}

BOOST_AUTO_TEST_CASE ( testEmptyTransitionRule ) {
    TransitionRule emptyRule;
    Ising::Reservoir reservoir(rng,c,6,6,emptyRule);
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
    Ising::Reservoir reservoir(rng,c,6,6,deadEndRule);
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
    
    Ising::Reservoir reservoir(rng,c,6,6,invalidRule);
    Reservoir::InteractionResult result;
    BOOST_REQUIRE_THROW(reservoir.wheelStep(result),InvalidTableSizeError);
}

BOOST_AUTO_TEST_CASE( testClusterInclusionProbability ) {
    c.setEpsilon(1);
    Ising::Reservoir reservoir1(rng,c,6,6);
    BOOST_CHECK_EQUAL(reservoir1.clusterInclusionProbability(), 1);
    c.setEpsilon(0);
    Ising::Reservoir reservoir2(rng,c,6,6);
    BOOST_CHECK_EQUAL(reservoir2.clusterInclusionProbability(), 0);
}

BOOST_AUTO_TEST_SUITE_END()

BOOST_AUTO_TEST_SUITE(IsingUtilityTest)

BOOST_AUTO_TEST_CASE( testNonbinaryParity ) {
    int inputState = Ising::s(0,0,157);
    BOOST_REQUIRE(inputState == 1);
}

BOOST_AUTO_TEST_SUITE_END()

typedef GridFixture<Ising::Grid> IsingGridTestFixture;
typedef GridOperationTestFixture<IsingGridTestFixture> IsingGridOperationFixture;

BOOST_FIXTURE_TEST_SUITE(ClusterMethodTest, IsingGridOperationFixture)

BOOST_AUTO_TEST_CASE( includeNoCells ) {
    Ising::Cell *startingPoint = grid[5];
    MockRandomnessDelegate delegate(false,0);
    Ising::ClusterMethodAgent<MockRandomnessDelegate> agent(delegate,1);
    agent.performMethodAtCell(startingPoint);
    
    std::set<Ising::Cell *> expectedCells;
    expectedCells.insert(startingPoint);
    
    std::set<Ising::Cell *> actualCells = changedCells();
    BOOST_REQUIRE_EQUAL_COLLECTIONS(expectedCells.begin(), expectedCells.end(), actualCells.begin(), actualCells.end());
}

BOOST_AUTO_TEST_CASE( includeAllCells ) {
    for (Ising::Grid::iterator it = grid.begin(); it!=grid.end(); ++it) {
        (*it)->setValue(0);
    }
    
    Ising::Cell *startingPoint = grid[5];
    MockRandomnessDelegate delegate(true,0);
    Ising::ClusterMethodAgent<MockRandomnessDelegate> agent(delegate,1);
    agent.performMethodAtCell(startingPoint);
    
    std::set<Ising::Cell *> expectedCells;
    expectedCells.insert(grid.begin(),grid.end());
    
    std::set<Ising::Cell *> actualCells = changedCells();
    
    BOOST_REQUIRE_EQUAL_COLLECTIONS(expectedCells.begin(), expectedCells.end(), actualCells.begin(), actualCells.end());
}

BOOST_AUTO_TEST_CASE( includeSomeCells ) {
    Ising::Cell *startingPoint = grid[5];
    std::set<Ising::Cell *> expectedCells;
    std::stack<Ising::Cell *> stack;
    stack.push(startingPoint);
    
    size_t size = 12;
    
    BOOST_REQUIRE_LT(size,grid.size());
    
    while (expectedCells.size()!=size) {
        if (stack.empty()) {
            BOOST_FAIL("Stack should not be empty.");
        }
        Ising::Cell *currentCell = stack.top();
        stack.pop();
        Ising::Cell::Neighbors neighbors = currentCell->getNeighbors();
        for (Ising::Cell::Neighbors::iterator it = neighbors.begin(); it!=neighbors.end(); ++it) {
            if (!expectedCells.count(*it)) {
                stack.push(*it);
            }
        }
        expectedCells.insert(currentCell);
    }
    
    BOOST_REQUIRE_EQUAL(expectedCells.size(),size);
    
    for (Ising::Grid::iterator it = grid.begin(); it!=grid.end(); ++it) {
        if (expectedCells.count(*it)) {
            (*it)->setValue(1);
        } else {
            (*it)->setValue(0);
        }
    }
    
    resetInitialValues();
    
    MockRandomnessDelegate delegate(true,0);
    Ising::ClusterMethodAgent<MockRandomnessDelegate> agent(delegate,1);
    agent.performMethodAtCell(startingPoint);
    
    std::set<Ising::Cell *> actualCells = changedCells();
    
    BOOST_REQUIRE_EQUAL_COLLECTIONS(expectedCells.begin(), expectedCells.end(), actualCells.begin(), actualCells.end());
}

BOOST_AUTO_TEST_CASE( testInclusionProbability ) {
    MockRandomnessDelegate m(1,1);
    BOOST_CHECK_THROW(Ising::ClusterMethodAgent<MockRandomnessDelegate>(m,-0.01), Ising::InvalidProbabilityError);
    BOOST_CHECK_THROW(Ising::ClusterMethodAgent<MockRandomnessDelegate>(m,1.01), Ising::InvalidProbabilityError);
}

BOOST_AUTO_TEST_SUITE_END()
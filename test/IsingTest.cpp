//
//  IsingTest.cpp
//  jdemon
//
//  Created by Mark Larus on 3/4/13.
//  Copyright (c) 2013 Kenyon College. All rights reserved.
//

#include "IsingTest.h"
#include "Ising.h"
#include "Utilities.h"
#include <set>

CPPUNIT_TEST_SUITE_REGISTRATION(TransitionRuleTest);
CPPUNIT_TEST_SUITE_REGISTRATION(IsingReservoirTest);

static Constants defaultConstants() {
    Constants c;
    c.setDelta(.5);
    c.setEpsilon(0);
    c.setTau(1);
    c.setNbits(8);
    return c;
}

void TransitionRuleTest::setUp() {
    rule = defaultTransitionRule();
    validStates = getValidStates();
}

void TransitionRuleTest::tearDown() {
    
}

void TransitionRuleTest::testTableSize() {
    for (TransitionRule::iterator it = rule.begin(); it!=rule.end(); ++it) {
        CPPUNIT_ASSERT(it->second.size() == 8 && "Transition table size should be 8");
    }
}

void TransitionRuleTest::testTableDeadEnds() {
    typedef TransitionRule::iterator RuleIterator;
    typedef TransitionTable::iterator TableIterator;
    
    for (RuleIterator currentRuleIterator = rule.begin(); \
         currentRuleIterator!=rule.end(); ++currentRuleIterator) {
        
        TransitionTable currentRule = currentRuleIterator->second;
        
        for (TableIterator currentTableIterator = currentRule.begin();\
             currentTableIterator != currentRule.end(); ++currentTableIterator) {
            CPPUNIT_ASSERT(currentTableIterator->second);
        }
    }
}

void TransitionRuleTest::testValidTargetStates() {
    typedef TransitionRule::iterator RuleIterator;
    typedef TransitionTable::iterator TableIterator;
    
    for (RuleIterator currentRuleIterator = rule.begin(); \
         currentRuleIterator!=rule.end(); ++currentRuleIterator) {
        
        TransitionTable currentRule = currentRuleIterator->second;
        
        for (TableIterator currentTableIterator = currentRule.begin();\
             currentTableIterator != currentRule.end(); ++currentTableIterator) {
            SystemState *targetState = currentTableIterator->second;
            CPPUNIT_ASSERT(validStates.count(targetState));
        }
    }
}

void IsingReservoirTest::setUp() {
    rng = GSLRandomNumberGenerator();
    gsl_rng_set(rng, 0);
}

void IsingReservoirTest::tearDown() {
    gsl_rng_free(rng);
}


void IsingReservoirTest::testWheelStep() {
    IsingReservoir reservoir(rng,defaultConstants(),5,5);
    Reservoir::InteractionResult result;
    CPPUNIT_ASSERT_NO_THROW(reservoir.wheelStep(result));
}

void IsingReservoirTest::testEmptyTransitionRule() {
    TransitionRule emptyRule;
    IsingReservoir reservoir(rng,defaultConstants(),5,5,emptyRule);
    Reservoir::InteractionResult result;
    CPPUNIT_ASSERT_THROW(reservoir.wheelStep(result), EmptyTransitionRuleError);
}

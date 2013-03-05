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

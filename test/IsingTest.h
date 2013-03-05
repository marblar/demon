//
//  IsingTest.h
//  jdemon
//
//  Created by Mark Larus on 3/4/13.
//  Copyright (c) 2013 Kenyon College. All rights reserved.
//

#ifndef __jdemon__IsingTest__
#define __jdemon__IsingTest__

#include <cppunit/extensions/HelperMacros.h>
#include <gsl/gsl_rng.h>
#include "Ising.h"

class TransitionRuleTest : public CppUnit::TestFixture {
    CPPUNIT_TEST_SUITE(TransitionRuleTest);
    CPPUNIT_TEST(testTableSize);
    CPPUNIT_TEST(testTableDeadEnds);
    CPPUNIT_TEST_SUITE_END();
    TransitionRule rule;
    std::set<SystemState *> validStates;
public:
    void setUp();
    void tearDown();
    void testTableSize();
    void testTableDeadEnds();
    void testValidTargetStates();
};

#endif /* defined(__jdemon__IsingTest__) */

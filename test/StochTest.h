//
//  StochTest.h
//  jdemon
//
//  Created by Mark Larus on 3/5/13.
//  Copyright (c) 2013 Kenyon College. All rights reserved.
//

#ifndef __jdemon__StochTest__
#define __jdemon__StochTest__

#include <cppunit/extensions/HelperMacros.h>
#include <gsl/gsl_rng.h>

class StochReservoirTest : public CppUnit::TestFixture {
    CPPUNIT_TEST_SUITE(StochReservoirTest);
    CPPUNIT_TEST(testInteractBit);
    CPPUNIT_TEST_SUITE_END();
    gsl_rng *rng;
public:
    void setUp();
    void tearDown();
    void testInteractBit();
};

#endif /* defined(__jdemon__StochTest__) */

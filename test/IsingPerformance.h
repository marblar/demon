//
//  IsingPerformance.h
//  jdemon
//
//  Created by Mark Larus on 3/3/13.
//  Copyright (c) 2013 Kenyon College. All rights reserved.
//

#ifndef __jdemon__IsingPerformance__
#define __jdemon__IsingPerformance__

#include <cppunit/extensions/HelperMacros.h>
#include <gsl/gsl_rng.h>

class IsingPerformance : public CppUnit::TestFixture {
    CPPUNIT_TEST_SUITE(IsingPerformance);
    CPPUNIT_TEST(testColdMetropolis);
    CPPUNIT_TEST(testLukewarmMetropolis);
    CPPUNIT_TEST(testHotMetropolis);
    CPPUNIT_TEST(testIsingStep);
    CPPUNIT_TEST_SUITE_END();
    gsl_rng *rng;
public:
    void setUp();
    void tearDown();
    void testColdMetropolis();
    void testLukewarmMetropolis();
    void testHotMetropolis();
    
    void metropolis(double e);
    
    void testIsingStep();
};

#endif /* defined(__jdemon__IsingPerformance__) */

//
//  MeasurementTest.h
//  jdemon
//
//  Created by Mark Larus on 3/3/13.
//  Copyright (c) 2013 Kenyon College. All rights reserved.
//

#ifndef __jdemon__MeasurementTest__
#define __jdemon__MeasurementTest__

#include <cppunit/extensions/HelperMacros.h>
#include <gsl/gsl_rng.h>

class MeasurementTest : public CppUnit::TestFixture {
    CPPUNIT_TEST_SUITE(MeasurementTest);
    CPPUNIT_TEST(testOutput);
    CPPUNIT_TEST(testWithTrivialReservoir);
    CPPUNIT_TEST(testTrivialCalculateJ);
    CPPUNIT_TEST(testResultsConstants);
    CPPUNIT_TEST(testIncompleteDestructor);
    CPPUNIT_TEST(testCompleteFlag);
    CPPUNIT_TEST(testZeroBits);
    CPPUNIT_TEST(testNegativeBits);
    CPPUNIT_TEST(testCompleteFlag);
    CPPUNIT_TEST_SUITE_END();
    gsl_rng *rng;
public:
    void setUp();
    void tearDown();
    void testOutput();
    void testWithTrivialReservoir();
    void testTrivialCalculateJ();
    void testResultsConstants();
    void testIncompleteDestructor();
    void testCompleteFlag();
    void testZeroBits();
    void testNegativeBits();
};

#endif /* defined(__jdemon__MeasurementTest__) */

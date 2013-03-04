//
//  SystemTest.h
//  jdemon
//
//  Created by Mark Larus on 3/1/13.
//  Copyright (c) 2013 Kenyon College. All rights reserved.
//

#ifndef __jdemon__SystemTest__
#define __jdemon__SystemTest__

#include <cppunit/extensions/HelperMacros.h>
#include <iostream>
#include <gsl/gsl_rng.h>

class SystemTest : public CppUnit::TestFixture {
    CPPUNIT_TEST_SUITE(SystemTest);
    CPPUNIT_TEST(testNbits);
    CPPUNIT_TEST(testEndingString);
    CPPUNIT_TEST(testSystemFactory);
    CPPUNIT_TEST(testConstructor);
    CPPUNIT_TEST(testReuseSystem);
    CPPUNIT_TEST_SUITE_END();
    gsl_rng *rng;
public:
    void setUp();
    void tearDown();
    void testConstructor();
    void testNbits();
    void testReuseSystem();
    void testSystemFactory();
    void testEndingString();
};

class ConstantsTest : public CppUnit::TestFixture {
    CPPUNIT_TEST_SUITE(ConstantsTest);
    CPPUNIT_TEST(testHighTemperature);
    CPPUNIT_TEST_SUITE_END();
public:
    void testHighTemperature();
};

#endif /* defined(__jdemon__SystemTest__) */

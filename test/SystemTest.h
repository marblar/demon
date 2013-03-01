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

class Systemtest : public CppUnit::TestFixture {
    CPPUNIT_TEST_SUITE(Systemtest);
    CPPUNIT_TEST(testNbits);
    CPPUNIT_TEST_SUITE_END();
public:
    void setUp();
    void tearDown();
    void testNbits();
};

#endif /* defined(__jdemon__SystemTest__) */

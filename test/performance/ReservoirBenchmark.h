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
#include <cppunit/extensions/RepeatedTest.h>
#include "InstrumentFactories.h"

class ReservoirBenchmark : public CppUnit::TestFixture {
    CPPUNIT_TEST_SUITE(ReservoirBenchmark);
    CPPUNIT_TEST_SUITE_ADD_CUSTOM_TESTS(addTest);
    CPPUNIT_TEST_SUITE_END_ABSTRACT();
    gsl_rng *rng;
public:
    void setUp();
    void tearDown();
    void performMeasurement();
    static void addTest(TestSuiteBuilderContextType &context) {
        using namespace CppUnit;
        Test *repeatMe = new CPPUNIT_NS::TestCaller<TestFixtureType>(
            context.getTestNameFor( "Measurement" ),
            &ReservoirBenchmark::performMeasurement,
            context.makeFixture());
        context.addTest( new RepeatedTest(repeatMe,5) );
    }
    virtual ReservoirFactory *createReservoirFactory() = 0;
    virtual int iterations() = 0;
};

#endif

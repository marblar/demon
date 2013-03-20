//
//  RandomnessDelegateTest.cpp
//  jdemon
//
//  Created by Mark Larus on 3/20/13.
//  Copyright (c) 2013 Kenyon College. All rights reserved.
//

#include <boost/test/unit_test.hpp>
#include <math.h>

#include "TestFixtures.h"
#include "RandomnessDelegate.h"

BOOST_FIXTURE_TEST_SUITE(RandomnessDelegate, RandomNumberTestFixture)

BOOST_AUTO_TEST_CASE(testGSLBinaryEvent_distribution) {
    for (double expectedRatio = 0; expectedRatio<=1; expectedRatio+=.1) {
        Randomness::GSLDelegate delegate(rng);
        int iterations = 1000;
        
        double standard_dev = sqrt(expectedRatio*(1-expectedRatio)/iterations);
        double acceptable_error = expectedRatio ? 2*standard_dev/expectedRatio : 0;
        
        int sum = 0;
        for (int k=0; k<iterations; ++k) {
            sum += delegate.binaryEventWithProbability(expectedRatio);
        }
        double actualRatio = static_cast<double>(sum) / iterations;
        BOOST_CHECK_CLOSE_FRACTION(expectedRatio, actualRatio, acceptable_error);
    }
}

BOOST_AUTO_TEST_CASE(testGSLBinaryEvent_neverHappens) {
    Randomness::GSLDelegate delegate(rng);
    int iterations = 1000;
    int sum = 0;
    for (int k=0; k<iterations; ++k) {
        sum += delegate.binaryEventWithProbability(0);
    }
    double actualRatio = static_cast<double>(sum) / iterations;
    double expectedRatio = 0;
    BOOST_REQUIRE_EQUAL(actualRatio, expectedRatio);
}

BOOST_AUTO_TEST_CASE(testGSLBinaryEvent_alwaysHappens) {
    Randomness::GSLDelegate delegate(rng);
    int iterations = 1000;
    int sum = 0;
    for (int k=0; k<iterations; ++k) {
        sum += delegate.binaryEventWithProbability(1);
    }
    double actualRatio = static_cast<double>(sum) / iterations;
    double expectedRatio = 1;
    BOOST_REQUIRE_EQUAL(actualRatio, expectedRatio);
}

BOOST_AUTO_TEST_SUITE_END()


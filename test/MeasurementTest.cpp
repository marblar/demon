//
//  MeasurementTest.cpp
//  jdemon
//
//  Created by Mark Larus on 3/3/13.
//  Copyright (c) 2013 Kenyon College. All rights reserved.
//

#include "MeasurementTest.h"
#include "Measurement.h"
#include "Reservoir.h"
#include "ReservoirFactory.h"
#include <map>

CPPUNIT_TEST_SUITE_REGISTRATION(MeasurementTest);

class TrivialReservoir : public Reservoir {
public:
    InteractionResult interactWithBit(int bit) {
        InteractionResult result;
        result.work = 0;
        result.bit = bit;
        return result;
    };
    void reset() { /* Do nothing */ }
    TrivialReservoir(Constants constants) : Reservoir(constants) {}
};

void MeasurementTest::testTrivialReservoir() {
    Constants c;
    c.setDelta(.5);
    c.setEpsilon(0);
    c.setTau(0);
    ReservoirFactory *rfactory = new DefaultArgsReservoirFactory<TrivialReservoir>;
    Measurement measurement(c,1000,rfactory);
    MeasurementResult result = measurement.getResult();
}

void MeasurementTest::setUp() {
    
}

void MeasurementTest::tearDown() {
    
}

void MeasurementTest::testOutput() {
    Constants c;
    c.setDelta(.7);
    c.setEpsilon(0);
    
    MeasurementResult result;
    result.constants = c;
    result.maxJ = 1000.93;
    result.averageJ = 1;
    
    // Verify that the output
    std::string actualResultString = outputString(result);
    std::string expectedResultString = "0.7,0,1,1000.93\n";
    CPPUNIT_ASSERT_EQUAL(actualResultString, expectedResultString);
}
//
//  MockObjects.h
//  jdemon
//
//  Created by Mark Larus on 3/6/13.
//  Copyright (c) 2013 Kenyon College. All rights reserved.
//

#ifndef jdemon_MockObjects_h
#define jdemon_MockObjects_h

#include <boost/array.hpp>
#include "RandomnessDelegate.h"


class TrivialReservoir : public Reservoir {
public:
    InteractionResult interactWithBit(int bit) {
        InteractionResult result;
        result.work = 0;
        result.bit = bit;
        return result;
    };
    void reset() { /* Do nothing */ }
    TrivialReservoir(gsl_rng *rng, Constants) : Reservoir(constants) {}
};

class MockReservoirFlipDown : public Reservoir {
public:
    InteractionResult interactWithBit(int bit) {
        InteractionResult result;
        result.work = 1;
        result.bit = 0;
        ++bitsProvided;
        return result;
    };
    void reset() { /* Do nothing */ }
    int bitsProvided;
    MockReservoirFlipDown(Constants constants) : bitsProvided(0), Reservoir(constants) {}
};

class SpecificEndStringReservoir : public Reservoir {
public:
    InteractionResult interactWithBit(int bit) {
        InteractionResult result;
        result.work = 1;
        result.bit = (end>>bitPos)&1;
        bitPos++;
        return result;
    };
    void reset() { /* Do nothing */ }
    int end;
    int bitPos;
    SpecificEndStringReservoir(Constants constants, int end_) :
    Reservoir(constants), end(end_), bitPos(0) {}
};

class MockRandomnessDelegate : public Randomness::Delegate<MockRandomnessDelegate> {
    bool event;
    int integer;
    friend class Randomness::Delegate<MockRandomnessDelegate>;
protected:
    bool binaryEventWithProbability_imp(double p) {
        return event;
    }
    int randomIntegerFromInclusiveRange_imp(int begin, int end) {
        return integer;
    }
public:
    MockRandomnessDelegate(bool eventValue, bool integerValue) : event(eventValue), integer(integerValue) {}
};

#endif

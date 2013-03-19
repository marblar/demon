//
//  MeasurementTest.h
//  jdemon
//
//  Created by Mark Larus on 3/3/13.
//  Copyright (c) 2013 Kenyon College. All rights reserved.
//

#ifndef __jdemon__MeasurementTest__
#define __jdemon__MeasurementTest__

#include <gsl/gsl_rng.h>
#include "Utilities.h"
#include "Ising.h"

struct RandomNumberTestFixture {
    RandomNumberTestFixture() {
        rng = GSLRandomNumberGenerator();
        gsl_rng_set(rng,0);
    }
    ~RandomNumberTestFixture() {
        gsl_rng_free(rng);
    }
    gsl_rng *rng;
};

struct ConstantsTestFixture {
    Constants c;
    ConstantsTestFixture() {
        c.setDelta(.5);
        c.setEpsilon(0);
        c.setTau(1);
        c.setNbits(8);
    }
};

struct EvenIsingGridFixture {
    Ising::Grid grid;
    EvenIsingGridFixture() : grid(6) {}
};

struct ValidStatesTestFixture {
    StateSet validStates;
    ValidStatesTestFixture () {
        validStates = getValidStates();
    }
};

struct DefaultTransitionRuleTestFixture {
    TransitionRule defaultRule;
    DefaultTransitionRuleTestFixture() {
        defaultRule = defaultTransitionRule();
    }
};


#endif /* defined(__jdemon__MeasurementTest__) */

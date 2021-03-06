//
//  StateMachine.cpp
//  FSM
//
//  Created by Mark Larus on 9/8/12.
//  Copyright (c) 2012 Kenyon College. All rights reserved.
//

#include <string>
#include <gsl/gsl_randist.h>
#include "Stochastic.h"
using namespace DemonBase;
using namespace Stochastic;

Stochastic::Reservoir::InteractionResult Stochastic::Reservoir::interactWithBit(int oldBit) {
    InteractionResult result;
    if (constants.getTau()<=0) {
        throw DemonBase::InvalidTauError("Tau must be a positive floating point number.");
    }
    if (constants.getEpsilon()< 0 || constants.getEpsilon()>1) {
        throw DemonBase::InvalidEpsilonError("Epsilon \\not\\in [0,1]");
    }
    if (constants.getDelta()<0 || constants.getDelta()>1.0) {
        throw DemonBase::InvalidDeltaError("Delta must be \\in [0,1]");
    }
    
    if (Stochastic::Reservoir::currentState->bit != oldBit) {
        Stochastic::Reservoir::currentState = DemonBase::Reservoir::currentState->bitFlipState;
    }
    
    double time_elapsed = 0;
    
    while (time_elapsed < 1.0) {
        /*This line is getting awful ugly. Let me explain: The transition rate is always 1, except
         when the bit is changing. A transition for which the bit decreases has rate 1-epsilon. A
         transition where the bit increases is 1 + epsilon. Implementing this with ^blocks was too slow,
         and subclassing using virtual functions wasn't much better. So I'm doing it inline.*/
        
        double rate1 = 1 + (DemonBase::Reservoir::currentState->nextState1->bit-DemonBase::Reservoir::currentState->bit)*constants.getEpsilon();
        double rate2 = 1 + (DemonBase::Reservoir::currentState->nextState2->bit-currentState->bit)*constants.getEpsilon();
        
        /*! The minimum of two exponentials is an exponential with their combined rates. Since there's a
         call to log() in this function, we want to use it as few times as possible. So we simply figure out
         when the next state switch is going ot happen, and if needed we roll again to see
         which came first. */
        double fastestTime = gsl_ran_exponential(RNG, 1/(1/rate1+1/rate2));
    
        if (fastestTime + time_elapsed > constants.getTau()) {
            /*! The tape moved first */
            break;
        }
        double probabilityThatState1CameFirst = rate1/(rate1+rate2);
        DemonBase::SystemState *nextState = probabilityThatState1CameFirst < gsl_rng_uniform(RNG) ? currentState->nextState1 : currentState->nextState2;
        currentState = nextState;
        time_elapsed+=fastestTime;
    }
    int newBit = currentState->bit;

    result.work = newBit - oldBit;
    result.bit = newBit;
    return result;
}

Stochastic::Reservoir::Reservoir(gsl_rng *RNG, DemonBase::Constants constants) :
DemonBase::Reservoir(constants) {
    this->RNG = RNG;
}

void Stochastic::Reservoir::reset() {
    currentState = DemonBase::randomState();
}



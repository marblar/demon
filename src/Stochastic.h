//
//  StateMachine.h
//  FSM
//
//  Created by Mark Larus on 9/8/12.
//  Copyright (c) 2012 Kenyon College. All rights reserved.
//

#ifndef __FSM__StateMachine__
#define __FSM__StateMachine__

#include <iostream>
#include <gsl/gsl_rng.h>
#include "Reservoir.h"

struct SystemState {
    SystemState *nextState1;
    SystemState *nextState2;
    SystemState *bitFlipState;
    int bit;
    char letter;
};

extern SystemState StateA1, StateB1, StateC1, StateA0, StateB0, StateC0;

void setupStates();
SystemState *randomState();
std::string print_state(SystemState *state);

class StochasticReservoir : public Reservoir {
public:
    SystemState *currentState;
    gsl_rng *RNG;
    StochasticReservoir(gsl_rng *RNG, Constants constants);
    int interactWithBit(int bit);
};

#endif /* defined(__FSM__StateMachine__) */

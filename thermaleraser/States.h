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

struct SystemState {
    SystemState *nextState1;
    SystemState *nextState2;
    SystemState *bitFlipState;
    int bit;
    char letter;
};

extern SystemState StateA1, StateB1, StateC1, StateA0, StateB0, StateC0;

void setupStates();

std::string print_state(SystemState *state);

#endif /* defined(__FSM__StateMachine__) */

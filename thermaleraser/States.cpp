//
//  StateMachine.cpp
//  FSM
//
//  Created by Mark Larus on 9/8/12.
//  Copyright (c) 2012 Kenyon College. All rights reserved.
//

#include "States.h"
#include <string>

SystemState StateA1, StateB1, StateC1, StateA0, StateB0, StateC0;


std::string print_state(SystemState *state) {
    std::string theString = "";
    theString.push_back(state->letter);
    theString.push_back('0'+state->bit);
    return theString;
}

void setupStates() {
    static bool alreadyInitialized = false;

    if(!alreadyInitialized) {
        
        StateA1.nextState1 = &StateB1;
        StateA1.nextState2 = &StateC0;
        StateA1.bitFlipState = &StateA0;
        StateA1.bit = 1;
        StateA1.letter = 'A';
        StateA1.rate1 = ^(double epsilon) {
            return 1.0;
        };
        StateA1.rate2 = ^(double epsilon) {
            return 1 - epsilon;
        };
        
        StateB1.nextState1 = &StateA1;
        StateB1.nextState2 = &StateC1;
        StateB1.bitFlipState = &StateB0;
        StateB1.bit = 1;
        StateB1.letter = 'B';
        StateB1.rate1 = ^(double epsilon) {
            return 1.0;
        };
        StateB1.rate2 = ^(double epsilon) {
            return 1.0;
        };
        
        StateC1.nextState1 = &StateB1;
        StateC1.nextState2 = &StateC1;
        StateC1.bitFlipState = &StateC0;
        StateC1.bit = 1;
        StateC1.letter = 'C';
        StateC1.rate1 = ^(double epsilon) {
            return 1.0;
        };
        StateC1.rate2 = ^(double epsilon) {
            return 1.0;
        };
        
        StateA0.nextState1 = &StateB0;
        StateA0.nextState2 = &StateA0;
        StateA0.bitFlipState = &StateA1;
        StateA0.bit = 0;
        StateA0.letter = 'A';
        StateA0.rate1 = ^(double epsilon) {
            return 1.0;
        };
        StateA0.rate2 = ^(double epsilon) {
            return 1.0;
        };
        
        StateB0.nextState1 = &StateA0;
        StateB0.nextState2 = &StateC0;
        StateB0.bitFlipState = &StateB1;
        StateB0.bit = 0;
        StateB0.letter = 'B';
        StateB0.rate1 = ^(double epsilon) {
            return 1.0;
        };
        StateB0.rate2 = ^(double epsilon) {
            return 1.0;
        };
        
        StateC0.nextState1 = &StateB0;
        StateC0.nextState2 = &StateA1;
        StateC0.bitFlipState = &StateC1;
        StateC0.bit = 0;
        StateC0.letter = 'C';
        StateC0.rate1 = ^(double epsilon) {
            return 1.0;
        };
        StateC0.rate2 = ^(double epsilon) {
            return 1.0 + epsilon;
        };
    }
}


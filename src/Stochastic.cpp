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
        
        StateB1.nextState1 = &StateA1;
        StateB1.nextState2 = &StateC1;
        StateB1.bitFlipState = &StateB0;
        StateB1.bit = 1;
        StateB1.letter = 'B';
        
        StateC1.nextState1 = &StateB1;
        StateC1.nextState2 = &StateC1;
        StateC1.bitFlipState = &StateC0;
        StateC1.bit = 1;
        StateC1.letter = 'C';
        
        StateA0.nextState1 = &StateB0;
        StateA0.nextState2 = &StateA0;
        StateA0.bitFlipState = &StateA1;
        StateA0.bit = 0;
        StateA0.letter = 'A';
        
        StateB0.nextState1 = &StateA0;
        StateB0.nextState2 = &StateC0;
        StateB0.bitFlipState = &StateB1;
        StateB0.bit = 0;
        StateB0.letter = 'B';
        
        StateC0.nextState1 = &StateB0;
        StateC0.nextState2 = &StateA1;
        StateC0.bitFlipState = &StateC1;
        StateC0.bit = 0;
        StateC0.letter = 'C';
    }
}

SystemState *randomState() {
    int index = rand()%6;
    SystemState *randomState[] = {&StateA0,&StateA1,&StateB0,&StateB1, &StateB0,&StateC0,&StateC1};
    return randomState[index];
}

int StochasticReservoir::interactWithBit(int bit) {
    if (currentState->bit != bit) {
        currentState=currentState->bitFlipState;
    }
    
    double time_elapsed = 0;
    
    while (time_elapsed < 1.0) {
        /*This line is getting awful ugly. Let me explain: The transition rate is always 1, except
         when the bit is changing. A transition for which the bit decreases has rate 1-epsilon. A
         transition where the bit increases is 1 + epsilon. Implementing this with ^blocks was too slow,
         and subclassing using virtual functions wasn't much better. So I'm doing it inline.*/
        
        double rate1 = 1 + (currentState->nextState1->bit-currentState->bit)*constants.epsilon;
        double rate2 = 1 + (currentState->nextState2->bit-currentState->bit)*constants.epsilon;
        
        /*! The minimum of two exponentials is an exponential with their combined rates. Since there's a
         call to log() in this function, we want to use it as few times as possible. So we simply figure out
         when the next state switch is going ot happen, and if needed we roll again to see
         which came first. */
        double fastestTime = gsl_ran_exponential(RNG, 1/(1/rate1+1/rate2));
    
        if (fastestTime + time_elapsed > constants.tau) {
            /*! The tape moved first */
            break;
        }
        double probabilityThatState1CameFirst = rate1/(rate1+rate2);
        SystemState *nextState = probabilityThatState1CameFirst < gsl_rng_uniform(RNG) ? currentState->nextState1 : currentState->nextState2;
        currentState = nextState;
        time_elapsed+=fastestTime;
    }
    return currentState->bit;
}

StochasticReservoir::StochasticReservoir(gsl_rng *RNG) {
    this->RNG = RNG;
    currentState = randomState();
}




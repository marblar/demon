//
//  System.cpp
//  thermaleraser
//
//  Created by Mark Larus on 10/30/12.
//  Copyright (c) 2012 Kenyon College. All rights reserved.
//

#include "System.h"
#include "Utilities.h"
#include "gsl_randist.h"

SystemState *randomState() {
    int index = rand()%6;
    SystemState *randomState[] = {&StateA0,&StateA1,&StateB0,&StateB1, &StateB0,&StateC0,&StateC1};
    return randomState[index];
}

System::System() {
    this->bitPosition = 0;
    this->timeSinceLastBit = 0;
    this->startingBitString = 0;
    this->endingBitString = 0;
    this->mass = 0;
    this->currentState = randomState();
}

void evolveSystem(System *currentSystem, gsl_rng *localRNG) {
    currentSystem->startingBitString = randomShortIntWithBitDistribution(currentSystem->constants.delta, currentSystem->nbits, localRNG);
    
    if (currentSystem->currentState->bit != (currentSystem->startingBitString & 1)) {
        currentSystem->currentState=currentSystem->currentState->bitFlipState;
    }
    
    while (currentSystem->bitPosition<currentSystem->nbits) {
        /*This line is getting awful ugly. Let me explain: The transition rate is always 1, except
         when the bit is changing. A transition for which the bit decreases has rate 1-epsilon. A
         transition where the bit increases is 1 + epsilon. Implementing this with ^blocks was too slow,
         and subclassing using virtual functions wasn't much better. So I'm doing it inline.*/
        SystemState *currentState = currentSystem->currentState;
        double rate1 = 1 + (currentState->nextState1->bit-currentState->bit)*currentSystem->constants.epsilon;
        double rate2 = 1 + (currentState->nextState2->bit-currentState->bit)*currentSystem->constants.epsilon;
        
        /*! The minimum of two exponentials is an exponential with their combined rates. Since there's a
         call to log() in this function, we want to use it as few times as possible. So we simply figure out
         when the next state switch is going ot happen, and if needed we roll again to see
         which came first. */
        double fastestTime = gsl_ran_exponential(localRNG, rate1+rate2);
        
        if (fastestTime+currentSystem->timeSinceLastBit>currentSystem->constants.tau) {
            /*! The tape moved first */
            
            currentSystem->timeSinceLastBit = 0;
            
            int currentBit = currentSystem->currentState->bit;
            currentSystem->endingBitString|=(currentBit<<currentSystem->bitPosition);
            
            ++currentSystem->bitPosition;
            
            int newBit = (currentSystem->startingBitString >> currentSystem->bitPosition) & 1;
            if(newBit!=currentSystem->currentState->bit) {
                currentSystem->currentState = currentSystem->currentState->bitFlipState;
            }
            
        } else {
            /*! The wheel state changed first.*/
            double probabilityThatState1CameFirst = rate1/(rate1+rate2);
            
            SystemState *nextState = probabilityThatState1CameFirst < gsl_rng_uniform(localRNG) ? currentSystem->currentState->nextState1 : currentSystem->currentState->nextState2;
            
            //Currently the mass is unused...
            currentSystem->mass += currentSystem->currentState->bit - nextState->bit;
            
            currentSystem->currentState = nextState;
            
            //It's important to keep this up to date.
            currentSystem->timeSinceLastBit+=fastestTime;
        }//End if
    }//End while

}

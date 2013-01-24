//
//  System.cpp
//  thermaleraser
//
//  Created by Mark Larus on 10/30/12.
//  Copyright (c) 2012 Kenyon College. All rights reserved.
//

#include "System.h"
#include "Utilities.h"
#include <gsl/gsl_randist.h>
#include <assert.h>
#include <math.h>

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

void evolveDiscreteSystem(System *currentSystem, gsl_rng *localRNG) {
    //Schumacher's stuff goes here.
}

void evolveSystem(System *currentSystem, gsl_rng *localRNG, bool discrete_system) {
    
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
        double fastestTime = gsl_ran_exponential(localRNG, 1/(1/rate1+1/rate2));
        
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
            
            currentSystem->mass -= currentSystem->currentState->bit - nextState->bit;
            currentSystem->currentState = nextState;
            currentSystem->timeSinceLastBit+=fastestTime;
        }//End if
    }//End while
}

int fourSignificantDecimalDigits(double x) {
    assert(x<=1);
    assert(x>=0);
    
    int value = (int)(floor(x*1000));

    assert(value<1000);
    assert(value>0);
    
    return value;
}

long packResult(System *system) {
    long result = 0;
    int offset = 0;
    
    bool too_many_bits = ((~0)<<16 & system->startingBitString);
    assert(!too_many_bits);
    
    too_many_bits = ((~0)<<16 & system->startingBitString);
    assert(!too_many_bits);
    
    result|=system->startingBitString;
    offset-=16;
    
    result|=system->endingBitString<<offset;
    offset-=16;
    
    result|=fourSignificantDecimalDigits(system->constants.delta)<<offset;
    offset-=10;

    result|=fourSignificantDecimalDigits(system->constants.epsilon)<<offset;
    offset-=10;
    
    assert(offset<64);
    return result;
}

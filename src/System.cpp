//
//  System.cpp
//  thermaleraser
//
//  Created by Mark Larus on 10/30/12.
//  Copyright (c) 2012 Kenyon College. All rights reserved.
//

#include "System.h"
#include "Reservoir.h"
#include "Utilities.h"
#include <gsl/gsl_randist.h>
#include <assert.h>
#include <math.h>

System::System() {
    this->bitPosition = 0;
    this->timeSinceLastBit = 0;
    this->startingBitString = 0;
    this->endingBitString = 0;
    this->mass = 0;
}

void evolveSystemWithReservoir(System *currentSystem, Reservoir *reservoir, gsl_rng *localRNG, bool discrete_system) {
    currentSystem->startingBitString = randomShortIntWithBitDistribution(currentSystem->constants.delta, currentSystem->nbits, localRNG);
    
    unsigned int& startingBitString = currentSystem->startingBitString;
    unsigned int& endingBitString = currentSystem->endingBitString;
    int& bitPosition = currentSystem->bitPosition;
    
    while (currentSystem->bitPosition < currentSystem->nbits) {
        int oldBit = (startingBitString >> bitPosition) & 1;
        int newBit = reservoir->interactWithBit(oldBit);
        currentSystem->mass -= oldBit - newBit;
        endingBitString |= ( newBit << bitPosition );
        bitPosition++;
    }//End while
}



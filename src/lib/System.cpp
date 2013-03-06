//
//  System.cpp
//  thermaleraser
//
//  Created by Mark Larus on 10/30/12.
//  Copyright (c) 2012 Kenyon College. All rights reserved.
//

#include "System.h"
#include "Reservoir.h"
#include <gsl/gsl_randist.h>
#include <assert.h>
#include <math.h>

System::System(Constants constants, int startingBitString) {
    this->bitPosition = 0;
    this->startingBitString = startingBitString;
    this->constants = constants;
    this->endingBitString = 0;
    this->mass = 0;
}

void System::evolveWithReservoir(Reservoir *reservoir) {
    if (this->bitPosition!=0) {
        throw InconsistentSystemState();
    }
    
    unsigned int& startingBitString = this->startingBitString;
    unsigned int& endingBitString = this->endingBitString;
    int& bitPosition = this->bitPosition;
    const int& nbits = constants.getNbits();
    
    while (this->bitPosition < nbits) {
        int oldBit = (startingBitString >> bitPosition) & 1;
        Reservoir::InteractionResult result = reservoir->interactWithBit(oldBit);
        
        int newBit = result.bit;
        this->mass += result.work;
        
//        this->mass -= oldBit - newBit;
        endingBitString |= ( newBit << bitPosition );
        bitPosition++;
    }//End while
}

void Constants::setDelta(const double& newDelta) {
    delta = newDelta;
}

void Constants::setEpsilon(const double& newEpsi) {
    epsilon = newEpsi;
    beta = -log((1-epsilon)/(1+epsilon));
}

void Constants::setTau(const double& newT) {
    tau = newT;
}

void Constants::setNbits(const int& newBits) {
    nBits = newBits;
}




//
//  Utilities.cpp
//  thermaleraser
//
//  Created by Mark Larus on 10/30/12.
//  Copyright (c) 2012 Kenyon College. All rights reserved.
//

#include "Utilities.h"
#include <gsl/gsl_randist.h>
#include <semaphore.h>
#include <assert.h>

int bitCount(unsigned int number, int nbits) {
    int setBits = 0;
    for(int k=0; k<nbits; k++) {
        setBits+=((number >> k) & 1);
    }
    return setBits;
}

unsigned int randomShortIntWithBitDistribution(double delta, int nbits, gsl_rng *randomNumberGenerator) {
    int bits=0;
    for(int k=0; k<nbits; k++) {
        double randomNumber = gsl_rng_uniform(randomNumberGenerator);
        int bit = delta > randomNumber;
        bits|=(bit<<k);
    }
    return bits;
}

gsl_rng *GSLRandomNumberGenerator() {
    gsl_rng *gen;
    #pragma omp critical
    {
        gen = gsl_rng_alloc(gsl_rng_rand);
        long seed = clock()*rand();
        gsl_rng_set(gen, seed);
    }
    
    return gen;
}

StateSet getValidStates() {
    StateSet states;
    #define StateInsert(XX) states.insert(&State##XX)
    StateInsert(A1);
    StateInsert(B1);
    StateInsert(C1);
    StateInsert(A0);
    StateInsert(B0);
    StateInsert(C0);
    return states;
}
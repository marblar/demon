//
//  Utilities.cpp
//  thermaleraser
//
//  Created by Mark Larus on 10/30/12.
//  Copyright (c) 2012 Kenyon College. All rights reserved.
//

#include "Utilities.h"
#include "gsl_rng.h"

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
    gsl_rng *gen = gsl_rng_alloc(gsl_rng_mt19937);
    long seed = time(NULL)*getpid()*rand();
    gsl_rng_set(gen, seed);
    return gen;
}
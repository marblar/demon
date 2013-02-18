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
#include <sqlite3.h>
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
        gen = gsl_rng_alloc(gsl_rng_ranlux389);
        long seed = clock()*rand();
        gsl_rng_set(gen, seed);
    }
    
    return gen;
}

double mbl_ran_exponential(const gsl_rng *r, double rate) {
    return gsl_ran_exponential(r,1/rate);
}
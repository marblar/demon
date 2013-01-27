//
//  System.h
//  thermaleraser
//
//  Created by Mark Larus on 10/30/12.
//  Copyright (c) 2012 Kenyon College. All rights reserved.
//

#ifndef __thermaleraser__System__
#define __thermaleraser__System__

#include <gsl/gsl_rng.h>

class Reservoir;

struct Constants {
    double delta;
    //The initial ratio of 1's to 0's
    
    double epsilon;
    //The weight of the mass.
    
    double tau;
    //The duration of the interaction with each bit in the stream.
    
    int nBits;
    //The number of bits the tape interacts with.
};

struct System {
    int bitPosition;
    double timeSinceLastBit;
    int nbits;
    
    unsigned int startingBitString;
    unsigned int endingBitString;
    int mass;
    Constants constants;
    Reservoir *reservoir;
    System();
};

void evolveSystem(System *system, gsl_rng *rng, bool discrete_system = false);

long packResult(System *system);
System *unpackResult(long long value);

#endif /* defined(__thermaleraser__System__) */

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
    int nbits;
    unsigned int startingBitString;
    unsigned int endingBitString;
    int mass;
    Constants constants;
    
    System(gsl_rng *rng, Constants, int nbits);
    void evolveWithReservoir(Reservoir *reservoir);
};




#endif /* defined(__thermaleraser__System__) */

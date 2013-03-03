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

class Constants {
    double delta;
    //The initial ratio of 1's to 0's
    
    double epsilon;
    //The weight of the mass.
    
    double beta;
    //Derived from epsilon.
    
    double tau;
    //The duration of the interaction with each bit in the stream.
    
    int nBits;
    //The number of bits the tape interacts with.
public:
    void setDelta(const double&);
    void setEpsilon(const double&);
    void setTau(const double&);
    void setNbits(const int&);
    const double &getEpsilon() { return epsilon; }
    const double & getTau() { return tau; }
    const double & getBeta() { return beta; }
    const int & getNbits() { return nBits; }
    const double & getDelta() { return delta; }
    Constants (double d, double e, double t) : delta(d),tau(t) {
        setEpsilon(e);
        setNbits(-1);
    }   
    Constants () {}
};

struct System {
    int bitPosition;
    unsigned int startingBitString;
    unsigned int endingBitString;
    int mass;
    Constants constants;
    System(Constants,int startingBitString);
    void evolveWithReservoir(Reservoir *reservoir);
};

#endif /* defined(__thermaleraser__System__) */

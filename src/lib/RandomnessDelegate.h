//
//  RandomnessDelegate.h
//  jdemon
//
//  Created by Mark Larus on 3/19/13.
//  Copyright (c) 2013 Kenyon College. All rights reserved.
//

#ifndef __jdemon__RandomnessDelegate__
#define __jdemon__RandomnessDelegate__

#include <gsl/gsl_rng.h>
#include <stdexcept>

namespace Randomness {
    class Delegate {
    protected:
        Delegate() {}
    public:
        virtual bool binaryEventWithProbability(double probabilityOfHappening) = 0;
        virtual int randomIntegerFromInclusiveRange(int begin, int end) = 0;
    };
    
    class GSLDelegate : public Delegate {
        gsl_rng *RNG;
    protected:
        bool binaryEventWithProbability( double probabilityOfHappening ) {
            return gsl_rng_get(RNG) < probabilityOfHappening ? 1 : 0;
        };
        int randomIntegerFromInclusiveRange(int begin, int end) {
            if (end<begin) {
                throw std::runtime_error("Invalid RNG range. 'begin' must be less than 'end'");
            }
            return (int)gsl_rng_uniform_int(RNG, end-begin) + begin;
        }
    public:
        GSLDelegate(gsl_rng *r) : RNG(r) {}
    };
}



#endif /* defined(__jdemon__RandomnessDelegate__) */

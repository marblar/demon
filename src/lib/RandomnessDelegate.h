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

// N.B. I want to use polymorphism here for testing reasons, but since RNG
//  is such an important performance constraint, I'm using static polymorphism
//  via the curiously-recurring-template pattern to avoid the memory lookup
//  associated with virtual functions.

template <class Subclass>
class AbstractRandomnessDelegate {
public:
    bool binaryEventWithProbability(double probabilityOfHappening) {
        return static_cast<Subclass *>(this)->binaryEvent( probabilityOfHappening );
    };
    
    int randomIntegerFromInclusiveRange(int begin, int end) {
        return static_cast<Subclass *>(this)->getInteger(begin,end);
    }
};

class GSLRandomnessDelegate : public AbstractRandomnessDelegate<GSLRandomnessDelegate> {
    gsl_rng *RNG;
    friend AbstractRandomnessDelegate;
protected:
    bool binaryEvent( double p ) {
        return gsl_rng_get(RNG) < p ? 1 : 0;
    };
    int getInteger(int begin, int end) {
        if (end<begin) {
            throw std::runtime_error("Invalid RNG range. 'begin' must be less than 'end'");
        }
        return (int)gsl_rng_uniform_int(RNG, end-begin) + begin;
    }
public:
    GSLRandomnessDelegate(gsl_rng *RNG);
};

#endif /* defined(__jdemon__RandomnessDelegate__) */

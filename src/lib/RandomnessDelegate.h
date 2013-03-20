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
    template <class subclass>
    class Delegate {
    protected:
        Delegate() {}
        
    public:
        bool binaryEventWithProbability(double probabilityOfHappening) {
            return static_cast<subclass *>(this)->binaryEventWithProbability_imp(probabilityOfHappening);
        };
        virtual int randomIntegerFromInclusiveRange(int begin, int end) {
            return static_cast<subclass *>(this)->randomIntegerFromInclusiveRange_imp(begin,end);
        }
    };
    
    class GSLDelegate : public Delegate <GSLDelegate> {
        gsl_rng *RNG;
        friend class Delegate<GSLDelegate>;
    protected:
        bool binaryEventWithProbability_imp( double probabilityOfHappening ) {
            return gsl_rng_uniform(RNG) < probabilityOfHappening ? 1 : 0;
        };
        int randomIntegerFromInclusiveRange_imp(int begin, int end) {
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

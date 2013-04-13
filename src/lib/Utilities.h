//
//  Utilities.h
//  thermaleraser
//
//  Created by Mark Larus on 10/30/12.
//  Copyright (c) 2012 Kenyon College. All rights reserved.
//

#ifndef __thermaleraser__Utilities__
#define __thermaleraser__Utilities__

#include <iostream>
#include <gsl/gsl_rng.h>
#include <set>
#include <boost/iterator/iterator_facade.hpp>
#include "Reservoir.h"

namespace DemonBase {
    typedef std::set<DemonBase::SystemState *> StateSet;
    StateSet getValidStates();
}
unsigned int randomShortIntWithBitDistribution(double ratioOfOnesToZeroes, int nbits, gsl_rng* generator);
int bitCount(unsigned int number, int nbits);

template <class BooleanIterator>
class PackedBooleanIterator : public boost::iterator_facade<PackedBooleanIterator<BooleanIterator>, const char, boost::forward_traversal_tag>{
public:
    PackedBooleanIterator(BooleanIterator b, BooleanIterator e) : iterator(b), end(e) {
        currentValue = getEightBits();
    }
private:
    BooleanIterator iterator;
    BooleanIterator end;
    char currentValue;
    friend class boost::iterator_core_access;
    
    void increment() {
        currentValue = getEightBits();
    }
    
    inline char getEightBits() {
        int count = 0;
        char result = '\0';
        while (iterator!=end && count != 8) {
            int bit = *iterator ? 1 : 0;
            result |= (bit << count);
            ++iterator;
            ++count;
        }
        return result;
    }
    
    const char &dereference() const { return currentValue; }
    
    bool equal(PackedBooleanIterator<BooleanIterator> const& other) const {
        return iterator == other.iterator;
    }
};

gsl_rng *GSLRandomNumberGenerator();

#endif /* defined(__thermaleraser__Utilities__) */

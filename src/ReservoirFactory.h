//
//  ReservoirFactory.h
//  thermaleraser
//
//  Created by Mark Larus on 2/17/13.
//  Copyright (c) 2013 Kenyon College. All rights reserved.
//

#ifndef __thermaleraser__ReservoirFactory__
#define __thermaleraser__ReservoirFactory__

#include <iostream>
#include "Reservoir.h"

class ReservoirFactory {
public:
    virtual Reservoir *create(gsl_rng *RNG, Constants constants) = 0;
    virtual ~ReservoirFactory() {}
};

template <class ReservoirKind>
class DefaultArgsReservoirFactory : public ReservoirFactory  {
public:
    virtual Reservoir *create(gsl_rng *RNG, Constants constants) {
        return new ReservoirKind(RNG,constants);
    }
    DefaultArgsReservoirFactory() {}
};



#endif /* defined(__thermaleraser__ReservoirFactory__) */

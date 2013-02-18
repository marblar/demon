//
//  StateMachine.h
//  FSM
//
//  Created by Mark Larus on 9/8/12.
//  Copyright (c) 2012 Kenyon College. All rights reserved.
//

#ifndef __FSM__StateMachine__
#define __FSM__StateMachine__
#include "Reservoir.h"

class StochasticReservoir : public Reservoir {
public:
    gsl_rng *RNG;
    StochasticReservoir(gsl_rng *RNG, Constants constants);
    int interactWithBit(int bit);
};

#endif /* defined(__FSM__StateMachine__) */
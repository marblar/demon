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

namespace Stochastic {
    class Reservoir : public DemonBase::Reservoir {
    public:
        gsl_rng *RNG;
        Reservoir(gsl_rng *RNG, DemonBase::Constants constants);
        InteractionResult interactWithBit(int bit);
        virtual void reset();
    };
}

#endif /* defined(__FSM__StateMachine__) */

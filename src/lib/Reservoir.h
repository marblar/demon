#include <gsl/gsl_rng.h>
#include <string>
#include "System.h"

#ifndef MBL_SYSTEM
#define MBL_SYSTEM

namespace DemonBase {
    class Constants;
    
    struct SystemState {
        SystemState *nextState1;
        SystemState *nextState2;
        SystemState *bitFlipState;
        int bit;
        char letter;
        SystemState *stateForBit(const int &_bit) {
            if (_bit==bit) {
                return this;
            } else {
                return bitFlipState;
            }
        }
    };
    
    class Reservoir {
    protected:
        Reservoir(Constants);
    public:
        SystemState *currentState;
        Constants constants;
        
        struct InteractionResult {
            int work;
            int bit;
            InteractionResult() : work(0), bit(0) {}
        };
        
        virtual InteractionResult interactWithBit(int bit) = 0;
        virtual ~Reservoir() {}
        virtual void reset() = 0;
    };
    
    extern SystemState StateA1, StateB1, StateC1, StateA0, StateB0, StateC0;
    
    void setupStates();
    SystemState *randomState();
    std::string print_state(SystemState *state);
}

#endif
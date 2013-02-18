#include <gsl/gsl_rng.h>
#include <string>
#include "System.h"

#ifndef MBL_SYSTEM
#define MBL_SYSTEM

class Constants;

struct SystemState {
    SystemState *nextState1;
    SystemState *nextState2;
    SystemState *bitFlipState;
    int bit;
    char letter;
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
};

extern SystemState StateA1, StateB1, StateC1, StateA0, StateB0, StateC0;

void setupStates();
SystemState *randomState();
std::string print_state(SystemState *state);

#endif
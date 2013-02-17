#include <gsl/gsl_rng.h>
#include "System.h"

class Constants;

class Reservoir {
public:
    Constants constants;
    virtual int interactWithBit(int bit) = 0;
};

struct SystemState {
    SystemState *nextState1;
    SystemState *nextState2;
    SystemState *bitFlipState;
    int bit;
    char letter;
};

extern SystemState StateA1, StateB1, StateC1, StateA0, StateB0, StateC0;

void setupStates();
SystemState *randomState();
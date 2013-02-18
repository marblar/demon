#include <gsl/gsl_rng.h>
#include <string>
#include "System.h"

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
    virtual int interactWithBit(int bit) = 0;
};

extern SystemState StateA1, StateB1, StateC1, StateA0, StateB0, StateC0;

void setupStates();
SystemState *randomState();
std::string print_state(SystemState *state);
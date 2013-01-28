#include <gsl/gsl_rng.h>
#include "System.h"

class Constants;

class Reservoir {
public:
    Constants constants;
    virtual int interactWithBit(int bit) = 0;
};
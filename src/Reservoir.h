#include <gsl/gsl_rng.h>
#include "System.h"

class Constants;

class Reservoir {
public:
    Constants constants;
    virtual int evolve_bit(int bit) = 0;
};
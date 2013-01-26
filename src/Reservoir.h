#include <gsl/gsl_rng.h>

class Constants;

class Reservoir {
 public:
  Constants constants;
  virtual int evolve_bit(int bit);
};

class StochasticReservoir: public Reservoir {
 public:
  StochasticReservoir(gsl_rng *RNG);
  gsl_rng *RNG;
};

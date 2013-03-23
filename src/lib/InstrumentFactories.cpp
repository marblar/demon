#include "InstrumentFactories.h"
#include "Utilities.h"
using namespace DemonBase;

System *BinomialSystemFactory::create(gsl_rng *RNG, Constants constants) {
    unsigned short int startingString = randomShortIntWithBitDistribution(constants.getDelta(), constants.getNbits(), RNG);
    System *system = new System(constants,startingString);
    return system;
};
//
//  Measurement.h
//  jdemon
//
//  Created by Mark Larus on 3/2/13.
//  Copyright (c) 2013 Kenyon College. All rights reserved.
//

#ifndef __jdemon__Measurement__
#define __jdemon__Measurement__

#include <iostream>
#include "System.h"
#include "InstrumentFactories.h"
#include "Utilities.h"
#include <memory>

struct MeasurementResult {
    Constants constants;
    long double maxJ;
    long double averageJ;
};

class Measurement {
    Constants constants;
    int iterations;
    ReservoirFactory *rfactory;
    SystemFactory *sfactory;
    bool complete;
    MeasurementResult result;
    bool ownsRNG;
    gsl_rng *rng;
protected:
    bool initializedArrays;
    int *histogram;
    long double *p;
    long double *p_prime;
public:
    Measurement(Constants _c, int _it, ReservoirFactory *_rf, SystemFactory *_sf, gsl_rng *RNG = NULL);
    MeasurementResult &getResult();
    void performMeasurement();
    bool isComplete() {
        return complete;
    }
    ~Measurement();
};

struct Experiment {
    int iterations;
    int dimension;
    ReservoirFactory *rfactory;
    SystemFactory *sfactory;
    typedef std::pair<int,int> range;
    void performWork(range work);
    void performMeasurement();
};

long double calculateJ(System &system,long double *p, long double *p_prime);
std::string outputString(MeasurementResult&);
std::string outputHeader();

#endif /* defined(__jdemon__Measurement__) */

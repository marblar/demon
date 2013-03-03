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

struct MeasurementResult {
    Constants constants;
    long double maxJ;
    long double averageJ;
};

class Measurement {
    friend class MeasurementTest;
    Constants constants;
    int iterations;
    ReservoirFactory *rfactory;
    SystemFactory *sfactory;
    bool complete;
    MeasurementResult result;
protected:
    int *histogram;
    long double *p;
    long double *p_prime;
public:
    Measurement(Constants _c, int _it, ReservoirFactory *_rf, SystemFactory *_sf)
            : constants(_c), iterations(_it), rfactory(_rf), sfactory(_sf),
                complete(false) {
        result.constants = constants;
    }
    MeasurementResult &getResult();
    void performMeasurement();
    ~Measurement();
};

long double calculateJ(System &system,long double *p, long double *p_prime);
std::string outputString(MeasurementResult&);
std::string outputHeader();

#endif /* defined(__jdemon__Measurement__) */

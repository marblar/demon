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
#include "ReservoirFactory.h"

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
    int nbits;
    bool complete;
    MeasurementResult result;
protected:
    int *histogram;
    long double *p;
    long double *p_prime;
public:
    Measurement(Constants _c, int _it, ReservoirFactory *_rf, int nb = 8) : \
        constants(_c), iterations(_it), rfactory(_rf), \
            nbits(nb), complete(false) {
            
    }
    MeasurementResult &getResult();
    void performMeasurement();
};

long double calculateJ(System &system,long double *p, long double *p_prime);
std::string outputString(MeasurementResult&);
std::string outputHeader();

#endif /* defined(__jdemon__Measurement__) */

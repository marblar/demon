//
//  Measurement.cpp
//  jdemon
//
//  Created by Mark Larus on 3/2/13.
//  Copyright (c) 2013 Kenyon College. All rights reserved.
//

#include "Measurement.h"

#include <gsl/gsl_rng.h>
#include <gsl/gsl_randist.h>
#include <gsl/gsl_sf.h>
#include <math.h>
#include <assert.h>
#include <limits>
#include <sstream>
#include "System.h"
#include "Reservoir.h"
#include "InstrumentFactories.h"
#include "Utilities.h"

static InvalidNbitsError NbitsError;

std::string outputHeader() {
    return std::string("delta,epsilon,averageJ,maxJ");
}

std::string outputString(MeasurementResult &result) {
    std::stringstream out;
    double delta = result.constants.getDelta();
    double epsilon = result.constants.getEpsilon();
    out << delta \
        << "," \
        << epsilon \
        << "," \
        << result.averageJ \
        << "," \
        << result.maxJ;
    return out.str();
}

MeasurementResult &Measurement::getResult() {
    if (!complete) {
        performMeasurement();
    }
    return result;
}

long double calculateJ(System &currentSystem, long double *p, long double *p_prime) {
    long double workPart = exp(currentSystem.constants.getBeta()*currentSystem.mass);
    long double informationPart = p_prime[currentSystem.endingBitString]/p[currentSystem.startingBitString];
    long double J  = workPart * informationPart;
    return J;
}

void Measurement::performMeasurement() {
    if (constants.getTau()<.1) {
        std::clog<<"Warning: small tau.\n";
    }
    
    int first_pass_iterations = iterations;
    const int &nbits = constants.getNbits();
    
    if (constants.getNbits()<=0) {
        throw InvalidNbitsError();
    }
    
    // In the horribly degenerate case where this function runs twice,
    // we don't want to leak everywhere.
    if (!initializedArrays) {
        histogram = new int[1<<nbits];
        p = new long double[1<<nbits];
        p_prime = new long double[1<<nbits];
        initializedArrays = true;
    }
    
    std::fill_n(histogram, 1<<nbits, 0);
    
    long double sum = 0;
    long double max = 0;
    
    Reservoir *reservoir = rfactory->create(rng,constants);
    
    for (int k=0; k<first_pass_iterations; ++k) {
        System *currentSystem = sfactory->create(rng, constants);
        currentSystem->evolveWithReservoir(reservoir);
        histogram[currentSystem->endingBitString]++;
        delete currentSystem;
        reservoir->reset();
    }
    
    for(int k=0; k<1<<nbits; k++) {
        int setBits = bitCount(k,nbits);
        p[k] = gsl_ran_binomial_pdf(setBits, constants.getDelta(), nbits)/gsl_sf_choose(nbits,setBits);
        p_prime[k]=static_cast<long double>(histogram[k])/(first_pass_iterations);
    }
    
    for(int k=0; k<iterations; k++) {
        System *currentSystem = sfactory->create(rng, constants);
        currentSystem->evolveWithReservoir(reservoir);
        long double J = calculateJ(*currentSystem, p, p_prime);
        max = J > max ? J : max;
        sum += J;
        delete currentSystem;
        reservoir->reset();
    }
    
    result.averageJ = sum/iterations;
    result.maxJ = max;
    
    assert(result.maxJ!=NAN);
    
    delete reservoir;
    complete = true;
}

Measurement::Measurement(Constants _c, int _it, ReservoirFactory *_rf,
            SystemFactory *_sf, gsl_rng *RNG) {
    constants = _c;
    iterations = _it;
    rfactory = _rf;
    sfactory = _sf;
    complete = false;
    if (!RNG) {
        ownsRNG = true;
        rng = GSLRandomNumberGenerator();
    } else {
        rng = RNG;
        ownsRNG = false;
    }
    result.constants = constants;
    initializedArrays = false;
}

Measurement::~Measurement() {
    // Prevents a crash if a call to performMeasurement is optimized out.
    if (initializedArrays) {
        delete [] p_prime;
        delete [] p;
        delete [] histogram;
    }
    
    if (ownsRNG) {
        gsl_rng_free(rng);
    }
}
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
#include <limits>
#include <sstream>
#include "System.h"
#include "Reservoir.h"
#include "ReservoirFactory.h"
#include "Utilities.h"

std::string outputHeader() {
    return std::string("delta,epsilon,averageJ,maxJ");
}

std::string outputString(MeasurementResult &result) {
    std::stringstream out;
    double delta = result.constants.getDelta();
    double epsilon = result.constants.getEpsilon();
    out << result.constants.getDelta() \
        << "," \
        << result.constants.getEpsilon() \
        << "," \
        << result.averageJ \
        << "," \
        << result.maxJ \
        << std::endl;
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
    
    /*! Use this to change the length of the tape. */
    
    int first_pass_iterations = iterations;
    
    histogram = new int[1<<nbits];
    std::fill_n(histogram, 1<<nbits, 0);
    
    p = new long double[1<<nbits];
    p_prime = new long double[1<<nbits];
    int sum = 0;
    
    long double &maxJ = result.maxJ;
    
    gsl_rng *localRNG = GSLRandomNumberGenerator();
    Reservoir *reservoir = rfactory->create(localRNG,constants);
    
    for (int k=0; k<first_pass_iterations; ++k) {
        System *currentSystem = new System(localRNG, constants,nbits);
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
    
    delete [] histogram;
    
    for(int k=0; k<iterations; k++) {
        System *currentSystem = new System(localRNG, constants, nbits);
        currentSystem->evolveWithReservoir(reservoir);
        long double J = calculateJ(*currentSystem, p, p_prime);
        maxJ = J > maxJ ? J : maxJ;
        sum = sum + J;
        delete currentSystem;
        reservoir->reset();
    }
    
    delete [] p_prime;
    delete [] p;
    
    delete reservoir;
    gsl_rng_free(localRNG);
}
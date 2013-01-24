//
//  Utilities.h
//  thermaleraser
//
//  Created by Mark Larus on 10/30/12.
//  Copyright (c) 2012 Kenyon College. All rights reserved.
//

#ifndef __thermaleraser__Utilities__
#define __thermaleraser__Utilities__

#include <iostream>
#include <gsl/gsl_rng.h>
#include <sqlite3.h>
#include <map>

unsigned int randomShortIntWithBitDistribution(double ratioOfOnesToZeroes, int nbits, gsl_rng* generator);
int bitCount(unsigned int number, int nbits);

gsl_rng *GSLRandomNumberGenerator();

double mbl_ran_exponential(const gsl_rng *r, double rate);

class FunctionCache {
private:
    std::map<double, double> values;
    double(*function)(double);
    int capacity;
public:
    FunctionCache(double(*function)(double));
    double evaluate(double value);
};

#endif /* defined(__thermaleraser__Utilities__) */

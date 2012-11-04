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
#include <gsl_rng.h>
#include <sqlite3.h>

unsigned int randomShortIntWithBitDistribution(double ratioOfOnesToZeroes, int nbits, gsl_rng* generator);
int bitCount(unsigned int number, int nbits);

gsl_rng *GSLRandomNumberGenerator();

#endif /* defined(__thermaleraser__Utilities__) */

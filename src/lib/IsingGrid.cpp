//
//  IsingGrid.cpp
//  jdemon
//
//  Created by Mark Larus on 3/11/13.
//  Copyright (c) 2013 Kenyon College. All rights reserved.
//

#include "IsingGrid.h"

int Coordinate::boundsCheck(int x) {
    if (x < 0) {
        return dimension + x;
    }
    if (x >= dimension) {
        return x-dimension;
    }
    return x;
}
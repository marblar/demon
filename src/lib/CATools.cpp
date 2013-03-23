//
//  Grid.cpp
//  jdemon
//
//  Created by Mark Larus on 3/22/13.
//  Copyright (c) 2013 Kenyon College. All rights reserved.
//

#include "CATools.h"
using namespace CATools;

int Coordinate::boundsCheck(int x) {
    if (x < 0) {
        return dimension + x;
    }
    if (x >= dimension) {
        return x-dimension;
    }
    return x;
}

Coordinate::CNeighbors Coordinate::getNeighbors() {
    Coordinate::CNeighbors neighbors = { {
        Coordinate(x,y+1,dimension),
        Coordinate(x,y-1,dimension),
        Coordinate(x+1,y,dimension),
        Coordinate(x-1,y,dimension)
    }};
    return neighbors;
}

Coordinate::CNeighbors Coordinate::twoByTwoFromTopLeftCoordinate() {
    Coordinate::CNeighbors neighbors = { {
        Coordinate(x,y,dimension),
        Coordinate(x,y+1,dimension),
        Coordinate(x+1,y,dimension),
        Coordinate(x+1,y+1,dimension)
    }};
    return neighbors;
}
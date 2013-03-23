//
//  OtherAutomaton.cpp
//  jdemon
//
//  Created by Mark Larus on 3/22/13.
//  Copyright (c) 2013 Kenyon College. All rights reserved.
//

#include "OtherAutomaton.h"
using namespace OtherAutomaton;

Grid::Grid(int dim) : CATools::Grid<Cell>(dim),oddBlocks(_oddBlocks), evenBlocks(_evenBlocks) {
    _oddBlocks.reserve(size());
    _evenBlocks.reserve(size());
    auto oddOutputIterator = std::inserter(_oddBlocks, _oddBlocks.end());
    auto evenOutputIterator = std::inserter(_evenBlocks,_evenBlocks.end());
    for (int column = 0; column < getDimension(); column+=2) {
        for (int row = 0; row < getDimension(); row+=2 ) {
            CATools::Coordinate currentCoord(column,row,getDimension());
            *oddOutputIterator = cellsFromCoordinates<CATools::Coordinate::CNeighbors, 4>(currentCoord.twoByTwoFromTopLeftCoordinate());
        }
        for (int row = 1; row<getDimension(); row+=2) {
            CATools::Coordinate currentCoord(column,row,getDimension());
            *evenOutputIterator = cellsFromCoordinates<CATools::Coordinate::CNeighbors, 4>(currentCoord.twoByTwoFromTopLeftCoordinate());
        }
    }
}

Cell::Cell() : occupied(false) {
    
}
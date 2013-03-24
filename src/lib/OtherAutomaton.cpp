//
//  OtherAutomaton.cpp
//  jdemon
//
//  Created by Mark Larus on 3/22/13.
//  Copyright (c) 2013 Kenyon College. All rights reserved.
//

#include "OtherAutomaton.h"
#include <boost/typeof/typeof.hpp>
#include <boost/foreach.hpp>
#include <map>
using namespace OtherAutomaton;



Grid::Grid(int dim) : CATools::Grid<Cell>(dim),oddBlocks(_oddBlocks), evenBlocks(_evenBlocks) {
    _oddBlocks.reserve(size());
    _evenBlocks.reserve(size());
    std::insert_iterator<BlockList> oddOutputIterator = std::inserter(_oddBlocks,_oddBlocks.begin());
    std::insert_iterator<BlockList> evenOutputIterator = std::inserter(_evenBlocks,_evenBlocks.begin());
    std::multimap<Cell *, Block *> cellsToBlocks;
    for (int column = 0; column < getDimension(); column+=2) {
        for (int row = 0; row < getDimension(); row+=2 ) {
            const CATools::Coordinate currentCellCoord(column,row,getDimension());
            *oddOutputIterator = Block(cellsFromCoordinates<CATools::Coordinate::CNeighbors, 4>(currentCellCoord.twoByTwoFromTopLeftCoordinate()));
        }
        for (int row = 1; row < getDimension(); row+=2) {
            CATools::Coordinate currentCoord(column,row,getDimension());
            *evenOutputIterator = Block(cellsFromCoordinates<CATools::Coordinate::CNeighbors, 4>(currentCoord.twoByTwoFromTopLeftCoordinate()));
        }
    }
    for (int row = 0; row < getDimension()/2; ++row) {
        for (int column = 0; column < getDimension()/2; ++column) {
            const CATools::Coordinate blockCoordinate = CATools::Coordinate(column,row,getDimension()/2);
            const CATools::Coordinate aboveCoordinate = CATools::Coordinate(column,row,getDimension()/2);
            const CATools::Coordinate belowCoordinate = CATools::Coordinate(column,row+1,getDimension()/2);
            
            Block &middle = _evenBlocks[blockCoordinate.getGridIndex()];
            Block &above = _oddBlocks[aboveCoordinate.getGridIndex()];
            Block &below = _oddBlocks[belowCoordinate.getGridIndex()];
            
            middle.overlap[0] = &above;
            middle.overlap[1] = &below;
            above.overlap[1] = &middle;
            below.overlap[0] = &middle;
        }
    }
}

Block::Block(const boost::array<Cell *, 4>& array) {
    std::copy(array.begin(), array.end(), begin());
}
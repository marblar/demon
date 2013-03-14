//
//  IsingGrid.cpp
//  jdemon
//
//  Created by Mark Larus on 3/11/13.
//  Copyright (c) 2013 Kenyon College. All rights reserved.
//

#include "IsingGrid.h"
#include <boost/iterator/filter_iterator.hpp>
using namespace Ising;
using namespace detail;

const Kind Ising::detail::even = false;
const Kind Ising::detail::odd = true;

int Grid::Coordinate::boundsCheck(int x) {
    if (x < 0) {
        return dimension + x;
    }
    if (x >= dimension) {
        return x-dimension;
    }
    return x;
}

void Cell::setValue(const char &c) {
    if (value < 0 || value > 1) {
        throw InvalidCellValue();
    }
    value = c;
}

Grid::Coordinate::CNeighbors Grid::Coordinate::getNeighbors(){
    Coordinate::CNeighbors neighbors = { {
        Coordinate(x,y+1,dimension),
        Coordinate(x,y-1,dimension),
        Coordinate(x+1,y,dimension),
        Coordinate(x-1,y,dimension)
    }};
    return neighbors;
}

Grid::Grid(int dim) : dimension(dim), cells(new Ising::Cell[size()]),\
evens(*this,even), odds(*this,odd) {
    // CNeighbors give the neighbors to a coordinate in coordinate space
    // The function getGridIndex translate coordinate space to memory space
    // Cell::Neighbors should give the neighbors to a cell in memory space
    // This procedure boostraps the memory space using coordinate space.
    for (int k = 0; k!=size(); ++k) {
        int x = k % dimension;
        int y = k / dimension;
        Coordinate currentCoord(x,y,dimension);
        Cell *cell = cells.get() + currentCoord.getGridIndex();
        
        Coordinate::CNeighbors neighbors = currentCoord.getNeighbors();
        Cell::Neighbors::iterator neighborReference = cell->neighbors.begin();
        for (Coordinate::CNeighbors::iterator it = neighbors.begin();
             it!=neighbors.end();
             ++it) {
            Cell *neighbor = cells.get() + it->getGridIndex();
            *neighborReference = neighbor;
            ++neighborReference;
        }
    }
}

Grid::subset::iterator Grid::subset::begin() const {
    CheckerboardCellOffset offsetPredicate(base,kind,dimension);
    return iterator(offsetPredicate,Grid::iterator(base),Grid::iterator(last));
}

Grid::subset::iterator Grid::subset::end() const {
    CheckerboardCellOffset offsetPredicate(base,kind,dimension);
    return Grid::subset::iterator(offsetPredicate,Grid::iterator(last),Grid::iterator(last));
}


long Cell::getEnergy() {
    long e = 0;
    for (Neighbors::iterator it = neighbors.begin(); it!=neighbors.end(); ++it) {
        e += (*it)->getEnergy();
    }
    return e;
}
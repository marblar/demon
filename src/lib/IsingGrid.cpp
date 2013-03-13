//
//  IsingGrid.cpp
//  jdemon
//
//  Created by Mark Larus on 3/11/13.
//  Copyright (c) 2013 Kenyon College. All rights reserved.
//

#include "IsingGrid.h"
using namespace Ising;

int Coordinate::boundsCheck(int x) {
    if (x < 0) {
        return dimension + x;
    }
    if (x >= dimension) {
        return x-dimension;
    }
    return x;
}

Coordinate::CNeighbors Coordinate::getNeighbors(){
    Coordinate::CNeighbors neighbors { {
        Coordinate(x,y+1,dimension),
        Coordinate(x,y-1,dimension),
        Coordinate(x+1,y,dimension),
        Coordinate(x-1,y,dimension)
    }};
    return neighbors;
}

Grid::Grid(int dim) : dimension(dim) {
    cells.reset(new Ising::Cell[dimension*dimension]);
    for (int k = 0; k!=dimension*dimension; ++k) {
        int x = k % dimension;
        int y = k / dimension;
        Coordinate currentCoord(x,y,dimension);
        Cell *cell = cells.get() + currentCoord.getGridIndex();
        Coordinate::CNeighbors neighbors = currentCoord.getNeighbors();
        for (Coordinate::CNeighbors::iterator it = neighbors.begin();
             it!=neighbors.end();
             ++it) {
            // Fixup the reverse link in the first empty slot on the other side.
            Cell *neighbor = cells.get() + currentCoord.getGridIndex();
            Cell::Neighbors reverseNeighbors = neighbor->neighbors;
            Cell::Neighbors::iterator reverseLink = reverseNeighbors.begin();
            for (; reverseLink != reverseNeighbors.end(); ++reverseLink) {
                if (*reverseLink == nullptr) {
                    break;
                }
            }
            if (reverseLink != nullptr) {
                throw std::runtime_error("Something's wrong with the neighbor"
                                         " count, this should be unreachable.");
            }
            *reverseLink = cell;
        }
    }
}
//
//  IsingGrid.cpp
//  jdemon
//
//  Created by Mark Larus on 3/11/13.
//  Copyright (c) 2013 Kenyon College. All rights reserved.
//

#include "IsingGrid.h"
#include <boost/iterator/filter_iterator.hpp>
#include <iterator>
#include <algorithm>
using namespace Ising;
using namespace detail;

const Kind Ising::detail::even = false;
const Kind Ising::detail::odd = true;

const char &Cell::_setValue(const char &c) {
    if (c < 0 || c > 1) {
        throw InvalidCellValue();
    }
    return value = c;
}

Grid::Grid(int dim) : CATools::Grid<Cell>(dim), evens(*this,even), odds(*this,odd) {
    // CNeighbors give the neighbors to a coordinate in coordinate space
    // The function getGridIndex translate coordinate space to memory space
    // Cell::Neighbors should give the neighbors to a cell in memory space
    // This procedure boostraps the memory space using coordinate space.
    for (int k = 0; k!=size(); ++k) {
        int x = k % getDimension();
        int y = k / getDimension();
        CATools::Coordinate currentCoord(x,y,getDimension());
        Cell *cell = cells.get() + currentCoord.getGridIndex();
        
        CATools::Coordinate::CNeighbors neighbors = currentCoord.getNeighbors();
        Cell::Neighbors::iterator neighborReference = cell->neighbors.begin();
        for (CATools::Coordinate::CNeighbors::iterator it = neighbors.begin();
             it!=neighbors.end();
             ++it) {
            Cell *neighbor = cells.get() + it->getGridIndex();
            *neighborReference = neighbor;
            ++neighborReference;
        }
    }
}

template <class T>
bool CheckerboardPtrOffset<T>::operator()(T *n) const {
    if (n<base) {
        throw std::runtime_error("CheckerboardPtrOffset only valid for "
                                 "iterators greater than base.");
    }
    size_t offset = n-base;
    
    bool rowEven = (offset / dimension) % 2;
    bool columnEven = (offset % dimension) % 2;
    bool color = (rowEven + columnEven) % 2;
    
    return kind ? color : !color;
}


long Cell::getEnergy() {
    long e = 0;
    for (Neighbors::iterator it = neighbors.begin(); it!=neighbors.end(); ++it) {
        e += (*it)->getValue()==getValue() ? 0 : 1;
    }
    return e;
}

template <class Container>
std::pair<typename Container::iterator, typename Container::iterator> foreach_container(Container &c) {
    return std::make_pair(c.begin(), c.end());
}

Grid::subset::subset(const Grid &grid, detail::Kind k) : kind(k) {
    CheckerboardCellOffset filter(grid.cells.get(),k,grid.getDimension());
    boost::filter_iterator<CheckerboardCellOffset, Grid::iterator> begin = boost::make_filter_iterator(filter, grid.begin());
    boost::filter_iterator<CheckerboardCellOffset, Grid::iterator> end = boost::make_filter_iterator(filter, grid.end());
    std::copy(begin,end,std::back_inserter(*this));
}

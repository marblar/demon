//
//  IsingGridTest.cpp
//  jdemon
//
//  Created by Mark Larus on 3/11/13.
//  Copyright (c) 2013 Kenyon College. All rights reserved.
//

#include <boost/test/unit_test.hpp>
#include <boost/container/set.hpp>
#include <algorithm>

#include "IsingGrid.h"

using namespace boost::unit_test;
using namespace Ising;
typedef boost::container::set<Ising::Cell*> CellSet;

struct IsingGridFixture {
    Ising::Grid grid;
    IsingGridFixture() : grid(6) {}
};

BOOST_FIXTURE_TEST_SUITE(DetailedIsingGridTest, IsingGridFixture)

BOOST_AUTO_TEST_CASE( testEvenOddIteratorsDisjoint ) {
    CellSet evenSet;
    evenSet.insert(grid.evenIterator(),grid.endIterator());
    CellSet oddSet;
    oddSet.insert(grid.oddIterator(),grid.endIterator());
    
    for (CellSet::iterator it = evenSet.begin(); it!=evenSet.end(); ++it) {
        BOOST_REQUIRE(!oddSet.count(*it));
    }
    
    for (CellSet::iterator it = oddSet.begin(); it!=oddSet.end(); ++it) {
        BOOST_REQUIRE(!evenSet.count(*it));
    }
}

BOOST_AUTO_TEST_CASE( testEvenCellsCount ) {
    int count = 0;
    for (Grid::iterator it = grid.evenIterator(); it!=grid.endIterator(); ++it) {
        ++count;
    }
    BOOST_REQUIRE_EQUAL(count,grid.getDimension()*grid.getDimension()/2);
}

BOOST_AUTO_TEST_CASE( testOddCellsCount ) {
    int count = 0;
    for (Grid::iterator it = grid.oddIterator(); it!=grid.endIterator(); ++it) {
        ++count;
    }
    BOOST_REQUIRE_EQUAL(count,grid.getDimension()*grid.getDimension()/2);
}

BOOST_AUTO_TEST_CASE( testAllCellsCount ) {
    int count = 0;
    for (Grid::iterator it = grid.allIterator(); it!=grid.endIterator(); ++it) {
        ++count;
    }
    BOOST_REQUIRE_EQUAL(count, grid.getDimension()*grid.getDimension());
}

BOOST_AUTO_TEST_CASE( testAllCellsInEvenOdd ) {
    CellSet evenOddSet, allSet;
    evenOddSet.insert(grid.evenIterator(), grid.endIterator());
    evenOddSet.insert(grid.oddIterator(),grid.endIterator());
    
    allSet.insert(grid.allIterator(),grid.endIterator());
    for (CellSet::iterator it = allSet.begin(); it!=allSet.end(); ++it) {
        BOOST_REQUIRE(!evenOddSet.count(*it));
    }
    
    for (CellSet::iterator it = allSet.begin(); it!=allSet.end(); ++it) {
        BOOST_REQUIRE(!allSet.count(*it));
    }
}

BOOST_AUTO_TEST_CASE( testNeighborCount ) {
    for(Grid::iterator it = grid.allIterator(); it!=grid.endIterator(); ++it) {
        int count = 0;
        Cell::Neighbors neighbors = it->getNeighbors();
        for (Cell::Neighbors::iterator neighbor = neighbors.begin();
             neighbor!=neighbors.end();
             ++neighbor) {
            ++count;
        }
        BOOST_REQUIRE_EQUAL(count, 4);
    }
}

BOOST_AUTO_TEST_CASE( testNoNeighborsInOdd ) {
    
}

BOOST_AUTO_TEST_CASE( testGetDimension ) {
    Grid grid(10);
    BOOST_REQUIRE_EQUAL(grid.getDimension(),10);
}

BOOST_AUTO_TEST_CASE( testNeighborsTransitive ) {
    for(Grid::iterator it = grid.allIterator(); it!=grid.endIterator(); ++it) {
        Cell::Neighbors neighbors = it->getNeighbors();
        for (Cell::Neighbors::iterator neighbor = neighbors.begin(); neighbor!=neighbors.end(); ++neighbor) {
            Cell::Neighbors reverseNeighbors = (*neighbor)->getNeighbors();
            long count = std::count(reverseNeighbors.begin(), reverseNeighbors.end(), *it);
            BOOST_REQUIRE_EQUAL(count, 1);
        }
    }
}

BOOST_AUTO_TEST_CASE( testSetValue ) {
    Cell cell;
    cell.setValue(0);
    BOOST_REQUIRE_EQUAL(cell.getValue(), 0);
    cell.setValue(1);
    BOOST_REQUIRE_EQUAL(cell.getValue(), 1);
    BOOST_REQUIRE_THROW(cell.setValue(2), InvalidCellValue);
}

BOOST_AUTO_TEST_SUITE_END()
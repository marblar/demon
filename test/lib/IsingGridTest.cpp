//
//  IsingGridTest.cpp
//  jdemon
//
//  Created by Mark Larus on 3/11/13.
//  Copyright (c) 2013 Kenyon College. All rights reserved.
//

#include <boost/test/unit_test.hpp>
#include <boost/foreach.hpp>
#include <algorithm>
#include <set>
#include <sstream>
#include <map>

#include "IsingGrid.h"
#include "TestFixtures.h"

using namespace boost::unit_test;
using namespace Ising;
typedef std::set<Ising::Cell*> CellSet;

struct EvenIsingGridFixture {
    Ising::Grid grid;
    EvenIsingGridFixture() : grid(6) {}
};

BOOST_FIXTURE_TEST_SUITE(IsingGridTest, EvenIsingGridFixture)

BOOST_AUTO_TEST_CASE( testEvenOddIteratorsDisjoint__evenGrid ) {
    BOOST_REQUIRE_EQUAL(grid.getDimension()%2, 0);
    CellSet evenSet;
    evenSet.insert(grid.evens.begin(),grid.evens.end());
    CellSet oddSet;
    oddSet.insert(grid.odds.begin(),grid.odds.end());
    size_t count = 0;
    
    for (CellSet::iterator it = evenSet.begin(); it!=evenSet.end(); ++it) {
        BOOST_CHECK(!oddSet.count(*it));
        ++count;
        BOOST_REQUIRE_LE(count, grid.size());
    }
    count = 0;
    for (CellSet::iterator it = oddSet.begin(); it!=oddSet.end(); ++it) {
        BOOST_CHECK(!evenSet.count(*it));
        ++count;
        BOOST_REQUIRE_LE(count, grid.size());
    }
}

BOOST_AUTO_TEST_CASE( testEvenCellsCount ) {
    int count = 0;
    for (Grid::subset::const_iterator it = grid.evens.begin(); it!=grid.evens.end(); ++it) {
        ++count;
        // Infinite loop protection.
        BOOST_REQUIRE_LE(count, grid.size());
    }
    BOOST_REQUIRE_EQUAL(count,grid.size()/2);
}

BOOST_AUTO_TEST_CASE( testOddCellsCount ) {
    int count = 0;
    for (Grid::subset::const_iterator it = grid.odds.begin(); it!=grid.odds.end(); ++it) {
        ++count;
        // Infinite loop protection:
        BOOST_REQUIRE_LE(count, grid.size());
    }
    BOOST_REQUIRE_EQUAL(count,grid.size()/2);
}

BOOST_AUTO_TEST_CASE( testAllCellsCount ) {
    int count = 0;
    for (Grid::iterator it = grid.begin(); it!=grid.end(); ++it) {
        ++count;
        // Infinite loop protection:
        BOOST_REQUIRE_LE(count, grid.size());
    }
    BOOST_REQUIRE_EQUAL(count, grid.size());
}

BOOST_AUTO_TEST_CASE( testAllCellsInEvenOdd ) {
    CellSet evenOddSet, allSet;
    evenOddSet.insert(grid.evens.begin(), grid.evens.end());
    evenOddSet.insert(grid.odds.begin(),grid.odds.end());
    allSet.insert(grid.begin(),grid.end());
    
    for (CellSet::iterator it = allSet.begin(); it!=allSet.end(); ++it) {
        BOOST_CHECK(evenOddSet.count(*it));
    }
    
    for (CellSet::iterator it = evenOddSet.begin(); it!=evenOddSet.end(); ++it) {
        BOOST_CHECK(allSet.count(*it));
    }
}

BOOST_AUTO_TEST_CASE( testUniqueNeighbors ) {
    size_t loops = 0;
    for(Grid::iterator it = grid.begin(); it!=grid.end(); ++it) {
        Cell::Neighbors neighbors = (*it)->getNeighbors();
        CellSet neighborSet(neighbors.begin(),neighbors.end());
        
        BOOST_CHECK_EQUAL(neighborSet.size(), neighbors.size());
        
        // I wouldn't consider this an important unit test
        // But we might want to know if this changed:
        BOOST_CHECK_EQUAL(neighbors.size(), 4);
        BOOST_REQUIRE_LE(loops, grid.size());
    }
}

BOOST_AUTO_TEST_CASE( testNeighborsRelation_evenGrid ) {
    CellSet evenNeighbors, evenCells;
    CellSet oddNeighbors, oddCells;
    size_t loopGuard = 0;
    for (Grid::subset::const_iterator it = grid.evens.begin(); it!=grid.evens.end(); ++it) {
        ++loopGuard;
        evenCells.insert(*it);
        Cell::Neighbors neighbors = (*it)->getNeighbors();
        evenNeighbors.insert(neighbors.begin(),neighbors.end());
        BOOST_REQUIRE_LE(loopGuard, grid.size());
    }
    CellSet evenIntersection;
    std::insert_iterator<CellSet> insertionIterator(evenIntersection,evenIntersection.end());
    std::set_intersection(evenNeighbors.begin(), evenNeighbors.end(), evenCells.begin(), evenCells.end(), insertionIterator);
    
    BOOST_CHECK(evenIntersection.empty());
    
    BOOST_CHECK(!oddNeighbors.count(*grid.begin()));
    
    loopGuard = 0;
    for (Grid::subset::const_iterator it = grid.odds.begin(); it != grid.odds.end(); ++it) {
        ++loopGuard;
        oddCells.insert(*it);
        Cell::Neighbors neighbors = (*it)->getNeighbors();
        oddNeighbors.insert(neighbors.begin(),neighbors.end());
        BOOST_REQUIRE_LE(loopGuard, grid.size());
    }
    CellSet oddIntersection;
    std::insert_iterator<CellSet> oddInsertionIterator(evenIntersection,evenIntersection.end());
    std::set_intersection(oddNeighbors.begin(), oddNeighbors.end(), oddCells.begin(), oddCells.end(), oddInsertionIterator);
    BOOST_CHECK(oddIntersection.empty());
    
    BOOST_CHECK_EQUAL_COLLECTIONS(evenNeighbors.begin(), evenNeighbors.end(), oddCells.begin(), oddCells.end());
    BOOST_CHECK_EQUAL_COLLECTIONS(oddNeighbors.begin(), oddNeighbors.end(), evenCells.begin(), evenCells.end());
}

BOOST_AUTO_TEST_CASE( gridSize ) {
    BOOST_REQUIRE_EQUAL(grid.size(),grid.getDimension()*grid.getDimension());
}

BOOST_AUTO_TEST_CASE( testGetDimension ) {
    Grid grid(10);
    BOOST_REQUIRE_EQUAL(grid.getDimension(),10);
}

BOOST_AUTO_TEST_CASE( testNeighborsTransitive ) {
    size_t loops = 0;
    for(Grid::iterator it = grid.begin(); it!=grid.end(); ++it) {
        size_t innerLoops = 0;
        Cell::Neighbors neighbors = (*it)->getNeighbors();
        for (Cell::Neighbors::iterator neighbor = neighbors.begin(); neighbor!=neighbors.end(); ++neighbor) {
            Cell::Neighbors reverseNeighbors = (*neighbor)->getNeighbors();
            long count = std::count(reverseNeighbors.begin(), reverseNeighbors.end(), *it);
            BOOST_CHECK_EQUAL(count, 1);
            BOOST_REQUIRE_LE(innerLoops, neighbors.size());
            ++innerLoops;
        }
        ++loops;
        BOOST_REQUIRE_LE(loops, grid.size());
    }
}

BOOST_AUTO_TEST_CASE( testSetValue ) {
    Cell &cell = *grid[5];
    cell.setValue(0);
    BOOST_CHECK_EQUAL(cell.getValue(), 0);
    cell.setValue(1);
    BOOST_CHECK_EQUAL(cell.getValue(), 1);
    BOOST_REQUIRE_THROW(cell.setValue(2), InvalidCellValue);
    BOOST_REQUIRE_THROW(cell.setValue(-1), InvalidCellValue);
}

BOOST_AUTO_TEST_CASE( testRandomAccess ) {
    // The only interface contract here is that different grid indexes
    // return different cell pointers, and that an invalid index throws.
    CellSet cells;
    size_t size = grid.size();
    for (size_t k = 0; k != grid.size(); ++k) {
        cells.insert(grid[k]);
    }
    BOOST_REQUIRE(cells.size()==size);
    BOOST_REQUIRE_THROW(grid[2*size], CATools::InvalidGridIndex);
}

BOOST_AUTO_TEST_CASE( testEnergy ) {
    BOOST_FOREACH(Cell *center, std::make_pair(grid.begin(),grid.end())) {
        Cell::Neighbors neighbors = center->getNeighbors();
        for (int k = 0; k != (1 << neighbors.size()); ++k) {
            int highNeighbors = 0;
            for(int neighbor = 0; neighbor < neighbors.size(); ++neighbor) {
                int value = (k >> neighbor) & 1;
                highNeighbors += value;
                neighbors[neighbor]->setValue(value);
            }
            center->setValue(0);
            BOOST_CHECK_EQUAL(center->getEnergy(),highNeighbors);
            center->setValue(1);
            BOOST_CHECK_EQUAL(center->getEnergy(),neighbors.size()-highNeighbors);
        }
    }
}

BOOST_AUTO_TEST_CASE( testSpecificEnergy ) {
    Cell &center = *grid[5];
    Cell::Neighbors neighbors = center.getNeighbors();
    center.setValue(1);
    neighbors[0]->setValue(0);
    neighbors[1]->setValue(1);
    neighbors[2]->setValue(1);
    neighbors[3]->setValue(1);
    BOOST_CHECK_EQUAL(1,center.getEnergy());
}

BOOST_AUTO_TEST_CASE( testSubsetsAreDifferentKinds ) {
    BOOST_REQUIRE(grid.odds.getKind()!=grid.evens.getKind());
}

BOOST_AUTO_TEST_CASE( testDifferentSubsetBeginning ) {
    BOOST_REQUIRE(grid.odds.begin()!=grid.evens.begin());
}

BOOST_AUTO_TEST_CASE( testToggleEnergy ) {
    Cell &cell = *grid[5];
    long energy = cell.getEnergy();
    cell.toggle();
    BOOST_REQUIRE_EQUAL(cell.getEnergy(),cell.getNeighbors().size()-energy);
}

BOOST_AUTO_TEST_SUITE_END() 
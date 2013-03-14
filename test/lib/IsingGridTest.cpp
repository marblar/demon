//
//  IsingGridTest.cpp
//  jdemon
//
//  Created by Mark Larus on 3/11/13.
//  Copyright (c) 2013 Kenyon College. All rights reserved.
//

#include <boost/test/unit_test.hpp>
#include <algorithm>
#include <set>
#include <vector>

#include "IsingGrid.h"

using namespace boost::unit_test;
using namespace Ising;
typedef std::set<Ising::Cell*> CellSet;

struct IsingGridFixture {
    Ising::Grid grid;
    IsingGridFixture() : grid(6) {}
};

template<class Set1, class Set2>
bool is_disjoint(const Set1 &set1, const Set2 &set2)
{
    if(set1.empty() || set2.empty()) return true;
    
    typename Set1::const_iterator
    it1 = set1.begin(),
    it1End = set1.end();
    typename Set2::const_iterator
    it2 = set2.begin(),
    it2End = set2.end();
    
    if(*it1 > *set2.rbegin() || *it2 > *set1.rbegin()) return true;
    
    while(it1 != it1End && it2 != it2End)
    {
        if(*it1 == *it2) return false;
        if(*it1 < *it2) { it1++; }
        else { it2++; }
    }
    
    return true;
}

BOOST_FIXTURE_TEST_SUITE(DetailedIsingGridTest, IsingGridFixture)

BOOST_AUTO_TEST_CASE( testEvenOddIteratorsDisjoint ) {
    CellSet evenSet;
    evenSet.insert(grid.evens.begin(),grid.evens.end());
    CellSet oddSet;
    oddSet.insert(grid.evens.begin(),grid.evens.end());
    size_t count = 0;
    
    for (CellSet::iterator it = evenSet.begin(); it!=evenSet.end(); ++it) {
        BOOST_REQUIRE(!oddSet.count(*it));
        ++count;
        BOOST_REQUIRE_LE(count, grid.size());
    }
    count = 0;
    for (CellSet::iterator it = oddSet.begin(); it!=oddSet.end(); ++it) {
        BOOST_REQUIRE(!evenSet.count(*it));
        ++count;
        BOOST_REQUIRE_LE(count, grid.size());
    }
}

BOOST_AUTO_TEST_CASE( testEvenCellsCount ) {
    int count = 0;
    for (Grid::subset::iterator it = grid.evens.begin(); it!=grid.evens.end(); ++it) {
        ++count;
        // Infinite loop protection.
        BOOST_REQUIRE_LE(count, grid.size());
    }
    BOOST_REQUIRE_EQUAL(count,grid.size()/2);
}

BOOST_AUTO_TEST_CASE( testOddCellsCount ) {
    int count = 0;
    for (Grid::subset::iterator it = grid.odds.begin(); it!=grid.odds.end(); ++it) {
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
        BOOST_REQUIRE(evenOddSet.count(*it));
    }
    
    for (CellSet::iterator it = evenOddSet.begin(); it!=evenOddSet.end(); ++it) {
        BOOST_REQUIRE(allSet.count(*it));
    }
}

BOOST_AUTO_TEST_CASE( testUniqueNeighbors ) {
    size_t loops = 0;
    for(Grid::iterator it = grid.begin(); it!=grid.end(); ++it) {
        Cell::Neighbors neighbors = (*it)->getNeighbors();
        CellSet neighborSet(neighbors.begin(),neighbors.end());
        
        BOOST_REQUIRE_EQUAL(neighborSet.size(), neighbors.size());
        
        // I wouldn't consider this an important unit test
        // But we might want to know if this changed:
        BOOST_REQUIRE_EQUAL(neighbors.size(), 4);
        BOOST_REQUIRE_LE(loops, grid.size());
    }
}

BOOST_AUTO_TEST_CASE( testNoNeighborsInOdd ) {
    CellSet evenNeighbors, evenCells;
    CellSet oddNeighbors, oddCells;
    size_t loopGuard = 0;
    for (Grid::subset::iterator it = grid.evens.begin(); it!=grid.evens.end(); ++it) {
        ++loopGuard;
        evenCells.insert(*it);
        Cell::Neighbors neighbors = (*it)->getNeighbors();
        evenNeighbors.insert(neighbors.begin(),neighbors.end());
        BOOST_REQUIRE_LE(loopGuard, grid.size());
    }
    BOOST_REQUIRE(is_disjoint(evenNeighbors,evenCells));
    
    loopGuard = 0;
    for (Grid::subset::iterator it = grid.odds.begin(); it!=grid.odds.end(); ++it) {
        ++loopGuard;
        oddCells.insert(*it);
        Cell::Neighbors neighbors = (*it)->getNeighbors();
        oddNeighbors.insert(neighbors.begin(),neighbors.end());
        BOOST_REQUIRE_LE(loopGuard, grid.size());
    }

    BOOST_REQUIRE(is_disjoint(oddNeighbors, oddCells));
    
    BOOST_REQUIRE_EQUAL_COLLECTIONS(evenNeighbors.begin(), evenNeighbors.end(), oddCells.begin(), oddCells.end());
    BOOST_REQUIRE_EQUAL_COLLECTIONS(oddNeighbors.begin(), oddNeighbors.end(), evenCells.begin(), evenCells.end());
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
            BOOST_REQUIRE_EQUAL(count, 1);
            BOOST_REQUIRE_LE(innerLoops, neighbors.size());
            ++innerLoops;
        }
        ++loops;
        BOOST_REQUIRE_LE(loops, grid.size());
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

BOOST_AUTO_TEST_CASE( testRandomAccess ) {
    // The only interface contract here is that different grid indexes
    // return different cell pointers, and that an invalid index throws.
    CellSet cells;
    size_t size = grid.size();
    for (size_t k = 0; k!=grid.size(); ++k) {
        cells.insert(grid[k]);
    }
    BOOST_REQUIRE(cells.size()==size);
    BOOST_REQUIRE_THROW(grid[2*size], InvalidGridIndex);
}

BOOST_AUTO_TEST_SUITE_END()
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
typedef boost::container::set<IsingCell> CellSet;

struct IsingGridFixture {
    IsingGrid grid;
    IsingGridFixture() : grid(6) {}
};

void neighborsInfiniteLoopTest() {
    IsingGridFixture f;
    int count = 0;
    for (IsingIterator it = f.grid.allIterator(); it!=f.grid.endIterator(); ++it) {
        for (IsingIterator neighbor = it->neighborIterator(); neighbor!=f.grid.endIterator(); ++neighbor) {
            ++count;
        }
    }
    BOOST_REQUIRE_GT(count, 0);
}

void evenInfiniteLoopTest() {
    IsingGridFixture f;
    int count = 0;
    for (IsingIterator it = f.grid.evenIterator(); it!=f.grid.endIterator(); ++it) {
        ++count;
    }
    BOOST_REQUIRE_GT(count, 0);
}

void oddInfiniteLoopTest() {
    IsingGridFixture f;
    int count = 0;
    for (IsingIterator it = f.grid.oddIterator(); it!=f.grid.endIterator(); ++it) {
        ++count;
    }
    BOOST_REQUIRE_GT(count, 1);
}

test_suite*
init_unit_test_suite( int argc, char* argv[] )
{
    test_suite* tests = framework::master_test_suite().get("DetailedIsingGridTests");
    
    test_suite* loopTests = BOOST_TEST_SUITE( IsingGridInfiniteLoopTests );
    
    loopTests->add( BOOST_TEST_CASE(&neighborsInfiniteLoopTest),0,1);
    loopTests->add( BOOST_TEST_CASE(&evenInfiniteLoopTest),0,1);
    loopTests->add( BOOST_TEST_CASE(&oddInfiniteLoopTest),0,1);
    
    tests.depends_on(loopTests);
    framework::master_test_suite().add(loopTests);
    return 0;
}

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
    for (IsingIterator it = grid.evenIterator(); grid!=grid.endIterator(); ++it) {
        ++count;
    }
    BOOST_REQUIRE_EQUAL(count,grid.getDimension()*grid.getDimension()/2);
}

BOOST_AUTO_TEST_CASE( testOddCellsCount ) {
    int count = 0;
    for (IsingIterator it = grid.oddIterator(); grid!=grid.endIterator(); ++it) {
        ++count;
    }
    BOOST_REQUIRE_EQUAL(count,grid.getDimension()*grid.getDimension()/2);
}

BOOST_AUTO_TEST_CASE( testAllCellsCount ) {
    int count = 0;
    for (IsingIterator it = grid.allIterator(); grid!=grid.endIterator(); ++it) {
        ++count;
    }
    BOOST_REQUIRE_EQUAL(count, grid.getDimension()*grid.getDimension());
}

BOOST_AUTO_TEST_CASE( testAllCellsInEvenOdd ) {
    CellSet evenOddSet, allSet;
    evenOddSet.insert(grid.evenIterator(), grid.endIterator());
    evenOddSet.insert(grid.oddIterator(),grid.endIterator());
    
    allset.insert(grid.allIterator(),grid.endIterator());
    for (CellSet::iterator it = allSet.begin(); it!=allSet.end(); ++it) {
        BOOST_REQUIRE(!evenOddSet.count(*it));
    }
    
    for (CellSet::iterator it = allSet.begin(); it!=allSet.end(); ++it) {
        BOOST_REQUIRE(!allSet.count(*it));
    }
}

BOOST_AUTO_TEST_CASE( testNeighborCount ) {
    for(IsingIterator it = grid.allIterator(); it!=grid.endIterator(); ++it) {
        int count = 0;
        for (IsingIterator neighbor = it->neighborIterator(); it!=grid.endIterator(); ++it) {
            ++count;
        }
        BOOST_REQUIRE_EQUAL(count, 4);
    }
}

BOOST_AUTO_TEST_CASE( testNoNeighborsInOdd ) {
    
}

BOOST_AUTO_TEST_CASE( testGetDimension ) {
    IsingGrid grid(10);
    BOOST_REQUIRE_EQUAL(grid.getDimension(),10);
}

BOOST_AUTO_TEST_CASE( testNeighborsTransitive ) {
    for(IsingIterator it = grid.allIterator(); it!=grid.endIterator(); ++it) {
        for (IsingIterator neighbor = it->neighborIterator(); it!=grid.endIterator(); ++it) {
            int count = std::count(neighbor->neighborIterator(), grid.endIterator(), it);
            BOOST_REQUIRE_EQUAL(count, 1);
        }
    }
}

BOOST_AUTO_TEST_CASE( testSetValue ) {
    IsingCell cell;
    cell.setValue(0);
    BOOST_REQUIRE_EQUAL(cell.getValue(), 0);
    cell.setValue(1);
    BOOST_REQUIRE_EQUAL(cell.getValue(), 1);
    BOOST_REQUIRE_THROW(cell.setValue(2), InvalidCellValue);
}

BOOST_AUTO_TEST_SUITE_END()
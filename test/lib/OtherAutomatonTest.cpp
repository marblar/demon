//
//  OtherAutomatonTest.cpp
//  jdemon
//
//  Created by Mark Larus on 3/22/13.
//  Copyright (c) 2013 Kenyon College. All rights reserved.
//

#include <vector>
#include <set>
#include <map>
#include <iterator>
#include <boost/test/unit_test.hpp>
#include <boost/foreach.hpp>
#include "OtherAutomaton.h"
#include "TestFixtures.h"

typedef GridFixture<OtherAutomaton::Grid> OATestFixture;

static inline OATestFixture::CellSet testOverlap(OtherAutomaton::Block left, OtherAutomaton::Block right) {
    using namespace OtherAutomaton;
    OATestFixture::CellSet cellSet;
    BOOST_FOREACH(Cell *leftCell, left) {
        BOOST_FOREACH(Cell *rightCell, right) {
            if (leftCell==rightCell) {
                cellSet.insert(leftCell);
            }
        }
    }
    return cellSet;
}

BOOST_FIXTURE_TEST_SUITE(OtherAutomatonGridTests, OATestFixture)

BOOST_AUTO_TEST_CASE( testBlockOverlap ) {
    // Each even block should be disjoint from each other even block. Same is true for odds.
    
    typedef OtherAutomaton::BlockList::const_iterator iterator;
    typedef std::pair<const OtherAutomaton::Block *, CellSet> pair;
    
    for (iterator odd=grid.oddBlocks.begin(); odd!=grid.oddBlocks.end(); ++odd) {
        std::map<const OtherAutomaton::Block *,CellSet> overlap;
        for (iterator even=grid.evenBlocks.begin(); even!=grid.evenBlocks.end(); ++even) {
            CellSet sharedCells = testOverlap(*even, *odd);
            if (sharedCells.size()>0) {
                overlap.insert(std::make_pair(&*even, sharedCells));
            }
        }
        if (overlap.size()) {
            BOOST_CHECK_EQUAL(overlap.size(), 2);
            BOOST_FOREACH(pair leftItem, overlap) {
                BOOST_CHECK_EQUAL(leftItem.second.size(),2);
            }
        }
    }
    
    typedef OtherAutomaton::BlockList::const_iterator iterator;
    for (iterator even=grid.evenBlocks.begin(); even!=grid.evenBlocks.end(); ++even) {
        std::map<const OtherAutomaton::Block *,CellSet> overlap;
        for (iterator odd=grid.oddBlocks.begin(); odd!=grid.oddBlocks.end(); ++odd) {
            CellSet sharedCells = testOverlap(*odd, *even);
            if (sharedCells.size()>0) {
                overlap.insert(std::make_pair(&*odd, sharedCells));
            }
        }
        if (overlap.size()) {
            BOOST_CHECK_EQUAL(overlap.size(), 2);
            BOOST_FOREACH(pair leftItem, overlap) {
                BOOST_CHECK_EQUAL(leftItem.second.size(),2);
            }
        }
    }
}


BOOST_AUTO_TEST_CASE( testDisjoint ) {
    int leftCount = 0;
    int rightCount = 0;
    BOOST_FOREACH(const OtherAutomaton::Block &left, grid.oddBlocks) {
        BOOST_FOREACH(const OtherAutomaton::Block &right, grid.oddBlocks) {
            if (left != right) {
                CellSet shouldBeEmpty = testOverlap(left,right);
                BOOST_CHECK(shouldBeEmpty.empty());
            }
            ++rightCount;
        }
        ++leftCount;
    }
}

BOOST_AUTO_TEST_CASE( testNotEmpty ) {
    BOOST_CHECK_GT(grid.size(),0);
    BOOST_CHECK_GT(grid.evenBlocks.size(), 0);
    BOOST_CHECK_GT(grid.oddBlocks.size(), 0);
}

BOOST_AUTO_TEST_SUITE_END()

BOOST_AUTO_TEST_SUITE(OtherAutomatonGridReservoirTests)

BOOST_FIXTURE_TEST_CASE(testEmptyGridInitialization, RandomGridOperationTestFixture<OATestFixture>) {
    double p = .35;
    int expectedCount = floor(p*grid.size());
    OtherAutomaton::initializeGridWithOccupationProbability(grid,p,delegate);
    BOOST_CHECK_EQUAL(expectedCount, changedCells().size());
}

BOOST_FIXTURE_TEST_CASE(testNonEmptyGridInitialization, RandomGridOperationTestFixture<OATestFixture>) {
    double p = .57;
    int expectedCount = floor(p*grid.size());
    grid[5]->setValue(true);
    grid[20]->setValue(true);
    OtherAutomaton::initializeGridWithOccupationProbability(grid, p,delegate);
    BOOST_CHECK_EQUAL(expectedCount, changedCells().size());
}

BOOST_FIXTURE_TEST_CASE(testDifferentGridInitialization, RandomGridOperationTestFixture<OATestFixture>) {
    using namespace OtherAutomaton;
    const int iterations = 1000;
    
    boost::array<int, 36> array;
    std::fill(array.begin(), array.end(), 0);
    BOOST_REQUIRE_EQUAL(array.size(),grid.size());
    
    double p = (double)(floor(.2*grid.size()))/grid.size();
    double expectedRatio = p;
    double variance = expectedRatio*(1-expectedRatio)/iterations;
    double standard_deviation = sqrt(variance);
    double acceptable_error = 3*standard_deviation/expectedRatio;

    const Cell *base = *grid.begin();
    
    for (int k = 0; k<iterations; ++k) {
        resetInitialValues();
        OtherAutomaton::initializeGridWithOccupationProbability(grid, p, delegate);
        CellSet changes = changedCells();
        for (CellSet::iterator it = changes.begin(); it!=changes.end(); ++it) {
            Cell *cell = *it;
            if (cell->getValue()) {
                ++array[cell-base];
            }
        }
    }
    
    for (BOOST_AUTO(it,array.begin()); it!=array.end(); ++it) {
        double actualRatio = (double)(*it)/iterations;
        BOOST_CHECK_CLOSE_FRACTION(expectedRatio, actualRatio, acceptable_error);
    }
}
BOOST_AUTO_TEST_SUITE_END()

BOOST_AUTO_TEST_SUITE(OtherAutomatonCell)

BOOST_AUTO_TEST_CASE( defaultValue ) {
    OtherAutomaton::Cell cell;
    BOOST_REQUIRE(!cell.getValue());
}

BOOST_AUTO_TEST_SUITE_END()
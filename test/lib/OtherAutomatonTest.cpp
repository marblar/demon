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
#include <boost/math/tools/fraction.hpp>
#include "OtherAutomaton.h"
#include "TestFixtures.h"

struct OATestFixture : public GridFixture<OtherAutomaton::Grid> {
    
};

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

BOOST_AUTO_TEST_CASE( testBlockOverlapSize ) {
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
        BOOST_CHECK_EQUAL(overlap.size(), 2);
        BOOST_FOREACH(pair leftItem, overlap) {
            BOOST_CHECK_EQUAL(leftItem.second.size(),2);
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
        BOOST_CHECK_EQUAL(overlap.size(), 2);
        BOOST_FOREACH(pair leftItem, overlap) {
            BOOST_CHECK_EQUAL(leftItem.second.size(),2);
        }
    }
}

BOOST_AUTO_TEST_CASE( testBlockOverlap ) {
    for (BOOST_AUTO(odd,grid.oddBlocks.begin()); odd!=grid.oddBlocks.end(); ++odd) {
        std::map<const OtherAutomaton::Block *,CellSet> overlap;
        for (BOOST_AUTO(even,grid.evenBlocks.begin()); even!=grid.evenBlocks.end(); ++even) {
            CellSet sharedCells = testOverlap(*even, *odd);
            if (sharedCells.size()>0) {
                overlap.insert(std::make_pair(&*even, sharedCells));
            }
        }
        
        BOOST_REQUIRE_EQUAL(overlap.size(),2);
        boost::array<const OtherAutomaton::Block *, 2> blocks;
        int index = 0;
        BOOST_FOREACH(typeof(*overlap.begin()) leftItem, overlap) {
            blocks[index++] = leftItem.first;
        }
        const OtherAutomaton::Block &middle = *odd;
        
        const int above = 0;
        const int below = 1;
        const int unknown = 2;
        
        boost::array<int, 2> classified;
        for (int k = 0; k!=2; ++k) {
            int c = unknown;
            if (blocks[k]->isAbove(middle)) {
                BOOST_CHECK_EQUAL(c, unknown);
                c = above;
            }
            if (blocks[k]->isBelow(middle)) {
                BOOST_CHECK_EQUAL(c, unknown);
                c = below;
            }
            classified[k]=c;
        }
        size_t aboveCount = std::count(classified.begin(), classified.end(), above);
        size_t belowCount = std::count(classified.begin(), classified.end(), below);
        BOOST_CHECK_EQUAL(aboveCount, 1);
        BOOST_CHECK_EQUAL(belowCount, 1);
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
            } else {
                CellSet shouldBeFull = testOverlap(left, right);
                BOOST_CHECK_EQUAL(shouldBeFull.size(), 4);
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

BOOST_AUTO_TEST_CASE( testNumberOfBlocks ) {
    BOOST_REQUIRE_EQUAL(grid.evenBlocks.size(),grid.size()/4);
    BOOST_REQUIRE_EQUAL(grid.evenBlocks.size(), grid.size()/4);
}

BOOST_AUTO_TEST_CASE( testBadDimensions ) {
    BOOST_REQUIRE_THROW(OtherAutomaton::Grid tooSmall(2), CATools::InvalidGridSize);
    BOOST_REQUIRE_THROW(OtherAutomaton::Grid odd(15),CATools::InvalidGridSize);
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
    const int iterations = 1000*grid.size();
    
    boost::array<int, 36> array;
    std::fill(array.begin(), array.end(), 0);
    BOOST_REQUIRE_EQUAL(array.size(),grid.size());
    int cellCount = 15;
    
    double p = double(cellCount)/grid.size();
    double expectedRatio = p;
    double variance = expectedRatio*(1-expectedRatio)/iterations;
    double standard_deviation = sqrt(variance);
    double acceptable_error = 3*standard_deviation/expectedRatio;
    BOOST_REQUIRE_LE(acceptable_error,0.05);

    const Cell *base = *grid.begin();
    
    for (int k = 0; k<iterations; ++k) {
        OtherAutomaton::initializeGridWithOccupationProbability(grid, 0, delegate);
        resetInitialValues();
        OtherAutomaton::initializeGridWithOccupationProbability(grid, p, delegate);
        CellSet changes = changedCells();
        BOOST_REQUIRE_EQUAL(changes.size(), cellCount);
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

BOOST_FIXTURE_TEST_CASE(testTooSmallGridInitialization, RandomGridOperationTestFixture<OATestFixture>) {
    double p = 0.001;
    BOOST_REQUIRE_EQUAL((int)(grid.size()*p),0);
    BOOST_REQUIRE_THROW(OtherAutomaton::initializeGridWithOccupationProbability(grid, p, delegate), OtherAutomaton::InvalidProbabilityError);
}


BOOST_AUTO_TEST_SUITE_END()

BOOST_AUTO_TEST_SUITE(OtherAutomatonCell)

BOOST_AUTO_TEST_CASE( defaultValue ) {
    OtherAutomaton::Cell cell;
    BOOST_REQUIRE(!cell.getValue());
}

BOOST_AUTO_TEST_SUITE_END()
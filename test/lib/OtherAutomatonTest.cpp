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



struct OATestFixture {
    OtherAutomaton::Grid grid;
    OATestFixture() : grid(6) {}
};

typedef std::set<OtherAutomaton::Cell *> CellSet;

static inline CellSet testOverlap(OtherAutomaton::Block left, OtherAutomaton::Block right) {
    using namespace OtherAutomaton;
    CellSet cellSet;
    BOOST_FOREACH(Cell *leftCell, left) {
        BOOST_FOREACH(Cell *rightCell, right) {
            if (leftCell==rightCell) {
                cellSet.insert(leftCell);
            }
        }
    }
    return cellSet;
}

BOOST_FIXTURE_TEST_SUITE(OtherAutomatonTests, OATestFixture)

BOOST_AUTO_TEST_CASE( testBlockOverlap ) {
    // Each even block should be disjoint from each other even block. Same is true for odds.
    
    typedef OtherAutomaton::BlockList::const_iterator iterator;
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
            BOOST_FOREACH(auto leftItem, overlap) {
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
            BOOST_FOREACH(auto leftItem, overlap) {
                BOOST_CHECK_EQUAL(leftItem.second.size(),2);
            }
        }
    }
}


BOOST_AUTO_TEST_CASE( testDisjoint ) {
    int leftCount = 0;
    int rightCount = 0;
    BOOST_FOREACH(auto left, grid.oddBlocks) {
        
        BOOST_FOREACH(auto right, grid.oddBlocks) {
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

BOOST_AUTO_TEST_SUITE(OtherAutomatonCell)

BOOST_AUTO_TEST_CASE( defaultValue ) {
    OtherAutomaton::Cell cell;
    BOOST_REQUIRE(!cell.isOccupied());
}

BOOST_AUTO_TEST_SUITE_END()
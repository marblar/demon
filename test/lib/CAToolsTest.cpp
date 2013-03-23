//
//  CAToolsTest.cpp
//  jdemon
//
//  Created by Mark Larus on 3/22/13.
//  Copyright (c) 2013 Kenyon College. All rights reserved.
//

#include <boost/test/unit_test.hpp>
#include "MockObjects.h"
#include "TestFixtures.h"
#include "CATools.h"

#include <algorithm>

BOOST_AUTO_TEST_SUITE(CoordinateTest)

BOOST_AUTO_TEST_CASE( testCoordinateNeighbors ) {
    CATools::Coordinate center(5,5,7);
    CATools::Coordinate::CNeighbors neighbors = center.getNeighbors();
    std::vector<CATools::Coordinate> neighborSet(neighbors.begin(),neighbors.end());
    BOOST_REQUIRE(std::find(neighbors.begin(),neighbors.end(),CATools::Coordinate(5,6,7)));
    BOOST_REQUIRE(std::find(neighbors.begin(), neighbors.end(), CATools::Coordinate(5,4,7)));
    BOOST_REQUIRE(std::find(neighbors.begin(),neighbors.end(),CATools::Coordinate(6,5,7)));
    BOOST_REQUIRE(std::find(neighbors.begin(),neighbors.end(),CATools::Coordinate(4,5,7)));
}

BOOST_AUTO_TEST_CASE( TestCoordinateInitialization ) {
    BOOST_REQUIRE_THROW(CATools::Coordinate c(0,0,-1),CATools::InvalidGridSize);
}

BOOST_AUTO_TEST_SUITE_END()

class TestGridFixture {
public:
    CATools::Grid<TestCell> grid;
    TestGridFixture() : grid(6) {}
};

typedef CATools::Grid<TestCell> TestGrid;

BOOST_FIXTURE_TEST_SUITE(CAToolsTests, TestGridFixture)

BOOST_AUTO_TEST_CASE( oddGridInvalid ) {
    BOOST_REQUIRE_THROW(TestGrid k(3), CATools::InvalidGridSize);
}

BOOST_AUTO_TEST_CASE( tooSmallGrid ) {
    BOOST_REQUIRE_THROW(TestGrid k(2), CATools::InvalidGridSize);
}

BOOST_AUTO_TEST_CASE( testAllCellsCount ) {
    int count = 0;
    for (TestGrid::iterator it = grid.begin(); it!=grid.end(); ++it) {
        ++count;
        // Infinite loop protection:
        BOOST_REQUIRE_LE(count, grid.size());
    }
    BOOST_REQUIRE_EQUAL(count, grid.size());
}

BOOST_AUTO_TEST_SUITE_END()
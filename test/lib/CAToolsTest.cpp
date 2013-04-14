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
#include "Utilities.h"

#include <algorithm>
#include <map>

class TestCell : public CATools::Cell<TestCell, bool> {
    
};

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

BOOST_AUTO_TEST_CASE( testCoordinateInitialization ) {
    BOOST_REQUIRE_THROW(CATools::Coordinate bad(0,0,-1),CATools::InvalidGridSize);
    int x = 1;
    int y = 2;
    int dimension = 3;
    CATools::Coordinate good(x,y,dimension);
    BOOST_REQUIRE_EQUAL(good.getX(), x);
    BOOST_REQUIRE_EQUAL(good.getY(), y);
    BOOST_REQUIRE_EQUAL(good.getDimension(), dimension);
}

BOOST_AUTO_TEST_CASE( testCoordinateEdges ) {
    int dimension = 5;
    CATools::Coordinate center(-1,-1,dimension);
    BOOST_CHECK_EQUAL(center.getX(), dimension-1);
    BOOST_CHECK_EQUAL(center.getY(), dimension-1);
}

BOOST_AUTO_TEST_SUITE_END()

class TestGridFixture {
public:
    typedef CATools::Grid<TestCell> Grid;
    Grid grid;
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

BOOST_AUTO_TEST_CASE( testTwoByTwo ) {
    
}

class RandomCellTestFixture : public RandomNumberTestFixture, public TestGridFixture {
};

BOOST_FIXTURE_TEST_CASE(testGetRandomCell, RandomCellTestFixture) {
    const size_t iterations = grid.size()*10000;
    std::map<TestCell *, int> counter;
    Randomness::GSLDelegate delegate(rng);
    for (size_t k = 0; k<iterations; ++k) {
        ++counter[&grid[delegate]];
    }
    double expectedRatio = 1.0f/grid.size();
    double variance = expectedRatio*(1-expectedRatio)/iterations;
    double standard_deviation = sqrt(variance);
    
    // At the typical two-sigma error margin, the probability of any
    // one measurement failing is 5%. The probability of 36 samples within those
    // bounds is about 15%.
    
    double acceptable_error = 3*standard_deviation/expectedRatio;
    BOOST_REQUIRE_LE(acceptable_error,0.05);
    
    for (TestGrid::iterator it = grid.begin(); it!=grid.end(); ++it) {
        double actualRatio = (double)(counter[*it])/iterations;
        BOOST_CHECK_CLOSE_FRACTION(expectedRatio, actualRatio,acceptable_error);
    }
}

BOOST_AUTO_TEST_CASE( testCount ) {
    std::set<TestCell *> cells(grid.begin(),grid.end());
    BOOST_REQUIRE_EQUAL(cells.size(), grid.size());
}


BOOST_AUTO_TEST_CASE( testValuesIterators ) {
    Grid::iterator cell = grid.begin();
    for (BOOST_AUTO(value,grid.values.begin()); value!=grid.values.end(); ++value) {
        BOOST_REQUIRE(cell!=grid.end());
        bool cellValue = (*cell)->getValue();
        bool iteratorValue = *value;
        BOOST_CHECK(iteratorValue==cellValue);
        ++cell;
    }
}

BOOST_AUTO_TEST_SUITE_END()

BOOST_AUTO_TEST_SUITE(CellTestSuite)

BOOST_AUTO_TEST_CASE(cellTransformer) {
    TestCell::ValueTransformer vt;
    TestCell cell;
    cell.setValue(false);
    BOOST_CHECK(vt(cell)==false);
    cell.setValue(true);
    BOOST_CHECK(vt(cell)==true);
}


BOOST_AUTO_TEST_SUITE_END()

BOOST_AUTO_TEST_SUITE(UtilitiesTestSuite)

BOOST_AUTO_TEST_CASE(testRandomState) {
    int iterations = 20000;
    double expectedRatio = 1.0/6.0;
    double variance = expectedRatio*(1-expectedRatio)/iterations;
    double standard_deviation = sqrt(variance);
    double acceptable_error = 3*standard_deviation/expectedRatio;
    BOOST_REQUIRE_LE(acceptable_error,0.05);
    
    std::map<DemonBase::SystemState *, int> counter;
    for (int k = 0; k<iterations; ++k) {
        ++counter[DemonBase::randomState()];
    }
    for (BOOST_AUTO(it,counter.begin());it!=counter.end(); ++it) {
        double actualRatio = it->second/static_cast<double>(iterations);
        BOOST_CHECK_CLOSE_FRACTION(actualRatio, expectedRatio, acceptable_error);
    }
}

BOOST_AUTO_TEST_CASE(testBitStringUnpack_fullBytes) {
    typedef boost::array<bool,80> charray;
    charray values;
    char str[9] = "abcdefgh";
    charray::iterator input = values.begin();
    for (char * letter = str; letter!=str+9; ++letter) {
        char c = *letter;
        for (int index = 0; index != 8; ++index) {
            (*input) = c & 1;
            c = (c >> 1);
            ++input;
        }
    }
    PackedBooleanIterator<charray::iterator> begin(values.begin(),values.end());
    PackedBooleanIterator<charray::iterator> end(values.end(),values.end());
    
    std::string other_string(begin,end);
    
    BOOST_CHECK_EQUAL_COLLECTIONS(begin, end, str, str+9);
}

BOOST_AUTO_TEST_SUITE_END()
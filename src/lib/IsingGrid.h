//
//  IsingGrid.h
//  jdemon
//
//  Created by Mark Larus on 3/11/13.
//  Copyright (c) 2013 Kenyon College. All rights reserved.
//

#ifndef __jdemon__IsingGrid__
#define __jdemon__IsingGrid__

#include <iterator>
#include <stdexcept>
#include <string>
#include <boost/scoped_array.hpp>
#include <boost/array.hpp>
#include <boost/iterator_adaptors.hpp>

namespace Ising {
    class Cell;
    class Grid;
    class Coordinate;
    class InvalidCellValue;
    
    class Cell {
        unsigned char value;
    protected:
        typedef boost::array<Cell*, 4> Neighbors;
        friend Grid;
        boost::array<Cell*, 4> neighbors;
        Cell() : value(0) { std::fill(neighbors.begin(), neighbors.end(), nullptr); }
    public:
        unsigned char &getValue();
        unsigned char setValue(const char &c);
        const Neighbors getNeighbors() { return neighbors; }
    };
    
    class Grid {
        int dimension;
        boost::scoped_array<Cell> cells;
    public:
        const int &getDimension() const;
        Grid(int dimension);
    };
    
    class InvalidCellValue : public std::runtime_error {
    public:
        InvalidCellValue() : std::runtime_error(std::string("Tried to set cell to invalid value.")) {}
    };
    
    class Coordinate {
    protected:
        friend Grid;
        int boundsCheck(int x);
        size_t getGridIndex() { return dimension * x + y; }
    private:
        typedef boost::array<const Coordinate, 4> CNeighbors;
        int x,y, dimension;
    public:
        CNeighbors getNeighbors();
        Coordinate(int x_, int y_,int dim) :
        x(boundsCheck(x_)), y(boundsCheck(y_)), dimension(dim) {}
        Coordinate(int dim) : x(0), y(0), dimension(dim) {}
        bool operator==(Coordinate &rhs) { return (x==rhs.x) && (y==rhs.y); }
        const int& getX() { return x; }
        const int& getY() { return y; }
        void setX(int x_) { x = boundsCheck(x_); }
        void setY(int y_) { y = boundsCheck(y_); }
    };
}



#endif /* defined(__jdemon__IsingGrid__) */

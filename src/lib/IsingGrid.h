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

class IsingIterator;
class IsingCell;
class IsingGrid;
class Coordinate;

class IsingCell {
public:
    unsigned char &getValue();
    unsigned char setValue(const char &);
    IsingIterator neighborIterator() const;
};

class IsingGrid {
public:
    IsingIterator oddIterator();
    IsingIterator evenIterator();
    IsingIterator allIterator();
    const IsingIterator &endIterator() const;
    const int &getDimension() const;
    IsingGrid(int dimension);
};

class InvalidCellValue : public std::runtime_error {
public:
    InvalidCellValue() : std::runtime_error(std::string("Tried to set cell to invalid value.")) {}
};

class Coordinate {
    int x,y, dimension;
protected:
    int boundsCheck(int x);
public:
    Coordinate(int x_, int y_,int dim) : x(x_), y(y_), dimension(dim) {}
    Coordinate(int dim) : x(0), y(0), dimension(dim) {}
    bool operator==(Coordinate &rhs) { return (x==rhs.x) && (y==rhs.y); }
    const int& getX() { return x; }
    const int& getY() { return y; }
    void setX(int x_) { x = boundsCheck(x_); }
    void setY(int y_) { y = boundsCheck(y_); }
};

#endif /* defined(__jdemon__IsingGrid__) */

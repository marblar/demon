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

class IsingCell {
public:
    unsigned char &getValue();
    unsigned char setValue(const char &);
    IsingIterator neighborIterator() const;
};

class IsingIterator :std::iterator<std::bidirectional_iterator_tag,IsingCell> {
public:
    virtual IsingCell &operator*() = 0;
    virtual IsingCell &operator++() = 0;
    virtual IsingCell &operator--() = 0;
    virtual ~IsingIterator();
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

#endif /* defined(__jdemon__IsingGrid__) */

//
//  Grid.h
//  jdemon
//
//  Created by Mark Larus on 3/22/13.
//  Copyright (c) 2013 Kenyon College. All rights reserved.
//

#ifndef __jdemon__Grid__
#define __jdemon__Grid__

#include <boost/smart_ptr.hpp>
#include <boost/iterator/counting_iterator.hpp>
#include <boost/iterator/transform_iterator.hpp>
#include <boost/array.hpp>
#include <boost/functional/hash.hpp>

namespace CATools {
    class InvalidGridSize : public std::runtime_error {
    public:
        InvalidGridSize() : std::runtime_error(std::string("Grid size must be an even integer greater than 3")) {}
    };
    
    class InvalidGridIndex : public std::runtime_error {
    public:
        InvalidGridIndex() : std::runtime_error(std::string("Tried to access a cell at an index greater than dimension*dimension.")){};
    };
    
    class InvalidCoordinateComparison : public std::runtime_error {
    public:
        InvalidCoordinateComparison() : std::runtime_error(std::string("Cannot compare cells of different dimension.")) {}
    };
    
    
    template <class Cell>
    class CellToValueTransformer : public std::unary_function<const Cell &, const typename Cell::valueType &>,
                                          std::unary_function<const Cell *, const typename Cell::valueType &>
    {
    public:
        const typename Cell::valueType& operator()(const Cell &cell) const {
            return cell.getValue();
        }
        const typename Cell::valueType& operator()(const Cell *cell) const {
            return cell->getValue();
        }
        typedef const typename Cell::valueType& result_type;
    };
    
    
    // This class uses the curiously recurring template pattern.
    template <class Subclass, class ValueType, ValueType defaultValue = 0>
    class Cell {
    protected:
        ValueType value;
        const ValueType& _setValue(const ValueType& val) {
            return value = val;
        }
    public:
        Cell() : value(defaultValue) {}
        const ValueType &getValue() const { return value; }
        
        // Subclasses can implement _setValue.
        ValueType setValue(const ValueType& value) {
            return static_cast<Subclass *>(this)->_setValue(value);
        }
        
        typedef ValueType valueType;
        typedef CellToValueTransformer<Subclass> ValueTransformer;
    };
    
    template <class Grid>
    class Values {
        Grid &grid;
    public:
        Values(Grid &_grid) : grid(_grid) {}
        typedef boost::transform_iterator<typename Grid::cellType::ValueTransformer, typename Grid::iterator> iterator;
        iterator begin() const { return iterator(grid.begin()); }
        iterator end() const { return boost::make_transform_iterator(grid.end(),grid.valueTransformer()); }
    };

    template <class CellType>
    class Grid {
        int dimension;
    protected:
        boost::scoped_array<CellType> cells;
    public:
        const int &getDimension() const { return dimension; }
        Grid(int dim) : dimension(dim), cells(new CellType[dim*dim]), values(*this) {
            if (dimension % 2 || dimension < 4) {
                throw InvalidGridSize();
            }
        }
        size_t size() const { return dimension*dimension; }
        
        CellType & operator[](size_t gridIndex) {
            if (gridIndex > size()) {
                throw InvalidGridIndex();
            }
            return cells[gridIndex];
        }
        CellType & operator[](const int &gridIndex) { return (*this)[(size_t)gridIndex]; }
        const CellType & operator[](const int &gridIndex) const { return (*this)[(size_t)gridIndex]; }
        template <class RandomnessDelegate>
        CellType & operator[](RandomnessDelegate &delegate) {
            size_t index = delegate.randomIntegerFromInclusiveRange(0,(int)size());
            return (*this)[index];
        }
        
        typename CellType::ValueTransformer valueTransformer() const { return typename CellType::ValueTransformer(); }
        
        typedef CellType cellType;
        
        template <class Container, int N>
        boost::array<CellType *, N> cellsFromCoordinates(const Container & coords) {
            boost::array<CellType *, N> result;
            for(int k = 0; k<N; ++k) {
                result[k] = cells.get() + coords[k].getGridIndex();
            }
            return result;
        }
       
        typedef boost::counting_iterator<CellType *> iterator;
        iterator begin() const { return iterator(cells.get()); }
        iterator end() const { return iterator(cells.get()+dimension*dimension); }
        
        Values<Grid> values;
    };
    
    template <class CellType>
    std::size_t hash_value(Grid<CellType> const& grid)
    {
        return boost::hash_range(grid.values.begin(), grid.values.end());
    }
    
    class Coordinate {
    protected:
        int boundsCheck(int x);
    private:
        int dimension,x,y;
    public:
        typedef boost::array<const Coordinate, 4> CNeighbors;
        CNeighbors getNeighbors();
        const int &getDimension() const { return dimension; }
        Coordinate(int x_, int y_,int dim) : dimension(dim) {
            x = boundsCheck(x_);
            y = boundsCheck(y_);
            validateCoordinateParams();
        }
        size_t getGridIndex() const { return dimension * x + y; }
        Coordinate(int dim) : x(0), y(0), dimension(dim) {
            validateCoordinateParams();
        }
        inline void validateCoordinateParams() {
            if(dimension<1) {
                throw InvalidGridSize();
            }
        }
        CNeighbors twoByTwoFromTopLeftCoordinate() const;
        bool operator==(Coordinate const &rhs) const  { return (x==rhs.x) && (y==rhs.y); }
        
        const int& getX() { return x; }
        const int& getY() { return y; }
        void setX(int x_) { x = boundsCheck(x_); }
        void setY(int y_) { y = boundsCheck(y_); }
    };
}

#endif /* defined(__jdemon__Grid__) */

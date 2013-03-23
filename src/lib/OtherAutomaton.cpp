//
//  OtherAutomaton.cpp
//  jdemon
//
//  Created by Mark Larus on 3/22/13.
//  Copyright (c) 2013 Kenyon College. All rights reserved.
//

#include "OtherAutomaton.h"
using namespace OtherAutomaton;

Grid::Grid(int dim) : CATools::Grid<Cell>(dim),oddBlocks(_oddBlocks), evenBlocks(_evenBlocks) {
    
}

Cell::Cell() {
    
}
//
//  Circos.cpp
//  jdemon
//
//  Created by Mark Larus on 2/27/13.
//  Copyright (c) 2013 Kenyon College. All rights reserved.
//

#include "Circos.h"
#include "Utilities.h"
#include "System.h"
#include <assert.h>
#include <numeric>
#include <stdlib.h>
#include <sstream>
#include <iomanip>


CircosLogger::CircosLogger(int tapeLength) : dimension(tapeLength+1), tape(tapeLength) {
    links = new unsigned int *[dimension];
    for (int k=0; k<dimension; k++) {
        links[k] = new unsigned int[dimension];
        std::fill(links[k], links[k]+dimension, 0);
    }
};

void CircosLogger::logSystem(System *system) {
    unsigned int start = toIndex(system->startingBitString);
    unsigned int end = toIndex(system->endingBitString);
    links[start][end]++;
}

void CircosLogger::writeLog(std::ostream &output) {
    output << "labels ";
    for (int k=0; k<dimension; k++) {
        output << (char)('A'+k) << ' ';
    }
    output<<std::endl;
    for (int k = 0; k<dimension; k++) {
        output << (char)('A'+k) << ' ';
        for (int col = 0; col<dimension; col++) {
            output << links[k][col] << ' ';
        }
        output << std::endl;
    }
}

unsigned int CircosLogger::toIndex(unsigned int key) {
    return bitCount(key, dimension-1);
}

std::string CircosLogger::linkString(int linkID, int chrID, int start, int end) {
    std::stringstream s;
    s << "link";
    s << linkID;
    s << " chr";
    s << chrID;
    s << " ";
    s << start;
    s << " ";
    s << end;
    s << "\n";
    return s.str();
}

std::string CircosLogger::chrString(int chrID, int start, int end) {
    std::stringstream output;
    output << "chr - ";
    output << "chr";
    output << chrID;
    output << " ";
    output << start;
    output << " ";
    output << end;
    output << "\n";
    return output.str();
}
//
//  Circos.h
//  jdemon
//
//  Created by Mark Larus on 2/27/13.
//  Copyright (c) 2013 Kenyon College. All rights reserved.
//

#ifndef __jdemon__Circos__
#define __jdemon__Circos__

class System;
#include <ostream>

class CircosLogger {
    unsigned int **links;
    const int dimension;
    unsigned int toIndex(unsigned int);
    std::string chrString(int,int,int);
    std::string linkString(int,int,int,int);
public:
    CircosLogger(int tapeSize);
    void logSystem(System *system);
    void writeLog(std::ostream&);
};


#endif /* defined(__jdemon__Circos__) */

//
//  clangomp.h
//  thermaleraser
//
//  Created by Mark Larus on 1/25/13.
//  Copyright (c) 2013 Kenyon College. All rights reserved.
//

#ifndef __clang__
    #include <omp.h>
#else
//Declare any used openmp items for clang.
#define omp_get_num_threads() 1

#endif

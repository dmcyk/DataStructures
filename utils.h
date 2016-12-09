//
//  utils.h
//  LexialAnalyzer
//
//  Created by Damian Malarczyk on 15.11.2016.
//  Copyright © 2016 Damian Malarczyk. All rights reserved.
//

#ifndef utils_h
#define utils_h

#include <stdio.h>
typedef enum { false, true } bool;

typedef void (*free_function)(void *);
typedef void (*copy_function)(void* src,void* dest);

#endif /* utils_h */

//
// Created by mereckaj on 21/01/16.
//

#ifndef ARCHLAB3_MISC_HPP
#define ARCHLAB3_MISC_HPP

#include "helper.h"

typedef struct Result {
    UINT64 nops = 0;
    UINT64 nodesCreated = 0;
    UINT64 nodesLeftInQueue = 0;
    UINT64 mallocedNodes = 0;
    UINT64 adds = 0;
    UINT64 removes = 0;
    UINT64 runTime = 0;
    UINT64 nodesReaddedToQueue = 0;
    UINT64 threadID = -1;
    UINT64 failedAdds = 0;
    UINT64 failedRemoves = 0;
} Result;

#endif //ARCHLAB3_MISC_HPP

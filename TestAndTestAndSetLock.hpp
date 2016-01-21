//
// Created by mereckaj on 12/3/15.
//

#ifndef ARCHLAB2_TESTANDTESTANDSETLOCK_HPP
#define ARCHLAB2_TESTANDTESTANDSETLOCK_HPP

#include "helper.h"

class TestAndTestAndSetLock{
public:
    void acquirePessimistic();

    void acquireOptimistic();

    void acquireHLE();

    void releaseHLE();

    void release();

    void init();

    volatile bool getValue();

private:
    volatile UINT64 cnt;
    volatile bool lock;

};
#endif //ARCHLAB2_TESTANDTESTANDSETLOCK_HPP

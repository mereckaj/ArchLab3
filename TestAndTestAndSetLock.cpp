//
// Created by mereckaj on 12/3/15.
//

#include "TestAndTestAndSetLock.hpp"
#include "helper.h"

void TestAndTestAndSetLock::acquirePessimistic() {
    do{
        while(lock){
            _mm_pause();
        }
    }while(InterlockedCompareExchange(&lock,true,false));
}
void TestAndTestAndSetLock::acquireOptimistic() {
    while(InterlockedCompareExchange(&lock,true,false)){
        while(lock){
            _mm_pause();
        }
    }
}

void TestAndTestAndSetLock::release() {
    lock = false;
}

void TestAndTestAndSetLock::init() {
    lock = false;
}
//
// Created by mereckaj on 12/3/15.
//

#include "TestAndTestAndSetLock.hpp"

using namespace std;
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
    cnt = 0;
    lock = false;
}

volatile bool TestAndTestAndSetLock::getValue() {
    return lock;
}

void TestAndTestAndSetLock::acquireHLE() {
    do{
        while(lock){
            _mm_pause();
        }
    }while(_InterlockedExchange_HLEAcquire(&lock,true));
}

void TestAndTestAndSetLock::releaseHLE() {
    _Store_HLERelease(&lock,false);
}

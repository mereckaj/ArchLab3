//
// Created by mereckaj on 12/3/15.
//

#include <iostream>
#include <assert.h>
#include "HardwareLockElision.hpp"
#include "helper.h"

void HardwareLockElision::acquire() {
    while(_InterlockedExchange_HLEAcquire(&lock,true)){
        do{
            _mm_pause();
        }while(lock==false);
    }
    c++;
    assert(c==1);
}

void HardwareLockElision::release() {
    c--;
    _Store_HLERelease(&lock,false);

}

void HardwareLockElision::init() {
    lock = false;
}

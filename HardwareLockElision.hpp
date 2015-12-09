//
// Created by mereckaj on 12/9/15.
//

#ifndef ARCHLAB3_HARDWARELOCKELISION_HPP
#define ARCHLAB3_HARDWARELOCKELISION_HPP
class HardwareLockElision{
public:
    void acquirePessimistic();

    void acquireOptimistic();

    void release();

    void init();
private:
    volatile bool lock;
};

#endif //ARCHLAB3_HARDWARELOCKELISION_HPP

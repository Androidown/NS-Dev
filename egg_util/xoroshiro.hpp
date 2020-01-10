#ifndef XOROSHIRO_H
#define XOROSHIRO_H

#include <iostream>
#include <random>
#include "lib/const.hpp"

class XoroShiro
{
private:
    std::random_device engine;

public:
    u64 seed[2];

    XoroShiro(u64 seed);
    u64 next();
    u64 nextInt(u64 max_int);
    u64 nextU32();
    u64 nextIntP2(u64 int_p2);
    inline void nextFrame();
};

inline u64 rotl(u64 x, int k)
{
    return (x << k) | (x >> (64 - k));
}

inline void XoroShiro::nextFrame()
{   
    seed[0] = engine();
    seed[1] = BASE_SEED;
}

#endif

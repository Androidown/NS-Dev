#ifndef XOROSHIRO_H
#define XOROSHIRO_H

#include <iostream>
#include "lib/const.hpp"

class XoroShiro
{
public:
    u64 seed[2];
    u64 next_seed;

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
    seed[0] = next_seed;
    seed[1] = BASE_SEED;
    next_seed += BASE_SEED;
}

#endif
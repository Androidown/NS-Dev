#include "xoroshiro.hpp"


u64 _nextP2(u64 x)
{
    x -= 1;

    for(int i=0; i<6; i++)
    {
        x |= x >> (1 << i);
    }

    return x;
}


XoroShiro::XoroShiro(u64 seed, u64 base_seed)
    :seed{seed, BASE_SEED}, base_seed(base_seed)
{
    next_seed = seed + base_seed;
}

XoroShiro::XoroShiro(u64 seed)
    :seed{seed, BASE_SEED}, base_seed(BASE_SEED)
{
    next_seed = seed + base_seed;
}

u64 XoroShiro::next()
{
    u64 s0 = seed[0];
    u64 s1 = seed[1];
    u64 rslt = s0 + s1;

    s1 ^= s0;
    seed[0] = rotl(s0, 24) ^ s1 ^ (s1 << 16);
    seed[1] = rotl(s1, 37);

    return rslt;
}

u64 XoroShiro::nextInt(u64 max_int)
{
    u64 mask = _nextP2(max_int);
    u64 rslt;

    do{
        rslt = next() & mask;
    }while(rslt >= max_int);

    return rslt;
}

u64 XoroShiro::nextU32()
{
    u64 rslt;

    do{
        rslt = next() & U32;
    }while(rslt >= U32);

    return rslt;
}

u64 XoroShiro::nextIntP2(u64 int_p2)
{
    // if the max_int is power of 2, then this method 
    // should be used with regard of efficiency.
    return (int_p2 - 1) & next();
}

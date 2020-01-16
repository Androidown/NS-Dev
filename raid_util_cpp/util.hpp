#ifndef UTIL_H
#define UTIL_H

#include <iostream>
#include <unordered_map>
#define R_SUCCEED(rc) ((rc)==1)?true:false
#define R_FAILED(rc) !R_SUCCEED(rc)

using STOI_MAP = std::unordered_map<std::string, int>;
using Result = int;

class BitMask
{
private:
    const int _max_mask;
    bool _added = false;
    const STOI_MAP &_bin_map;

public:
    int mask;

    BitMask(int mask, const STOI_MAP &bin_map);
    Result add(int key);
    Result add(std::string key);
};

#endif  // UTIL_H

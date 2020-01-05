#include <cassert>
#include "util.hpp"

BitMask::BitMask(int mask, STOI_MAP &bin_map)
{
    this->mask = mask;
    this->bin_map = bin_map;
}

BitMask& BitMask::add(std::string key)
{

    auto rslt = bin_map.find(key);
    assert(rslt != bin_map.end());
    int val = rslt->second;

    if (!added) 
    {
        this->mask = val;
        added = true;
    }
    else
    {
        this->mask |= val;
    }
    
    return *this;
}


BitMask& BitMask::add(int key)
{

    if (!added) 
    {
        this->mask = key;
        added = true;
    }
    else
    {
        this->mask |= key;
    }
    
    return *this;
}

BitMask::~BitMask()
{
    delete &bin_map;
}
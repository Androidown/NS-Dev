#include <cassert>
#include "util.hpp"

BitMask::BitMask(int mask, const STOI_MAP &bin_map)
    : _bin_map(bin_map), mask(mask), _max_mask(mask)
{
}

Result BitMask::add(int val)
{
    if (val > _max_mask || val <= 0)
    {
        std::cout << "given value: " << val << " is not in range (0, " 
                  << _max_mask << "]."  << std::endl;
        return 0;
    }
    else
    {
        if (!_added)
        {
            this->mask = val;
            _added = true;
        }
        else
        {
            this->mask |= val;
        }
    }
    return 1;
}

Result BitMask::add(std::string key)
{
    auto rslt = _bin_map.find(key);
    if (rslt == _bin_map.end())
    {
        std::cout << "key: \"" + key + "\" does not have a corresponding int value."
                  << std::endl;
        return 0;
    }
    else
    {
        int val = rslt->second;
        return add(val);
    }
}

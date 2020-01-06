#include <iostream>
#include <unordered_map>

using STOI_MAP = std::unordered_map<std::string, int>;

class BitMask
{
    private:
        bool added = false;
        STOI_MAP& bin_map;

    public:
        int mask;


    BitMask(int mask, STOI_MAP& bin_map);
    BitMask& add(int key);
    BitMask& add(std::string key);
    ~BitMask();
    
};

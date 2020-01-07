#include <iostream>
#include <memory>
#include "util.hpp"

using SP_BTMK = std::unique_ptr<BitMask>;

class PMTemplate
{
private:
    STOI_MAP *_shiny_type_map = new STOI_MAP({{"null", 1}, {"star", 2}, {"square", 4}});
    STOI_MAP *_ability_map = new STOI_MAP({{"H", 4}});
    STOI_MAP *_gender_map = new STOI_MAP({{"male", 1}, {"female", 2}, {"genderless", 4}});
    STOI_MAP *_nature_map = new STOI_MAP();
    BitMask _bm_shiny_type;
    BitMask _bm_ability;
    BitMask _bm_gender;
    BitMask _bm_nature;

public:
    int IVs_min[6] = {0};
    int IVs_max[6] = {31, 31, 31, 31, 31, 31};
    int shiny_type;
    int nature;
    int ability;
    int gender;

    PMTemplate();
    void numerize();
    void setIVsMax(int index, int iv);
    void setIVsMax(int *ivs);
    void setIVsMin(int index, int iv);
    void setIVsMin(int *ivs);
    void setIVsMinAll(int iv);
    template <typename stroi>
    inline PMTemplate &add_shiny_type(stroi key);
    template <typename stroi>
    inline PMTemplate &add_nature(stroi key);
    template <typename stroi>
    inline PMTemplate &add_ability(stroi key);
    template <typename stroi>
    inline PMTemplate &add_gender(stroi key);
    ~PMTemplate();
};

template <typename stroi>
inline PMTemplate &PMTemplate::add_shiny_type(stroi key)
{
    _bm_shiny_type.add(key);
    return *this;
}

template <typename stroi>
inline PMTemplate &PMTemplate::add_nature(stroi key)
{
    _bm_nature.add(key);
    return *this;
}

template <typename stroi>
inline PMTemplate &PMTemplate::add_ability(stroi key)
{
    _bm_ability.add(key);
    return *this;
}

template <typename stroi>
inline PMTemplate &PMTemplate::add_gender(stroi key)
{
    _bm_gender.add(key);
    return *this;
}

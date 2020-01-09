#include <vector>
#include <math.h>
#include "pm_template.hpp"
#include "lib/const.hpp"

PMTemplate::PMTemplate()
    :_bm_shiny_type(0b111, *_shiny_type_map), _bm_ability(0b111, *_ability_map),
     _bm_gender(0b111, *_gender_map), _bm_nature(0x1ffffff, *_nature_map)
{

    for (unsigned int i = 0; i < NATURES.size(); i++)
    {
        (*_nature_map)[NATURES[i]] = 1 << i;
    }
}

void PMTemplate::numerize()
{
    shiny_type = _bm_shiny_type.mask;
    nature = _bm_nature.mask;
    ability = _bm_ability.mask;
    gender = _bm_gender.mask;
}

void PMTemplate::setIVsMin(int index, int iv)
{
    index = std::min(std::max(index, 0), 5);
    int val = std::min(std::max(iv, 0), 31);

    val = std::min(IVs_max[index], val);
    IVs_min[index] = val;
}

void PMTemplate::setIVsMin(int *ivs)
{
    int val;
    for (int i = 0; i < 6; i++)
    {
        val = std::min(std::max(ivs[i], 0), 31);
        val = std::min(IVs_max[i], val);

        IVs_min[i] = val;
    }
}

void PMTemplate::setIVsMinAll(int iv)
{
    int val = std::min(std::max(iv, 0), 31);

    for (int i = 0; i < 6; i++)
    {
        val = std::min(IVs_max[i], val);
        IVs_min[i] = val;
    }
}

void PMTemplate::setIVsMax(int index, int iv)
{
    index = std::min(std::max(index, 0), 5);
    int val = std::min(std::max(val, 0), 31);

    val = std::max(IVs_min[index], val);
    IVs_max[index] = val;
}

void PMTemplate::setIVsMax(int *ivs)
{
    int val;
    for (int i = 0; i < 6; i++)
    {
        val = std::min(std::max(ivs[i], 0), 31);
        val = std::max(IVs_min[i], val);

        IVs_max[i] = val;
    }
}

/*** alternative solution
template PMTemplate& PMTemplate::add_shiny_type<int>(int key);
template PMTemplate& PMTemplate::add_nature<int>(int key);
template PMTemplate& PMTemplate::add_ability<int>(int key);
template PMTemplate& PMTemplate::add_gender<int>(int key);

template PMTemplate& PMTemplate::add_shiny_type<const char*>(const char* key);
template PMTemplate& PMTemplate::add_nature<const char*>(const char* key);
template PMTemplate& PMTemplate::add_ability<const char*>(const char* key);
template PMTemplate& PMTemplate::add_gender<const char*>(const char* key);
***/

PMTemplate::~PMTemplate()
{
    delete _nature_map;
    delete _ability_map;
    delete _shiny_type_map;
    delete _gender_map;
}

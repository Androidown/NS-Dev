#include <vector>
#include <math.h>
#include "pm_template.hpp"


PMTemplate::PMTemplate()
{
    std::vector <std::string> nature_list = {
        "Hardy", "Lonely", "Brave", "Adamant", "Naughty", "Bold", "Docile", "Relaxed",
        "Impish", "Lax", "Timid", "Hasty", "Serious", "Jolly", "Naive", "Modest",
        "Mild", "Quiet", "Bashful", "Rash", "Calm", "Gentle", "Sassy", "Careful", "Quirky"
    };

    STOI_MAP& nature_map = *new STOI_MAP();
    for(unsigned int i=0; i<nature_list.size(); i++)
    {
        nature_map[nature_list[i]] = 1 << i;
    }

    STOI_MAP& shiny_type_map = *new STOI_MAP({{"null", 1}, {"star", 2}, {"square", 4}});
    STOI_MAP& ability_map = *new STOI_MAP({{"H", 4}});
    STOI_MAP& gender_map = *new STOI_MAP({{"male", 1}, {"female", 2}, {"genderless", 4}});

    stoimap_col[0] = &nature_map;
    stoimap_col[1] = &shiny_type_map;
    stoimap_col[2] = &ability_map;
    stoimap_col[3] = &gender_map;

    bm_shiny_type = SP_BTMK(new BitMask(0b111, shiny_type_map));
    bm_nature = SP_BTMK(new BitMask(0x1ffffff, nature_map));
    bm_ability = SP_BTMK(new BitMask(0b111, ability_map));
    bm_gender = SP_BTMK(new BitMask(0b111, gender_map));

}


void PMTemplate::numerize()
{
    shiny_type = bm_shiny_type->mask;
    nature = bm_nature->mask;
    ability = bm_ability->mask;
    gender = bm_gender->mask;
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
    for(int i=0; i<6; i++)
    {
        val = std::min(std::max(ivs[i], 0), 31);
        val = std::min(IVs_max[i], val);

        IVs_min[i] = val;
    }

}

void PMTemplate::setIVsMinAll(int iv)
{
    int val = std::min(std::max(iv, 0), 31);

    for(int i=0; i<6; i++)
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
    for(int i=0; i<6; i++)
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
    for(int i=0; i<4; i++) delete stoimap_col[i];
    std::cout << "PMTemplate deleted." << std::endl;
}

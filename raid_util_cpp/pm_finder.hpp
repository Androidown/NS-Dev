#include <iostream>
#include "xoroshiro.hpp"
#include "pm_template.hpp"

class PMFinder
{
private:
    bool _checkShiny();
    bool _checkIVs();
    bool _checkAbility();
    bool _checkGender();
    bool _checkNature();
    int _IVs_min[6];
    int _IVs_max[6];
    int _shiny_type;
    int _nature;
    int _ability;
    int _gender;

public:
    XoroShiro xoro;
    int iv_cnt;
    bool allow_hidden;
    bool random_gender;
    u64 gender_ratio;
    bool foundPM();

    PMFinder(u64 seed, int iv_count, bool allow_hidden, bool random_gender, PMTemplate &pm_tmpl, u64 gender_ratio);
};

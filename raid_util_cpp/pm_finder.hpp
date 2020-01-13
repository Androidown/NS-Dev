#ifndef PM_FINDER_H
#define PM_FINDER_H

#include <iostream>
#include "xoroshiro.hpp"
#include "pm_template.hpp"
#include "lib/const.hpp"

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
    PMInfo& pm_info;
    bool foundPM();

    PMFinder(u64 seed, PMTemplate &pm_tmpl, PMInfo& pm_info);
    PMFinder(u64 seed, u64 base_seed, PMTemplate &pm_tmpl, PMInfo& pm_info);
};

#endif

#include "pm_finder.hpp"

PMFinder::PMFinder(u64 seed, int iv_count, bool allow_hidden, bool random_gender, PMTemplate& pm_tmpl, u64 gender_ratio)
{
    xoro = UP_XORO(new XoroShiro(seed));

    pm_tmpl.numerize();
    for(int i=0; i<6; i++)
    {
        _IVs_min[i] = pm_tmpl.IVs_min[i];
        _IVs_max[i] = pm_tmpl.IVs_max[i];
    }

    _shiny_type = pm_tmpl.shiny_type;
    _nature = pm_tmpl.nature;
    _ability = pm_tmpl.ability;
    _gender = pm_tmpl.gender;

    this->iv_cnt = iv_count;
    this->allow_hidden = allow_hidden;
    this->random_gender = random_gender;
    this->gender_ratio = gender_ratio;
}

bool PMFinder::_checkShiny()
{
    bool rtn_code;
    u64 otid = xoro->nextU32();
    u64 pid = xoro->nextU32();
    u64 otsv = (otid >> 16) ^ (otid & 0xffff);
    u64 psv = (pid >> 16) ^ (pid & 0xffff);

    if (otsv == psv)
        rtn_code = _shiny_type & 4;
    else if (otsv >> 4 == psv >> 4)
        rtn_code = _shiny_type & 2;
    else
        rtn_code = _shiny_type & 1;

    return rtn_code;
}


bool PMFinder::_checkIVs()
{
    int ivs[6] = {-1, -1, -1, -1, -1, -1};
    int i = 0;
    int stat, iv;
    bool rtn_code = true;

    while (i < iv_cnt)
    {
        stat = xoro->nextInt(6);
        if (_IVs_max[stat] != 31)
        {
            rtn_code = false;
            break;
        }
        else if (ivs[stat] == -1)
        {
            ivs[stat] = 31;
            i ++;
        }
    }

    if(rtn_code)
    {
        for(i=0; i<6; i++)
        {
            if (ivs[i] == -1)
            {
                iv = xoro->nextInt(32);
                if (_IVs_min[i] > iv || iv > _IVs_max[i])
                    {
                        rtn_code = false;
                        break;
                    }
            }
        }
    }

    return rtn_code;
}

bool PMFinder::_checkAbility()
{
    int ability;

    if (allow_hidden)
        ability = 1 << xoro->nextInt(3);
    else
        ability = 1 << xoro->nextInt(2);

    return ability & _ability;
}


bool PMFinder::_checkGender()
{

    if (random_gender)
    {
        int gender = 1 << (xoro->nextInt(252) + 1  < gender_ratio);
        return gender & _gender;
    }
    else
        return true;
}


bool PMFinder::_checkNature()
{
    int nature = 1 << xoro->nextInt(25);
    return nature & _nature ;
}


bool PMFinder::foundPM()
{
    xoro->nextU32();
    if (_checkShiny() && _checkIVs() && _checkAbility() && _checkGender() && _checkNature())
        return true;

    xoro->nextFrame();
    return false;
}


PMFinder::~PMFinder()
{
    std::cout << "PMFinder deleted." << std::endl;
}

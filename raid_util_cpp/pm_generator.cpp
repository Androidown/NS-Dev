#include "pm_generator.hpp"
#include <iomanip>

PMGenerator::PMGenerator(u64 seed, PMInfo& pm_info, u64 ttid, u64 tsid)
    :seed(seed), pm_info(pm_info), real_tsv(ttid ^ tsid)
{
    xoro = new XoroShiro(seed);
}


PMGenerator::PMGenerator(PMInfo& pm_info, u64 ttid, u64 tsid)
    :pm_info(pm_info), real_tsv(ttid ^ tsid)
{
}

void PMGenerator::_setShiny()
{
    ec = xoro->nextU32();
    
    u64 otid = xoro->nextU32();
    pid = xoro->nextU32();
    u64 otsv = (otid >> 16) ^ (otid & 0xffff);
    u64 psv = (pid >> 16) ^ (pid & 0xffff);

    if (otsv == psv)
    {
        shiny_type = "squa";
        u64 high = (pid & 0xffff) ^ real_tsv ^ 0;
        pid = (high << 16) | (pid & 0xffff);
    }
    else if (otsv >> 4 == psv >> 4)
    {
        shiny_type = "star";
        u64 high = (pid & 0xffff) ^ real_tsv ^ 1;
        pid = (high << 16) | (pid & 0xffff);
    }
    else
    {
        shiny_type = "null";
        if ((psv >> 4) == real_tsv)
            pid ^= 0x10000000;
    }
}

void PMGenerator::_setIVs()
{
    ivs = {-1, -1, -1, -1, -1, -1};
    int i = 0;
    int stat;

    while (i < pm_info.iv_cnt)
    {
        stat = xoro->nextInt(6);
        if (ivs[stat] == -1)
        {
            ivs[stat] = 31;
            i++;
        }
    }

    for (i = 0; i < 6; i++)
    {
        if (ivs[i] == -1)
        {
            ivs[i] = xoro->nextIntP2(32);
        }
    }
}

void PMGenerator::_setAbility()
{
    int ability_num;

    if (pm_info.allow_hidden)
        ability_num = xoro->nextInt(3);
    else
        ability_num = xoro->nextIntP2(2);

    switch (ability_num)
    {
    case 0:
        ability = '1';
        break;
    case 1:
        ability = '2';
        break;
    case 2:
        ability = 'H';
        break;
    }
}

void PMGenerator::_setGender()
{
    if (pm_info.random_gender)
    {
        if (xoro->nextInt(252) + 1 < pm_info.gender_ratio)
            gender = 'F';
        else
            gender = 'M';
    }
    else
        gender = '-';
}

void PMGenerator::_setNature()
{
    nature = NATURES[xoro->nextInt(25)];
}

void PMGenerator::setSeed(u64 seed)
{
    this->seed = seed;

    if (this->xoro) delete this->xoro;
    this->xoro = new XoroShiro(seed);
}

void PMGenerator::display()
{
    _setShiny();
    _setIVs();
    _setAbility();
    _setGender();
    _setNature();
    std::cout << "------------------------------------------------------------------------------------"
              << std::endl; 

    std::cout << " HP|ATK|DEF|SPA|SPD|SPE|ABL|GEN|SHINY|  NATURE|       EC|      PID|             SEED"
              << std::endl; 

    for(int i=0; i<6; i++)
        std::cout << std::setw(3) << std::right << std::dec << ivs[i] << "|";

    std::cout << std::setw(3) << std::right << ability << "|"
              << std::setw(3) << std::right << gender << "|"
              << std::setw(5) << std::right << shiny_type << "|"
              << std::setw(8) << std::right << nature << "|"
              << std::setw(9) << std::hex << std::right << ec << "|"
              << std::setw(9) << std::hex << std::right << pid << "|"
              << std::setw(17) << std::hex << std::right << seed
              << std::endl;
              
    std::cout << "------------------------------------------------------------------------------------"
              << std::endl; 
}

PMGenerator::~PMGenerator()
{
    delete this->xoro;
}

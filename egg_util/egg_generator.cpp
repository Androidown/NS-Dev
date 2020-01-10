#include "egg_generator.hpp"
#include <iomanip>

EggGenerator::EggGenerator(u64 seed, ParentInfo &parent_info, EggFilter& filter, u64 ttid, u64 tsid)
    : seed(seed), parent_info(parent_info), filter(filter), real_tsv(ttid ^ tsid)
{
    xoro = new XoroShiro(seed);
}

bool EggGenerator::_setSpecies()
{
    if (parent_info.random_species)
        species = xoro->nextIntP2(2);
    else
        species = 2;

    return filter.compareSpecies(species);
}

bool EggGenerator::_setGender()
{
    if (parent_info.random_gender)
    {
        if (xoro->nextInt(252) + 1 < parent_info.gender_ratio)
            gender = 'F';
        else
            gender = 'M';
    }
    else
        gender = '-';

    return filter.compareGender(gender);
}

bool EggGenerator::_setNature()
{
    nature = NATURES[xoro->nextInt(25)];

    if (parent_info.everstone)
        nature = "fixed";

    return filter.compareNature(nature);
}

bool EggGenerator::_setAbility()
{
    int ability_num = xoro->nextInt(100);

    switch (parent_info.ability)
    {
    case 1:
        ability = (ability_num < 80) ? '1' : '2';
        break;
    case 2:
        ability = (ability_num < 80) ? '2' : '1';
        break;
    case 4:
        ability = (ability_num < 20) ? '1' : 
                  (ability_num < 40) ? '2' : 'H';
        break;
    }

    return filter.compareAbility(ability);
}

bool EggGenerator::_setIVs()
{
    ivs = {-1, -1, -1, -1, -1, -1};
    int inherit_cnt = (parent_info.destiny_knot) ? 5 : 3;
    int i = 0;
    int stat;
    bool rtn_code = true;

    while (i < inherit_cnt)
    {
        stat = xoro->nextInt(6);
        if (ivs[stat] == -1)
        {
            xoro -> nextIntP2(2);
            ivs[stat] = parent_info.ivs[stat];
            i++;
        }
    }

    for (i = 0; i < 6; i++)
    {
        int iv = xoro->nextIntP2(32);
        if (ivs[i] == -1)
        {
            rtn_code &= filter.compareIVs(i, iv);
            ivs[i] = iv;
        }
    }

    return rtn_code;
}

bool EggGenerator::_setShiny()
{
    ec = xoro->nextU32();
    int rerolls = parent_info.masuada*6 + parent_info.shiny_charm*2;
    shiny_type = "null";
    u64 psv;

    for (int i = 0; i<rerolls; i++)
    {
        pid = xoro->nextU32();
        psv = (pid >> 16) ^ (pid & 0xffff);
        if ((psv >> 4) == (real_tsv >> 4))
        {
            shiny_type = "star";
            break;
        }
    }

    if (psv == real_tsv)
        shiny_type = "quad";

    return filter.compareShiny(shiny_type);
}

void EggGenerator::findEgg()
{
    int cnt = 0;

    while(!(_setShiny() && _setIVs() && _setAbility() && _setGender() &&_setNature()))
    {
        xoro->nextFrame();
        if (++cnt % 10000000 == 0)
        {
            std::cout << "Calculated " << cnt << " frames." << std::endl;
        }
    }
    std::cout << "Seed found at " << cnt << " frames." << std::endl;
}

void EggGenerator::display()
{
    std::cout << "------------------------------------------------------------------------------------"
              << std::endl;

    std::cout << " HP|ATK|DEF|SPA|SPD|SPE|ABL|GEN|SHINY|  NATURE|       EC|      PID|             SEED"
              << std::endl;

    for (int i = 0; i < 6; i++)
        std::cout << std::setw(3) << std::right << std::dec << ivs[i] << "|";

    std::cout << std::setw(3) << std::right << ability << "|"
              << std::setw(3) << std::right << gender << "|"
              << std::setw(5) << std::right << shiny_type << "|"
              << std::setw(8) << std::right << nature << "|"
              << std::setw(9) << std::hex << std::right << ec << "|"
              << std::setw(9) << std::hex << std::right << pid << "|"
              << std::setw(17) << std::hex << std::right << xoro->seed[0]
              << std::endl;

    std::cout << "------------------------------------------------------------------------------------"
              << std::endl;
}

EggGenerator::~EggGenerator()
{
    delete this->xoro;
}

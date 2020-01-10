#ifndef EGG_GENERATOR_H
#define EGG_GENERATOR_H

#include "xoroshiro.hpp"
#include "egg_filter.hpp"

class EggGenerator
{
private:
    XoroShiro* xoro = 0;
    EggFilter& filter;
    u64 seed;
    ParentInfo& parent_info; 
    u64 real_tsv;

    u64 ec;
    u64 pid;
    u64 species;
    std::string shiny_type;
    std::vector<short> ivs;
    char ability;
    char gender;
    std::string nature;

    bool _setSpecies();
    bool _setGender();
    bool _setNature();
    bool _setAbility();
    bool _setIVs();
    bool _setShiny();

public:
    EggGenerator(u64 seed, ParentInfo& parent_info, EggFilter& filter, u64 ttid, u64 tsid);

    void findEgg();
    void display();

    ~EggGenerator();
};

#endif
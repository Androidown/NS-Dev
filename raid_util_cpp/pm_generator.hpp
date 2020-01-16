#ifndef PM_GENERATOR_H
#define PM_GENERATOR_H

#include "xoroshiro.hpp"
#include "lib/const.hpp"

class PMGenerator
{
private:
    u64 seed;
    XoroShiro* xoro = 0;
    const PMInfo& pm_info;
    const u64 real_tsv;

    u64 ec;
    u64 pid;
    std::string shiny_type;
    std::vector<short> ivs;
    char ability;
    char gender;
    std::string nature;
    void _setShiny();
    void _setIVs();
    void _setAbility();
    void _setGender();
    void _setNature();

public:
    PMGenerator(u64 seed, const PMInfo& pm_info, u64 ttid, u64 tsid);
    PMGenerator(const PMInfo& pm_info, u64 ttid, u64 tsid);

    void display() const;
    void setSeed(u64 seed);

    ~PMGenerator();
};

#endif  // PM_GENERATOR_H

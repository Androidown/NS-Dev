#include "egg_filter.hpp"

EggFilter::EggFilter(PMTemplate& pm_tmpl)
    : pm_tmpl(pm_tmpl)
{
    pm_tmpl.numerize();
}

bool EggFilter::compareSpecies(int species)
{
    if (species == 2)
        return true;
    else
        return species == pm_tmpl.species;
}

bool EggFilter::compareGender(char gender)
{
    int gender_mask;
    switch (gender)
    {
    case 'F':
        gender_mask = 1;
        break;
    case 'M':
        gender_mask = 2;
        break;
    case '-':
        gender_mask = 0b111;
        break;
    }
    return gender_mask & pm_tmpl.gender;
}

bool EggFilter::compareNature(std::string& nature)
{
    if (nature == "fixed")
        return true;
    else
    {
        int nature_code = (*pm_tmpl.nature_map)[nature];
        return nature_code & pm_tmpl.nature;
    }
    
}

bool EggFilter::compareAbility(int ability)
{
    return ability & pm_tmpl.ability;
}

bool EggFilter::compareIVs(int stat, int iv)
{
    return iv >= pm_tmpl.IVs_min[stat] && iv <= pm_tmpl.IVs_max[stat];
}

bool EggFilter::compareShiny(std::string& shiny_type)
{
    int shiny_code;
    if (shiny_type == "null")
        shiny_code = 1;
    else if (shiny_type == "star")
        shiny_code = 2;
    else if (shiny_type == "quad")
        shiny_code = 4;
    
    return shiny_code & pm_tmpl.shiny_type;
}

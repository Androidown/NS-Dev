#ifndef EGG_FILTER_H
#define EGG_FILTER_H

#include "pm_template.hpp"
#include "lib/const.hpp"

class EggFilter
{
private:
    PMTemplate& pm_tmpl;

public:
    bool compareSpecies(int species);
    bool compareGender(char gender);
    bool compareNature(std::string& nature);
    bool compareAbility(int ability);
    bool compareIVs(int stat, int iv);
    bool compareShiny(std::string& shiny_type);
    
    EggFilter(PMTemplate& pm_tmpl);

};

#endif

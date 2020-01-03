#include <iostream>
#include "util.hpp"


class PMTemplate
{
    private:
        BitMask* bm_shiny_type;
        BitMask* bm_nature;
        BitMask* bm_ability;
        BitMask* bm_gender;
    public:
        int IVs_min[6] = {0};
        int IVs_max[6] = {31, 31, 31, 31, 31, 31};
        int shiny_type;
        int nature;
        int ability;
        int gender;

    PMTemplate();
    void numerize();
    void setIVsMax(int index, int iv);
    void setIVsMax(int *ivs);
    void setIVsMin(int index, int iv);
    void setIVsMin(int *ivs);
    void setIVsMinAll(int iv);
    template <typename stroi> inline PMTemplate& add_shiny_type(stroi key);
    template <typename stroi> inline PMTemplate& add_nature(stroi key);
    template <typename stroi> inline PMTemplate& add_ability(stroi key);
    template <typename stroi> inline PMTemplate& add_gender(stroi key);
    ~PMTemplate();

};


template <typename stroi>
inline PMTemplate& PMTemplate::add_shiny_type(stroi key)
{
    bm_shiny_type->add(key);
    return *this;
}

template <typename stroi>
inline PMTemplate& PMTemplate::add_nature(stroi key)
{
    bm_nature->add(key);
    return *this;
}

template <typename stroi>
inline PMTemplate& PMTemplate::add_ability(stroi key)
{
    bm_ability->add(key);
    return *this;
}

template <typename stroi>
inline PMTemplate& PMTemplate::add_gender(stroi key)
{
    bm_gender->add(key);
    return *this;
}

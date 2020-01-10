#include "egg_generator.hpp"
#include "egg_filter.hpp"
#include "lib/json.hpp"
#include <fstream>

using json = nlohmann::json;

int main()
{
    json jsn;
    Result main_rc;

    std::ifstream in("conf/config.json");
    in >> jsn;
    json filter = jsn["filter"];
    json parent = jsn["parent"];

    PMTemplate pm_tmpl;

    // load seed
    if (jsn["seed"].is_null())
    {
        std::cout << "seed is not presented." << '\n';
        return 0;
    }
    u64 seed = std::stoull(jsn["seed"].get<std::string>(), nullptr, 16);
    std::cout << "seed loaded." << '\n';


    // load parameters in filter
    auto keyAddList = [&filter] (std::string key, auto&& func) -> Result
    {
        Result rc = 1;
        std::cout << "loading " + key + "..." << '\n';
        if (!filter[key].is_null())
        {
            for (auto &itor : filter[key].items())
            {
                if (itor.value().is_string())
                {
                    std::string val = itor.value();
                    rc = std::forward<decltype(func)>(func)(val);
                }
                else if (itor.value().is_number_unsigned())
                {
                    int val = itor.value();
                    rc = std::forward<decltype(func)>(func)(val);
                }
                if (R_FAILED(rc)) break;
            }
            if (R_SUCCEED(rc))
                std::cout << key + " loaded." << '\n';
        }
        else
        {
            std::cout << key + " unspecified, use default config." << '\n';
        }
        
        return rc;
    };

    std::cout << "Process start." << '\n';
    main_rc = keyAddList("shiny_type", [&pm_tmpl] (auto val) {return pm_tmpl.add_shiny_type(val);}) &&
              keyAddList("gender", [&pm_tmpl] (auto val) {return pm_tmpl.add_gender(val);}) &&
              keyAddList("ability", [&pm_tmpl] (auto val) {return pm_tmpl.add_ability(val);}) &&
              keyAddList("nature", [&pm_tmpl] (auto val) {return pm_tmpl.add_nature(val);});

    if (R_FAILED(main_rc)) return 0;

    if (!filter["IVs_max"].is_null())
    {
        std::vector<int> iv_max = filter["IVs_max"].get<std::vector<int>>();
        pm_tmpl.setIVsMax(iv_max.data());
    }
    std::cout << "Max IVs loaded." << '\n';

    if (!filter["IVs_min"].is_null())
    {
        std::vector<int> iv_min = filter["IVs_min"].get<std::vector<int>>();
        pm_tmpl.setIVsMin(iv_min.data());
    }
    std::cout << "Min IVs loaded." << '\n';

    // load parameters in parent
    ParentInfo parent_info;

    parent_info.ivs = parent["ivs"].get<std::vector<int>>();
    std::cout << "ivs loaded." << '\n';

    parent_info.destiny_knot = parent["destiny_knot"].get<bool>();
    std::cout << "destiny_knot loaded." << '\n';

    parent_info.everstone = parent["everstone"].get<bool>();
    std::cout << "everstone loaded." << '\n';

    parent_info.ability = parent["ability"].get<int>();
    std::cout << "ability loaded." << '\n';

    parent_info.random_gender = parent["random_gender"].get<bool>();
    std::cout << "random_gender loaded." << '\n';

    parent_info.gender_ratio = parent["gender_ratio"].get<u64>();
    std::cout << "gender_ratio loaded." << '\n';

    parent_info.random_species = parent["random_species"].get<bool>();
    std::cout << "random_species loaded." << '\n';

    parent_info.masuada = parent["masuada"].get<bool>();
    std::cout << "masuada loaded." << '\n';

    parent_info.shiny_charm = parent["shiny_charm"].get<bool>();
    std::cout << "shiny_charm loaded." << '\n';

    EggFilter eggfilter(pm_tmpl);
    EggGenerator egg(seed, parent_info, eggfilter, 1234, 2345);

    char key_in;
    unsigned long long cnt = 0;

loop:
    
    egg.findEgg();
    egg.display();

    std::cout << std::dec << "Press y|Y to continue." << std::endl;
    key_in = std::cin.get();
    if (key_in == 'y' || key_in == 'Y')
    {
        std::cin.get();
        goto loop;
    }

    return 1;
}

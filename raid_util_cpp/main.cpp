#include "pm_finder.hpp"
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

    PMTemplate pm_tmpl;

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

    if (jsn["seed"].is_null())
    {
        throw "seed is not presented.\n";
    }

    u64 seed = std::stoull(jsn["seed"].get<std::string>(), nullptr, 16);
    std::cout << "seed loaded." << '\n';

    bool allow_hidden = jsn["allow_hidden"].get<bool>();
    std::cout << "allow_hidden loaded." << '\n';

    bool random_gender = jsn["random_gender"].get<bool>();
    std::cout << "random_gender loaded." << '\n';

    int iv_cnt = jsn["iv_count"].get<int>();
    std::cout << "iv_count loaded." << '\n';

    int gender_ratio = jsn["gender_ratio"].get<int>();
    std::cout << "gender_ratio loaded." << '\n';

    PMFinder pmf(seed, iv_cnt, allow_hidden, random_gender, pm_tmpl, gender_ratio);

    char key_in;
    unsigned long long cnt = 0;

loop:
    while (!pmf.foundPM())
    {
        if (++cnt % 100000000 == 0)
        {
            std::cout << "Calculated " << cnt << " frames." << std::endl;
        }
    }

    std::cout << "Seed found at " << cnt << " frames." << std::endl;
    std::cout << std::hex << "seed 3frames back: " 
              << pmf.xoro.next_seed - BASE_SEED * 4
              << std::endl;

    std::cout << std::hex << "seed: " 
              << pmf.xoro.next_seed - BASE_SEED 
              << std::endl;

    std::cout << std::dec << "Press y|Y to continue." << std::endl;
    key_in = std::cin.get();
    if (key_in == 'y' || key_in == 'Y')
    {
        std::cin.get();
        goto loop;
    }

    return 1;
}

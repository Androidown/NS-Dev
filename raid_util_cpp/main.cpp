#include "pm_finder.hpp"
#include "lib/json.hpp"
#include <fstream>

using json = nlohmann::json;


int main()
{
    json jsn;
    json::iterator it;
    
    std::ifstream in("conf/config.json");
    in >> jsn;
    json filter = jsn["filter"];

    PMTemplate &pm_tmpl = *new PMTemplate();
    std::cout << "Process start." << '\n';

    if (!filter["shiny_type"].is_null())
    {
        for (it=filter["shiny_type"].begin(); it != filter["shiny_type"].end(); ++it)
        {
            if (it.value().is_string())
            {
                std::string val = it.value();
                pm_tmpl.add_shiny_type(val);
            }
            else if (it.value().is_number_unsigned())
            {
                int val = it.value();
                pm_tmpl.add_shiny_type(val);
            }
            
        }
    }
    std::cout << "shiny_type loaded." << '\n';

    if (!filter["gender"].is_null())
    {
        for (it=filter["gender"].begin(); it != filter["gender"].end(); ++it)
        {
            if (it.value().is_string())
            {
                std::string val = it.value();
                pm_tmpl.add_gender(val);
            }
            else if (it.value().is_number_unsigned())
            {
                int val = it.value();
                pm_tmpl.add_gender(val);
            }
            
        }
    }
    std::cout << "gender loaded." << '\n';

    if (!filter["ability"].is_null())
    {
        for (it=filter["ability"].begin(); it != filter["ability"].end(); ++it)
        {
            if (it.value().is_string())
            {
                std::string val = it.value();
                pm_tmpl.add_ability(val);
            }
            else if (it.value().is_number_unsigned())
            {
                int val = it.value();
                pm_tmpl.add_ability(val);
            }
            
        }
    }
    std::cout << "ability loaded." << '\n';

    if (!filter["IVs_max"].is_null())
    {
        std::vector<int> iv_max = filter["IVs_max"].get<std::vector<int>>();
        pm_tmpl.setIVsMin(iv_max.data());
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

    u64 seed = std::stoul(jsn["seed"].get<std::string>(), nullptr, 16);
    std::cout << "seed loaded." << '\n';

    bool allow_hidden = jsn["allow_hidden"].get<bool>();
    std::cout << "allow_hidden loaded." << '\n';

    bool random_gender = jsn["random_gender"].get<bool>();
    std::cout << "random_gender loaded." << '\n';

    int iv_cnt = jsn["iv_count"].get<int>();
    std::cout << "iv_count loaded." << '\n';

    int gender_ratio = jsn["gender_ratio"].get<int>();
    std::cout << "gender_ratio loaded." << '\n';

    PMFinder *pmf = new PMFinder(seed, iv_cnt, allow_hidden, random_gender, pm_tmpl, gender_ratio);


    int cnt = 0;
    while(!pmf->foundPM())
    {
    	if(++cnt % 50000000 == 0)
        {
            std::cout << "Calculated " << cnt << " frames." << std::endl;
        }

    }

    std::cout << "Seed found at " << cnt << " frames." << std::endl;
    std::cout << std::hex << 
    pmf->xoro->next_seed - BASE_SEED - BASE_SEED - BASE_SEED - BASE_SEED 
    << std::endl;
    
    delete pmf;

}

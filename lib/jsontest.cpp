// #include "headers/crbn_json.hpp"
#include "crbn_json.cpp"
#include "crbn_logging.cpp"
#include <string>
#include <iostream>

/*
g++ jsontest.cpp -I .
*/

void fn()
{
    // json j = crbn::jsn::jsonConfigRead(crbn::jsn::configFileName);

    // std::string value = j["application_name"].get<std::string>();

    // std::cout << value << std::endl;
    // crbn::jsn::generateDatFile("1234aabc");

    crbn::jsn::Json_Helper cfg;
    cfg.init("config.json", crbn::jsn::jsonLiterals::config);

    crbn::jsn::Json_Helper j;
    j.init("test.json", crbn::jsn::jsonLiterals::data);

    // j.j["dates_out"] = {};
    std::vector<std::string> v;

    // int i = 0;

    // if (j.j["consumption_times"].size() > 0)
    // {
    //     v = j.j["consumption_times"];
    // }
    // else
    // {
    // }

    j.update();
    // j.j["consumption_times"].push_back("jerry");
    j.save();

    // v.push_back("boingus");
    // v.push_back("bingle");
    // j.j["consumption_times"] = v;
    // j.save();
    j.print();
}

int main()
{
    // crbn::jsn::Json_Helper j3;
    // j3.print();
    // j3.init();
    // j3.print();


    
    crbn::jsn::Json_Helper j1("bob.json", crbn::jsn::jsonLiterals::data);
    std::cout << std::endl;
    
    
    j1.lock();
    j1.intGet("number_of");
    j1.print();
    j1.unlock();
    

    // if(j3.isLocked())
    // {
    //     std::cout << "whajt" << std::endl;
    // }

    // if (j1.setSafeModeUnsafe<crbn::jsn::Safemode>()) {}
    // {
        // std::cout << "thank god " << std::endl;
    // }


    // j1["tag_id"] = "bob";
    // std::cout << "json \"tag_id\" = " << j1["tag_id"] << std::endl;

    // j1.save();
    // j1.setSafeModeUnsafe();
    return 0;
}

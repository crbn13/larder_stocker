#include <nlohmann/json.hpp>
using json = nlohmann::json;

#include <iostream>
#include <fstream>
#include <string>

struct config
{
    int i_screen_width = 500;
    int i_screen_height = 300;
    std::string s_uiName = "application";
};

int main(int argc, char **argv)
{

    json baseCfg =
        {
            {"screen_width", 500},
            {"screen_height", 300},
            {"application_name", "dingus"},
        };

    std::ifstream file("config.json");
    json config;

    // if (file.peek() != nullptr)
    std::string str;
    std::string tempstr;
    while (std::getline(file, tempstr))
    {
        str.append(tempstr);
    }

    file.close();

    if (str.length() != 0)
    {
        config = json::parse(str);
        std::cout << std::setw(4) << config;
        std::cout << std::endl;
    }
    else
    {
        config = baseCfg;
        std::ofstream out("config.json");
        out << std::setw(4) << config;
        out.close();

        std::cout << std::setw(4) << config;
        std::cout << std::endl;
    }
}

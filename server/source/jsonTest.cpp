
#include <fstream>
#include <iostream>

#include <nlohmann/json.hpp>
using json = nlohmann::json;

int main(int argc, char **argv)
{
    json j; // declare instance

    // assign dataa
    j["1234"]["type"] = "bingbong";
    j["12345"]["data"] = 123455;
    j["12345"]["number_of"] = 12;

    //write to text file
    std::ofstream o("txt.json");
    o << std::setw(4) << j << std::endl;
    o.close();

    //read from text file and assign to new instance of json
    std::ifstream x("txt.json");
    json js = json::parse(x);
    x.close();

    //output data       // no i dont fucking know how std::setw works and quite frankly im too scared to try find out 
    std::cout << std::setw(4) << js.at("1234").at("data") << std::endl;

    return EXIT_SUCCESS;
}
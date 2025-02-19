#include "crbn_json.hpp"
#include "crbn_dataSerialiser.hpp"
int main()
{
    crbn::jsn::Json_Helper j;

    // j.strGet("bingus");
    std::cout << j.strGet(crbn::jsn::keys::server_path_to_data) << "\n"
              << j.intGet(crbn::jsn::keys::screen_height) << "\n";

    crbn::serialiser dat;

    dat.initialise((uint8_t*)"bingus" , 6, 4, 123);

    std::cout << dat.bodyAsString() << "\n";
    

    // j.json_write(crbn::jsn::keys::screen_width, 800);
    // j.save();
}
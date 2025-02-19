#include <crbn_jobManager.hpp>
// #include <crbn_simpleNetworking.hpp>
#include <queue>

#include <chrono>
void print(crbn::serialiser *dat)
{
    uint8_t *arr = dat->rawDatOut();

    for (auto i = 0; i < dat->size(); i++)
    {
        std::cout << +arr[i] << " ";
    }
    std::cout << std::endl;
}

// compile :
// g++ jobTest.cpp -pthread -I . -I asio-1.28.0/include/

int main(const int argc, const char **argv)
{

    //  crbn::jobs client;

    //    client.async_clientStart();

    // crbn::simpleNetworking serverr;

    // std::mutex temp;
    // crbn::serialiser *dat;
    // serverr.serverStart(temp, dat, true, true);

    // std::cout << "\n i wonder if that worked \n";
    std::string str = "brian";
    std::string ip = "127.0.0.1";
    if (argc == 3)
    {
        str = std::string(argv[1]);
        ip = std::string(argv[2]);
    }

    crbn::serialiser data((uint8_t *)str.c_str(), 9, crbn::op::ID_ENQUIRY, 50000);
    json jsn;
    crbn::Jobs j;
    j.async_clientStart(ip);

    auto start = std::chrono::high_resolution_clock::now();
    j.enqueOUT(&data);

    while (!j.isJsonAvailable(std::string(str.c_str())))
    {
        crbn::log(" not available yet");
    }

    jsn = j.getJsonData(std::string(str.c_str()));
    auto end = std::chrono::high_resolution_clock::now();

    std::cout << std::setw(4) << jsn;
    std::cout << std::endl;
    std::cout << " time taken = " << std::setprecision(100) <<  (end - start).count()/1000000 << " ms " << std::endl;

    return EXIT_SUCCESS;
}
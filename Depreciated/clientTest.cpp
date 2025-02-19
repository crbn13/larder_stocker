#include <simpleNetworking.hpp>
#include <iostream>
#include <string>
#if !defined(DATA_TRANSFER)
#include <dataTransfer.hpp>
#endif

void nett(simpleNetworking net, networkingData dat)
{
    net.sendDataNewConnection(dat.rawDatOut(), dat.size());
}

int main()
{
    simpleNetworking net;
    char *ip = (char *)"192.168.1.110";

    net.clientStart(ip, int(5473));

    char *arr = (char *)"binglebongle";

    networkingData dat((uint8_t *)arr, 4, 13);

    std::cout << dat.size() << " | " << dat.bodySize() << std::endl;

    std::cout << "\n   body : \n";
    uint8_t *array = new uint8_t[dat.size()];
    array = dat.rawDatOut();
    for (int i = 0; i < dat.size(); i++)
    {
        std::cout << +array[i] << " ";
    }
    std::cout << std::endl;

    networkingData dat2(dat.rawDatOut());

    // std::cout << dat2.bodySize() << std::endl;
    // std::cout << std::endl;
    /*
        std::thread thr(&simpleNetworking::sendDataNewConnection, &net, dat.rawDatOut(), dat.size());
        while (!thr.joinable())
        {
            thr.join();
        }
        */

    std::cout << dat2.size() << std::endl;

    if (!net.sendDataNewConnection(array, dat2.size()))
        std::cout << " failiure\n" ;

    std::cout << "\n goodbye world " << std::endl;
    return 0;
}
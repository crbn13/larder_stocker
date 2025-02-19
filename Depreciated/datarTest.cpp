#include <iostream>

#include <crbn_dataSerialiser.hpp>

int main()
{

    crbn::serialiser dat((uint8_t *)"dilf\0", 5, 3);
    std::cout << " dat bodysize : " << dat.bodySize() << "\n";

    char *arr = (char *)+dat.rawDatOut();
    for (int i = 0; i < dat.size(); i++)
    {
        std::cout << +arr[i] << " ";
    }
    std::cout << std::endl;

    crbn::serialiser dat2((uint8_t*)dat.rawDatOut(), true);

    std::cout << " dat2 bodysize : " << dat2.bodySize() << "\n";
    std::cout << " dat2 size : " << dat2.size() << "\n";
    //char * arr2 = (char *)+

    for (int i = 0; i < dat2.size(); i++)
    {
        std::cout << +arr[i] << " ";
    }
    std::cout << std::endl;


    char* arr3 = (char *)dat2.c_bodyStr();

    for (int i = 0; i < dat2.bodySize(); i++)
    {
        std::cout << +arr3[i] << " ";
    }

    dat2.rawDatOut();
    std::cout << arr3 << "\n";
}
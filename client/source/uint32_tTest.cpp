
#include <iostream>
#include <math.h>
int main()
{
    const int TICKET_SIZE_T = 4;
    const int HEADER_SIZE_T = 4;

    std::cout << " please work???? : \n";

    for (uint32_t x = 0; x < 65800; x++)
    {
        uint8_t *dat = new uint8_t[4];
        std::cout << std::to_string(+x) << std::endl;
        uint32_t temp1 = x;

        /*
                for (auto i = 0; i < TICKET_SIZE_T; i++)
                {
                    // std::cout << " serialiser : i = " << i;
                    temp1 = temp1 >> (i * 8);
                    dat[i + (HEADER_SIZE_T - TICKET_SIZE_T)] = (uint8_t)temp1;
                }
        */

        dat[0] = (uint8_t)temp1;
        temp1 = temp1 >> 8;
        dat[1] = (uint8_t)temp1;
        temp1 = temp1 >> 8;
        dat[2] = (uint8_t)temp1;
        temp1 = temp1 >> 8;
        dat[3] = (uint8_t)temp1;

        std::cout << "\n";
        for (auto i = 0; i < TICKET_SIZE_T; i++)
        {
            std::cout << +dat[i] << " ";
        }
        std::cout << "\n";
    }
    return 0;
}
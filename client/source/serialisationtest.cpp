#include <iostream>
#include <vector>
#include <crbn_dataSerialiser.hpp>
#include <crbn_logging.hpp>

class test
{
public:
    crbn::serialiser *dat;

    test(const std::string str)
    {
        dat = new crbn::serialiser((uint8_t *)str.c_str(), strlen(str.c_str()));
    }

    void out()
    {
        uint8_t *arr = dat->rawDatOut();

        for (int i = 0; i < dat->size(); i++)
        {
            std::cout << +arr[i] << " ";
        }
    }
};

int main()
{
    int i1 = 0;
    for(;;)
    {

        std::vector<test *> vec;

        for (int i = 0; i < 1; i++)
        {
            vec.push_back(new test("bingus"));
        }

        int i = 0;

        for (test *x : vec)
        {
            std::cout << "i1 = " << i1 << " | i = " << i << " | "; 
            i+= 1;
            x->out();
            std::cout << " size : " << x->dat->size() << "\n";
        }
        i1++;
    }

    return 0;
}
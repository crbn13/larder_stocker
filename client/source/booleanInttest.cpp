#include <string>
#include <iostream>



// declaration of namespace :
namespace crbn
{
    void myfunction(int &variable)
    {
        variable++;
    }
}

// usage of namespace 

int main () {

    int x = 5;

    crbn::myfunction(x);

    std::cout << x << std::endl;
}




#include <crbn_instructions.hpp>
#include <crbn_jobManager.hpp>

#include <iostream>

int main()
{
    crbn::Jobs job = crbn::Jobs();

    crbn::serialiser data = crbn::serialiser();
    data.initialise((uint8_t*)"12345abc", 9, crbn::op::ID_ENQUIRY,);

    

}
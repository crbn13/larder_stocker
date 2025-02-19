// #pragma once

#ifndef ERROR_CODES
#define ERROR_CODES
#include "crbn_logging.hpp"

namespace crbn
{
    namespace err
    {
        const int16_t ERROR1 = -1;
        const int16_t NETWORK_TIMED_OUT = -100; 
        const int16_t NO_SOCKET_CONNECTION = -101;
        const int16_t INCORRECT_DATA_SENT = -102;
        const int16_t THREAD_COULDNT_LOCK = -200;
        const int16_t UNEXPECTED_NULL_PTR = -201;
        
        /// @brief returns true or false based on if the program should continue, based on returned error code 
        /// @return 
        template <typename T>
        bool errorCheck(const T)
        {
            //static std::array<int16_t> halt;
            //static std::array<int16_t> tryAgain;
            crbn::log(" Error Codes : UNFINISHED ; WILL JUST RETURN TRUE ");
            return true;
        }
        
    }
}

#endif

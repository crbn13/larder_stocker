// #pragma once
#ifndef CRBN_INSTRUCTIONS_HPP
#define CRBN_INSTRUCTIONS_HPP

#include "crbn_dataSerialiser.hpp"
#include "crbn_json.hpp"

#include "crbn_jobManager.hpp"

#ifdef DEF_SERVER
    #include "crbn_imageDownloader.hpp"
    #include "../stb_image.h"
    #include "../stb_image_write.h"
#endif



namespace crbn
{
    class Jobs; // forward declaration

    namespace op
    {
#ifdef DEF_SERVER
        /// @brief takes data, checks what to do with it, does it
        /// @param data 
        /// @param j use this
        void instructionDecoder(std::shared_ptr<crbn::serialiser> data, crbn::Server_Jobs &);
#endif

#ifdef DEF_CLIENT
        /// @brief takes data, checks what to do with it, does it
        /// @param data 
        /// @param j use this
        void instructionDecoder(std::shared_ptr<crbn::serialiser> data, crbn::Client_Jobs &);
#endif
    }
}


#endif
// #pragma once
#ifndef SIMPLE_NETWORKING
#define SIMPLE_NETWORKING

// #include <asio.hpp>
// #include <asio/ts/buffer.hpp>
// #include <asio/ts/internet.hpp>

#include "../asio-1.30.2/include/asio.hpp"
#include "../asio-1.30.2/include/asio/ts/buffer.hpp"
#include "../asio-1.30.2/include/asio/ts/internet.hpp"

// #include "crbn_logging.hpp"
#include "crbn_errorCodes.hpp"
#include "crbn_dataSerialiser.hpp"
#include "crbn_instructions.hpp"
#include "crbn_json.hpp"

#include <stdio.h>
#include <stdlib.h>
#include <string>
#include <iostream>
#include <chrono>
#include <thread>
#include <utility>
#include <vector>
#include <mutex>


namespace crbn
{
    class simpleNetworking
    {

    public:
        std::string c_ip = "127.0.0.1";
        int i_port = 7897;
        const int i_serverToClientPort = 7898;

        /// @brief number of milliseconds network will wait for new data untill giving up
        int ms_networkTimeOut = 1000;
    private:
        bool client = 0;
        bool svr = 0;

        //  timeout = std::chrono;

        asio::error_code ec; // inbuilt error code declaration

        // create a "context " essentially the platform specific interface
        asio::io_context context;

    public:

        bool clientStart();

        int clientStart(const std::string ip, const int port);
        

    public:
        /// @brief
        /// @param socket
        /// @return
        int sendConfirmation(asio::ip::tcp::socket *socket, uint32_t ticket);

        /// @brief
        /// @param socket
        /// @return
        int16_t recieveConfirmation(asio::ip::tcp::socket *socket, const uint32_t ticket);
        
    public:
        int sendData(crbn::serialiser *dat);

    private:
        /// @brief sends raw data, array of bytes with size of size
        /// @param data
        /// @param size
        /// @return
        int sendDataNewConnection(crbn::serialiser *dat);
        
    public:
        /// @brief
        /// @param datLock mutext to protect dual writing of data serialiser
        /// @param dat pointer to data serialiser class instance
        /// @param loopIndef loops indefinitely true/false
        /// @param sendConfirmation boolean, sends confirmation true or false
        /// @return returns positive if success , if failiure, returns crbn::err code ( NOT IMPLEMENTED YET )
        int serverStart(std::mutex &datLock, crbn::serialiser *dat, const bool loopIndef, bool b_sendConfirmation);
        
        int serverStart(std::mutex &datLock, crbn::serialiser *dat, const bool loopIndef);
    };
}

#endif

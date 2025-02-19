// #pragma once
#ifndef TICKET_MANAGER
#define TICKET_MANAGER

#ifndef DEF_CLIENT
#ifndef DEF_SERVER
#define DEF_CLIENT
#define DEF_SERVER
#endif
#endif

// #include "crbn_dataSerialiser.hpp"
#include "crbn_simpleNetworking.hpp"
// #include "crbn_json.hpp"
// #include "crbn_logging.hpp"
// #include "crbn_instructions.hpp"

#include <queue>
#include <thread>
#include <mutex>
#include <chrono>
#include <atomic>
#include <map>
#include <utility>
#include <fstream>
#include <filesystem>
#include <string.h>
#include <ios>

#ifdef DEF_SERVER
#include "crbn_imageDownloader.hpp"

// #define STB_IMAGE_IMPLEMENTATION
// #define STB_IMAGE_WRITE_IMPLEMENTATION
#include "../stb_image.h"
#include "../stb_image_write.h"
#endif

namespace crbn
{
    class Jobs
    {
    private:
        enum active
        {
            CLIENT = 1,
            SERVER = 2
        };

        int8_t currentRunning = 0;

        // int8_t currentRunning = 0;

        std::queue<crbn::serialiser> incomingTasksQueue;
        std::mutex m_incomingTasksQueue;

        std::queue<crbn::serialiser> outgoingTasksQueue;
        std::mutex m_outgoingTasksQueue;
        std::chrono::microseconds threadSleepFor = std::chrono::microseconds(10000);

        // ~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~  SERVER ONLY ~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~
#ifdef DEF_SERVER
        std::mutex m_imageDownloader;

        // the active data tag which the client wants to edit ;
        std::string activeTag;
#endif
#ifdef DEF_CLIENT

        // ~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~  CLIENT ONLY ~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~
        std::queue<uint32_t> expectedIncomingQueue;
        std::mutex m_expectedIncomingQueue;

    public:
        bool isServerConnectable = false;

    private:
        std::mutex clientDataReciever; // client side server mutex ,

        std::map<std::string, std::pair<bool, void *>> dataMap;
        std::mutex m_dataMap;

        uint32_t ticket = 50000;

        std::string ipSaved = "127.0.0.1";
        std::mutex m_ip;

    public:
        void setIp(std::string ipInput);

        /// @brief used to aquire a unique ticket to be used to send and recieve data
        /// @return
        uint32_t getTicket();

        bool isJsonAvailable(std::string tag);

        void checkServerConnection();

    public:
        json *getJsonData(std::string tag);

#endif // ~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~ CLIENT ONLY END ~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~##

    public:
        /// @brief takes data, checks what to do with it, does it
        /// @param job
        /// @param data
        void instructionDecoder(crbn::serialiser *data);

#ifdef DEF_CLIENT
        void async_clientStart(std::string ip);

        void async_clientStart();

#endif

#ifdef DEF_SERVER
    public:
        void async_serverStart();

#endif

    private:
        int enque(serialiser dat, std::queue<crbn::serialiser> queue, std::mutex mutex);

    public:
        /// @brief adds data to outgoing queue : to be sent out
        int enqueOUT(crbn::serialiser *dat);

        /// @brief adds data to incoming queue to be decoded and executed
        int enqueIN(crbn::serialiser *dat);
    };
};

#endif

// #pragma once
#ifndef CRBN_JSON_FUNCS
#define CRBN_JSON_FUNCS
#include <nlohmann/json.hpp>
using json = nlohmann::json;
#include "crbn_logging.hpp"
#include <fstream>
#include <string>
#include <chrono>
#include <thread>
#include <stdio.h>
#include <cstdio>
#include <memory>

namespace crbn
{
    /// @brief contains come implementations of nlohmann json for ease of use
    namespace jsn
    {
        inline const std::string configFileName = "config.json";

        namespace keys
        {
            inline const std::string application_name = "application_name";
            inline const std::string screen_width = "screen_width";
            inline const std::string screen_height = "screen_height";
            inline const std::string server_path_to_data = "server_path_to_data";
            inline const std::string client_ip = "client_ip";
            inline const std::string server_ip = "server_ip";
        }

        namespace jsonLiterals
        {
            inline const char *config = R"(
            {
                "application_name": "bob",
                "screen_height": 1080,
                "screen_width": 1920,
                "server_path_to_data": "data/", 
                "client_ip": "",
                "server_ip": "127.0.0.1",
                "shopping_list_time_buffer": 432000,
                "shopping_list_recency_bias": 2
            }
            )";
            inline const char *data = R"(
            {
                "tag_id": "",
                "name": "",
                "number_of": 0,
                "consumption_times": [],
                "last_input_date": "",
                "rebuy": false,
                "container": {}
            }
            )";
            inline const char *tagsStore = R"(
            {
                "tags": []
            }
            )";

        }

        /// @brief generates config file
        void generateCfgFile();

        json jsonConfigRead(std::string fileName, std::string path);

        json jsonConfigRead(std::string fileName);

        json jsonConfigRead();

        void jsonWrite(const std::string &path_fileName, const json &json);

        json jsonRead(const std::string &path_fileName, const char *RAW);

        /// @brief generates an empty data file template, does not need path to data or ".json"
        json generateDatFile(std::string NO_PATH_tagId);

        class JsonMutex
        {
        protected:
            bool locked;

        private:
            static inline std::mutex globalJsonMutex;

        public:
            JsonMutex();

            /// @brief locks the mutex if avavilable | if unavailable it will wait till available
            void lock();

            /// @brief releases the mutex
            void unlock();

            /// @brief checks to see if the mutex is locked by THIS process
            /// @return
            const bool isLocked();
        };

        class Json_Helper;

        class Safemode;

        class Json_Helper : public JsonMutex
        {
        protected:
            /// @brief  f: allows mutexLess reading/writing of data ;
            ///         t : blocks all access to read/writes unless mutex locked

            bool safemode;

            /// @brief used to set safemode
            enum
            {
                UNSAFE = false,
                SAFE = true
            };

            json m_j;
            std::string fileName;
            const char *JSON_RAW = crbn::jsn::jsonLiterals::config;
            
            void setvals();
        public:
            /// @brief by deault this class will read from config file
            Json_Helper();

            /// @brief verry dangerous, sets the safemode to unsafe for the duration of the life of the returned smart pointer
            /// @param
            // template <typename T>
            std::unique_ptr<Safemode> setSafeModeUnsafe();

            /// gain access to internal instance of nlohmann::json
            const json &j();

            /// overwritten functions which still make use of JsonMutex base functions
            /// @brief locks mutex then updates json file
            void lock();
            /// @brief saves json file to memory then unlocks mutex
            void unlock();



            /// @brief calls init() function
            /// @param fileToRead
            /// @param JSON_RAW_SET
            Json_Helper(const std::string &fileToRead, const char *JSON_RAW_SET);

            /// @brief loads config data into json file
            void init(const std::string &fileToRead, const char *JSON_RAW_SET);

            /// @brief provide access to raw json data
            /// @param
            template <typename T>
            json &operator[](T &tag);

        public:
            /// @brief depreciated, use update()
            void init();

            /// @brief updates locally stored file to file in memory
            void update();

            /// @brief changes value then saves the json
            /// @tparam T
            /// @param key
            /// @param value
            template <typename T>
            void json_write(const std::string key, const T &value);

        public:
            /// @brief writes locally stored json file to memory
            /// be careful using this function as it is not parrelel data safe
            void save();

        public:
            /// @brief dont use
            /// @param key
            /// @return
            std::string strGet(const std::string &key);

            int intGet(const std::string &key);

            /// @brief couts syntaxed json
            void print();

            /// @brief checks all elements to check that they exist, if they dont, the file is reset
            void elementsCheck();
        };

        class Safemode : public Json_Helper
        {
            private:
            bool * sm;
            bool * lkd;
        public:
            Safemode(bool *, bool *);
            ~Safemode();
        };
    }
}

#include "crbn_json.tpp"

#endif

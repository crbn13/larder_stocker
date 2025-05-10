#pragma once
                        // ~~~~~~~~~~~~~~~~~~~~~~~  CRBN_JOBMANAGER_HPP     ~~~~~~~~~~~~~~~~~~~~~~~
#ifndef CRBN_JOBMANAGER_HPP
    #define CRBN_JOBMANAGER_HPP

#ifndef DEF_CLIENT
    #ifndef DEF_SERVER
        #ifdef __INTELLISENSE__ // code that generates an error squiggle 
            #define DEF_CLIENT
            #define DEF_SERVER
        #endif
        #ifdef __clang__
            #define DEF_CLIENT
            #define DEF_SERVER 
        #endif
        #ifndef __INTELLISENSE__
            #ifndef __clang__
                #error "neither DEF_SERVER or DEF_CLIENT are defined, define at least 1 to compile"
            #endif
        #endif
    #endif
#endif

#include <queue>        // std::queue
#include <chrono>       // time
#include <map>          // std::map
#include <utility>      // std::pair
#include <fstream>      // file inp output
#include <string.h>     // 
#include <ios>          // binary data streams
#include <filesystem>   // file.exists
#include <mutex>        // mutexes
#include <atomic>
#include <thread>       // threads
#include <memory>       // smart pointers
#include <future>       // std::async

namespace crbn          // ~~~~~~~~~~~~~~~~~~~~~~~  UTIL STUFF              ~~~~~~~~~~~~~~~~~~~~~~~
{
    namespace
    {
        /// @brief used to give shared_ptr<void> a deconstructor
        /// @tparam T actual type which void pointer points to
        template <class T>
        auto deconstructor = [](void * p) {delete static_cast<T*>(p); };
    }
    /// @brief class to protect local variables which are accessed by outside functions that could by async
    /// @tparam T datatype to hold + protect
    template <typename T>
    class Variable_Mutex
    {
    public:
        /// copy constructer
        Variable_Mutex(const T data);

        /// move constructer
        // Variable_Mutex(T&& );

        Variable_Mutex();
        
        ~Variable_Mutex();
        
        /// @brief access (copy of) local variable
        const T operator()() const;
        /// @brief assign data to local variable
        Variable_Mutex &operator=(const T &);
        
        /// @brief locks mutex, runs lambda, returns lambda output
        // / @param [&](T& /*name*/ ) { /* do stuff */ }
        /// @return return value of almbda is returned
        template <typename Lambda>
        auto read(Lambda lambda) const;
        
        /// @brief locks mutex, runs lambda, returns lambda output
        /// @param [&](T& /*name*/ ) { /* do stuff */ }
        /// @return return value of almbda is returned
        template <typename Lambda>
        auto write(Lambda lambda);

    private:
        T _variable;
        mutable std::mutex _mutex;
        auto _lock();
        auto _lock() const; // constant version for other constant functions
    };
}
                        // ~~~~~~~~~~~~~~~~~~~~~~~  UTIL STUFF END          ~~~~~~~~~~~~~~~~~~~~~~~

#include "crbn_jobManager.tpp" // implementations of util stuff

#include "crbn_operations.hpp"
#include "crbn_simpleNetworking.hpp"
#include "crbn_logging.hpp"
#include "crbn_json.hpp"
#include "crbn_dataSerialiser.hpp"

namespace crbn          // ~~~~~~~~~~~~~~~~~~~~~~~  namespace crbn START    ~~~~~~~~~~~~~~~~~~~~~~~
{
    class Jobs          // ~~~~~~~~~~~~~~~~~~~~~~~  BASE CLASS              ~~~~~~~~~~~~~~~~~~~~~~~
    {
    public:
        Jobs();
        ~Jobs() = default;
        
        void sendNetworkRequest(crbn::op::OPERATIONS_TYPE operation, crbn::serc::TICKET_TYPE ticket, u_char *data = 0, size_t arrayLenght = 0);
        void sendNetworkRequest(crbn::op::OPERATIONS_TYPE operation, crbn::serc::TICKET_TYPE ticket, const std::string &copy_data);

        /// @brief adds data to outgoing queue : to be sent out
        void enqueOUT(crbn::serialiser& dat);
        void enqueOUT(std::shared_ptr<crbn::serialiser>);

        /// @brief adds data to incoming queue to be decoded and executed
        void enqueIN(crbn::serialiser& dat);
        void enqueIN(std::shared_ptr<crbn::serialiser>);
    public:
        // the active data tag which the client wants to edit ;
        Variable_Mutex<std::string> activeTag;

    protected:
        enum active
        {
            CLIENT = true,
            SERVER = true
        };

        static bool currentRunning;

        std::queue<std::shared_ptr<crbn::serialiser>> incomingTasksQueue;
        std::mutex m_incomingTasksQueue;

        std::queue<std::shared_ptr<crbn::serialiser>> outgoingTasksQueue;
        std::mutex m_outgoingTasksQueue;

        std::chrono::microseconds threadSleepFor = std::chrono::microseconds(10000);
    
    };                  
                        // ~~~~~~~~~~~~~~~~~~~~~~~  BASE CLASS END          ~~~~~~~~~~~~~~~~~~~~~~~
    
#ifdef DEF_CLIENT       // ~~~~~~~~~~~~~~~~~~~~~~~  CLIENT ONLY             ~~~~~~~~~~~~~~~~~~~~~~~

    class Client_Jobs : public Jobs 
    {
    public:
        Client_Jobs();
        ~Client_Jobs() = default;
        
        /// @brief start client side communication manager
        /// @param ip ip address of the server
        void async_clientStart(const std::string & ip);

        /// @brief start client side communication manager
        void async_clientStart();

        /// generates new ticket and adds request to outgoing queue-(towards other device)
        ///
        /// abstracts away from crbn::serialiser and provides easy function to 
        /// communicate with other device,
        /// @param data raw array to add, 
        /// @param operation crbn::op::enum specifies what the request is
        crbn::serc::TICKET_TYPE sendNetworkRequest(crbn::op::OPERATIONS_TYPE operation, u_char* data = nullptr, size_t arrayLenght = 0);
        crbn::serc::TICKET_TYPE sendNetworkRequest(crbn::op::OPERATIONS_TYPE operation, const std::string& copy_data);

    private:
        std::queue<uint32_t> expectedIncomingQueue;
        std::mutex m_expectedIncomingQueue;

    public:
        std::atomic_bool isServerConnectable = false;

    private:
        static inline std::mutex clientDataReciever; // client side networking server mutex 

        //                                                    returntype (name) (perameters*)
        // std::map<std::string, std::pair<bool, std::unique_ptr<void, void (*) (void*)>>> dataMap;
        // std::map<std::string, std::pair<bool, std::unique_ptr<void, decltype(deconstructor<void>)>>> dataMap;
    public:
        // used to store data from the server
        // ticket, pointer, tag(barcode number (might not exist)) 
        
        Variable_Mutex<std::unordered_map<serc::TICKET_TYPE, std::tuple<bool, std::shared_ptr<void>, std::string, serc::OPERATION_TYPE>>> dataMap;
        /// a queue of recently enqued tickets, not async safe
        /// .first = ticket | .second = operation;
        std::queue<std::pair<serc::TICKET_TYPE, serc::OPERATION_TYPE>> ticketQueue;


        Variable_Mutex<crbn::op::OPERATIONS_TYPE> ticket ;

        std::string ipSaved = "127.0.0.1";
        std::mutex m_ip;

    public:
        void setIp(std::string ipInput);

        /// @brief used to aquire a unique ticket to be used to send and recieve data
        /// @return
        crbn::serc::TICKET_TYPE getTicket();

        // warning, super slow
        bool isDataAvailable(const std::string & tag, const serc::OPERATION_TYPE);
        // not super slow :p
        bool isDataAvailable(const serc::TICKET_TYPE);

        void checkServerConnection();

    public:    
        std::shared_ptr<json> getJsonData(const std::string &);
        std::shared_ptr<json> getJsonData(const serc::TICKET_TYPE);

        template <class T>
        std::shared_ptr<T> getData(const std::string &);
        template <class T>
        std::shared_ptr<T> getData(const serc::TICKET_TYPE);

    };
#endif                  // ~~~~~~~~~~~~~~~~~~~~~~~  CLIENT ONLY END         ~~~~~~~~~~~~~~~~~~~~~~~


#ifdef DEF_SERVER       // ~~~~~~~~~~~~~~~~~~~~~~~  SERVER ONLY             ~~~~~~~~~~~~~~~~~~~~~~~
    
    class Server_Jobs : public Jobs 
    {
    public:
        Server_Jobs() = default;
        ~Server_Jobs() = default;

        /// @brief start server side communication manager
        void async_serverStart();

        mutable std::mutex m_imageDownloader;    
    };
#endif                  // ~~~~~~~~~~~~~~~~~~~~~~~  SERVER ONLY END         ~~~~~~~~~~~~~~~~~~~~~~~
    
};                      // ~~~~~~~~~~~~~~~~~~~~~~~  namespace crbn          ~~~~~~~~~~~~~~~~~~~~~~~

#include "crbn_instructions.hpp"

#endif                  // ~~~~~~~~~~~~~~~~~~~~~~~  CRBN_JOBMANAGER_HPP     ~~~~~~~~~~~~~~~~~~~~~~~

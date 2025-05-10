#include "headers/crbn_jobManager.hpp"
#include "headers/crbn_dataSerialiser.hpp"
#include "headers/crbn_operations.hpp"
#include <algorithm>
#include <memory>
#include <stdexcept>
#include <vector>

namespace crbn
{
                        // ~~~~~~~~~~~~~~~~~~~~~~~  BASE CLASS              ~~~~~~~~~~~~~~~~~~~~~~~
#pragma region base_class

    bool Jobs::currentRunning = false;
    
    Jobs::Jobs()
        : activeTag(std::string("null"))
    {
    }
    
    void Jobs::sendNetworkRequest(crbn::op::OPERATIONS_TYPE operation, crbn::serc::TICKET_TYPE ticket, u_char *data, size_t arrayLenght )
    {
        enqueOUT(std::make_shared<crbn::serialiser>((uint8_t *)data, arrayLenght, operation, ticket));
    }
    
    void Jobs::sendNetworkRequest(crbn::op::OPERATIONS_TYPE operation, crbn::serc::TICKET_TYPE ticket, const std::string &copy_data)
    {
        enqueOUT(std::make_shared<crbn::serialiser>((uint8_t *)copy_data.c_str(), copy_data.size(), operation, ticket));
    }

    void Jobs::enqueOUT(crbn::serialiser& dat)
    {
        std::lock_guard<std::mutex> lock(m_outgoingTasksQueue);
        outgoingTasksQueue.push(
            std::make_shared<crbn::serialiser>( std::move ( dat ) ));
        // crbn::log("crbn::Jobs::enqueOUT written data to queue ");
    }
    void Jobs::enqueOUT(std::shared_ptr<crbn::serialiser> dat)
    {
        std::lock_guard<std::mutex> lock(m_outgoingTasksQueue);
        outgoingTasksQueue.push(dat);
    }

    void Jobs::enqueIN(crbn::serialiser& dat)
    {
        std::lock_guard<std::mutex> lock(m_incomingTasksQueue);
        incomingTasksQueue.push(
            std::make_shared<crbn::serialiser>(std::move( dat )));
        // crbn::log("crbn::Jobs::enqueIN written data to queue ");
    };

    void Jobs::enqueIN(std::shared_ptr<crbn::serialiser> dat)
    {
        std::lock_guard<std::mutex> lock(m_incomingTasksQueue);
        incomingTasksQueue.push(dat);
    }

#pragma endregion base_class
                        // ~~~~~~~~~~~~~~~~~~~~~~~  BASE CLASS END          ~~~~~~~~~~~~~~~~~~~~~~~


#ifdef DEF_CLIENT       // ~~~~~~~~~~~~~~~~~~~~~~~  CLIENT ONLY             ~~~~~~~~~~~~~~~~~~~~~~~

    Client_Jobs::Client_Jobs()
        : ticket(uint32_t(12345u))
    {}

    crbn::serc::TICKET_TYPE Client_Jobs::sendNetworkRequest(crbn::op::OPERATIONS_TYPE operation, u_char *data, size_t arrayLenght)
    {
        auto ticket = getTicket();
        enqueOUT(std::make_shared<crbn::serialiser>((uint8_t *)data, arrayLenght, operation, ticket));
        return ticket;
    }

    crbn::serc::TICKET_TYPE Client_Jobs::sendNetworkRequest(crbn::op::OPERATIONS_TYPE operation, const std::string &copy_data)
    {
        auto ticket = getTicket();
        enqueOUT(std::make_shared<crbn::serialiser>((uint8_t *)copy_data.c_str(), copy_data.size(), operation, ticket));
        return ticket;
    }

    void Client_Jobs::setIp(std::string ipInput)
    {
        std::scoped_lock<std::mutex> lock(m_ip);
        ipSaved = ipInput;
    }

    /// @brief used to aquire a unique ticket to be used to send and recieve data
    /// @return
    crbn::serc::TICKET_TYPE Client_Jobs::getTicket()
    {
       return ticket.write([&](auto &t)
                     { t++; return t; });
    }

    bool Client_Jobs::isDataAvailable(const std::string &tag, const serc::OPERATION_TYPE op)
    {
        // crbn::log(" crbn::Jobs::isJsonAvailable waiting for mutex ????");
        return dataMap.read(
            [&](decltype(dataMap())& map)
            {
                for (auto& x : map)
                {
                    if (std::get<serc::OPERATION_TYPE>(x.second) == op)
                        if (std::get<std::string>(x.second) == tag)
                        {
                            return std::get<bool>(x.second);
                        }
                }
                return false;
            });
    }

    bool Client_Jobs::isDataAvailable(const serc::TICKET_TYPE ticket)
    {
        if (dataMap().count(ticket))
        {
            return std::get<bool>(dataMap().at(ticket));
        }
        else
        {
            //crbn::log(" crbn::Jobs::isJsonAvailable map key not initialised ");
            return false;
        }
        

    }

    void Client_Jobs::checkServerConnection()
    {
        isServerConnectable = false;
        crbn::serialiser dat(nullptr, 0, crbn::op::CHECK_CONNECTION, getTicket());
        enqueOUT(dat);
    }

    std::shared_ptr<json> Client_Jobs::getJsonData(const serc::TICKET_TYPE ticket)
    {
        if (isDataAvailable(ticket))
        {
            return std::shared_ptr<json>(std::static_pointer_cast<json>(std::get<std::shared_ptr<void>>(dataMap().at(ticket))));
        }
        else
        {
            return std::shared_ptr<json>(); // do something angry
        }
    }
    std::shared_ptr<json> Client_Jobs::getJsonData(const std::string & tag)
    {
        throw std::logic_error("havent tested this dont use");

        return dataMap.write(
            [&](decltype(dataMap())& map)
            {
                std::vector<serc::TICKET_TYPE> temp_tickets;
                for (const auto& x : map)
                {
                    if (std::get<serc::OPERATION_TYPE>(x.second) == op::SEND_JSON_TO_CLIENT)
                        if (std::get<std::string>(x.second) == tag)
                            temp_tickets.push_back(x.first);
                }

                serc::TICKET_TYPE biggest_ticket = 0;
                if (!temp_tickets.empty())
                {
                    for (const auto& x : temp_tickets)
                    {
                        biggest_ticket = std::max(biggest_ticket, x);
                    }
                }
                else
                {
                    crbn::warn("getJsonData, warning returning empty pointer");
                    return std::shared_ptr<json>();
                }
                return std::static_pointer_cast<json>(std::get<std::shared_ptr<void>>(map.at(biggest_ticket)));
            });
    }

    void Client_Jobs::async_clientStart(const std::string &ip)
    {
        if (currentRunning)
        {
            crbn::log("crbn::Jobs::async_clientStart ALREADY RUNNING CANNOT START ANOTHER ASYNC instance ");
            return;
        }
        else
        {
            currentRunning = active::CLIENT;
        }

        /// checks incoming queue to
        std::thread incomingQueueCheck(
            [this]()
            {
                bool b_go = false;

                for (;;)
                {
                    std::shared_ptr<crbn::serialiser> dat(nullptr);
                    b_go = false;
                    {
                        std::lock_guard<std::mutex> lock(m_incomingTasksQueue);
                        if (!incomingTasksQueue.empty())
                        {
                            dat = incomingTasksQueue.front();
                            incomingTasksQueue.pop();
                            b_go = true;
                        }
                    }
                    if (b_go)
                    {
                        crbn::log("crbn::Jobs::async_clientStart incomingQueueCheck sending data to instruction decoder");
                        // std::async( &Jobs::instructionDecoder, *this, dat); FUNCTION IS NOT ASYNC SAFE ARRRR
                        // op::instructionDecoder(dat, *this);
                        auto future = std::async(std::launch::async, [&]() {return crbn::op::instructionDecoder(dat, *this);} );
                    }
                    else
                    {
                        std::this_thread::sleep_for(threadSleepFor);
                    }
                }
            });
        incomingQueueCheck.detach();
        
        /// sends data in outgoing queue to server
        std::thread outgoingCliNet(
            [this](std::string ip)
            {
                crbn::simpleNetworking net;
                std::shared_ptr<crbn::serialiser> dat(nullptr);
                net.clientStart();
                net.c_ip = ip;
                for (;;)
                {
                    bool b_go = false;
                    
                    {
                        std::lock_guard<std::mutex> lock(m_outgoingTasksQueue);
                        if (!outgoingTasksQueue.empty())
                        {
                            dat = outgoingTasksQueue.front();
                            outgoingTasksQueue.pop();
                            {
                                std::scoped_lock<std::mutex> lock(m_expectedIncomingQueue);
                                expectedIncomingQueue.push(dat->ticket());
                            }
                            
                            b_go = true;
                        }
                    }
                    
                    if (b_go)
                    {
                        // std::this_thread::sleep_for(std::chrono::milliseconds(5000));
                        
                        int temp = net.sendData(dat.get());
                        
                        if (temp == 0)
                        {
                            crbn::log("crbn::Jobs::async_clientStart outgoingCliNet failed sending data :( from client");
                        }
                        else
                        {
                            
                            crbn::log("./crbn_jobManager.hpp::Jobs::async_clientStart data sent ?!");
                        }
                    }
                    else
                    {
                        // crbn::log("crbn::Jobs::async_clientStart::outgoingCliNet : queue empty");
                        std::this_thread::sleep_for(threadSleepFor);
                    }
                    // delete dat; // this breaks : double free detected in tcache 2  ???????
                }
            },
            ip);
            outgoingCliNet.detach();
            
            std::thread clientDataRecieve(
                [this]()
                {
                crbn::simpleNetworking net;
                std::shared_ptr<crbn::serialiser> dat(nullptr);
                
                for (;;)
                {
                    dat.reset(new crbn::serialiser);
                    
                    net.i_port = net.i_serverToClientPort;
                    
                    crbn::log("crbn::Jobs::async_clientStart recieve : listening on port : " + std::to_string(net.i_serverToClientPort));
                    
                    int temp = net.serverStart(clientDataReciever, dat.get(), false, true);
                    
                    if (temp > 0) // if server successfully recieved data
                    {
                        crbn::log("crbn::Jobs::async_clientStart recieve : recieved data !!! ");
                        std::scoped_lock<std::mutex> lock(m_incomingTasksQueue);
                        // crbn::warn(dat.get()->bodyAsString());
                        incomingTasksQueue.push(dat);
                        crbn::log("crbn::Jobs::async_clientStart clientDataRecieve pushed data to incomingQueue ");
                    }
                    else // server failed to recieve data, 
                    {
                        crbn::log("crbn::Jobs::async_clientStart clientDataRecieve failed to recieve data");
                        std::scoped_lock<std::mutex> lock(m_expectedIncomingQueue);
                        expectedIncomingQueue.push(dat->ticket());      // putting the ticket back on the queue to try again
                    }
                }
            });
            clientDataRecieve.detach();
        }

        void Client_Jobs::async_clientStart()
        {
            async_clientStart(ipSaved);
        }
#endif                  // ~~~~~~~~~~~~~~~~~~~~~~~  CLIENT ONLY END         ~~~~~~~~~~~~~~~~~~~~~~~
        

#ifdef DEF_SERVER       // ~~~~~~~~~~~~~~~~~~~~~~~  SERVER ONLY             ~~~~~~~~~~~~~~~~~~~~~~~
        
        void Server_Jobs::async_serverStart()
        {
            if (currentRunning)
            {
                crbn::log("crbn::Jobs::async_serverStart ALREADY RUNNING CANNOT START ANOTHER ASYNC instance ");
                return;
            }
            else
            {
                currentRunning = active::SERVER;
            }

        std::thread serverInputThr(
            [this]()
            {
                crbn::log(" crbn::Jobs::async_serverStart SERVER THREAD STARTED ");
                crbn::simpleNetworking net;
                std::shared_ptr<crbn::serialiser> dat(nullptr);

                std::mutex local;

                for (;;)
                {
                    crbn::log("crbn::Jobs::serverInputThr about to call serverStart to recieve client input ");
                    dat.reset( new crbn::serialiser ) ;

                    if (net.serverStart(local, dat.get(), false) > 0)
                    {
                        enqueIN(dat);
                    }
                }
            });
        serverInputThr.detach();

        std::thread serverIncomingExecuter(
            [this]()
            {
                bool b_go = false;
                std::shared_ptr<crbn::serialiser> dat(nullptr);

                for (;;)
                {
                    b_go = false;

                    {
                        std::lock_guard<std::mutex> lock(m_incomingTasksQueue);
                        if (!incomingTasksQueue.empty())
                        {
                            dat = incomingTasksQueue.front();
                            incomingTasksQueue.pop();
                            b_go = true;
                        }
                    }
                    if (b_go)
                    {
                        if (dat == nullptr)
                        {
                            crbn::log(" DLJEFLESJFIOEJSIOFJEISOI UNITNITIELNLALISED AWOOGA \n");
                        }
                        else
                        {
                            auto future = std::async(std::launch::async, [&]() {return crbn::op::instructionDecoder(dat, *this); } ); 
                            // op::instructionDecoder(dat, *this);
                        }
                    }
                    else
                    {
                        std::this_thread::sleep_for(threadSleepFor);
                    }   
                }
                
            });
        serverIncomingExecuter.detach();

        std::thread serverOutgoingSender(
            [this]()
            {
                bool b_go = false;
                std::shared_ptr<crbn::serialiser> dat(nullptr);
                crbn::simpleNetworking net;
                net.clientStart();
                crbn::jsn::Json_Helper j;
                j.init(crbn::jsn::configFileName, crbn::jsn::jsonLiterals::config);

                j.lock();
                std::string client_ip = j.strGet("client_ip");
                j.unlock();

                for (;;)
                {
                    b_go = false;
                    {
                        std::scoped_lock<std::mutex> lock(m_outgoingTasksQueue);
                        if (!outgoingTasksQueue.empty())
                        {
                            b_go = true;
                            dat = outgoingTasksQueue.front();
                            outgoingTasksQueue.pop();
                        }
                    }
                    if (b_go)
                    {
                        j.init();
                        j.setSafeModeUnsafe();
                        client_ip = j.strGet("client_ip");
                        net.i_port = net.i_serverToClientPort;
                        net.c_ip = client_ip;

                        crbn::log("crbn::Jobs::ServerOutGoingSender about to send data to client");
                        int temp = net.sendData(dat.get());
                        if (temp < 1)
                        {
                            crbn::log("crbn::Jobs::async_serverStart failed sent data from server to client");
                        }
                        else
                        {
                            crbn::log("crbn::Jobs::async_serverStart data succsessfuly sent and authenticated to client");
                        }
                    }
                    else
                    {
                        std::this_thread::sleep_for(threadSleepFor);
                    }
                    
                }
            });
        serverOutgoingSender.detach();
    }
#endif                  // ~~~~~~~~~~~~~~~~~~~~~~~  SERVER ONLY END         ~~~~~~~~~~~~~~~~~~~~~~~


};                      // ~~~~~~~~~~~~~~~~~~~~~~~  crbn:: END               ~~~~~~~~~~~~~~~~~~~~~~~

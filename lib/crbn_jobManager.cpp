#include "headers/crbn_jobManager.hpp"

namespace crbn
{

#ifdef DEF_CLIENT // ~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~  CLIENT ONLY ~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~

    void Jobs::setIp(std::string ipInput)
    {
        std::scoped_lock<std::mutex> lock(m_ip);
        ipSaved = ipInput;
    }

    /// @brief used to aquire a unique ticket to be used to send and recieve data
    /// @return
    uint32_t Jobs::getTicket()
    {
        ticket++;
        return +ticket;
    }

    bool Jobs::isJsonAvailable(std::string tag)
    {
        crbn::log(" crbn::Jobs::isJsonAvailable waiting for mutex ????");
        std::scoped_lock<std::mutex> lock(m_dataMap);

        if (dataMap.count(tag))
        {
            return dataMap[tag].first;
        }
        else
        {
            crbn::log(" crbn::Jobs::isJsonAvailable map key not initialised ");
            return false;
        }
    }

    void Jobs::checkServerConnection()
    {
        isServerConnectable = false;
        crbn::serialiser *dat = new crbn::serialiser(nullptr, 0, crbn::op::CHECK_CONNECTION, getTicket());
        enqueOUT(dat);
        delete dat;
    }

    json *Jobs::getJsonData(std::string tag)
    {
        if (isJsonAvailable(tag))
        {
            std::scoped_lock<std::mutex> lock(m_dataMap);
            return static_cast<json *>(dataMap[tag].second);
        }
        else
        {
            return nullptr; // do something angry
        }
    }
#endif
    // ~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~ CLIENT ONLY END ~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~##

    /// @brief takes data, checks what to do with it, does it
    /// @param job
    /// @param data
    void Jobs::instructionDecoder(crbn::serialiser *data)
    {

        crbn::jsn::Json_Helper jsonConfig;
        jsonConfig.init(crbn::jsn::configFileName, crbn::jsn::jsonLiterals::config);
        auto xyzy = jsonConfig.setSafeModeUnsafe();


        // ~~~~~~~~~~~~~~~~~~~~~~~~~~~ SERVER ONLY ~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~
        switch (data->operation())
        {
#ifdef DEF_SERVER
        case crbn::op::ID_ENQUIRY:
        {
            crbn::log("crbn::Jobs::instructionDecoder() | ID_ENQUIRY");

            std::string tag = data->bodyAsString();

            jsonConfig.init();

            crbn::jsn::Json_Helper j;
            
            j.init(jsonConfig.strGet(crbn::jsn::keys::server_path_to_data) + tag + ".json", crbn::jsn::jsonLiterals::data);
            
            auto xyz = j.setSafeModeUnsafe();

            j.lock();
            j.json_write("tag_id", tag);
            j.unlock();

            std::string tempstr = j.j().dump();

            crbn::serialiser *dat = new crbn::serialiser(
                (uint8_t *)tempstr.c_str(),
                tempstr.size(),
                crbn::op::SEND_JSON_TO_CLIENT,
                data->ticket());

            crbn::log(" crbn::Jobs::instructionDecoder data to return to client generated");
            enqueOUT(dat);

            delete dat;
            break;
        }
        case crbn::op::CHECK_CONNECTION:
        {
            crbn::log("crbn::Jobs::instructionDecoder() | CHECK_CONNECTION");
            crbn::serialiser *dat = new crbn::serialiser(nullptr, 0, crbn::op::CONNECTION_CONFIRMED, data->ticket());
            enqueOUT(dat);
            delete dat;

            break;
        }

        case crbn::op::IMAGE_REQUEST:
        {
            crbn::log("crbn::Jobs::instructionDecoder() | IMAGE_REQUEST");

            // tell client what the name of the file is so that it can write it
            std::string temp = data->bodyAsString();
            temp.append(".png");
            crbn::serialiser *d = new crbn::serialiser((uint8_t *)temp.c_str(), temp.size(), crbn::op::TELL_CLIENT_PNG_NAME, data->ticket());
            enqueOUT(d);
            delete d;

            // initialise json helper to read config.json file
            crbn::jsn::Json_Helper j;
            j.init();
            auto x = j.setSafeModeUnsafe();


            std::ifstream file;
            bool downloaded = false;

            crbn::log("crbn::Jobs::IMAGE_REQUEST : OPENING FILE : " + j.strGet(crbn::jsn::keys::server_path_to_data) + data->bodyAsString() + ".png");

            // check if file doesnt exist
            if (!std::filesystem::exists(j.strGet(crbn::jsn::keys::server_path_to_data) + data->bodyAsString() + ".png"))
            {

                crbn::log("crbn::Jobs::IMAGE_REQUEST : file could not be opened");

                crbn::jsn::Json_Helper jdat;

                jdat.init(j.strGet(crbn::jsn::keys::server_path_to_data) + data->bodyAsString() + ".json", crbn::jsn::jsonLiterals::data);

                if (m_imageDownloader.try_lock())
                {
                    crbn::log("crbn::Jobs::IMAGE_REQUEST : DOWNLOADING IMAGE | NAME :" + data->bodyAsString());
                    if (crbn::imageDownload(jdat.strGet("name"), data->bodyAsString()))
                        downloaded = true;
                    m_imageDownloader.unlock();

                    int width, height, channels;

                    std::string tmp(j.strGet(crbn::jsn::keys::server_path_to_data) + (char *)data->c_bodyStr());
                    //  char *name = j.strGet(crbn::jsn::keys::server_path_to_data).c_str();
                    // strcat(name(char *) data->c_bodyStr());
                    // char png[] = ".png";
                    // strcat(name, png);
                    unsigned char *img = stbi_load(tmp.c_str(), &width, &height, &channels, 0);

                    if (!img)
                    {
                        crbn::log("Failed to load image | wrong File type? ");
                    }
                    else
                    {
                        std::string tmp2 = tmp + ".png";
                        stbi_write_png(tmp2.c_str(), width, height, channels, img, width * channels);
                        stbi_image_free(img);
                    }
                }
                else
                {
                    crbn::log("crbn::Jobs::IMAGE_REQUEST : WRITING CLONE DATA TO INCOMING QUEUE");
                    crbn::serialiser *dat = new crbn::serialiser(data->rawDatOut(), true); // THIS IS JUST WRONG
                    enqueIN(dat);
                }
            }
            else
            {
                crbn::log("crbn::Jobs::IMAGE_REQUEST : FILE OPENED");

                downloaded = true;
            }

            file.close();
            // send file to client
            if (downloaded)
            {
                // send file to client
                crbn::log("crbn::Jobs::IMAGE_REQUEST : READING BINARY FILE");
                std::ifstream rawBinaryFile(j.strGet(crbn::jsn::keys::server_path_to_data) + data->bodyAsString() + ".png", std::ios::binary | std::ios::in);
                rawBinaryFile.seekg(0, std::ios::end);
                auto size = rawBinaryFile.tellg();

                crbn::log("crbn::Jobs::IMAGE_REQUEST : IMAGE FILESIZE = " + std::to_string(size));

                rawBinaryFile.seekg(0, std::ios::beg);

                char *raw = new char[size];
                rawBinaryFile.readsome(raw, size);

                crbn::log("crbn::Jobs::IMAGE_REQUEST : initialising serialiser ");

                crbn::serialiser *dat = new crbn::serialiser((uint8_t *)raw, size, crbn::op::SEND_PNG_TO_CLIENT, data->ticket());

                crbn::log("crbn::Jobs::IMAGE_REQUEST : Data about to be written to queue");

                enqueOUT(dat);
                delete[] raw;
            }
            else
            { // GOTTA DO SOMETHING ANGRY
            }
            break;
        }

        case crbn::op::SELECT_KEY:
        {
            crbn::log("crbn::Jobs::instructionDecoder() | SELECT_KEY");
            activeTag = data->bodyAsString();
            break;
        }
        case crbn::op::SET_NAME:
        {
            crbn::log("crbn::Jobs::instructionDecoder() | SET_NAME");
            crbn::log("crbn::Jobs::SET_NAME | active tag == " + activeTag);

            crbn::jsn::Json_Helper j;
            j.init(jsonConfig.strGet(crbn::jsn::keys::server_path_to_data) + activeTag + ".json", crbn::jsn::jsonLiterals::data);
            auto x = j.setSafeModeUnsafe();

            j.json_write("name", data->bodyAsString());
            break;
        }
#endif

#ifdef DEF_CLIENT
            // CLIENT SIDE OPERATIONS ~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~

        case crbn::op::TELL_CLIENT_PNG_NAME:
        {
            crbn::log("crbn::Jobs::instructionDecoder() | TELL_CLIENT_PNG_NAME");
            std::scoped_lock<std::mutex> lock(m_dataMap);
            dataMap[std::to_string(data->ticket())] = {true, data};
            break;
        }
        case crbn::op::SEND_PNG_TO_CLIENT:
        {
            crbn::log("crbn::Jobs::instructionDecoder() | SEND_PNG_TO_CLIENT ");
            {
                std::scoped_lock<std::mutex> lock(m_dataMap);

                if (!dataMap.contains(std::to_string(data->ticket())))
                {
                    enqueIN(data);
                    break;
                }
            }

            crbn::log(" GOING TO DECODE AND WRITE IMAGE TO MEMORY :p hopefully...;");
            {
                std::scoped_lock<std::mutex> lock(m_dataMap);

                std::ofstream binFile((static_cast<crbn::serialiser *>(dataMap[std::to_string(data->ticket())].second))->bodyAsString());

                for (auto i = 0; i < data->bodySize(); i++)
                {
                    binFile << data->bodyAccsess(i);
                }
                binFile.close();
            }
            crbn::log("crbn::Jobs::SENDPNGTOCLIENT | written png ");

            break;
        }
        case crbn::op::SEND_JSON_TO_CLIENT:
        {
            crbn::log("crbn::Jobs::instructionDecoder() | SEND_JSON_TO_CLIENT ");
            char *cstr = (char *)data->c_bodyStr();
            // std::string tempStr = std::string(cstr);
            json *j = new json;

            *j = json::parse(cstr);

            {
                std::scoped_lock<std::mutex> lock(m_dataMap);
                crbn::log("crbn::Jobs::instructionDecoder  writing json data to map");

                // std::pair<bool, json *> temp;
                // temp.first = true;
                // temp.second = j;

                if (j->contains("tag_id"))
                {
                    // *j = j2;
                    // dataMap[(*j)["tag_id"].get<std::string>()] = {true , j};

                    dataMap[(*j)["tag_id"].get<std::string>()] = {true, j};
                }
                else
                {
                    crbn::log("crbn::Jobs::instructionDecoder JSON DOES NOT CONTAIN \"tag_id\" :<< ");
                }
                crbn::log("crbn::Jobs::instructionDecoder written json data to map !!!");
            }
            break;
        }
        case crbn::op::CONNECTION_CONFIRMED:
        {
            crbn::log("crbn::Jobs::instructionDecoder() | CONNECTION_CONFIRMED");
            isServerConnectable = true;
            break;
        }
        default:
        {
            std::cout << "crbn::op::whatdo() oi this bit of codes unfinished\n";
            break;
        }
#endif
        }
    }

    #ifdef DEF_CLIENT
    void Jobs::async_clientStart(std::string ip)
    {
        if (currentRunning)
        {
            crbn::log("crbn::Jobs::async_clientStart ALREADY RUNNING CANNOT START ANOTHER ASYNC instance ");
            return;
        }
        else if (currentRunning == active::CLIENT)
        {
            crbn::log("crbn::Jobs::async_clientStart ALREADY RUNNING CANNOT START ANOTHER CLIENT");
            return;
        }
        else if (currentRunning == active::SERVER)
        {
            crbn::log("crbn::Jobs::async_clientStart ERROR : server already running, cannot start client ");
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
                crbn::serialiser *dat = nullptr;
                bool b_go = false;

                for (;;)
                {
                    b_go = false;

                    {
                        std::lock_guard<std::mutex> lock(m_incomingTasksQueue);

                        if (incomingTasksQueue.size() > 0)
                        {
                            dat = new crbn::serialiser(incomingTasksQueue.front().c_bodyStr(),
                                                       incomingTasksQueue.front().bodySize(),
                                                       incomingTasksQueue.front().operation(),
                                                       incomingTasksQueue.front().ticket());
                            incomingTasksQueue.pop();
                            b_go = true;
                        }
                    }
                    if (b_go)
                    {
                        crbn::log("crbn::Jobs::async_clientStart incomingQueueCheck sending data to instruction decoder");
                        instructionDecoder(dat);
                    }
                    else
                    {
                        std::this_thread::sleep_for(threadSleepFor);
                    }
                    // delete dat; // this also breaks :(
                }
            });
        incomingQueueCheck.detach();

        /// sends data in outgoing queue to server
        std::thread outgoingCliNet(
            [this](std::string ip)
            {
                crbn::simpleNetworking net;
                crbn::serialiser *dat = nullptr;
                net.clientStart();
                net.c_ip = ip;
                for (;;)
                {
                    bool b_go = false;

                    {
                        std::lock_guard<std::mutex> lock(m_outgoingTasksQueue);
                        if (outgoingTasksQueue.size() > 0)
                        {
                            dat = new crbn::serialiser(outgoingTasksQueue.front().c_bodyStr(),
                                                       outgoingTasksQueue.front().bodySize(),
                                                       outgoingTasksQueue.front().operation(),
                                                       outgoingTasksQueue.front().ticket());
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

                        int temp = net.sendData(dat);

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
                for (;;)
                {
                    // bool b_go = false;
                    // uint32_t ip = 0;

                    // {
                    //     std::scoped_lock<std::mutex> lock(m_expectedIncomingQueue);
                    //     if (expectedIncomingQueue.size() > 0)
                    //     {
                    //         crbn::log("crbn::Jobs::async_clientStart CLIENT DATA reciever found data to recieve ");
                    //         b_go = true;
                    //         ip = expectedIncomingQueue.front();
                    //         expectedIncomingQueue.pop();
                    //     }
                    // }

                    if (true)
                    {
                        std::thread recieve(
                            [this]()
                            {
                                crbn::serialiser *dat = new crbn::serialiser;
                                crbn::simpleNetworking *net = new crbn::simpleNetworking;

                                net->i_port = net->i_serverToClientPort;

                                crbn::log("crbn::Jobs::async_clientStart recieve : listening on port : " + std::to_string(net->i_serverToClientPort));

                                int temp = net->serverStart(clientDataReciever, dat, false, true);

                                crbn::log("crbn::Jobs::async_clientStart recieve : recieved data !!! ");

                                if (temp != false)
                                {
                                    std::scoped_lock<std::mutex> lock(m_incomingTasksQueue);

                                    // crbn::log("mutex locked");
                                    incomingTasksQueue.push(*dat);
                                    // crbn::log("pushed to queue");
                                    crbn::log("crbn::Jobs::async_clientStart clientDataRecieve pushed data to incomingQueue ");
                                    // std::scoped_lock<std::mutex> lock(m_expectedIncomingQueue);
                                    // expectedIncomingQueue.(dat->ticket());
                                }
                                else
                                {
                                    crbn::log("crbn::Jobs::async_clientStart clientDataRecieve failed to recieve data");
                                    // std::scoped_lock<std::mutex> lock(m_expectedIncomingQueue);
                                    // expectedIncomingQueue.push(dat->ticket());
                                }
                            });
                        recieve.join();
                    }
                    else
                    {
                        std::this_thread::sleep_for(threadSleepFor);
                    }
                }
            });
        clientDataRecieve.detach();
    }
    void Jobs::async_clientStart()
    {
        async_clientStart(ipSaved);
    }
#endif
#ifdef DEF_SERVER
    void Jobs::async_serverStart()
    {
        if (currentRunning)
        {
            crbn::log("crbn::Jobs::async_serverStart ALREADY RUNNING CANNOT START ANOTHER ASYNC instance ");
            return;
        }
        else if (currentRunning == active::CLIENT)
        {
            crbn::log("crbn::Jobs::async_serverStart ALREADY RUNNING CLIENT cannot start server");
            return;
        }
        else if (currentRunning == active::SERVER)
        {
            crbn::log("crbn::Jobs::async_serverStart ERROR : server already running, cannot start another ");
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
                crbn::simpleNetworking *net = new crbn::simpleNetworking;
                crbn::serialiser *dat = nullptr;

                std::mutex local;

                for (;;)
                {
                    crbn::log("crbn::Jobs::serverInputThr about to call serverStart to recieve client input ");
                    dat = new crbn::serialiser;

                    // net = new crbn::simpleNetworking;
                    if (net->serverStart(local, dat, false))
                    {
                        if (enqueIN(dat))
                            crbn::log("crbn::Jobs::async_serverStart successfully queued data ");
                        else
                            crbn::log("crbn::Jobs::async_serverStart DATA FAILED TO BE queued");
                    }

                    // delete net;

                    delete dat;
                }
                // delete net; // is this neccessary ?? im guessing maby idfk
            });
        serverInputThr.detach();

        std::thread serverIncomingExecuter(
            [this]()
            {
                bool b_go = false;
                crbn::serialiser *dat = nullptr;

                for (;;)
                {
                    dat = nullptr;
                    b_go = false;

                    {
                        std::lock_guard<std::mutex> lock(m_incomingTasksQueue);
                        if (incomingTasksQueue.size() > 0)
                        {
                            dat = new crbn::serialiser(incomingTasksQueue.front().c_bodyStr(),
                                                       incomingTasksQueue.front().bodySize(),
                                                       incomingTasksQueue.front().operation(),
                                                       incomingTasksQueue.front().ticket());
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
                            instructionDecoder(dat);
                    }
                    else
                    {
                        std::this_thread::sleep_for(threadSleepFor);
                    }
                    delete dat;
                }
            });
        serverIncomingExecuter.detach();

        std::thread serverOutgoingSender(
            [this]()
            {
                bool b_go = false;
                crbn::serialiser *dat = nullptr;
                crbn::simpleNetworking *net = new crbn::simpleNetworking;
                crbn::jsn::Json_Helper j;
                j.init(crbn::jsn::configFileName, crbn::jsn::jsonLiterals::config);
                

                for (;;)
                {
                    b_go = false;
                    dat = nullptr;
                    {
                        std::scoped_lock<std::mutex> lock(m_outgoingTasksQueue);
                        if (outgoingTasksQueue.size() > 0)
                        {
                            b_go = true;
                            dat = new crbn::serialiser(outgoingTasksQueue.front().c_bodyStr(),
                                                       outgoingTasksQueue.front().bodySize(),
                                                       outgoingTasksQueue.front().operation(),
                                                       outgoingTasksQueue.front().ticket());
                            outgoingTasksQueue.pop();
                        }
                    }
                    if (b_go)
                    {
                        if (dat != nullptr)
                        {
                            j.init();
                            auto xyz = j.setSafeModeUnsafe();

                            net->i_port = net->i_serverToClientPort;

                            // json j = crbn::jsn::jsonConfigRead(jsn::configFileName);
                            // net->c_ip = j["client_ip"].get<std::string>();

                            net->c_ip = j.strGet(crbn::jsn::keys::client_ip);
                            net->clientStart();

                            crbn::log("crbn::Jobs::ServerOutGoingSender about to send data");
                            int temp = net->sendData(dat);
                            if (temp == 0)
                            {
                                crbn::log("crbn::Jobs::async_serverStart failed sent data from server to client");
                            }
                            else
                            {
                                crbn::log("crbn::Jobs::async_serverStart data succsessfuly sent and authenticated to client");
                            }
                        }
                    }
                    else
                    {
                        std::this_thread::sleep_for(threadSleepFor);
                    }
                    delete dat;
                }
            });
        serverOutgoingSender.detach();
    }
#endif

    int Jobs::enque(serialiser dat, std::queue<crbn::serialiser> queue, std::mutex mutex)
    {
        std::lock_guard<std::mutex> lock(mutex);

        queue.push(dat);

        return true;
    }

    int Jobs::enqueOUT(crbn::serialiser *dat)
    {
        crbn::log("crbn::Jobs::enqueOUT waiting to aquire Lock ");
        std::lock_guard<std::mutex> lock(m_outgoingTasksQueue);
        if (dat != nullptr)
            outgoingTasksQueue.push(*dat);
        else
            return false;
        crbn::log("crbn::Jobs::enqueOUT written data to queue ");
        return true;
    }

    int Jobs::enqueIN(crbn::serialiser *dat)
    {
        crbn::log("crbn::Jobs::enqueIN waiting to aquire Lock ");
        std::lock_guard<std::mutex> lock(m_incomingTasksQueue);
        if (dat != nullptr)
            incomingTasksQueue.push(*dat);
        else
            return false;
        crbn::log("crbn::Jobs::enqueIN written data to queue ");
        return true;
    };
};

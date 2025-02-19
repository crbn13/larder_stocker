#include "headers/crbn_simpleNetworking.hpp"

namespace crbn
{
    bool simpleNetworking::clientStart()
    {
        if (svr)
            return false;
        else
            client = true;
        return true;
    }

    int simpleNetworking::clientStart(const std::string ip, const int port)
    {
        if (svr)
            return false;
        else
            client = true;

        c_ip = ip;
        i_port = port;

        return 1;
    }

    /// @brief
    /// @param socket
    /// @return
    int simpleNetworking::sendConfirmation(asio::ip::tcp::socket *socket, uint32_t ticket)
    {
        crbn::log("crbn::simpleNetworkign::sendConfirmation CALLED");
        if (socket == nullptr)
        {
            crbn::log("crbn::simpleNetworkign::sendConfirmation BAD ERROR SOCKET IS NULLPTR");
            return crbn::err::UNEXPECTED_NULL_PTR;
        }
        if (!socket->is_open())
        {
            crbn::log("crbn::simpleNetworkign::sendConfirmation BAD ERROR SOCKET CLOSED");
            return crbn::err::NO_SOCKET_CONNECTION;
        }
        crbn::serialiser *net = new crbn::serialiser(crbn::op::SERVER_RECIEVED_REQUEST, ticket);
        // crbn::log(" sendconfirmation ; bodysize, op, ticket  : ", false);
        // crbn::log(+net->bodySize(), false);
        // crbn::log(" ", false);
        // crbn::log(+net->operation(), false);
        // crbn::log(" ", false);
        // crbn::log(+net->ticket());

        socket->write_some(asio::buffer(net->rawDatOut(), net->size()), ec);
        crbn::log(" sendConfirmation : confirmation Sent ");
        socket->close();

        // #ifdef LOGGING
        //
        // auto func = [&]() -> char *
        // {
        // char *str = new char[net->size()];
        // str = (char *)net->rawDatOut();
        // return str;
        // };
        // crbn::log(func());
        // #endif
        if (socket != nullptr)
            delete socket;
        return 1;
    }

    /// @brief
    /// @param socket
    /// @return
    int16_t simpleNetworking::recieveConfirmation(asio::ip::tcp::socket *socket, const uint32_t ticket)
    {
        crbn::log("crbn::simpleNetworking::recieveConfirmation | started waiting ");

        if (!socket->is_open())
            return crbn::err::NO_SOCKET_CONNECTION;

        auto tStart = clock(); // clock used to timeout connected users which have not responded data for too long
        auto start = std::chrono::high_resolution_clock::now();

        auto bytes = socket->available();

        uint8_t *headerArray = new uint8_t[serc::HEADER_SIZE_T]; // array to store recieved data

        while (bytes < serc::HEADER_SIZE_T)
        {
            bytes = socket->available();
            if (std::chrono::duration_cast<std::chrono::milliseconds>(std::chrono::high_resolution_clock::now() - start) > std::chrono::milliseconds(ms_networkTimeOut))
            // if no information has been recieved then ends connection
            {
                crbn::log(" simpleNet : confrmation NOT recieved (timed out) ");
                return crbn::err::NETWORK_TIMED_OUT;
            }
            std::this_thread::sleep_for(std::chrono::milliseconds(10));
        }

        if (bytes >= serc::HEADER_SIZE_T)
        {
            socket->read_some(asio::buffer(headerArray, serc::HEADER_SIZE_T), ec);
            socket->close();

            crbn::log("crbn::simpleNetworking::recieveConfirmation() : reading header ");

            crbn::serialiser dat(headerArray, false);

            if ((dat.bodySize() == (size_t)0) and (dat.operation() == crbn::op::SERVER_RECIEVED_REQUEST) and (dat.ticket() == ticket))
            {
                crbn::log("crbn::simpleNetworking::recieveConfirmation() conmfirmation Recieved ! ");
                return true;
            }
            // else if (dat.bodySize() != 0)
            // {
            // }
            else
            {
                return crbn::err::INCORRECT_DATA_SENT;
            }
        }

        crbn::log(" simpleNet : confirmation NOT recieved (timed out) ");
        return crbn::err::INCORRECT_DATA_SENT;
    }

    
    int simpleNetworking::sendData(crbn::serialiser *dat)
    {
        bool done = false;
        auto temp = 0;
        while (!done)
        {
            temp = sendDataNewConnection(dat);
            if (temp > 0)
            {
                done = true;
            }
            else
            {
                std::this_thread::sleep_for(std::chrono::milliseconds(300));
                if (!crbn::err::errorCheck(temp))
                {
                    return false;
                }
            }
            crbn::log(" sendData : looping :p");
        }
        return 1;
    }

    /// @brief sends raw data, array of bytes with size of size
    /// @param data
    /// @param size
    /// @return
    int simpleNetworking::sendDataNewConnection(crbn::serialiser *dat)
    {

        // std::cout << " defined socket \n tyring to define endpoint\n";
        asio::ip::tcp::endpoint endpoint(asio::ip::make_address(c_ip.c_str(), ec), i_port);
        asio::ip::tcp::socket socket(context);
        socket.connect(endpoint, ec);

        if (!ec)
        {
            crbn::log(" simpleNet : senddata : no error code, socket connected to server ");
        }
        else
        {
            crbn::log(ec);
            return crbn::err::NO_SOCKET_CONNECTION;
        }

        if (socket.is_open())
        {
            crbn::log(" crbn::simpleNetworking::SENDdata:: BYTES WRITTEN = " +
                      std::to_string(
                          socket.write_some(asio::buffer(dat->rawDatOut(), dat->size()), ec)));

            if (ec)
            {
                crbn::log(ec);
                return -1;
            }
            else
            {
                crbn::log("simpleNetworking : wrote " + std::to_string(dat->size()) + " bytes ");

                // uint8_t *arr = dat->c_bodyStr();
                // std::cout << "BODY STRING dat IN NEt SEND DATA : ";
                // for (size_t i = 0; i < dat->bodySize(); i++)
                // {
                //     std::cout << +arr[i] << " ";
                // }
                // std::cout << std::endl;

                return recieveConfirmation(&socket, dat->ticket());
            }
        }
        else
        {
            return crbn::err::NO_SOCKET_CONNECTION;
        }

        return 1;
    }

    /// @brief
    /// @param datLock mutext to protect dual writing of data serialiser
    /// @param dat pointer to data serialiser class instance
    /// @param loopIndef loops indefinitely true/false
    /// @param sendConfirmation boolean, sends confirmation true or false
    /// @return returns positive if success , if failiure, returns crbn::err code ( NOT IMPLEMENTED YET )
    int simpleNetworking::serverStart(std::mutex &datLock, crbn::serialiser *dat, const bool loopIndef, bool b_sendConfirmation)
    {
        std::scoped_lock<std::mutex> lock(datLock);

        if (dat == nullptr)
        {
            crbn::log(" serverStart failed : dat was nullptr ");
            return false;
        }
        // std::lock_guard<std::mutex> lock(datLock);

        if (client)
            return false;
        else
            svr = true;

        asio::error_code ec;
        // create a "context " essentially the platform specific interface
        asio::io_context context;

        asio::ip::tcp::acceptor *acceptor = nullptr;
        asio::ip::tcp::socket *socket = nullptr;
        do
        {

            bool done = false;
            while (!done)
            {
                try
                {
                    acceptor = new asio::ip::tcp::acceptor(context, asio::ip::tcp::endpoint(asio::ip::tcp::v4(), i_port));
                    done = true;
                }
                catch (std::exception &e)
                {
                    crbn::log("crbn::simpleNetworking::serverStart() caught error trying to generate acceptor");
                    std::this_thread::sleep_for(std::chrono::milliseconds(500));
                    std::cerr << "\n"
                              << e.what() << " | port = " << i_port << "\n";
                }

                catch (...)
                {
                    crbn::log("crbn::simpleNetworking::serverStart() caught error trying to generate acceptor");
                }
            }

            done = false;
            while (!done)
            {
                try
                {

                    socket = new asio::ip::tcp::socket(context);
                    done = true;
                }
                catch (std::exception &e)
                {
                    crbn::log("crbn::simpleNetworking::serverStart() caught error trying to generate socket");
                    std::this_thread::sleep_for(std::chrono::milliseconds(500));
                    std::cerr << "\n"
                              << e.what() << " | port = " << i_port << "\n";
                }
                catch (...)
                {
                    crbn::log("crbn::simpleNetworking::serverStart() caught error trying to generate socket");
                }
            }
            crbn::log("crbn::simpleNetworking::serverStart() acceptor generated");
            crbn::log("crbn::simpleNetworking::serverStart() socket generated");

            crbn::log("crbn::simpleNetworking::serverStart() acceptor waiting for connection (after this message)");

            done = false;
            while (!done)
            {
                try
                {
                    acceptor->accept(*socket);
                    done = true;
                }
                catch (...)
                {
                    crbn::log("crbn::simpleNetworking::serverStart() caught error trying to accpe socket");
                }
            }

            crbn::log(" simpleNet : serverStart : connection accepted ");

            // save client ip to config file

            asio::ip::tcp::endpoint enp;
            enp = socket->remote_endpoint(ec);
            std::string tempip = enp.address().to_string(ec);
            std::cout << "                  CLIENT IP ADDRESS == " << tempip << std::endl;

            crbn::jsn::Json_Helper j;

            j.init(crbn::jsn::configFileName, crbn::jsn::jsonLiterals::config);
            j.lock();
            // json j = crbn::jsn::jsonConfigRead(crbn::jsn::configFileName);
            j.json_write(crbn::jsn::keys::client_ip, tempip);
            // j["client_ip"] = tempip;
            j.unlock();
            // std::ofstream file(jsn::configFileName);
            // file << std::setw(4) << j;
            // file.close();
            // done

            size_t bytes = socket->available(); // chech how many bytes are available to be read from the scoket

            auto tStart = clock(); // clock used to timeout connected users which have not responded data for too long
            auto start = std::chrono::high_resolution_clock::now();

            while (bytes < crbn::serc::HEADER_SIZE_T) // dont read any data untill enough data has been recieved for a header to be constructed
            {                                         // more documentation in dataTransfer.hpp
                bytes = socket->available();
                // // crbn::log(" simpleNet : server : waiting for enough bytes to read header ");
                // if ((clock() - tStart) > 100000) // if no information has been recieved then ends connection
                // {
                //     break;
                // }
                if (std::chrono::duration_cast<std::chrono::milliseconds>(std::chrono::high_resolution_clock::now() - start) > std::chrono::milliseconds(ms_networkTimeOut))
                {
                    crbn::log("crbn::simpleNetworking::serverStart | Timed OUT waiting for Header");
                    break;
                }
            }

            if (bytes >= crbn::serc::HEADER_SIZE_T)
            {
                // Read header , decode data

                /// define array to write recieved data to;
                uint8_t *headerArr = new uint8_t[crbn::serc::HEADER_SIZE_T]{0};

                socket->read_some(asio::buffer(headerArr, crbn::serc::HEADER_SIZE_T), ec);

                dat->rawHeaderIn(headerArr, false);
                crbn::log("simpleNetworking : header read :. bodysize = " + std::to_string(+dat->bodySize()));
                // networkingData data(headerArr); // class instance of networking data to unpakage data recieved;

                // std::cout << " header read " << std::endl;

                auto start = std::chrono::high_resolution_clock::now(); // clock used to timeout connected users which have not responded data for too long
                bytes = socket->available();
                long int bytesRead = 0;

                if (dat->bodySize() > 0)
                {
                    uint8_t *bodyArr = new uint8_t[dat->bodySize()]{0};

                    while (bytesRead < dat->bodySize())
                    {
                        bytes = socket->available();
                        if (bytes > 0) // resets clock so that it doesnt time out if new bytes have been sent but they are taking a while
                            start = std::chrono::high_resolution_clock::now();
                        if ((bytes + bytesRead) < dat->bodySize()) //
                            socket->read_some(asio::buffer(bodyArr + bytesRead, bytes), ec);
                        else
                            socket->read_some(asio::buffer(bodyArr + bytesRead, dat->bodySize() - bytesRead), ec);
                        bytesRead += bytes;
                        auto end = std::chrono::high_resolution_clock::now();

                        if (std::chrono::duration_cast<std::chrono::milliseconds>(end - start) > std::chrono::milliseconds(ms_networkTimeOut)) // if no information has been recieved then ends connection
                        {
                            crbn::log("crbn::simpleNetworking::serverStart | TIMED OUT WHILE WAITING FOR BODY BYTES ");
                            break;
                        }
                        crbn::log(" crbn::simpleNetworking::serverStart | Waiting for body data | available bytes = " + std::to_string(bytes) + " | bytes Read = " + std::to_string(bytesRead));
                        std::this_thread::sleep_for(std::chrono::milliseconds(10));
                    }

                    if (bytesRead >= dat->bodySize()) // if enough bytes have been read, then deserialise data;
                    {
                        dat->bodyInput(bodyArr);
                    }

                    else
                    {
                        crbn::log(" simpleNet : not enough bites to read body :( | bodysize = " + std::to_string(dat->bodySize()) + " | bytes Read = " + std::to_string(bytesRead));
                        break;
                    }
                }
                // socket->close();

                if (b_sendConfirmation)
                {

                    if (acceptor != nullptr)
                        delete acceptor;
                    crbn::log(" SERVER COMPLETEd ACTIONS ");

                    return sendConfirmation(socket, +dat->ticket());
                }
                else
                {
                    if (acceptor != nullptr)
                        delete acceptor;

                    if (socket != nullptr)
                    {
                        socket->close();
                        delete socket;
                    }
                    // return true;
                }
            }
            else
            {
                socket->close();
                delete socket;
                delete acceptor;
                crbn::log("crbn::simpleNetworking::serverStart | reading header failed , not enough bytes to read +/ timed out");
                return crbn::err::NETWORK_TIMED_OUT;
            }
        } while (loopIndef);

        if (socket->is_open())
        {
            socket->close();
        }

        if (socket != nullptr)
            delete socket;
        if (acceptor != nullptr)
            delete acceptor;

        crbn::log(" SERVER COMPLETEd ACTIONS ");
        return true;
    }

    int simpleNetworking::serverStart(std::mutex &datLock, crbn::serialiser *dat, const bool loopIndef)
    {
        return serverStart(datLock, dat, loopIndef, true);
    }
};

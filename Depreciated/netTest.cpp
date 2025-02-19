#include <crbn_simpleNetworking.hpp>
#include <crbn_instructions.hpp>

#include <vector>
#include <thread>
#include <mutex>
#include <fstream>
#include <chrono>
#include <cstdlib>

int main()
{
    crbn::simpleNetworking *server;

    crbn::serialiser *srvDat = new crbn::serialiser();
    std::vector<std::thread> v_server;

    std::vector<std::thread> v_client;

    crbn::simpleNetworking *client;

    std::ofstream filee("serverReads.txt");
    filee.clear();
    filee.close();

    std::ofstream fileee("cliWrite.txt");
    fileee.clear();
    fileee.close();

    std::ifstream file("write.txt");

    std::string str;
    std::getline(file, str);

    crbn::serialiser *cliDat = new crbn::serialiser((uint8_t *)str.c_str(), str.size(), 0, 0);

    file.close();

    std::mutex mut;
    std::mutex serverMutex;

    std::mutex cliDatMutex;

    auto serverStart = [&]()
    {
        for (;;)
        {
            // std::lock_guard<std::mutex> lock(serverMutex);
            crbn::serialiser *localDat = new crbn::serialiser();
            crbn::log("server start");
            std::mutex localMutex;
            server = new crbn::simpleNetworking;

            int temp = server->serverStart(&mut, localDat, false);
            while (temp < 1)
            {
                temp = server->serverStart(&mut, localDat, false);
            }

            crbn::log(" server : Going to output raw data ");
            crbn::log(" SERVER SIDE BODYSIZE == " + std::to_string(localDat->bodySize()));
            crbn::log(" SERVER SIDE TICKET ==== " + std::to_string(localDat->ticket()));
            std::ofstream write("serverReads.txt", std::ios::app);

            uint8_t *arr = localDat->rawDatOut();

            // write.write((char*)+arr, srvDat->size());

            write << "|ticket = " << std::to_string(localDat->ticket()) << "|";
            for (int i = 0; i < localDat->size(); i++)
            {
                write << std::to_string(+arr[i]) << " ";
            }
            write << "\n";
            write.close();
            crbn::log("server end");
        }
    };
    uint32_t cliticket = 70000;

    auto clientStart = [&]()
    {
        for (int i = 0; i < 100; i++)
        {

            std::lock_guard<std::mutex> lock(cliDatMutex);
            // std::system("gnome-terminal");

            // uint32_t ticket = rand() % 6000;
            crbn::log(" client ticket = " + std::to_string(cliticket));
            crbn::serialiser *locDat = new crbn::serialiser((uint8_t *)str.c_str(), str.size(), crbn::op::SEND_C_STRING, cliticket);
            std::ofstream write("cliWrite.txt", std::ios::app);

            uint8_t *arr = locDat->rawDatOut();
            for (int i = 0; i < locDat->size(); i++)
            {
                write << std::to_string(+arr[i]) << " ";
            }
            write << std::endl;
            write.close();
            // int sleepfor = rand() % 1001;
            // std::cout << " sleeping for " << sleepfor << " miliseconds \n";
            // std::this_thread::sleep_for(std::chrono::nanoseconds(sleepfor));

            client = new crbn::simpleNetworking;
            cliticket ++;

            client->clientStart("127.0.0.1", 5473);
            crbn::log(" CLIENT SERIALISED TICKETT = " + std::to_string(+locDat->ticket()));
            crbn::log(" clientStart : about to send data");
            client->sendData(locDat);
        }
    };

    for (int i = 0; i < 1; i++)
    {
        v_server.push_back(std::thread(serverStart));
        v_server.back().detach();
    }
    for (int i = 0; i < 1; i++)
    {
        v_client.push_back(std::thread(clientStart));
        v_client.back().detach();
    }
    crbn::log(" Threads Detached ");

    for (;;)
    {
    };
}
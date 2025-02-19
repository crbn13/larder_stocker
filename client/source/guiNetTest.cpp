#include <thread>
#include <mutex>
#include <string>

#include <crbn_simpleNetworking.hpp>
#include <crbn_dataSerialiser.hpp>
#include <crbn_logging.hpp>
#include <crbn_PGE_SceneManager.hpp>
#include <crbn_errorCodes.hpp>
#include <crbn_instructions.hpp>

#define OLC_PGE_APPLICATION
#define OLC_PGEX_QUICKGUI
#include <olcPixelGameEngine.h>
#include <olcPGEX_QuickGUI.h>

#define CLIENT -1
#define MENU -3
#define SERVER -2

std::string glob_ip;

//im a comment :)

class server
{
public:
    int i_screenWidth = 0;
    int i_screenHeight = 0;

    crbn::simpleNetworking *net;
    crbn::serialiser *dat;
    //  std::thread *thread;
    std::mutex m_thrLock;
    std::thread *srvThr;
    bool threadActive = false;

    olc::vf2d textPosition = {0, 0};

    server(olc::PixelGameEngine *pge)
    {
        i_screenWidth = pge->ScreenWidth();
        i_screenHeight = pge->ScreenHeight();
        net = new crbn::simpleNetworking;
        // std::thread thr(&server::localServerStart, *this, this);
        // thr.detach();

        /*
                thread = new std::thread([this](bool threadActive, crbn::simpleNetworking *net, crbn::networkingData *dat, std::mutex m_thrLock)
                                         {
                    threadActive = true;
                    net->serverStart(&m_thrLock, dat); } );

                thread->detach();
        */
        pge->Clear(olc::BLUE);
    }

    bool serverStart = false;
    int run(olc::PixelGameEngine *pge)
    {

        if (!serverStart)
        {

            dat = new crbn::serialiser;

            srvThr = new std::thread{
                [this]()
                {
                    threadActive = true;
                    if (net->serverStart(m_thrLock, dat, false))
                    {
                        crbn::log("guiNetTest : server Done executing");
                    }
                    else
                    {
                        crbn::log("guiNetTest : something bad happened to server ");
                    }

                    crbn::log("guiNetTest : bodysize = " + std::to_string(+dat->bodySize()));

                    for (int i = 0; i < dat->bodySize(); i++)
                    {
                        crbn::log((char)+dat->bodyAccsess(i), false);
                    }
                    crbn::log("");

                    threadActive = false;
                }};
            srvThr->detach();
            serverStart = true;
        }

        else
        {
            if (!threadActive)
            {
                if (m_thrLock.try_lock())
                {
                    crbn::log("THREAD FINISHED DATA RECIEVED");
                    // crbn::log((char *)dat->bodyBegin());
                    // crbn::log((char*)dat->rawDatOut());
                    char *str = new char[dat->bodySize() + 1];
                    for (int i = 0; i < dat->bodySize(); i++)
                    {
                        str[i] = (char)dat->bodyAccsess(i);
                    }

                    str[dat->bodySize()] = (char)0;

                    if (textPosition.y >= i_screenHeight)
                    {
                        pge->Clear(olc::DARK_BLUE);
                        textPosition.y = 0;
                    }
                    pge->DrawString(textPosition, std::string(str), olc::WHITE, 1);
                    textPosition.y += 10;

                    crbn::log("guiNetTest : bodysize = " + std::to_string(+dat->bodySize()));

                    crbn::log("");

                    serverStart = false;
                    m_thrLock.unlock();
                }
            }
        }
        return 1;
    }
};

class client
{
public:
    int scrWidth;
    int scrHeight;

    olc::QuickGUI::Manager guiManager;
    std::mutex m_tb;
    olc::QuickGUI::TextBox *tb;
    olc::vf2d buttonSize = {200, 50};

    std::mutex m_datNet;

    crbn::serialiser *dat;
    crbn::simpleNetworking *nett;

    uint8_t *uint8_Array;

    std::mutex m_wait;

    client(olc::PixelGameEngine *pge)
    {
        crbn::log("client constructed");
        scrWidth = pge->ScreenWidth();
        scrHeight = pge->ScreenHeight();
        tb = new olc::QuickGUI::TextBox(guiManager, "", {(float)scrWidth / 2, (float)scrHeight / 2}, buttonSize);
        guiManager.Update(pge);
    }

    int run(olc::PixelGameEngine *pge)
    {
        guiManager.Update(pge);

        pge->Clear(olc::BLACK);

        if (pge->GetKey(olc::ENTER).bPressed)
        {

            if (tb->sText.size() != 0)
            {

                crbn::log("guiNetTest : stext.size = " + std::to_string(tb->sText.size()));
                std::string tempStr = tb->sText;

                std::atomic_bool b_thrAquiredMutex = false;

                std::thread t_sendData{
                    [&, tempStr]()
                    {
                        
                        std::lock_guard<std::mutex> lock(m_datNet);
                        b_thrAquiredMutex = true;
                        crbn::log(" m_datNet : lock aquired");

                        std::string ip = "192.168.1.110";
                        std::string ip2 = "172.20.10.3";
                        // m_wait.lock();

                        nett = new crbn::simpleNetworking();

                        nett->clientStart(glob_ip, 5473);

                        // m_wait.unlock();

                        crbn::log(" t_sendData : string size = " + std::to_string(tempStr.size()));

                        dat = new crbn::serialiser((uint8_t *)tempStr.c_str(), (size_t)tempStr.size());

                        crbn::log(" guiNetTest : networking data initialised ");

                        nett->sendData(dat);

                        crbn::log(" guiNetTest : sent data ");
                        crbn::log(" guiNetTest : t_sendData : thread Finished ");
                        crbn::log("");
                    }};

                t_sendData.detach();
                crbn::log(" guiNetTest : thread Detached");

                while (!b_thrAquiredMutex)
                {
                    std::this_thread::sleep_for(std::chrono::milliseconds(1000));
                    crbn::log(" guiNetTst : main thr : waiting for thread to aquire mutexes");
                }

                crbn::log(" guiNetTest : thread has aquired mutexes, waiting until aquisitino of other mutexes");

                tb->sText.erase();
                tb = new olc::QuickGUI::TextBox(guiManager, "", {float(scrWidth / 2), float(scrHeight / 2)}, buttonSize);
                
                // tb->m_state = olc::QuickGUI::BaseControl::State::Normal;
                tb->Enable(true);
                crbn::log(" guiNetTest : text box reset");
            }
            else
            {
                crbn::log(" guiNetTest : cannot send data of 0 size ");
                tb = new olc::QuickGUI::TextBox(guiManager, "", {float(scrWidth / 2), float(scrHeight / 2)}, buttonSize);
            }
        }

        guiManager.Update(pge);
        guiManager.Draw(pge);

        return 1;
    }
};

class menu
{
public:
    olc::QuickGUI::Manager guiManager;
    olc::QuickGUI::Button *b_server;
    olc::QuickGUI::Button *b_client;

    int scrWidth;
    int scrHeight;

    olc::vf2d buttonSize = {50, 50};

    menu(olc::PixelGameEngine *pge)
    {
        scrWidth = pge->ScreenWidth();
        scrHeight = pge->ScreenHeight();

        b_server = new olc::QuickGUI::Button(guiManager, "server", {(scrWidth / 3) - (buttonSize.x / 2), (scrHeight / 2) - buttonSize.y / 2}, buttonSize);
        b_client = new olc::QuickGUI::Button(guiManager, "client", {((scrWidth / 3) * 2) - (buttonSize.x / 2), (scrHeight / 2) - buttonSize.y / 2}, buttonSize);

        guiManager.Update(pge);
    }

    int run(olc::PixelGameEngine *pge)
    {
        guiManager.Update(pge);

        if (b_client->bPressed)
        {
            return CLIENT;
        }
        else if (b_server->bPressed)
        {
            return SERVER;
        }

        guiManager.Update(pge);
        guiManager.Draw(pge);
        // crbn::log("i ran");
        return 1;
    }
};

class Example : public olc::PixelGameEngine
{
public:
    Example()
    {
        // Name your application
        sAppName = "Example";
    }

    crbn::Screens<menu> menuScr;
    crbn::Screens<client> clientScr;
    crbn::Screens<server> serverScr;

    int num = 0;
    int numb = MENU;

    bool OnUserCreate() override
    {
        menuScr.initialize(this);

        return true;
    }

    bool OnUserUpdate(float fElapsedTime) override
    {

        if (numb == MENU)
            num = menuScr.run();
        else if (numb == CLIENT)
        {
            num = clientScr.run();
        }
        else if (numb == SERVER)
        {
            num = serverScr.run();
        }

        if (num == 1)
            return true;
        else if (num == CLIENT)
        {
            crbn::log("client selected");
            numb = CLIENT;
            clientScr.initialize(this);
            return true;
        }
        else if (num == SERVER)
        {
            crbn::log("server Selected");
            numb = SERVER;
            serverScr.initialize(this);

            return true;
        }
        else
        {
            std::cout << " waa im a crybaby program \n";
        }
        return true;
    }
};

int main(const int argc, const char **argv)
{

    if (argc == 2)
        glob_ip = std::string(argv[1]);
    else
        glob_ip = "192.168.1.110";

    crbn::log(glob_ip);

    Example demo;

    if (demo.Construct(500, 200, 1, 1, 0, 0, 0))
    {
        crbn::log("PGE : Constructed");
        demo.Start();
    }

    return 0;
}
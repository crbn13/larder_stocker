
#define OLC_PGE_APPLICATION
#include "../../lib/olcPixelGameEngine.h"
#define OLC_PGEX_QUICKGUI
#include "../../lib/olcPGEX_QuickGUI.h"

#if !defined(DATA_TRANSFER)
#include <dataTransfer.hpp>
#endif

#include <simpleNetworking.hpp>

#include <string>
#include <iostream>
#include <utility>
#include <memory>
#include <stdlib.h>
#include <chrono>
#include <thread>

int argcGlob = 0;
char **argvGlob;

class serverGUI : public olc::PixelGameEngine
{
public:
    serverGUI()
    {
        sAppName = "Server";
    }

protected:
    olc::QuickGUI::Manager guiManager;
    olc::QuickGUI::Button *exit;

public:
    bool OnUserCreate() override
    {

        exit = new olc::QuickGUI::Button(guiManager, "exit", olc::vf2d(ScreenHeight() / 2 - 50 / 2, ScreenWidth() / 2 - 50 / 2), {50, 50});
        guiManager.Update(this);
        return true;
    }
    bool OnUserUpdate(float fElapsedTime) override
    {
        guiManager.Update(this);
        Clear(olc::CYAN);

        if (exit->bPressed)
        {
            return false;
        }

        guiManager.Draw(this);
        return true;
    }
};

class clientGUI : public olc::PixelGameEngine
{
public:
    clientGUI()
    {
        sAppName = "Client";
    }

public:
    bool OnUserCreate() override
    {
        return true;
    }
    bool OnUserUpdate(float fElapsedTime) override
    {
        Clear(olc::BLUE);

        return true;
    }
};

void runClient()
{
    clientGUI engine;
    if (engine.Construct(std::stoi(argvGlob[1]), std::stoi(argvGlob[2]), std::stoi(argvGlob[3]), std::stoi(argvGlob[3])))
        engine.Start();
    else
    {
        std::cout << " WAA WAA  FAILED TO START ENGINE \n";
        std::string str;
        std::cin >> str;
    }
}

class olcEngine : public olc::PixelGameEngine
{
public:
    olcEngine()
    {
        sAppName = "bingusbongus";
    }

protected:
    olc::vi2d zerozero = {0, 0};
    olc::vi2d thirty2 = {32, 32};

    olc::QuickGUI::Manager guiManager;
    // std::unique_ptr<olc::QuickGUI::Button> bingus;
    olc::QuickGUI::Button *serverButton;
    olc::QuickGUI::Button *clientButton;

    olc::Sprite *xbutton;

    olc::QuickGUI::Label *lab;
    olc::QuickGUI::TextBox *tb;
    olc::Pixel *backgroundColour;
    int i_screenWidth = 0;
    int i_screenHeight = 0;
    olc::vi2d vi2d_spriteSize = {32, 32};
    // std::string
    bool sentData = false;
    std::thread thr;
    std::vector<std::thread> vthread;

public:
    bool OnUserCreate() override
    {
        i_screenWidth = ScreenWidth();
        i_screenHeight = ScreenHeight();
        serverButton = new olc::QuickGUI::Button(guiManager, "Server", {float((i_screenWidth / 3) - (50 / 2)), 20}, {50, 50});
        clientButton = new olc::QuickGUI::Button(guiManager, "Client", {float(((i_screenWidth / 3) * 2) - (50 / 2)), 20}, {50, 50});
        lab = new olc::QuickGUI::Label(guiManager, " enter nanme", {0, 100}, {100, 20});
        tb = new olc::QuickGUI::TextBox(guiManager, "", {0, 120}, {200, 20});

        xbutton = new olc::Sprite("../media/x.png");
        backgroundColour = new olc::Pixel(olc::VERY_DARK_GREY);

        guiManager.Update(this);
        return true;
    }

    bool OnUserUpdate(float fElapsedTime) override
    {
        guiManager.Update(this);
        guiManager.colBorder = olc::Pixel(olc::YELLOW);
        guiManager.colNormal = olc::Pixel(olc::BLACK);
        guiManager.colHover = olc::Pixel(olc::GREY);
        // make label
        // make text box
        // make button
        // print name
        Clear(*backgroundColour);
        SetPixelMode(olc::Pixel::MASK); // Dont draw pixels which have any transparency
        DrawSprite({i_screenWidth - vi2d_spriteSize.x, 0}, xbutton, 1, 0);

        DrawString({thirty2.x, 16}, std::to_string(GetMousePos().x) + " | " + std::to_string(GetMousePos().y), olc::WHITE, 1);

        /*
                if (GetMousePos().x > i_screenWidth - vi2d_spriteSize.x and GetMousePos().y < vi2d_spriteSize.y)
                {
                    FillCircle({i_screenWidth - vi2d_spriteSize.x / 2 - 1, 0 + vi2d_spriteSize.y / 2 - 1}, 15, olc::WHITE);
                    if (GetMouse(0).bPressed)
                    {
                        return false;
                    }
                }
        */

        if (clientButton->bPressed)
        {
            runClient();
        }

        if (serverButton->bPressed)
        {

            serverGUI engine;

            if (engine.Construct(std::stoi(argvGlob[1]), std::stoi(argvGlob[2]), std::stoi(argvGlob[3]), std::stoi(argvGlob[3])))
            {
                vthread.push_back(std::thread(&clientGUI::Start, &engine));
                vthread.back().detach();
            }
            else
            {
                std::cout << " WAA WAA  FAILED TO START ENGINE \n";
                std::string str;
                std::cin >> str;
            }
        }

        if (GetKey(olc::ENTER).bHeld)
        {
            if (!sentData)
            {

                simpleNetworking net;
                net.clientStart((char *)"192.168.1.110", 5473);
                networkingData dat((uint8_t *)tb->sText.c_str(), tb->sText.size());
                net.sendDataNewConnection(dat.rawDatOut(), dat.size());
            }
        }
        /*
                std::cout << tb->sText << std::endl;
                std::cout << "charnums : ";
                for (int i = 0; i < tb->sText.length(); i++)
                    std::cout << " " << (int)tb->sText[i];
                std::cout << std::endl;
        */

        guiManager.Draw(this);
        return true;
    };
};

int main(int argc, char **argv)
{
    argcGlob = argc;
    argvGlob = argv;

    olcEngine engine;
    std::thread thr;

    if (argc != 4)
        std::cout << "failiure : wrong ammount of arguments \n";
    else if (engine.Construct(std::stoi(argv[1]), std::stoi(argv[2]), std::stoi(argv[3]), std::stoi(argv[3])))
    {
        thr = std::thread(&olcEngine::Start, &engine);
        if (thr.joinable() == false)
        {
            thr.detach();
        }
        else
        {
            std::cout << " Thread Error \n";
        }
    }
    else
        std::cout << "Failed to initialise game" << std::endl;
    // engine.Construct(width, height, pixelw, pixelh, fullscreen, vsync, cohesion)
    std::cout << " game started \n";
    thr.join();
    for (;;)
    {
    }

    return 0;
}

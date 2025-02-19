

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
    olc::QuickGUI::Button *bongus;
    olc::Sprite *xbutton;
    olc::QuickGUI::Label *lab;
    olc::QuickGUI::TextBox *tb;
    olc::Pixel *backgroundColour;
    int i_screenWidth = 0;
    int i_screenHeight = 0;
    olc::vi2d vi2d_spriteSize = {32, 32};
    // std::string
public:
    bool OnUserCreate() override
    {
        i_screenWidth = ScreenWidth();
        i_screenHeight = ScreenHeight();
        bongus = new olc::QuickGUI::Button(guiManager, "hii", thirty2, {50, 50});
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
        guiManager.colNormal = olc::Pixel(olc::WHITE);

        // make label
        // make text box
        // make button
        // print name
        Clear(*backgroundColour);
        SetPixelMode(olc::Pixel::MASK); // Dont draw pixels which have any transparency
        DrawSprite({i_screenWidth - vi2d_spriteSize.x, 0}, xbutton, 1, 0);

        DrawString({thirty2.x, 16}, std::to_string(GetMousePos().x) + " | " + std::to_string(GetMousePos().y), olc::WHITE, 1);

        if (GetMousePos().x > i_screenWidth - vi2d_spriteSize.x and GetMousePos().y < vi2d_spriteSize.y)
        {
            FillCircle({i_screenWidth - vi2d_spriteSize.x / 2 - 1, 0 + vi2d_spriteSize.y / 2 - 1}, 15, olc::WHITE);
            if (GetMouse(0).bPressed)
            {
                return false;
            }
        }
        
        if (GetKey(olc::ENTER).bHeld)
        {
            simpleNetworking net;
            net.clientStart("192.168.1.110", 5476);
            networkingData dat((uint8_t *)tb->sText.c_str(), tb->sText.size());
            net.sendDataNewConnection(dat.rawDatOut(), dat.size());
        }

        std::cout << tb->sText << std::endl;
        std::cout << "charnums : ";
        for (int i = 0; i < tb->sText.length(); i++)
            std::cout << " " << (int)tb->sText[i];
        std::cout << std::endl;
        guiManager.Draw(this);
        return true;
    };
};

int main(int argc, char **argv)
{
    olcEngine engine;

    if (argc != 4)
        std::cout << "failiure \n";
    else if (engine.Construct(std::stoi(argv[1]), std::stoi(argv[2]), std::stoi(argv[3]), std::stoi(argv[3])))
        engine.Start();
    else
        std::cout << "Failed to initialise game" << std::endl;
    // engine.Construct(width, height, pixelw, pixelh, fullscreen, vsync, cohesion)
    return 0;
}


#ifndef CRBN_GUI
#define CRBN_GUI

// #include "client.hpp"

// #include "pgePLUSqgui.hpp"
// #define OLC_PGE_APPLICATION
#include "../olcPixelGameEngine.h"
#include "../olcPGEX_QuickGUI.h"

#include "crbn_jobManager.hpp"
#include "crbn_PGE_SceneManager.hpp"

#include "crbn_screens.hpp"
#include "crbn_json.hpp"


std::string glob_ip;

class Gui : public olc::PixelGameEngine
{
public:
    
    Gui();

    crbn::Jobs job;

    crbn::scr::Screens_ExtraClass<crbn::scr::MainMenu, crbn::Jobs> mainMenu;
    crbn::scr::Screens_ExtraClass<crbn::scr::ScannInput, crbn::Jobs> scannInput;

    void delScreens();

    int activeScreen = 0;
    int changeScreen = 0;

    bool OnUserCreate() override;

    bool OnUserUpdate(float fElapsedTime) override;
    
};

namespace crbn
{
    void guiStart(const std::string &ipAddress);

    void guiStart();
}

int main();

#endif

// #include "../crbn_gui.cpp"
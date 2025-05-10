#ifndef CRBN_GUI_HPP
#define CRBN_GUI_HPP

#include "../olcPixelGameEngine.h"
#include "../olcPGEX_QuickGUI.h"

#include "crbn_jobManager.hpp"
#include "crbn_PGE_SceneManager.hpp"

#include "crbn_screens.hpp"
#include "crbn_json.hpp"


inline std::string glob_ip;

class Gui : public olc::PixelGameEngine
{
public:
    
    Gui();

    std::shared_ptr<crbn::Client_Jobs> job;

    crbn::scr::Screens_ExtraClass<crbn::scr::MainMenu, crbn::Client_Jobs*> mainMenu;
    crbn::scr::Screens_ExtraClass<crbn::scr::ScannInput, std::shared_ptr<crbn::Client_Jobs>> scannInput;
    crbn::scr::Screens_ExtraClass<crbn::scr::ShoppingList, std::shared_ptr<crbn::Client_Jobs>> shoppingList;

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

// int main();

#endif

// #include "../crbn_gui.cpp"

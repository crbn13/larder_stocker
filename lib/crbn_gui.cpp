#include "headers/crbn_gui.hpp"

Gui::Gui()
    : job(new crbn::Client_Jobs)
{
    // Name your application
    sAppName = "Example";
}

void Gui::delScreens()
{
    mainMenu.del();
    scannInput.del();
    shoppingList.del();
}

bool Gui::OnUserCreate()
{
    job->async_clientStart(glob_ip);
    activeScreen = crbn::scr::active_screen::MAIN_MENU;
    changeScreen = activeScreen;
    mainMenu.initialize(this, job.get());

    return true;
}

bool Gui::OnUserUpdate(float fElapsedTime)
{
    if (changeScreen == crbn::scr::active_screen::QUIT_PROGRAM)
        return false;

    if (changeScreen == crbn::scr::no_screen_change)
    {
        if (activeScreen == crbn::scr::active_screen::MAIN_MENU)
        {
            changeScreen = mainMenu.run(&fElapsedTime);
        }
        else if (activeScreen == crbn::scr::active_screen::SCANN_INPUT)
        {
            changeScreen = scannInput.run(&fElapsedTime);
        }
        else if (activeScreen == crbn::scr::active_screen::SHOPPING_LIST)
        {
            changeScreen = shoppingList.run(&fElapsedTime);
        }
        return true;
    }
    else if (changeScreen == crbn::scr::active_screen::MAIN_MENU)
    {
        crbn::log(" changing screen to main menu");
        delScreens();
        activeScreen = crbn::scr::active_screen::MAIN_MENU;
        mainMenu.initialize(this, job.get());
        changeScreen = crbn::scr::no_screen_change;
    }
    else if (changeScreen == crbn::scr::active_screen::SCANN_INPUT)
    {
        crbn::log(" changing screen to ScannInput");
        delScreens();
        activeScreen = crbn::scr::active_screen::SCANN_INPUT;
        scannInput.initialize(this, job);
        changeScreen = crbn::scr::no_screen_change;
    }
    else if (changeScreen == crbn::scr::active_screen::SHOPPING_LIST)
    {
        crbn::log(" changing screen to shopping list");
        delScreens();
        activeScreen = crbn::scr::active_screen::SHOPPING_LIST;
        shoppingList.initialize(this, job);
        changeScreen = crbn::scr::no_screen_change;
    }
    else
        changeScreen = crbn::scr::no_screen_change;
    return true;
}

namespace crbn
{
    void guiStart(const std::string &ipAddress)
    {

        glob_ip = ipAddress;

        crbn::log(glob_ip);

        Gui demo;

        // demo.Construct()
        if (demo.Construct(1000, 500, 1, 1) );
        {
            crbn::log("PGE : Constructed");
            demo.Start();
        }
    }
    void guiStart()
    {
        crbn::jsn::Json_Helper j;
        j.init("config.json", crbn::jsn::jsonLiterals::config);
        auto lock = j.setSafeModeUnsafe();
        guiStart(j.strGet("server_ip"));
    }
}

// int main()
// {
//     crbn::guiStart();
//     return 0;
// }

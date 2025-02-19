#include <crbn_jobManager.hpp>
#include <crbn_PGE_SceneManager.hpp>
#include <crbn_screens.hpp>

std::string glob_ip;

class Example : public olc::PixelGameEngine
{
public:
    Example()
    {
        // Name your application
        sAppName = "Example";
    }

    crbn::Jobs job;

    crbn::scr::Screens_ExtraClass<crbn::scr::MainMenu, crbn::Jobs> mainMenu;
    crbn::scr::Screens_ExtraClass<crbn::scr::ScannInput, crbn::Jobs> scannInput;

    void delScreens()
    {
        mainMenu.del();
        scannInput.del();
    }

    int activeScreen = 0;
    int changeScreen = 0;

    bool OnUserCreate() override
    {
        job.async_clientStart(glob_ip);
        activeScreen = crbn::scr::active_screen::MAIN_MENU;
        changeScreen = activeScreen;
        mainMenu.initialize(this, &job);

        return true;
    }

    bool OnUserUpdate(float fElapsedTime) override
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
            return true;
        }
        else if (changeScreen == crbn::scr::active_screen::MAIN_MENU)
        {

            crbn::log(" changing screen to main menu");
            delScreens();
            activeScreen = crbn::scr::active_screen::MAIN_MENU;
            mainMenu.initialize(this, &job);
            changeScreen = crbn::scr::no_screen_change;
        }
        else if (changeScreen == crbn::scr::active_screen::SCANN_INPUT)
        {
            crbn::log(" changing screen to ScannInput");
            delScreens();
            activeScreen = crbn::scr::active_screen::SCANN_INPUT;
            scannInput.initialize(this, &job);
            changeScreen = crbn::scr::no_screen_change;
        }
        else
            changeScreen == crbn::scr::no_screen_change;
        return true;
    }
};

int main(const int argc, const char **argv)
{

    // crbn::jobs serverJob;
    // serverJob.async_serverStart();

    if (argc == 2)
    {
        crbn::log(" GLOB ASSIGNED TO ARG PERMS");
        glob_ip = std::string(argv[1]);
    }
    else
        glob_ip = "127.0.0.1";

    crbn::log(glob_ip);

    Example demo;

    if (demo.Construct(1000, 500, 4, 4, 0, 0, 1))
    {
        crbn::log("PGE : Constructed");
        demo.Start();
    }

    return 0;
}
// #pragma once
#ifndef CRBN_SCREENS
#define CRBN_SCREENS

#include "../olcPixelGameEngine.h"
#include "../olcPGEX_QuickGUI.h"
#include "crbn_json.hpp"
#include "crbn_jobManager.hpp"


// #include "crbn_PGE_SceneManager.hpp"
// #include "crbn_jobManager.hpp"


namespace crbn
{
    /// @brief screen related activities :p
    namespace scr
    {
        enum active_screen
        {
            MAIN_MENU = 1,
            SETTINGS,
            SCANN_INPUT,
            INITIALISE_DATA,
            /// @brief this is more of a module used to go over the top of other screens
            KEYBOARD_OVERLAY,
            QUIT_PROGRAM
        };

        const int no_screen_change = 0;
        
        olc::vi2d posFinder3d(std::pair<int, int> elements, std::pair<int, int> relativePosition, std::pair<int, int> screen_Dimensions);

        class MainMenu
        {
        private:
            olc::QuickGUI::Manager manager;
            olc::QuickGUI::Button *buttonToInpScr = nullptr;
            olc::QuickGUI::Button *buttonToOptns = nullptr;
            olc::QuickGUI::Button *buttonQuit = nullptr;

            float WidthScreen = 0.0f;
            float HeightScreen = 0.0f;

            float howManyItemsVertiacal = 0.0f;

            olc::vf2d buttonSize = {100, 100};

            olc::vf2d posFinder(float position)
            {
                return {+WidthScreen / +14.0f, (+HeightScreen / +howManyItemsVertiacal) * position};
            }

            std::pair<int, int> elements = {0, 0};
            std::pair<int, int> screenDims = {0, 0};

            float frameTime = 0.0f;

            json jConfig;

        public:
            MainMenu(olc::PixelGameEngine *pge, crbn::Jobs *jobs);

            int run(float *felapsedTime, olc::PixelGameEngine *pge, crbn::Jobs *jobs);

        };

        class ScannInput
        {
        private:
            olc::QuickGUI::Manager guiManager;
            olc::QuickGUI::TextBox *tb = nullptr;
            olc::QuickGUI::Button *clearTextEntry = nullptr;
            olc::QuickGUI::Button *confirmTextInput = nullptr;
            olc::QuickGUI::Button *returnToMainMenu = nullptr;
            olc::QuickGUI::CheckBox *toggleSetname = nullptr;
            olc::QuickGUI::CheckBox *toggleGetImage = nullptr;

            olc::Sprite *image = nullptr;
            std::string imageName;
            olc::Decal *decalImage = nullptr;

            float widthScreen = 0.0f;
            float heightScreen = 0.0f;

            olc::vf2d buttonSize = {100, 100};

            std::pair<int, int> elements = {0, 0};
            std::pair<int, int> screenDims = {0, 0};

            float howManyItemsVertiacal = 0.0f;
            float howManyItemsHorisontal = 0.0f;

            std::vector<std::string> tempJsons;
            std::vector<std::string> expectedJsons;

            int toStopLag = 0;

        private:
            std::string selfTextInputString;

            void rawTextInputUpdate(olc::PixelGameEngine *pge);

            void tbInit();

            bool imageSelected = false;

        public:

            ScannInput(olc::PixelGameEngine *pge, crbn::Jobs *jobs);
            
            int run(float *felapsedTime, olc::PixelGameEngine *pge, crbn::Jobs *jobs);
            
        };

    }
}


#endif

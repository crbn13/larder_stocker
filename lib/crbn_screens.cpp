#include "headers/crbn_screens.hpp"

namespace crbn
{
    /// @brief screen related activities :p
    namespace scr
    {
        
    olc::vi2d posFinder3d(std::pair<int, int> elements, std::pair<int, int> relativePosition, std::pair<int, int> screen_Dimensions)
    {
        return {((screen_Dimensions.first / elements.first) * relativePosition.first), ((screen_Dimensions.second / elements.second) * relativePosition.second)};
    }
    
    MainMenu::MainMenu(olc::PixelGameEngine *pge, crbn::Jobs *jobs)
    {
        manager.textScale = 3;
        manager.Update(pge);
        jConfig = crbn::jsn::jsonConfigRead();
        std::ofstream file(crbn::jsn::configFileName);
        file << std::setw(4) << jConfig;
        file.close();
        
        howManyItemsVertiacal = 7;
        WidthScreen = pge->ScreenWidth();
        HeightScreen = pge->ScreenHeight();
        
        buttonSize.x = WidthScreen / howManyItemsVertiacal - 15;
        buttonSize.y = HeightScreen / howManyItemsVertiacal - 15;
        // buttonSize = {100, 100};
        buttonToInpScr = new olc::QuickGUI::Button(manager, "Add items", posFinder(1), buttonSize);
        buttonToOptns = new olc::QuickGUI::Button(manager, "Options", posFinder(2), buttonSize);
        buttonQuit = new olc::QuickGUI::Button(manager, "QUIT", posFinder(6), buttonSize);
        
        manager.Update(pge);
        jobs->checkServerConnection();
        pge->Clear(olc::BLUE);
    }
    
    int MainMenu::run(float *felapsedTime, olc::PixelGameEngine *pge, crbn::Jobs *jobs)
    {
        // frameTime += *felapsedTime;
        // if (frameTime > .5)
        // {
            //     pge->Clear(olc::WHITE);
            //     frameTime == 0;
            // }
            pge->Clear(olc::BLUE);
            
            if (jobs->isServerConnectable)
            {
                pge->DrawString(posFinder3d({7, 7}, {5, 1}, {WidthScreen, HeightScreen}), "CONNECTED", olc::WHITE, 3);
            }
            
            if (buttonQuit->bPressed)
            {
                return active_screen::QUIT_PROGRAM;
            }
            if (buttonToInpScr->bPressed)
            {
                return active_screen::SCANN_INPUT;
            }
            if (buttonToOptns->bPressed)
            {
                return active_screen::SETTINGS;
            }
            
            manager.Update(pge);
            manager.Draw(pge);
            
            return crbn::scr::no_screen_change;
        }
        
        void ScannInput::tbInit()
        {
            // tb = new olc::QuickGUI::TextBox(guiManager, "", posFinder3d(elements, {1, 1}, screenDims), {buttonSize.x * 5, buttonSize.y});
        }
        
        ScannInput::ScannInput(olc::PixelGameEngine *pge, crbn::Jobs *jobs)
        {
            pge->TextEntryEnable(true, "");
            howManyItemsVertiacal = 8.0f;
            howManyItemsHorisontal = 8.0f;
            
            widthScreen = pge->ScreenWidth();
            heightScreen = pge->ScreenHeight();
            
            elements = {howManyItemsHorisontal, howManyItemsVertiacal};
            screenDims = {widthScreen, heightScreen};
            
            buttonSize.x = widthScreen / howManyItemsVertiacal - 15.0f;
            buttonSize.y = heightScreen / howManyItemsVertiacal - 15.0f;
            
            jobs->async_clientStart();
            tbInit();
            clearTextEntry = new olc::QuickGUI::Button(guiManager, "CLEAR", posFinder3d(elements, {1, 2}, screenDims), buttonSize);
            confirmTextInput = new olc::QuickGUI::Button(guiManager, "Confirm", posFinder3d(elements, {1, 3}, screenDims), buttonSize);
            returnToMainMenu = new olc::QuickGUI::Button(guiManager, "Exit", posFinder3d(elements, {1, 7}, screenDims), buttonSize);
            toggleSetname = new olc::QuickGUI::CheckBox(guiManager, "SETNAME", false, posFinder3d(elements, {2, 3}, screenDims), buttonSize);
            toggleGetImage = new olc::QuickGUI::CheckBox(guiManager, "GET_IMAGE", false, posFinder3d(elements, {3, 3}, screenDims), buttonSize);
            
            // std::string garry = "x.png";
            // image = new olc::Sprite("1234.png");
            
            guiManager.Update(pge);
        }
        
        void ScannInput::rawTextInputUpdate(olc::PixelGameEngine *pge)
        {
            const auto &c = pge->GetKeyPressCache();
            if (c.size() > 0)
            {
                const auto *vector = pge->GetKeyboardMap();
                for (const auto &key : c)
                for (const auto &x : *vector)
                {
                    if (std::get<0>(x) == key)
                    {
                        crbn::log("crbn::scr::ScannInput::rawTextInputUpdate key equals : " + std::to_string(key) + "\n");
                        // if (pge->GetKey(olc::BACK).bPressed)
                        // {
                            //     std::cout << "erasing char \n";
                            //     selfTextInputString.erase(selfTextInputString.size() - 1, 1);
                            // }
                            // else
                            selfTextInputString.append(std::get<1>(x));
                        }
                        else
                        continue;
                    }
                }
            }
            
            int ScannInput::run(float *felapsedTime, olc::PixelGameEngine *pge, crbn::Jobs *jobs)
        {
            toStopLag++;
            // selfTextInputString = tb->sText;
            pge->Clear(olc::BLACK);
            
            rawTextInputUpdate(pge);
            // crbn::log(" self text input string == " + selfTextInputString);
            pge->DrawString(posFinder3d(elements, {1, 1}, screenDims), selfTextInputString, olc::WHITE, 3);
            
            // tb->sText = selfTextInputString;
            
            if (image != nullptr)
            {
                // pge->DrawSprite(posFinder3d(elements, {3, 4}, screenDims), image, 1, 0);
                pge->DrawDecal(posFinder3d(elements, {3, 4}, screenDims), decalImage, olc::vf2d(float(200.0F / float(image->Size().x)), float(200.0F / float(image->Size().y))));
            }
            
            if (returnToMainMenu->bPressed)
            {
                return crbn::scr::active_screen::MAIN_MENU;
            }
            if (clearTextEntry->bPressed)
            {
                // tb->sText = "";
                selfTextInputString = "";
            }
            
            if (pge->GetKey(olc::ENTER).bPressed or confirmTextInput->bPressed)
            {
                
            std::string tempstr = selfTextInputString;
            
            crbn::serialiser *dat = new crbn::serialiser((uint8_t *)tempstr.c_str(), tempstr.size(), crbn::op::SELECT_KEY, jobs->getTicket());
            // jobs->enqueOUT(dat);
            
            if (toggleSetname->bChecked)
            {
                // dat = new crbn::serialiser((uint8_t *)tempstr.c_str(), tempstr.size(), crbn::op::SELECT_KEY, jobs->getTicket());
                // jobs->enqueOUT(dat);
                dat = new crbn::serialiser((uint8_t *)tempstr.c_str(), tempstr.size(), crbn::op::SET_NAME, jobs->getTicket());
                jobs->enqueOUT(dat);
            }
            else if (toggleGetImage->bChecked)
            {
                dat = new crbn::serialiser((uint8_t *)tempstr.c_str(), tempstr.size(), crbn::op::IMAGE_REQUEST, jobs->getTicket());
                jobs->enqueOUT(dat);
                imageName = tempstr;
            }
            else
            {
                expectedJsons.push_back(tempstr);
                dat = new crbn::serialiser((uint8_t *)tempstr.c_str(), tempstr.size(), crbn::op::ID_ENQUIRY, jobs->getTicket());
                jobs->enqueOUT(dat);
                dat = new crbn::serialiser((uint8_t *)tempstr.c_str(), tempstr.size(), crbn::op::SELECT_KEY, jobs->getTicket());
                jobs->enqueOUT(dat);
            }
            
            crbn::log(" \t\t\t\t\tenqued outgoing data");
            
            // crbn::log(" hello are you there ??? ");
            
            // tempJsons.push_back(jobs->getJsonData(tempstr)->dump());
            
            selfTextInputString.erase();
            // tb->sText.erase();
            // tb->Update(pge);
            
            // guiManager.Update(pge);
            // guiManager.Draw(pge);
            
            tbInit();
        }
        
        if (toStopLag > 1000)
        {
            toStopLag = 0;
            crbn::log(" CHECKING IF IMAGE EXISTS | IMAGE == " + imageName + ".png");
            if (std::filesystem::exists(imageName + ".png"))
            {
                image = new olc::Sprite(imageName + ".png");
                decalImage = new olc::Decal(image);
                // decalImage->vUVScale = {0.5, 0.5};
                
                if (image != nullptr)
                imageSelected = true;
            }
        }
        // tb->sText = selfTextInputString;
        
        for (int i = 0; i < expectedJsons.size(); i++)
        {
            if (jobs->isJsonAvailable(expectedJsons[i]))
            {
                tempJsons.push_back(jobs->getJsonData(expectedJsons[i])->dump());
                expectedJsons.erase(expectedJsons.begin() + i);
            }
        }
        
        for (int i = 0; i < tempJsons.size(); i++)
        {
            pge->DrawString({500, i * 15 + 300}, tempJsons[i], olc::WHITE, 1);
        }

        guiManager.Draw(pge);
        guiManager.Update(pge);
        
        return no_screen_change;
    };
    
}
}


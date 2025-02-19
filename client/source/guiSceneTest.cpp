#define OLC_PGE_APPLICATION
#include "olcPixelGameEngine.h"
#define OLC_PGEX_QUICKGUI
#include "olcPGEX_QuickGUI.h"

#include <iostream>
#include <map>
#include <vector>
#include <string>
#include <memory>
#include <utility>

class SceneData
{
public:
    olc::QuickGUI::Manager guiManager;
    olc::QuickGUI::Button *exit;

    std::vector<int64_t> vec;

    SceneData() {};
};

class scene2
{
public:
    class LocalSceneData
    {
    public:
        olc::QuickGUI::Manager guiManager;
        olc::QuickGUI::Button  *exit;
        olc::QuickGUI::TextBox *tb;
        olc::QuickGUI::Button  *button2;


        LocalSceneData() {};
    };

    scene2(olc::PixelGameEngine *engine, LocalSceneData *dat)
    {
        dat->guiManager.Update(engine);
        dat->guiManager.colBorder = olc::Pixel(olc::YELLOW);
        dat->guiManager.colNormal = olc::Pixel(olc::BLACK);
        dat->guiManager.colHover = olc::Pixel(olc::GREY);
        dat->guiManager.fHoverSpeedOff = (float)100;
        dat->button2 = new olc::QuickGUI::Button(dat->guiManager, "shutUpJamie", olc::vf2d(engine->ScreenWidth() /2 - 50/2, engine->ScreenHeight() / 4 - 50 /2), {100, 50});
        dat->exit = new olc::QuickGUI::Button(dat->guiManager, "ShutUpRosie", olc::vf2d(engine->ScreenWidth() / 2 - 50 / 2, engine->ScreenHeight() / 2 - 50 / 2), {100, 50});
        dat->guiManager.Update(engine);
    }

    int run(olc::PixelGameEngine *engine, LocalSceneData *dat)
    {
        dat->guiManager.Update(engine);
        engine->Clear(olc::GREY);

        if (dat->exit->bPressed)
        {
            return 1;
        }

        dat->guiManager.Update(engine);
        dat->guiManager.Draw(engine);
        return 2;
    };
};

class scene1
{
public:
    class LocalSceneData
    {
    public:
        olc::QuickGUI::Manager guiManager;
        olc::QuickGUI::Button *exit;

        std::vector<int64_t> vec;

        LocalSceneData() {};
    };
    scene1(olc::PixelGameEngine *engine, LocalSceneData *dat)
    {
        dat->guiManager.Update(engine);
        dat->guiManager.colBorder = olc::Pixel(olc::YELLOW);
        dat->guiManager.colNormal = olc::Pixel(olc::BLACK);
        dat->guiManager.colHover = olc::Pixel(olc::GREY);
        dat->exit = new olc::QuickGUI::Button(dat->guiManager, "exit", olc::vf2d(engine->ScreenHeight() / 2 - 50 / 2, engine->ScreenWidth() / 2 - 50 / 2), {50, 50});
        dat->guiManager.Update(engine);
    }

    int run(olc::PixelGameEngine *engine, LocalSceneData *dat)
    {

        dat->guiManager.Update(engine);
        engine->Clear(olc::CYAN);

        if (dat->exit->bPressed)
        {
            return 2;
        }

        dat->guiManager.Update(engine);
        dat->guiManager.Draw(engine);
        return 1;
    };
};

template <class ScreenClass, class DataClass>
class Screens
{
public:
    ScreenClass *scr;
    DataClass *dat;
    olc::PixelGameEngine *pge;
    bool bInit = false;

    void initialize(olc::PixelGameEngine *eng)
    {
        if (!bInit)
        {
            std::cout << " template makerer called" << std::endl;
            pge = eng;
            dat = new DataClass();
            scr = new ScreenClass(pge, dat);
            bInit = true;
        }
    }
    int run()
    {
        return scr->run(pge, dat);
    }
    void del()
    {
        bInit = false;
        delete this->dat;
        delete this->scr;
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

    int scene = 0;
    int prevScene = 0;

    /*
    class SceneClass
    {
    public:
        std::vector<scene1 *> v_scene1;
        std::vector<scene2 *> v_scene2;

        std::vector<SceneData *> data;

        int sceneOne(olc::PixelGameEngine *eng)
        {
            return v_scene1.back()->run(eng, data.back());
        };
        int sceneTwo(olc::PixelGameEngine *eng)
        {
            return v_scene2.back()->run(eng, data.back());
        }
        void desteroy()
        {
            for (int i = 0; i < v_scene1.size(); i++)
                delete v_scene1[i];
            for (int i = 0; i < v_scene2.size(); i++)
                delete v_scene2[i];
            for (int i = 0; i < data.size(); i++)
                delete data[i];
            v_scene1.clear();
            v_scene2.clear();
            data.clear();
        }
    };

    SceneClass scenes;
    */

    Screens<scene1, scene1::LocalSceneData> sceneOne;
    Screens<scene2, scene2::LocalSceneData> sceneTwo;

    std::pair<Screens<scene1, scene1::LocalSceneData> *, Screens<scene2, scene2::LocalSceneData *>> scenesPair;

    bool OnUserCreate() override
    {
        scenesPair.first = &sceneOne;
        scene = 1;
        prevScene = scene;
        sceneOne.initialize(this);
        sceneOne.run();
        return true;
    }

    bool OnUserUpdate(float fElapsedTime) override
    {
        if (scene == prevScene)
        {
            //std::cout << " re playing previous scene \n";
            if (scene == 1)
            {
                scene = sceneOne.run();
            }
            else if (scene == 2)
            {
                scene = sceneTwo.run();
            }
        }
        else
        {
            if (scene == 1)
            {
                prevScene = scene;
                sceneTwo.del();
                sceneOne.initialize(this);
                scene = sceneOne.run();

            }
            if (scene == 2)
            {
                prevScene = scene;
                sceneOne.del();
                sceneTwo.initialize(this);
                scene = sceneTwo.run();
            }
        }
        return true;
    }
};

int main()
{
    Example demo;

    if (demo.Construct(256, 240, 1, 1, 0, 0, 0))
    {
        std::cout << "constructed \n";
        demo.Start();
    }

    return 0;
}
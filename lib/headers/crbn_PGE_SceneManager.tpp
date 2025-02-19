#pragma once
#include "crbn_PGE_SceneManager.hpp"

namespace crbn
{
    namespace scr
    {

        template <class ScreenClass, class ExtraClass>
        void Screens_ExtraClass<ScreenClass, ExtraClass>::initialize(olc::PixelGameEngine *eng, ExtraClass *extra)
        {
            if (!bInit)
            {
                crbn::log("SceneManager : Scene Initialisation Called");
                pge = eng;
                ext = extra;
                scr = new ScreenClass(pge, ext);
                bInit = true;
            }
        }

        template <class ScreenClass, class ExtraClass>
        int Screens_ExtraClass<ScreenClass, ExtraClass>::run(float *felapsedTime)
        {
            return scr->run(felapsedTime, pge, ext);
        }

        template <class ScreenClass, class ExtraClass>
        void Screens_ExtraClass<ScreenClass, ExtraClass>::del()
        {
            if (bInit == true)
            {
                bInit = false;
                delete this->scr;
            }
        }

        template <class ScreenClass>
        void Screens<ScreenClass>::initialize(olc::PixelGameEngine *eng)
        {
            if (!bInit)
            {
                crbn::log("SceneManager : Scene Initialisation Called");
                pge = eng;
                scr = new ScreenClass(pge);
                bInit = true;
            }
        }
        template <class ScreenClass>
        int Screens<ScreenClass>::run(float *felapsedTime)
        {
            return scr->run(felapsedTime, pge);
        }

        template <class ScreenClass>
        void Screens<ScreenClass>::del()
        {
            if (bInit == true)
            {
                bInit = false;
                delete this->scr;
            }
        }

        template <class ScreenClass, class DataClass>
        void Screen_WithDataClass<ScreenClass, DataClass>::initialize(olc::PixelGameEngine *eng)
        {
            if (!bInit)
            {
                crbn::log("SceneManager : Scene Initialisation Called");
                pge = eng;
                dat = new DataClass();
                scr = new ScreenClass(pge, dat);
                bInit = true;
            }
        }

        template <class ScreenClass, class DataClass>
        int Screen_WithDataClass<ScreenClass, DataClass>::run(float *felapsedTime)
        {
            return scr->run(felapsedTime, pge, dat);
        }

        template <class ScreenClass, class DataClass>
        void Screen_WithDataClass<ScreenClass, DataClass>::del()
        {
            if (bInit == true)
            {
                bInit = false;
                delete this->scr;
                delete this->dat;
            }
        }

    };
}
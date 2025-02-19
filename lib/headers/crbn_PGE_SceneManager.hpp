// #pragma once

#ifndef CRBN_SCENE_SWITCH
#define CRBN_SCENE_SWITCH

#include "crbn_logging.hpp"
// #include "crbn_screens.hpp"
// #define OLC_PGE_APPLICATION
// #include "../olcPixelGameEngine.h"

// #include "../olcPixelGameEngine.h"
// #include "pgePLUSqgui.hpp"

namespace olc {
    class PixelGameEngine;
}

namespace crbn
{
    namespace scr
    {
        template <class ScreenClass, class ExtraClass>
        class Screens_ExtraClass
        {
        private:
            ScreenClass *scr;
            ExtraClass *ext;
            olc::PixelGameEngine *pge;
            bool bInit = false;

        public:
            void initialize(olc::PixelGameEngine *eng, ExtraClass *extra);

            int run(float *felapsedTime);

            void del();
        };

        template <class ScreenClass>
        class Screens
        {
        private:
            ScreenClass *scr;
            olc::PixelGameEngine *pge;
            bool bInit = false;

        public:
            void initialize(olc::PixelGameEngine *eng);

            int run(float *felapsedTime);

            void del();
        };

        template <class ScreenClass, class DataClass>
        class Screen_WithDataClass
        {
        public:
            ScreenClass *scr;
            DataClass *dat;
            olc::PixelGameEngine *pge;
            bool bInit = false;

            void initialize(olc::PixelGameEngine *eng);

            int run(float *felapsedTime);

            void del();
        };

    };
}

#include "crbn_PGE_SceneManager.tpp"

#endif
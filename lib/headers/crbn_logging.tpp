#pragma once

#include "crbn_logging.hpp"

#ifdef LOGGING

namespace crbn
{
    // void log(const std::string str)
    // {
    //     std::scoped_lock<std::mutex> lock(m_logging);
    //     std::cout << str << std::endl;
    // }

    // void log(const char *str)
    // {
    //     std::scoped_lock<std::mutex> lock(m_logging);
    //     std::cout << str << std::endl;
    // }

    template <typename T>
    void log(const T &str, bool endl)
    {
        std::scoped_lock<std::mutex> lock(m_logging);
        if (endl)
        {
            std::cout << str << std::endl;
        }
        else if (!endl)
        {
            std::cout << str << std::flush;
        }
    }
    template <typename T>
    void log(const T &str)
    {
        std::scoped_lock<std::mutex> lock(m_logging);
        std::cout << str << std::endl;
    }
}

#else

namespace crbn
{
    // void log(const char *str)
    // {
    // }
    template <typename T>
    void log(const T &, bool)
    {
    }
    template <typename T>
    void log(const T &)
    {
    }
};
#endif

namespace crbn
{
    // namespace
    // {
    //     void warn();
    // }
    namespace
    {
        inline void warn()
        {
            std::cout << "\033[0m" << std::endl;
            m_logging.unlock();
        };
    }

    template <typename T, typename... RecursiveVals>
    void warn(const T& str, RecursiveVals... Rv)
    {
        m_logging.lock();
        std::cout << "\x1b[1;31m" << str;
        warn(Rv...);
    }

}
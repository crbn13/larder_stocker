// #pragma once

#ifndef LOGGING_HEADER_DEFINED
#define LOGGING_HEADER_DEFINED

#include <iostream>
#include <mutex>

// #define LOGGING

namespace crbn
{
    namespace
    {
        inline std::mutex m_logging;
    }
    // void log(const std::string str);

    // void log(const char *str); // because it was getting angry about ambiguity

    /// @brief couts str to terminal
    /// @tparam T
    /// @param str what it prints
    /// @param endl t/f print newline character
    template <typename T>
    void log(const T &str, bool endl);

    /// @brief couts str to terminal
    /// @tparam T
    /// @param str
    template <typename T>
    void log(const T &str);

    /// @brief Always prints output in fancy red oooo
    /// @tparam T
    /// @param
    template <typename T, typename... RecursiveVals>
    void warn(const T&, RecursiveVals...);
}

#include "crbn_logging.tpp"

#endif
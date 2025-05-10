#pragma once
#include "crbn_json.hpp"

namespace crbn
{
    namespace jsn
    {
        template <typename T>
        void Json_Helper::json_write(const std::string &key, const T &value)
        {
            if (isLocked())
                m_j[key] = value;
            else
                // throw std::runtime_error("Json_Helper logic : Reading/Writing json file without mutex or safemode disabled");
                crbn::warn("crbn::jsn::Json_Helper::json_write WARNING writing to local cache, not file memory");
        }

        template <typename T>
        json &Json_Helper::operator[](T &tag)
        {
            if (!isLocked())
            {
                crbn::warn("crbn::jsn::Json_Helper::op[] | Warning , reading cached json data from memory, could be out of date");
            }
            return m_j[tag];
        }

    }
}
// #pragma once
#ifndef graphEstimation
#define graphEstimation

#include "crbn_json.hpp"
#include "crbn_logging.hpp"
#include <vector>
#include <ctime>
#include <memory>

#define coordType int
#define decimalType double

namespace crbn
{
    /// @brief namespace for estimations
    namespace est
    {

        long int findEpochTime();

        /// @brief generates list of tags which should be bought because either out of stock,
        ///        or estimated to run out within interval (stored in config.json)
        /// @return
        std::vector<std::string> tagListGenerator();

        /// @brief
        /// @param j
        /// @param timeBuffer number of seconds to allow preemptive shopping list
        /// @param epochTime the current epoch time
        /// @return
        bool estimateBoundsCheck(crbn::jsn::Json_Helper &j, decimalType timeBuffer, time_t epochTime);

        /// @brief calculates gradient and y intercept from array of coordinate values
        /// @param coord {x[], y[]}
        /// @param arrayLength
        /// @return y = mx + c | {m, c}
        std::pair<decimalType, decimalType> linearRegression(std::pair<coordType *, coordType *> coord, int arrayLength);

    }
}
#endif
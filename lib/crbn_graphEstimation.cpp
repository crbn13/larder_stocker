#include "headers/crbn_graphEstimation.hpp"
#include "headers/crbn_json.hpp"
#include "headers/crbn_logging.hpp"
#include <sstream>
#include <string>
#include <vector>

namespace crbn
{
    /// namespace for estimations
    namespace est
    {

        std::vector<std::string> tagListGenerator()
        {
            long int currentTime = findEpochTime();

            decimalType timeBuffer = (decimalType)1000;

            // variable declaration

            int dataPartitions = 2; // how many parts to split the data up into

            std::vector<std::string> files;
            files.reserve(200);
            std::vector<crbn::jsn::Json_Helper> jsons;
            jsons.reserve(200);

            std::vector<std::string> tagsToBuy;
            tagsToBuy.reserve(200);
            crbn::jsn::Json_Helper tags("data/tags.json",
                                        crbn::jsn::jsonLiterals::tagsStore);

            std::vector<std::unique_ptr<crbn::jsn::Safemode>> locks;
            locks.reserve(200);

            tags.lock();

            // locks.emplace_back();
            // std::unique_ptr<crbn::jsn::Safemode> lock =

            //
            for (int i = 0; i < tags["tags"].size(); i++)
            {
                crbn::jsn::Json_Helper j;
                locks.emplace_back(j.setSafeModeUnsafe());
                j.init("data/" + tags["tags"].at(i).get<std::string>() + ".json", crbn::jsn::jsonLiterals::data );

                jsons.emplace_back(j);

                if (jsons.back()["tag_id"].is_null() or jsons.back().strGet("tag_id").length() < 1)
                {
                    // std::cout << " \n \t\t NULL OR < 1   \n";
                    jsons.back().json_write("tag_id", tags["tags"].at(i).get<std::string>());
                }
            }
            tags.unlock();

            // iterate through all of the data
            // and generate shopping list
            for (auto i : jsons)
            {
                std::cout << " \n \t\t CHECK  MEOW  " << i.strGet("tag_id") << "\n";

                if (i["rebuy"].get<bool>() == false)
                    continue; // skip this loop and continue onto the
                              // next loop
                else if (i.intGet("number_of") < 1)
                {
                    tagsToBuy.push_back(i.strGet("tag_id"));
                    continue;
                }

                // prediction algorithm starts here
                else
                {
                    if (estimateBoundsCheck(i, timeBuffer, currentTime) == true)
                    {
                        tagsToBuy.push_back(i.strGet("tag"));

                    }
                }
                continue;
            }

            // check stored date of last input?
            // each output will be stored in a number of hours
            // from this date

            // each use results in -1 to the amount stored
            // GRAPHING :
            // plot change in hours from last save point to current time
            // plot all data of how many hours from input dates
            return files;
        }

        long int findEpochTime()
        {
            return static_cast<long int>(time(0));
        }

        /// @brief
        /// @param j
        /// @param timeBuffer number of seconds to allow preemptive shopping list
        /// @param epochTime the current epoch time
        /// @return
        bool estimateBoundsCheck(crbn::jsn::Json_Helper &j, decimalType timeBuffer, time_t epochTime)
        {
            crbn::log("crbn::est::estimateBoundChech() start");
            /// number of coordinate pairs

            j.lock();
            int numCoords = 0;

            if (j[jsn::keys::consumption_times].empty()
                or j[jsn::keys::input_times].empty()) 
                // arrays cannot be empty, so needs to be checked
            {
                j.unlock();
                return false;
            }
            std::vector<long int> consumption_times;
            std::vector<long int> input_times;

            consumption_times = j[jsn::keys::consumption_times].get<std::vector<long int>>();
            input_times = j[jsn::keys::input_times].get<std::vector<long int>>();

            // need to only use the latter half of consumption_times, the times After the last input
            // so we iterate untill we find the recent relevant data, then we delete the non
            // relevant data
            for (int i = 0; i < consumption_times.size(); i++)
            {
                if (input_times.back() > consumption_times[i]) // last input was more recent
                    continue;
                else // consumption_times at point where recent data is present as it is larger
                     // epoch time than last input time
                {
                    crbn::log(
                        " consumption_times recent data found at i = " + std::to_string(i)
                        + " | size = " + std::to_string(consumption_times.size()));
                    consumption_times.erase(
                        consumption_times.begin(),
                        consumption_times.begin() + i); // erase non relevant data
                    crbn::log("After erase size = " + std::to_string(consumption_times.size()));
                    break;
                }
            }

            numCoords = consumption_times.size();
            crbn::log("input times back = " + std::to_string(input_times.back()));

            j.unlock();

            std::pair<coordType *, coordType *> coordinates; // a pair is a single
                                                             // variable which holds
                                                             // 2 variables

            coordinates.first = new coordType[numCoords]{0};
            coordinates.second = new coordType[numCoords]{0};

            // this variable is used to store the gradients and y intercepts
            // which will be returned by the linearRegression() function
            std::vector<std::pair<decimalType, decimalType>> lineEquations;

            int initialItems = numCoords + j.intGet("number_of");

            //std::cout << "initial items " << initialItems << "\n";

            int x = 0;
            for (auto i : consumption_times)
            {
                coordinates.first[x] = i;
                coordinates.second[x] = initialItems;

                initialItems--;
                x++;
            }

            // ~~~~~~~~~~~~~~ linear regression calculation ~~~~~~~~~~~~~~

            decimalType bias = 2; // NEEED TO MAKE INTEGRATION WITH CONFIG FILE

            // CALCULATEsed to add bias to more recent data | bias >= 1

            int dataPartitions = numCoords / 3; // how many times to split up data
            
            if (dataPartitions == 0 || dataPartitions == 1) // dataPartitions is used to divide numbers, so CANNOT be 0
                lineEquations.push_back(linearRegression( {coordinates.first, coordinates.second}, consumption_times.size() ));
            else
            {
                // split up the coordinates into sections then run the linearRegression function on
                // them to find the gradient and y intercept. this is to give more bias towards
                // recent data which is likely more accurate
 
                //std::cout << "numCoords =" << numCoords << "\n";

                bool extranious = false; // extranious is used to know if there will be extra data
                                         // left over due to there being an odd number of element

                if ((numCoords % dataPartitions) > 0)
                    extranious = true;

                //std::cout << numCoords % dataPartitions << "\n";

                int partitionLen = (int)(numCoords / dataPartitions); // specifing (int) makes sure
                                                                      // that the number is
                                                                      // truncated

                for (int i = 0; i < dataPartitions; i++)
                {
                    int pos = i * partitionLen;
                    //std::cout << "pos = " << pos << "\n";

                    if (!(i + 1 < dataPartitions)) // check if this is the last iteration of the loop
                    {
                        if (extranious) // check if there will be extra values which need
                                        // incorporating in final calculation
                        {
                            partitionLen = partitionLen + numCoords - ((i + 1) * partitionLen);
                            // adds the extranious values which could have been ignored
                        }
                    }
                    //std::cout << "part len =  " << partitionLen << "\n";

                    lineEquations.push_back(linearRegression({coordinates.first + pos,
                                                              coordinates.second + pos},
                                                             partitionLen));
                }
            }

            lineEquations.back().first *= bias;  // multiply the final gradient
                                                 // by the bias
            lineEquations.back().second *= bias; // also multiply the final y intercept

            // find the mean average of the gradients and y intercepts
            decimalType gradientAverage = (decimalType)0;
            decimalType yInterceptAverage = (decimalType)0;

            for (const auto &i : lineEquations)
            {
                gradientAverage += i.first; // .first accesses the "first" variable in the
                                            //  pair of variables
                yInterceptAverage += i.second;
            }
            // divide total value by number of elements
            if (lineEquations.size() == 1) 
            {
                // no need to divide
                // average of 1 element = data / 1
            }
            else if (lineEquations.size() == 0)
            {
                crbn::warn("graphEstimation : Error, no estimation data ?");
                return false;
            }
            else
            {
                gradientAverage /= (lineEquations.size() - 1 + bias); // what if
                                                                      // lineEquations.size() ==
                                                                      // 1 ? need to fix
                yInterceptAverage /= (lineEquations.size() - 1 + bias);
            }

            crbn::log(" grad average      " + std::to_string(gradientAverage) );
            crbn::log(" intercept average " + std::to_string(yInterceptAverage));

            // CALCULATE WETHER CROSSES X AXIS BEFORE OR AFTER

            /*
            y = 0
            0 = mx + c
            x = -c/m
            */
            decimalType xinter = (-yInterceptAverage) / gradientAverage; // x intercept
            crbn::log("crbn_graphEstimation():: x intercept = " + std::to_string(xinter));

            if (xinter < (epochTime + timeBuffer)) // check if x intercept is smaller than the time
                                                   // stored by timeBuffer
                return true;
            return false;
        }

        /// @brief calculates gradient and y intercept from array of coordinate values
        /// @param coord {x[], y[]}
        /// @param arrayLength
        /// @return y = mx + c | {m, c}
        std::pair<decimalType, decimalType> linearRegression(std::pair<coordType *, coordType *> coord, int arrayLength)
        {
            int N = arrayLength;

            decimalType xSum = (decimalType)0;
            decimalType xSumSqr = (decimalType)0;
            decimalType ySum = (decimalType)0;
            decimalType xySum = (decimalType)0;

            decimalType yMean = (decimalType)0;
            decimalType xMean = (decimalType)0;

            // y = mx + c
            decimalType m = (decimalType)0;
            decimalType c = (decimalType)0;

            for (int i = 0; i < N; i++)
            {
                //std::cout << "{" << coord.first[i] << ", " << coord.second[i] << "}\n";
                xSum += coord.first[i];
                xSumSqr += coord.first[i] * coord.first[i];
                ySum += coord.second[i];
                xySum += coord.second[i] * coord.second[i];
            }

            yMean = ySum / (decimalType)N;
            xMean = xSum / (decimalType)N;

            m = [=]() -> decimalType
            {
                decimalType x = (decimalType)0;
                for (int i = 0; i < N; i++)
                {
                    x += ((decimalType)coord.first[i] - xMean)
                        * ((decimalType)coord.second[i] - yMean);
                }
                return x;
            }() /  // divide the outputs of the two functions
                [=]() -> decimalType
            {
                decimalType x = (decimalType)0;
                for (int i = 0; i < N; i++)
                {
                    x += ((decimalType)coord.first[i] - xMean)
                        * ((decimalType)coord.first[i] - xMean);
                }
                return x;
            }();

            c = ((yMean) - (m * (xMean)));

            //std::cout << "\n";
            // std::cout << "y = mx + c\n";
            // std::cout << "m =" << std::to_string(m) << "\n";
            // std::cout << "c =" << std::to_string(c) << "\n";

            return {m, c};
        }

    }
}

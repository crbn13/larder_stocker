#include "headers/crbn_instructions.hpp"
#include "headers/crbn_dataSerialiser.hpp"
#include "headers/crbn_graphEstimation.hpp"
#include "headers/crbn_json.hpp"
#include "headers/crbn_operations.hpp"
#include <cstddef>
#include <cstdint>
#include <filesystem>
#include <iomanip>
#include <iterator>
#include <memory>
#include <ostream>
#include <set>
#include <string>
#include <sys/types.h>
#include <tuple>
#include <type_traits>
#include <vector>

#ifdef DEF_CLIENT       // ~~~~~~~~~~~~~~~~~~~~~~~  CLIENT START            ~~~~~~~~~~~~~~~~~~~~~~~

void crbn::op::instructionDecoder(std::shared_ptr<crbn::serialiser> data, crbn::Client_Jobs & job)
{
    crbn::jsn::Json_Helper jsonConfig(crbn::jsn::configFileName, crbn::jsn::jsonLiterals::config);

    auto pointer = jsonConfig.setSafeModeUnsafe();

    switch (data->operation())
    {
        case crbn::op::TELL_CLIENT_PNG_NAME:
        {
            crbn::warn("UNTESTED TELL_CLIENT_PNG_NAME");
            crbn::log("crbn::Jobs::instructionDecoder() | TELL_CLIENT_PNG_NAME");
            job.dataMap.write(
                [&](std::remove_const<decltype(job.dataMap())>::type & map)
                {
                    map[data->ticket()] = { true,
                                            std::shared_ptr<void>(new crbn::serialiser(*data), deconstructor<crbn::serialiser> ),
                                            data->bodyAsString(),
                                            data->operation() };
                });
            break;
        }
        case crbn::op::SEND_PNG_TO_CLIENT:
        {
            crbn::log("crbn::Jobs::instructionDecoder() | SEND_PNG_TO_CLIENT ");

            if (job.dataMap().find(data->ticket()) == job.dataMap().end()) // checks to see if tag already present in the map
            {
                job.enqueIN(data); // puts back into queue because it is expecting tag to already exist
                                   // the TELL_CLIENT_PNG_NAME tag needs to exist to tell the client the
                //          name of the png
                break;
            }
            else
            {
                // crbn::warn(__FILE__, __LINE__, "UNFINISHED ELSE");
            }

            crbn::log(" GOING TO DECODE AND WRITE IMAGE TO MEMORY :p hopefully...;");

            job.dataMap.read(
                [&](decltype(job.dataMap())& map)
                {
                    // uses the data from TELL_CLIENT_PNG_NAME and writes the binary data into that buffer
                    std::ofstream binFile(
                        static_cast<serialiser*>(
                            std::get<std::shared_ptr<void>>(map.at(data->ticket())).get())
                            ->bodyAsString());
                    //  (static_cast<crbn::serialiser *> (dataMap[std::to_string(data->ticket())].second.get()))->bodyAsString());

                    for (size_t i = 0; i < data->bodySize(); i++)
                    {
                        binFile << data->bodyAccsess(i);
                        // binFile.write((char*)data->c_bodyStr(), data->bodySize());
                    }
                    binFile.close();
                    return;
                });

            crbn::log("crbn::Jobs::SENDPNGTOCLIENT | written png ");

            break;
        }
        case crbn::op::SEND_JSON_TO_CLIENT:
        {
            crbn::log("crbn::Jobs::instructionDecoder() | SEND_JSON_TO_CLIENT ");
            if (data->bodySize())
            {
                json j;

                crbn::log("crbn::Jobs::instructionDecoder  writing json data to map");

                if (json::accept(data->c_bodyStr()))
                {
                    j = json::parse(data->c_bodyStr());
                    std::string tmp = "";
                    if (j.contains(jsn::keys::tag_id))
                        tmp = j[jsn::keys::tag_id].get<std::string>();

                    job.dataMap.write(
                        [&](std::remove_const_t<decltype(job.dataMap())>& map)
                        {
                            map[data->ticket()] = std::make_tuple(
                                true,
                                std::shared_ptr<void>(new json(std::move(j)), deconstructor<json>),
                                tmp,
                                data->operation());
                            return;
                        });
                    crbn::log("crbn::Jobs::instructionDecoder written json data to map !!!");
                }
                else
                {
                    crbn::warn("Failed data : ", data->bodyAsString());
                    crbn::log("crbn::Jobs::instructionDecoder | not correct json form");
                }
                break;
            }
            else
            {
                crbn::warn("NEED TO MAKE A JOB MANAGER FUNCTION TO SAY FAILLLLLLLLLLL");
            }
            break;
        }
        case crbn::op::CONNECTION_CONFIRMED:
        {
            crbn::log("crbn::Jobs::instructionDecoder() | CONNECTION_CONFIRMED");
            job.isServerConnectable = true;
            break;
        }
        case crbn::op::SELECTED_KEY:
        {
            crbn::warn(data->bodyAsString());
            job.activeTag = data->bodyAsString();
            break;
        }
        case crbn::op::IMAGE_REQUEST_FAILED:
        {
            crbn::warn("Image request failed, check api key validity, or try a different string");
            break;
        }
        default:
        {
            std::cout << "hihihihi " << std::endl;
            crbn::warn(" why not working ");
            crbn::warn( "crbn::op::whatdo() UNIMPLEMENTED CODE RECIEVED , code = " + std::to_string((int16_t)data->operation()) );
            break;
        }

    }
}
#endif                  // ~~~~~~~~~~~~~~~~~~~~~~~  CLIENT END              ~~~~~~~~~~~~~~~~~~~~~~~

#ifdef DEF_SERVER       // ~~~~~~~~~~~~~~~~~~~~~~~  SERVER START            ~~~~~~~~~~~~~~~~~~~~~~~


void crbn::op::instructionDecoder(std::shared_ptr<crbn::serialiser> data, crbn::Server_Jobs & job)
{
    crbn::jsn::Json_Helper jsonConfig(crbn::jsn::configFileName, crbn::jsn::jsonLiterals::config);
    
    crbn::jsn::Json_Helper jsonTags("tags.json", crbn::jsn::jsonLiterals::tagsStore);
    crbn::jsn::Json_Helper jsonData;
    bool initialized_jsonData = false;
    if (!job.activeTag().empty())
        if (std::filesystem::exists(
                jsonConfig.strGet(jsn::keys::server_path_to_data) + job.activeTag() + ".json"))
        {
            crbn::log(
                "instructionDecoder() : Auto Initializing jsonData with job.activeTag = "
                + jsonConfig.strGet(jsn::keys::server_path_to_data) + job.activeTag() + ".json");
            jsonData.init(
                jsonConfig.strGet(jsn::keys::server_path_to_data) + job.activeTag() + ".json",
                crbn::jsn::jsonLiterals::data);
            initialized_jsonData = true;
        }

    switch (data->operation())
    {
    case crbn::op::ID_ENQUIRY:
    {
        crbn::log("crbn::Jobs::instructionDecoder() | ID_ENQUIRY");

        std::string tag = data->bodyAsString();

        crbn::jsn::Json_Helper j;

        jsonConfig.init();
        j.init(jsonConfig.strGet(crbn::jsn::keys::server_path_to_data) + tag + ".json", crbn::jsn::jsonLiterals::data);
        crbn::log("crbn::Jobs::instructionDecoder() | ID_ENQUIRY : Reading Json file : " + jsonConfig.strGet(crbn::jsn::keys::server_path_to_data) + tag + ".json");

        jsonTags.lock();
        if (!(jsonTags["tags"].count(tag))) // if tag not already stored in system store
        {
            jsonTags["tags"][tag] = nullptr;
        }
        jsonTags.unlock();

        j.lock();
        j.json_write("tag_id", tag);
        j.unlock();
        
        std::string tempstr = j.j().dump();

        crbn::serialiser dat((uint8_t*)tempstr.c_str(), tempstr.size(), crbn::op::SEND_JSON_TO_CLIENT, data->ticket());

        crbn::log(" crbn::Jobs::instructionDecoder data to return to client generated");
        job.enqueOUT(dat);
        break;
    }
    case crbn::op::CHECK_CONNECTION:
    {
        crbn::log("crbn::Jobs::instructionDecoder() | CHECK_CONNECTION");
        crbn::serialiser dat(nullptr, 0, crbn::op::CONNECTION_CONFIRMED, data->ticket());
        job.enqueOUT(dat);

        break;
    }

    case crbn::op::IMAGE_REQUEST:
    {
        crbn::log("crbn::Jobs::instructionDecoder() | IMAGE_REQUEST");

        if (!(data->bodySize() > 0))
        {
            crbn::warn("crbn::jobs::instrctionDecoder::IMAGE_REQUEST | cannot get image with size from string with size " + std::to_string( data->bodySize()) + " : ");
            job.sendNetworkRequest(op::IMAGE_REQUEST_FAILED, data->ticket());
            break;
        }

        // initialise json helper to read config.json file
        crbn::jsn::Json_Helper j;
        j.init();
        auto x = j.setSafeModeUnsafe();

        bool downloaded = false;

        crbn::log("crbn::Jobs::IMAGE_REQUEST : OPENING FILE : " + j.strGet(crbn::jsn::keys::server_path_to_data) + data->bodyAsString() + ".png");

        // check if .png file doesnt exist
        if (!std::filesystem::exists(j.strGet(crbn::jsn::keys::server_path_to_data) + data->bodyAsString() + ".png")) // if file doesnt exist
        {
            crbn::log("crbn::Jobs::IMAGE_REQUEST : File not found cached locally ");

            crbn::jsn::Json_Helper jdat;

            jdat.init(j.strGet(crbn::jsn::keys::server_path_to_data) + data->bodyAsString() + ".json", crbn::jsn::jsonLiterals::data);

            if (job.m_imageDownloader.try_lock()) // download image
            {
                crbn::log("crbn::Jobs::IMAGE_REQUEST : DOWNLOADING IMAGE | NAME :" + data->bodyAsString());
                if (crbn::imageDownload(jdat.strGet("name"), data->bodyAsString()))
                    downloaded = true;
                else
                {
                    job.m_imageDownloader.unlock();
                    job.sendNetworkRequest(op::IMAGE_REQUEST_FAILED, data->ticket());
                    crbn::warn("Image downloader failed");
                    break;
                }
                job.m_imageDownloader.unlock();

                int width, height, channels;

                            //                      data/12345       no .png
                std::string tmp(j.strGet(crbn::jsn::keys::server_path_to_data) + (char *)data->c_bodyStr());
                //  char *name = j.strGet(crbn::jsn::keys::server_path_to_data).c_str();
                // strcat(name(char *) data->c_bodyStr());
                // char png[] = ".png";
                // strcat(name, png);

                u_char *img = stbi_load(tmp.c_str(), &width, &height, &channels, 0); // loads the image (data/1234   no .png)

                if (!img) // if failed
                {
                    crbn::log("Failed to load image | wrong File type? ");
                    job.sendNetworkRequest(op::IMAGE_REQUEST_FAILED, data->ticket());
                    break;
                }
                else 
                {
                    crbn::log("crbn::op::IMAGE_REQUEST image successfully parsed by stbi");
                    tmp = tmp + ".png";
                    stbi_write_png(tmp.c_str(), width, height, channels, img, width * channels);
                    
                    stbi_image_free(img);
                }
            }
            else
            {
                crbn::log("crbn::Jobs::IMAGE_REQUEST : mutex locked, adding request to back of queue to not hold up instruction decoding threads");
                job.enqueIN(data);
                // crbn::serialiser dat(data->rawDatOut(), true); // THIS IS JUST WRONG
            }
        }
        else // if file exists 
        {
            crbn::log("crbn::Jobs::IMAGE_REQUEST : FILE OPENED");
            crbn::warn("crbn::op::IMAGE_REQUEST | WArning , no check for validity of image file");
            downloaded = true;
        }

        // send file to client
        if (downloaded)
        {
            // tell client what the name of the file is so that it can write it
            std::string temp = data->bodyAsString();
            temp.append(".png");
            job.sendNetworkRequest(op::TELL_CLIENT_PNG_NAME, data->ticket(), (uint8_t *)temp.c_str(), temp.size());

            // send file to client
            crbn::log("crbn::Jobs::IMAGE_REQUEST : READING BINARY FILE");

            std::ifstream rawBinaryFile(j.strGet(crbn::jsn::keys::server_path_to_data) + data->bodyAsString() + ".png", std::ios::binary | std::ios::in);
            crbn::warn( "crbn::op::IMAGE_REQUEST NO check for if file input has worked" );

            auto size = rawBinaryFile.tellg();
            rawBinaryFile.seekg(0, std::ios::end);
            size = rawBinaryFile.tellg() - size;
            crbn::log("crbn::Jobs::IMAGE_REQUEST : IMAGE FILESIZE = " + std::to_string(size));

            rawBinaryFile.seekg(0, std::ios::beg);

            char *raw = new char[size];
            rawBinaryFile.readsome(raw, size);

            if(size != rawBinaryFile.tellg());

            crbn::log("crbn::Jobs::IMAGE_REQUEST : initialising serialiser ");

            crbn::serialiser dat((uint8_t *)raw, size, crbn::op::SEND_PNG_TO_CLIENT, data->ticket());

            crbn::log("crbn::Jobs::IMAGE_REQUEST : Data about to be written to queue");

            job.enqueOUT(dat);
            delete[] raw;
        }
        else
        {
            crbn::warn("crbn::Jobs::IMAGE_REQUEST : NOT SENDING IMAGE TO CLIENT, UNKNOWN REASON ");
        }
        break;
    }

    case crbn::op::SELECT_KEY:
    {
        crbn::log("crbn::Jobs::instructionDecoder() | SELECT_KEY");
        job.activeTag = data->bodyAsString();
        break;
    }
    case crbn::op::SET_NAME:
    {
        crbn::log("crbn::Jobs::instructionDecoder() | SET_NAME");
        crbn::log("crbn::Jobs::SET_NAME | active tag == " + job.activeTag() );
        
        crbn::jsn::Json_Helper j;
        j.lock();
        j.init(jsonConfig.strGet(crbn::jsn::keys::server_path_to_data) + job.activeTag() + ".json", crbn::jsn::jsonLiterals::data);
        j.json_write("name", data->bodyAsString());
        j.unlock();
        break;
    }
    case crbn::op::WHICH_KEY_SELECTED:
    {
        crbn::log("crbn::Jobs::instructionDecoder() | WHICH_KEY_SELECTED");
        crbn::serialiser dt((uint8_t*)job.activeTag().c_str(), job.activeTag().size(), crbn::op::SELECTED_KEY, data->ticket());

        job.enqueOUT(std::make_shared<crbn::serialiser>(std::move(dt)));

        break;
    }
    case crbn::op::CONSUME_1:
    {
        crbn::log("crbn::Jobs::instructionDecoder() | CONSUME_1");
        jsonData.lock();
        if (initialized_jsonData)
        {
            jsonData[jsn::keys::number_of] = jsonData.intGet(jsn::keys::number_of) - 1;
            jsonData[jsn::keys::consumption_times].push_back( static_cast<int64_t>( std::time(0) ));
        }
        jsonData.unlock();
        break;
    }
    case crbn::op::ADD_1:
    {
        crbn::log("crbn::Jobs::instructionDecoder() | ADD_1");
        jsonData.lock();
        if (initialized_jsonData)
        {
            jsonData[jsn::keys::number_of] = jsonData.intGet(jsn::keys::number_of) + 1;
            jsonData[jsn::keys::input_times].push_back( static_cast<int64_t>( std::time(0) ));
        }
        jsonData.unlock();
        break;
    }
    case crbn::op::SET_JSON:
    {
        crbn::log("crbn::Jobs::instructionDecoder() | SET_JSON");
        std::string str = job.activeTag();
        bool valid = false;

        if (str.size())
        {
            if (data->bodySize()) // if theres no bodysize then theres no data to write
            {
                if (json::accept(data->bodyAsString())) // checks whether the input is valid
                {
                    crbn::log("instructionDecoder()::SET_JSON : data input valid");
                    valid = true;
                }
            }
        }

        if (valid)
        {
            json jsonFromClient(json::parse(data->bodyAsString()));

            jsonConfig.init();
            jsonData.init(
                jsonConfig.strGet(jsn::keys::server_path_to_data) + str + ".json", jsn::jsonLiterals::data);

            jsonData.lock();
            
            for (auto x : jsonFromClient.items())
            {
                // check the remote and local key exist
                if (jsonFromClient.contains(x.key()) && jsonData.j().contains(x.key()))
                {
                    crbn::log("instrctionDecoder()::SET_JSON : key = " + x.key() + " | Both keys Exist");

                    if (jsonFromClient.at(x.key())
                        == jsonData.j().at(x.key())) // key data is the same
                    {
                        crbn::log("instrctionDecoder()::SET_JSON : key = " + x.key() + " | is equal");
                        // data is the same so nothing needs doing
                    }
                    else if (jsonFromClient.at(x.key()) != jsonData.j().at(x.key()))
                    {
                        crbn::log("instrctionDecoder()::SET_JSON : key = " + x.key() + " | is different ");
                        // keys different so have to override, unless specific keys
                        if ((std::set<std::string> { jsn::keys::number_of,
                                                     jsn::keys::consumption_times,
                                                     jsn::keys::input_times,
                                                     jsn::keys::tags })
                                .count(x.key()))
                        {
                            crbn::log("instructionDecoder()::SET_JSON : key = " + x.key() + " | is being ignored");
                            if (x.key() == jsn::keys::number_of)
                            // the number_of is different to
                            // the stored value
                            {
                                if (jsonData.intGet(jsn::keys::number_of)
                                    > jsonFromClient[jsn::keys::number_of].get<int>()) // items consumed
                                {
                                    int difference = jsonData.intGet(jsn::keys::number_of)
                                        - jsonFromClient[jsn::keys::number_of].get<int>();

                                    for (int i = 0 ; i < difference; i++) 
                                        job.enqueIN(
                                            std::make_shared<crbn::serialiser>(
                                                crbn::op::CONSUME_1,
                                                data->ticket())); // send data back to incoming
                                                                  // queue
                                }
                                else if (
                                    jsonData.intGet(jsn::keys::number_of)
                                    < jsonFromClient[jsn::keys::number_of].get<int>()) // items added
                                {
                                    int difference = jsonFromClient[jsn::keys::number_of].get<int>()
                                        - jsonData.intGet(jsn::keys::number_of);

                                    for (int i = 0; i < difference; i++)
                                        job.enqueIN(
                                            std::make_shared<crbn::serialiser>(
                                                crbn::op::ADD_1,
                                                data->ticket())); // send data back to incoming
                                                                  // queue
                                }
                                else // same number of itmes?? which should not be possible
                                {
                                    crbn::warn("instructionDecoder()::SET_JSON : invalid input from client about number_of tag");
                                }
                                // outputd date
                            }
                        }
                        else // key not found , so data will be copied
                        {
                            crbn::log(
                                "instrctionDecoder()::SET_JSON : copying tag data : " + x.key());
                            jsonData.json_write(x.key(), jsonFromClient.at(x.key()));
                        }
                    } // else
                }// if (keys both exist)
            } // for (tags in tags)
            jsonData.unlock();
        } // if (valid) {}
        else
        {
            crbn::warn(
                "instructionDecoder()::SET_JSON : INPUT IS NOT VALID ; either : active tag wrong, "
                "client input either size 0 or not valid json ");
        }

        // check changes in numberof values,
        jsonTags.update();
        json j = json::parse(data->bodyAsString());
        jsonConfig.lock(); // locking this mutex, but just locking it to stop json files
                           // from being read/written
        std::ofstream file(jsonConfig.strGet(jsn::keys::server_path_to_data) + str + ".json");
        file << std::setw(4) << j;
        jsonConfig.unlock();

        break;
    }
    case GENERATE_SHOPPING_LIST:
    {
        jsonConfig.init();
        jsonTags.init();
        int timebuffer = jsonConfig.intGet(crbn::jsn::keys::shopping_list_time_buffer);
        size_t elements = jsonTags[crbn::jsn::keys::tags].size();

        json json_shopping_list(json::parse(jsn::jsonLiterals::shopping_list));
        std::vector<std::string> tags;
        tags.reserve(elements);

        crbn::log("json Dump shopping list : " + json_shopping_list.dump());
        for (auto& iterator : jsonTags[crbn::jsn::keys::tags].items())
        {
            tags.emplace_back(iterator.key());
        }

        crbn::log(" Tags : ");
        for (auto &x : tags) {
            crbn::log(x);
        }

        for (auto& x : tags)
        {
            //            first check existence
            //            then validate required tags
            //            then generate list of known_run_out_of items and smart data prediction
            //            algorithm

            std::string path = (jsonConfig.strGet(jsn::keys::server_path_to_data) + x + ".json");
            if (std::filesystem::exists(path))
            {
                crbn::log(
                    "instructionDecoder()::GENERATE_SHOPPING_LIST : path -> " + path + " | EXISTS");
            }
            else
            {
                crbn::log(
                    "instructionDecoder()::GENERATE_SHOPPING_LIST : path -> " + path
                    + " | DOES NOT EXIST");
                continue;
            }

            jsn::Json_Helper j_data(path, crbn::jsn::jsonLiterals::data);

            // check that all required tags are present

            if (not j_data[jsn::keys::rebuy].get<bool>())
            {
                crbn::log(
                    "instructionDecoder()::GENERATE_SHOPPING_LIST : key = " + x
                    + " rebuy = false , continuing");
                continue;
            }
            else // rebuy == true
            {
                crbn::log(
                    "instructionDecoder()::GENERATE_SHOPPING_LIST : key = " + x
                    + " rebuy = true , checking if rebuy required");

                if (j_data.intGet(jsn::keys::number_of) == 0)
                {
                    crbn::log("number_of = 0");
                    json_shopping_list[jsn::keys::known_have_none].push_back(x);
                    crbn::log("added data");
                }
                else
                {
                    crbn::log("instructionDecoder()::GENERATE_SHOPPING_LIST : running estimation function");
                    if (crbn::est::estimateBoundsCheck(j_data, timebuffer, std::time(0)))
                    {
                        crbn::log(
                            "instructionDecoder()::GENERATE_SHOPPING_LIST : Estimate returned "
                            "true");
                        json_shopping_list[jsn::keys::predicted].push_back(x);
                    }
                    else {
                         crbn::log(
                            "instructionDecoder()::GENERATE_SHOPPING_LIST : Estimate returned "
                            "False");
                        continue;
                    }

                }

            }
        }
        crbn::log("instructionDecoder()::GENERATE_SHOPPING_LIST | RETURNING DATA");
        crbn::log(json_shopping_list.dump());
        job.sendNetworkRequest(crbn::op::SEND_JSON_TO_CLIENT, data->ticket(), std::string(json_shopping_list.dump()));
        
        break;
    }
    default:
    {
        std::cout << "hihihihi " << std::endl;
        crbn::warn(" why not working ");
        crbn::warn( "crbn::op::whatdo() UNIMPLEMENTED CODE RECIEVED , code = " + std::to_string((int16_t)data->operation()) );
        break;
    }
    }
}
#endif                  // ~~~~~~~~~~~~~~~~~~~~~~~  SERVER END              ~~~~~~~~~~~~~~~~~~~~~~~

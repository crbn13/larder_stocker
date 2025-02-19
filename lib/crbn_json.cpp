#include "headers/crbn_json.hpp"

namespace crbn
{
    namespace jsn
    {

        // |-----------------------------------------------|
        // |               class JsonMutex                 |
        // |-----------------------------------------------|

        JsonMutex::JsonMutex()
        {
            locked = false;
        }

        const bool JsonMutex::isLocked()
        {
            return locked;
        }

        void JsonMutex::lock()
        {
            globalJsonMutex.lock();
            locked = true;
        }

        /// @brief releases the mutex
        void JsonMutex::unlock()
        {
            locked = false;
            globalJsonMutex.unlock();
        }

        // |-----------------------------------------------|
        // |               class Json_Helper               |
        // |-----------------------------------------------|
        Safemode::Safemode(bool* safemode, bool* locked)
        {
            sm = safemode;
            lkd= locked;

            *sm = UNSAFE;
            *lkd= true; // defining the local bool to true means that no function will try lock the mutex, also safemode cannot be initialized unless
            // std::cout << "safemode init" << std::endl;
        }
        Safemode::~Safemode()
        {
            // std::cout << "safemode deconstruct" << std::endl;
            // std::cout << std::to_string((bool)safemode) << " safemode | islocked() " << std::to_string(isLocked()) << std::endl;

            *lkd = false;
            *sm = SAFE;
        }

        Json_Helper::Json_Helper()
        {
            setvals();
            // safemode.operator bool()
        }
        void Json_Helper::setvals()
        {
            // std::cout << "  IF I DONT EXIST THEN CRY " << std::endl;
            fileName = configFileName;
            safemode = SAFE;
            // std::cout << std::to_string((bool)safemode) << "safemode | islocked() " << std::to_string(isLocked()) << std::endl;
        }

        /// @brief calls init() function
        /// @param fileToRead name of file to read
        /// @param JSON_RAW_SET a const char * of a json literal
        Json_Helper::Json_Helper(const std::string &fileToRead, const char *JSON_RAW_SET)
        {
            // Json_Helper(); //   idk why but calling a overrided constructor from
            //      within a different ovveride of the constructor breaks things
            setvals(); // moving init vals here instead
            init(fileToRead, JSON_RAW_SET);
        }

        /// @brief loads config data into json file | uses MUTEX
        void Json_Helper::init(const std::string &fileToRead, const char *JSON_RAW_SET)
        {
            fileName = fileToRead;
            JSON_RAW = JSON_RAW_SET;

            if (isLocked())
            {
                m_j = jsonRead(fileToRead, JSON_RAW);
            }
            else
            {
                lock(); // jsonRead is called in lock()
                unlock();
            }
        }

        /// @brief updates local data using mutex
        void Json_Helper::init()
        {
            init(fileName, JSON_RAW);
        }

        // template <typename T>
        std::unique_ptr<Safemode> Json_Helper::setSafeModeUnsafe()
        {

            if ((safemode == SAFE) && (!isLocked())) // switching safemode from SAFE -> UNSAFE
            {
                return std::unique_ptr<Safemode>{new Safemode(&safemode, &locked)};
            }
            else if (safemode == UNSAFE)
            {
                crbn::log("crbn::jsn::Json_Helper::setSafeModeUnsafe | safemode already in Unsafemode");
            }
            else if (isLocked())
            {
                throw std::runtime_error("Json_Helper logic error : Cannot change safeModeSetting while mutex is already locked ");
            }
            else
            {
                throw std::runtime_error("Json_Helper logic error : idek how");
            }
            return std::unique_ptr<Safemode>();
        }

        const json &Json_Helper::j()
        {
            return m_j;
        }

        void Json_Helper::lock()
        {
            if (!isLocked())
                JsonMutex::lock();
            // updates local file
            update();
        }

        void Json_Helper::unlock()
        {
            save();
            if ((safemode == SAFE) and (isLocked()))
                JsonMutex::unlock();
        }

        /// @brief WARNING : NO MUTEX | updates locally stored file to file in memory
        void Json_Helper::update()
        {
            if (isLocked())
                m_j = jsonRead(fileName, JSON_RAW);
            else
            {
                throw std::runtime_error("Json_Helper logic : Reading/Writing json file without mutex or safemode disabled");
            }
        }

        /// @brief writes locally stored json file to memory
        /// be careful using this function as it is not parrelel data safe
        void Json_Helper::save()
        {
            if (isLocked())
            {
                std::ofstream file(fileName);
                file << std::setw(4) << m_j;
                file.close();
            }
            else
            {
                throw std::runtime_error("Json_Helper logic : Reading/Writing json file without mutex or safemode disabled");
            }
        }

        /// @brief dont use NOT parralel data safe
        /// @param key
        /// @return
        std::string Json_Helper::strGet(const std::string &key)
        {
            if (isLocked())
            {
                try
                {
                    return m_j[key].get<std::string>();
                }
                catch (const std::exception &e)
                {
                    std::cerr << e.what() << "\n";
                }
            }
            else
            {
                crbn::log("crbn::jsn::Json_Helper::strGet | WARNING : reading cached data, could be out of date");
                try
                {
                    return m_j[key].get<std::string>();
                }
                catch (const std::exception &e)
                {
                    std::cerr << e.what() << "\n";
                }
            }
            return "";
        }

        int Json_Helper::intGet(const std::string &key)
        {
            if (isLocked())
            {
                try
                {
                    return m_j[key].get<int>();
                }
                catch (const std::exception &e)
                {
                    std::cerr << e.what() << "\n";
                }
            }
            else
            {
                crbn::log("crbn::jsn::Json_Helper::strGet | WARNING : reading cached data, could be out of date");
                try
                {
                    return m_j[key].get<int>();
                }
                catch (const std::exception &e)
                {
                    std::cerr << e.what() << "\n";
                }
            }
            return 0;
        }
        void Json_Helper::print()
        {
            // std::cout << std::to_string((bool)safemode) << " safemode | islocked() " << std::to_string(isLocked()) << std::endl;

            std::cout << std::setw(4) << m_j << std::endl;
        }

        /// @brief checks all elements to check that they exist, if they dont, the file is reset
        void Json_Helper::elementsCheck()
        {
        }

        // |-----------------------------------------------|
        // |               Functions                       |
        // |-----------------------------------------------|

        /// @brief generates config file
        void generateCfgFile()
        {
            std::ofstream write("config.json");
            json m_j = json::parse(crbn::jsn::jsonLiterals::config);

            write << std::setw(4) << m_j;
            write.close();
        }

        /// generates a data file for a certain tag based on

        json jsonConfigRead(std::string fileName, std::string path)
        {
            std::ifstream file_input(path + fileName);

            json m_j;

            if (file_input.is_open())
            {
                file_input >> m_j;
                file_input.close();
                return m_j;
            }
            else
            {
                file_input.close();
                // crbn::log("crbn::jsn::jsonConfigRead() could not open file \n\tassuming does not exist\n\tgenerating file");
                // if (fileName == configFileName)
                generateCfgFile();
                // else
                // generateDatFile(fileName);
                m_j = jsonConfigRead(fileName, path);
            }

            if (file_input.is_open())
            {
                file_input.close();
            }

            return m_j;
        }

        json jsonConfigRead(std::string fileName)
        {
            return jsonConfigRead(fileName, "");
        }
        json jsonConfigRead()
        {
            return jsonConfigRead(crbn::jsn::configFileName, "");
        }

        void jsonWrite(const std::string &path_fileName, const json &json)
        {
            std::ofstream write(path_fileName);
            write << std::setw(4) << json;
            write.close();
        }

        /// @brief generates config file
        json jsonRead(const std::string &path_fileName, const char *RAW)
        {
            std::ifstream file_input(path_fileName);

            json m_j;

            if (file_input.is_open())
            {
                crbn::log("crbn::jsn::jsonRead() about to parse file");
                file_input >> m_j;
                file_input.close();
                return m_j;
            }
            else
            {
                crbn::log("crbn::jsn::jsonRead() could not open file \n\tassuming does not exist\n\tgenerating file");

                m_j = json::parse(RAW);
                jsonWrite(path_fileName, m_j);
            }

            if (file_input.is_open())
            {
                file_input.close();
            }

            return m_j;
        }

        /// @brief generates an empty data file template, does not need path to data or ".json"
        json generateDatFile(std::string NO_PATH_tagId)
        {
            json config = crbn::jsn::jsonConfigRead(crbn::jsn::configFileName, "");

            std::string path = config["server_path_to_data"].get<std::string>();

            crbn::log(std::string(" path = " + path));
            crbn::log("crbn::jsn::generateDatFile : writing to file : \"" + path + NO_PATH_tagId + ".json\"");

            std::ofstream file(path + NO_PATH_tagId + ".json");

            json dat = json::parse(crbn::jsn::jsonLiterals::data);

            dat["tag_id"] = NO_PATH_tagId;

            file << std::setw(4) << dat;
            file.close();
            return dat;
        }

    }
}

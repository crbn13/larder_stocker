#include "headers/crbn_dataSerialiser.hpp"

namespace crbn
{

    serialiser::serialiser(uint8_t *rawInput, bool readBody)
    {
        rawHeaderIn(rawInput, readBody);
    }

    void serialiser::rawHeaderIn(uint8_t *rawInput, bool readBody)
    {
        crbn::log(" dataSerialiser : rawHeaderIn called");

        uint8_t *headerArr = new uint8_t[serc::HEADER_SIZE_T]{0};

        for (auto i = 0; i < serc::HEADER_SIZE_T; i++)
        {
            headerArr[i] = rawInput[i];
            // std::cout << +rawInput[i] << " ";
        }
        // std::cout << std::endl;

        header.operation = ((uint16_t)rawInput[1] << 8) | rawInput[0];

        header.bodysize = ((uint64_t)rawInput[9] << (64 - 8)) |
                          ((uint64_t)rawInput[8] << (64 - 16)) |
                          ((uint64_t)rawInput[7] << (64 - 24)) |
                          ((uint64_t)rawInput[6] << (64 - 32)) |
                          ((uint64_t)rawInput[5] << (64 - 40)) |
                          ((uint64_t)rawInput[4] << (64 - 48)) |
                          ((uint64_t)rawInput[3] << (64 - 56)) |
                          ((uint64_t)rawInput[2] << (64 - 64));

        header.ticket = ((uint32_t)rawInput[13] << (32 - 8)) |
                        ((uint32_t)rawInput[12] << (32 - 16)) |
                        ((uint32_t)rawInput[11] << (32 - 24)) |
                        ((uint32_t)rawInput[10] << (32 - 32));
        // std::cout << " data being depacked " << std::endl;
        // std::cout << " header.operation = " << header.operation << std::endl;
        // std::cout << " header.bodysize = " << header.bodysize << std::endl;
        if (readBody)
        {
            bodyInput(rawInput + serc::HEADER_SIZE_T);
        }
    }

    serialiser::serialiser()
    {
    }

    serialiser::serialiser(uint16_t op, uint32_t ticket)
    {
        uint8_t *body = nullptr;
        initialise(body, 0, op, ticket);
    }

    serialiser::serialiser(uint8_t *bodyInp, size_t size)
    {
        initialise(bodyInp, size, 0, 0);
    }
    serialiser::serialiser(uint8_t *bodyInp, size_t size, uint16_t op)
    {
        initialise(bodyInp, size, op, 0);
    }
    serialiser::serialiser(uint8_t *bodyInp, size_t size, uint16_t operation, uint32_t ticket)
    {
        initialise(bodyInp, size, operation, ticket);
    }

    void serialiser::initialise(uint8_t *bodyInp, size_t size, uint16_t operation, uint32_t ticket)
    {
        header.bodysize = size;
        header.operation = operation;
        header.ticket = ticket;

        body.rawData.clear();

        crbn::log("crbn::serialiser::initialise | bodysize = " + std::to_string(header.bodysize));

        body.rawData.reserve(size);
        crbn::log("crbn::serialiser::initialise | RESERVED MEMORY ");

        for (auto i = 0; i < size; i++)
        {
            // crbn::log("PUSHING BACK DATAR");
            body.rawData.push_back(bodyInp[i]);
        }
    }

    void serialiser::bodyAppend(uint8_t *bodyInp, size_t size)
    {
        for (int i = 0; i < size; i++)
        {
            body.rawData.push_back(bodyInp[i]);
        }
    }

    /// @brief ovverides body via size given
    /// @param bodyInp
    /// @param size
    void serialiser::bodyInput(uint8_t *bodyInp, size_t size)
    {
        header.bodysize = size;
        bodyInput(bodyInp);
    }

    /// @brief rewrites body with array data according to size stored in header
    /// @param bodyInp
    void serialiser::bodyInput(uint8_t *bodyInp)
    {
        body.rawData.clear();

        for (int i = 0; i < header.bodysize; i++)
        {
            body.rawData.push_back(bodyInp[i]);
        }

        // auto func = [&]() -> std::string
        // { std::string temp; for ( auto i = 0 ; i < +header.bodysize; i++ ){ temp.push_back((char)+bodyInp[i]); } return temp ; };
        // crbn::log(func());
    }
    /// @brief
    /// @return returns pointer to uint8_t array
    uint8_t * serialiser::rawDatOut()
    {
        crbn::log(" dataSerialiser : rawDatOut called");
        uint8_t *dat = new uint8_t[HEADER_SIZE_T + header.bodysize]{0};

        uint64_t temp = 0;

        temp = (uint64_t)header.operation;

        dat[0] = (uint8_t)temp;
        temp = (uint8_t)temp >> 8;
        dat[1] = temp;

        /// ADD BODYSIZE
        temp = header.bodysize;

        for (auto i = 0; i < (BODYSIZE_T); i++)
        {
            dat[i + OPERATION_SIZE_T] = (uint8_t)temp;
            temp = temp >> 8;
            // std::cout << (+(uint8_t)header.bodysize >> i) << " ";
        }


        crbn::log(" rawDatOut : ticket = " + std::to_string(+header.ticket));

        /// ADD TICKET
        temp = header.ticket;
        for (auto i = 0; i < TICKET_SIZE_T; i++)
        {
            dat[i + (HEADER_SIZE_T - TICKET_SIZE_T)] = (uint8_t)temp;
            temp = temp >> 8;
        }

        /// ADD BODY
        for (auto i = 0; i < header.bodysize; i++)
        {
            dat[i + serc::HEADER_SIZE_T] = bodyAccsess(i);
        }

        return dat;
    }

    /// @brief returns size of raw data, header.bodysize + header size
    /// @return returns size
    size_t serialiser::size()
    {
        return +(header.bodysize + serc::HEADER_SIZE_T);
    }
    
    /// @brief returns size of body
    /// @return
    size_t serialiser::bodySize()
    {
        // std::cout <<" bodysize = " <<  header.bodysize << std::endl;
        return +(header.bodysize);
    }

    /// @brief returns the 1 byte of data as uint8_t at the given index of the body
    /// @param element
    /// @return
    uint8_t serialiser::bodyAccsess(const int element)
    {
        return +body.rawData[element];
    }
    uint16_t serialiser::operation()
    {
        return +header.operation;
    }
    uint32_t serialiser::ticket()
    {
        return +header.ticket;
    }

    /// @brief returns pointer to c style array of the body
    /// @return
    uint8_t * serialiser::c_bodyStr()
    {
        uint8_t *array = new uint8_t[bodySize() + 4]{0};

        for (size_t i = 0; i < bodySize(); i++)
        {
            array[i] = body.rawData[i];
        }
        return array;
    }
    /// @brief creates a std string and assigns body data to it
    std::string serialiser::bodyAsString()
    {
        std::string str;
        for (size_t i = 0; i < bodySize(); i++)
        {
            str.push_back((char)bodyAccsess(i));
        }
        return str;
    }

}

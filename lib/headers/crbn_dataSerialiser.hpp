// #pragma once

#ifndef DATA_TRANSFER
#define DATA_TRANSFER

#include <memory> //remove ???
#include <vector>
#include <iostream>
#include <algorithm>
#include <chrono>
#include <cstdint>
#include <cstring>

#include "crbn_logging.hpp"

class headerClass
{
public:
    // specifies what to do with the data sent
    uint16_t operation = +0;
    // how big is the body
    uint64_t bodysize = +0;

    uint32_t ticket = +0;
};

class bodyClass
{
public:
    std::vector<uint8_t> rawData;
};

namespace crbn
{
    /// @brief used to store namespace constants
    namespace serc
    {
        const size_t OPERATION_SIZE_T = 2;
        const size_t BODYSIZE_T = 8;
        const size_t TICKET_SIZE_T = 4;
        const size_t HEADER_SIZE_T = OPERATION_SIZE_T + BODYSIZE_T + TICKET_SIZE_T;
    }

    class serialiser
    {
    public:
        const size_t OPERATION_SIZE_T = serc::OPERATION_SIZE_T;
        const size_t BODYSIZE_T = serc::BODYSIZE_T;
        const size_t TICKET_SIZE_T = serc::TICKET_SIZE_T;
        const size_t HEADER_SIZE_T = serc::HEADER_SIZE_T;

    private:
        bodyClass body;
        headerClass header;

    public:
        /// @brief calls rawHeaderIn();
        /// @param rawInput
        /// @param readBody t: reads header + body | f: doesnt read body, just reads header
        serialiser(uint8_t *rawInput, bool readBody);

        /// @brief interprets raw header which has been output by rawDatOut()
        /// @param rawInput
        void rawHeaderIn(uint8_t *rawInput, bool readBody);

        /// @brief calls initialise() with data provided
        serialiser();

        /// @brief calls initialise() with data provided
        serialiser(uint16_t op, uint32_t ticket);

        /// @brief calls initialise() with data provided
        serialiser(uint8_t *bodyInp, size_t size);

        /// @brief calls initialise() with data provided
        serialiser(uint8_t *bodyInp, size_t size, uint16_t op);

        /// @brief calls initialise() with data provided
        serialiser(uint8_t *bodyInp, size_t size, uint16_t operation, uint32_t ticket);

        /// @brief sets up class with data
        /// @param bodyInp raw data
        /// @param size size in bytes of raw data
        /// @param operation crbn::op enum
        /// @param ticket unique identifier to be used to identify each client request
        void initialise(uint8_t *bodyInp, size_t size, uint16_t operation, uint32_t ticket);

        /// @brief appends (size) number of bytes to the end of the body
        /// @param bodyInp raw data (not serialized data)
        /// @param size
        void bodyAppend(uint8_t *bodyInp, size_t size);

        /// @brief ovverides body via size given
        /// @param bodyInp
        /// @param size
        void bodyInput(uint8_t *bodyInp, size_t size);

        /// @brief rewrites body with array data according to size stored in header
        /// @param bodyInp
        void bodyInput(uint8_t *bodyInp);

        /// @brief serializes data into raw array pointer
        /// @return returns pointer to uint8_t array
        uint8_t *rawDatOut();
        
        /// @brief returns size of raw data, header.bodysize + header size
        /// @return 
        size_t size();

        /// @brief returns size of body
        /// @return
        size_t bodySize();

        /// @brief returns the 1 byte of data as uint8_t at the given index of the body
        /// @param element
        /// @return
        uint8_t bodyAccsess(const int element);

        /// @brief returns the stored operation
        /// @return 
        uint16_t operation();
        
        /// @brief returns the stored ticket
        /// @return 
        uint32_t ticket();

        /// @brief returns pointer to c style array of the body
        /// @return
        uint8_t *c_bodyStr();

        /// @brief creates a std string and assigns body data to it
        std::string bodyAsString();
        
    };
}

#endif
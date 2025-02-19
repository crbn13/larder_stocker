// #pragma once
#ifndef CRBN_INSTRUCTIONS

#define CRBN_INSTRUCTIONS

// #include <crbn_jobManager.hpp>
// #include "crbn_logging.hpp"
// #include "crbn_json.hpp"

// #include <fstream>
// #include <iostream>

// #include <../nlohmann/json.hpp>
// using json = nlohmann::json;

namespace crbn
{
    /// @brief DEPRECIATED DO NOT USE
    // namespace ins
    // {
    //     const int sendString = 100;

    //     /// @brief enquires data about the id, server returns json file of the data
    //     const int ID_ENQUIRY = 200;

    // }

    
    /// @brief Defined operations, number values assigned to tasks
    namespace op
    {

        enum
        {
            /// @brief tells client that its request has been recieved by server
            SERVER_RECIEVED_REQUEST = 1,

            /// @brief tells client that this is the data which was requested
            REQUESTED_DATA,

            /// @brief used by client to check if it can communicate with server
            CHECK_CONNECTION,

            /// @brief server responds to aformentioned check
            CONNECTION_CONFIRMED,

            /// @brief likely used for testing, sends a c string to server
            SEND_C_STRING,

            /// @brief sends a json string literal to client from the clients request of a specific tag
            SEND_JSON_TO_CLIENT,

            /// PRODUCE SPECIFIC ~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~

            /// @brief sets the ammount of a specific produce, sends a string number
            SET_AMMOUNT,

            SET_NAME,
            
            /// @brief tells server which item of produce that the client wants to edit
            SELECT_KEY,
            /// @brief client -> server | enquires data about the id, server returns json file of the data
            ID_ENQUIRY,
            /// @brief server -> client | returns json string binary of the data by id requested by client
            ID_ENQUIRY_RESPONSE,

            /// @brief client -> server | requests server to respond with the tag data of the image
            IMAGE_REQUEST,

            /// @brief sends png to client duh
            SEND_PNG_TO_CLIENT,
            
            /// @brief tells client that the image request has FAILED, BOOOOO , im really not sure that i can do anything about this sadly
            IMAGE_REQUEST_FAILED,

            /// @brief this ones a bit silly i wont lie, gives client data which client already has but oh well im not giong to do thinking right now
            TELL_CLIENT_PNG_NAME
        };

    }
}

#endif
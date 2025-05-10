#ifndef CRBN_OPERATIONS_HPP
#define CRBN_OPERATIONS_HPP

#include "crbn_dataSerialiser.hpp"

namespace crbn
{
    /// @brief Defined operations, number values assigned to tasks
    namespace op
    {
        /// uint16_t
        typedef decltype(headerClass::operation) OPERATIONS_TYPE;

        /// @brief type == uint16_t
        enum : OPERATIONS_TYPE
        {
            //~~~~~~~~~ Server -> Client ~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~

            /// @brief tells client that its request has been recieved by server
            SERVER_RECIEVED_REQUEST = 1,

            /// @brief tells client that this is the data which was requested
            REQUESTED_DATA,

            /// @brief tells client what the currently selected string is as a char[]
            SELECTED_KEY,

            /// @brief server responds to aformentioned check
            CONNECTION_CONFIRMED,

            /// @brief sends a json string literal to client from the clients request of a specific tag
            SEND_JSON_TO_CLIENT,

            /// * sends .png picture name to client as a string
            ///
            /// * this ones a bit silly i wont lie, gives client data which client
            /// * already has but oh well im not giong to do thinking right now
            TELL_CLIENT_PNG_NAME,

            /// @brief sends png to client duh
            SEND_PNG_TO_CLIENT,

            /// @brief Start of possible fail codes
            FAIL_RANGE_START,

            /// @brief tells client that the image request has FAILED, BOOOOO
            /// im really not sure that i can do anything about this sadly
            IMAGE_REQUEST_FAILED,

            /// @brief end of range of possible fail codes
            FAIL_RANGE_END,

            //~~~~~~~~~ Client -> Server ~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~

            /// @brief used by client to check if it can communicate with server
            CHECK_CONNECTION,

            /// @brief tells server which item of produce that the client wants to edit
            SELECT_KEY,

            /// @brief asks server to return the currently selectred string key
            WHICH_KEY_SELECTED,

            /// @brief decrements the active tag "number_of" by 1 and records date that it happened, likely will be called internaly
            CONSUME_1,
            
            /// @brief increases the active tag "number_of" by 1 and records input date:
            /// not sure if it will actually be used
            ADD_1,

            /// @brief sets the human readable name of the selected produce tag.
            ///
            /// eg : tag = 123457890 , name = "baked beans"
            SET_NAME,

            /// @brief enquires data about the id, server returns json file of the data
            ID_ENQUIRY,

            /// @brief sends a char* json literal data to the server which is written to the "active tag"
            SET_JSON,

            /// requests server to respond with the tag data of the image
            ///
            /// bodydata = string tag_name ,, eg : "123456789"
            /// server will read the json data and lookup image based on name stored there
            IMAGE_REQUEST, 

            /// requests the server to crate a shopping list
            /// returns a json as a c string 
            GENERATE_SHOPPING_LIST,
        };
    }
}

#endif

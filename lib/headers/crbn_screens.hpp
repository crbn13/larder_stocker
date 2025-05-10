// #pragma once
#ifndef CRBN_SCREENS
#define CRBN_SCREENS

#include "../olcPixelGameEngine.h"
#include "../olcPGEX_QuickGUI.h"

#include "crbn_dataSerialiser.hpp"
#include "crbn_operations.hpp"
#include "crbn_json.hpp"
#include "crbn_jobManager.hpp"

#include <cstdint>
#include <optional>//std::optional
#include <memory>  //smart pointers
#include <algorithm> //std::transform
#include <sys/types.h>
#include <tuple>

// #include "crbn_instructions.hpp"


// #include "crbn_PGE_SceneManager.hpp"
// #include "crbn_jobManager.hpp"


namespace crbn
{
    /// @brief screen related activities :p
    namespace scr
    {

namespace buttonNames
{
enum : u_char
{
    START = 1,          // used for iteration
    CONSUME,            // set count --
    CONSUME_X_AMMOUNT,
    SET_NAME,           // human readable name
    //SET_NUM_PER_SCAN,   // multi pack boxes, how many individual??
    ADD_TO_X,           // variable x used for doing multi additions, multi removes, or just sending numbers
    MINUS_TO_X,
    SET_ONE_X,          // setx variable x to 1, for doing multiple things at once
    ADD_ONE_CONTAINER,
    ADD_ONE_INDIVIDUAL,
    SAVE,               // saves and updates server
    CANCEL,             // cancelles changes
    REQUEST_SHOPPING_LIST, // reuqiweiwier
    
    END                 // used for iteration and creating array
};
};

namespace labelNames
{
enum : u_char
{
    BACKGROUND = 0, // used as a bakground box to go behind text 
    TEXT_INPUT,
    END_SIZE, // used for array creation
};
};


enum active_screen
{
    MAIN_MENU = 1,
    SETTINGS,
    SCANN_INPUT,
    INITIALISE_DATA,
    /// @brief this is more of a module used to go over the top of other screens
    KEYBOARD_OVERLAY,
    SHOPPING_LIST,
    QUIT_PROGRAM
};
float textScale(int dims);
olc::vf2d textScale(olc::vi2d screensize);

class TagDataManager
{
public:
    TagDataManager();

    void giveJobPointer(std::shared_ptr<crbn::Client_Jobs>);
    
    void giveString(const std::string textinput);
    void run(float, olc::PixelGameEngine *);
   
    void reset();
    
    bool render = false; // render graphics
    bool _has_json_data = false; // has the local json data? 

    bool _is_self_tag_set = false; // has the _tag been assigned a tag
    bool _has_tag_select_key_been_made = false; // has the crbn::op::SELECT_KEY been sent to server
    bool _has_tag_enquiry_been_made = false;    // has crbn::op::ID_ENQUIRY been sent to server
    bool _has_tag_which_key_selected_been_made = false; // has crbn::op::WHICH_KEY_SELECTED been sent to server
    bool _is_server_tag_same = false; // checks if return of crbn::op::WHICH_KEY_SELECTED is the same as _tag
    bool _update_gui_elements = true; // tells gui to redo teh gui elemets because of new data
    bool _is_text_input = false;
    int8_t _active_input = 0;

    serc::TICKET_TYPE _json_data_recieve_ticket = 0;
    serc::TICKET_TYPE _is_server_tag_same_ticket= 0;
    serc::TICKET_TYPE _shopping_list_ticket = 0;
    std::string _shopping_list = " waiting ";
    bool _is_shopping_list_requested = false;

    const float wait_time = 0.0f; // unimplemented : used to resend out requests if waiting for too
                                  // long from server
    enum active_input : int8_t
    {
        name_change = 1,
        otherchange
    };
    std::string _tag;
    std::string _string_input;
    std::string _string_inupt_saved;
    std::queue<crbn::serialiser> tasksQueue; // a queue for sending tasks to the server, incase 
    
    std::vector<std::string> _stringData; // stores string data of the important data;

    bool setTag(const std::string s);
    
    std::shared_ptr<json> _j;
    std::shared_ptr<crbn::Client_Jobs> _job;            
    
    int _gui_mod_by_val = 1; // used to send bulk requests
    std::unique_ptr<olc::QuickGUI::Label> _modByLabel;
    // gui objects :
    std::unique_ptr<olc::QuickGUI::Manager> _guimanager;
    std::unique_ptr<olc::QuickGUI::Manager> _layer2manager;
    
    std::vector<std::unique_ptr<olc::QuickGUI::Label>> labels;
    std::array<std::unique_ptr<olc::QuickGUI::Button>, crbn::scr::buttonNames::END> buttons;
    
    // image stuff : 
    bool _has_made_image_request = false;
    serc::TICKET_TYPE _image_ticket = 0;
    bool _has_image_name = false;
    bool _has_image = false;
    std::string _image_name;
    olc::Sprite *image = nullptr;
    olc::Decal *image_decal = nullptr;

    ~TagDataManager() = default;
};



const int no_screen_change = 0;

olc::vi2d posFinder3d(olc::vi2d elements, olc::vi2d relativePosition, std::optional<olc::vi2d> = std::nullopt );

olc::vi2d posFinder3d(olc::vi2d elements, olc::vi2d relativePosition, olc::vi2d );

class ShoppingList
{
public:
    void reset();
    ShoppingList(olc::PixelGameEngine *pge, std::shared_ptr<crbn::Client_Jobs> jobs);
    int run(float *felapsedTime, olc::PixelGameEngine *pge, std::shared_ptr<crbn::Client_Jobs> jobs);
private:

    float widthScreen = 0.0f;
    float heightScreen = 0.0f;
    olc::vf2d buttonSize = {100, 100};

    olc::QuickGUI::Manager _guiManager;
    olc::QuickGUI::Button* _reset = nullptr;
    olc::QuickGUI::Button* _back = nullptr;
    olc::QuickGUI::ListBox* _dataList = nullptr;
    bool _has_made_shopping_list_request;
    bool _has_shopping_list;
    crbn::serc::TICKET_TYPE _shopping_list_request_ticket;
    json _shopping_list_tags;
    std::vector<json> _jsonDatas;
    bool _has_all_data;
    bool _requested_all_data;
    std::vector<std::tuple<serc::TICKET_TYPE, bool>> _json_data_tickets;
    std::vector<std::string> _list_text;
};


class MainMenu
{
public:
    MainMenu(olc::PixelGameEngine *pge, crbn::Client_Jobs *jobs);
    void resetGuiElements(olc::PixelGameEngine *pge);
    int run(float *felapsedTime, olc::PixelGameEngine *pge, crbn::Client_Jobs *jobs);

private:
    std::unique_ptr<olc::QuickGUI::Manager> manager;

    std::unique_ptr<olc::QuickGUI::Button > buttonToInpScr;
    std::unique_ptr<olc::QuickGUI::Button > buttonToOptns;
    std::unique_ptr<olc::QuickGUI::Button > buttonToShoppingList;
    std::unique_ptr<olc::QuickGUI::Button > buttonQuit;

    float howManyItemsVertiacal = 0.0f;
    float howManyItemsHorisontal = 0.0f;

    float WidthScreen = 0.0f;
    float HeightScreen = 0.0f;

    olc::vf2d buttonSize;

    olc::vi2d elements  ;
    olc::vi2d screenDims;

    float frameTime = 0.0f;

    json jConfig;
};

class ScannInput
{
private:
    std::unique_ptr<olc::QuickGUI::Manager> guiManager;

    std::unique_ptr<olc::QuickGUI::Button > clearTextEntry;
    std::unique_ptr<olc::QuickGUI::Button > confirmTextInput;
    std::unique_ptr<olc::QuickGUI::Button > returnToMainMenu;

    std::vector<std::string> expectedJsons;

    olc::Sprite *image = nullptr;
    std::string imageName;
    olc::Decal *decalImage = nullptr;

    float widthScreen = 0.0f;
    float heightScreen = 0.0f;

    olc::vf2d buttonSize = {100, 100};

    olc::vi2d elements = {0, 0};
    olc::vi2d screenDims = {0, 0};

    float howManyItemsVertiacal = 0.0f;
    float howManyItemsHorisontal = 0.0f;

    float toStopLag = 0.0F;

private:
    std::string selfTextInputString;

    void rawTextInputUpdate(olc::PixelGameEngine *pge);

    void tbInit();

    bool imageSelected = false;

    void resetGuiElements(olc::PixelGameEngine * pge);

    TagDataManager _tagManager;

public:

    ScannInput(olc::PixelGameEngine *pge, std::shared_ptr<crbn::Client_Jobs> jobs);
    
    int run(float *felapsedTime, olc::PixelGameEngine * pge, std::shared_ptr<crbn::Client_Jobs> jobs);
    ~ScannInput() = default;
};

}
}

#endif

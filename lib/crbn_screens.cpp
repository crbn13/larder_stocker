#include "headers/crbn_screens.hpp"
#include "asio/associated_allocator.hpp"
#include "headers/crbn_dataSerialiser.hpp"
#include "headers/crbn_jobManager.hpp"
#include "headers/crbn_json.hpp"
#include "headers/crbn_logging.hpp"
#include "headers/crbn_operations.hpp"
#include "olcPGEX_QuickGUI.h"
#include <bit>
#include <cstddef>
#include <filesystem>
#include <memory>
#include <optional>
#include <string>
#include <sys/types.h>

namespace crbn
{
/// @brief screen related activities :p
namespace scr
{

olc::vi2d posFinder3d(olc::vi2d elements, olc::vi2d relativePosition, std::optional<olc::vi2d> screen_Dims)
{
    static olc::vi2d screen_Dimensions = olc::vi2d(0, 0);
    if (screen_Dims.has_value())
        screen_Dimensions = screen_Dims.value();
    return { ((screen_Dimensions.x / elements.x) * relativePosition.x),
             ((screen_Dimensions.y / elements.y) * relativePosition.y) };
}

olc::vi2d posFinder3d(olc::vi2d elements, olc::vi2d relativePosition, olc::vi2d screen_Dimensions)
{
    return { ((screen_Dimensions.x / elements.x) * relativePosition.x),
             ((screen_Dimensions.y / elements.y) * relativePosition.y) };
}

float textScale(int screen_x)
{
    float x = (float)(screen_x / 600.0F);
    return (x > 0.5F) ? x : 1.0F;
}
olc::vf2d textScale(olc::vi2d screensize)
{
    float x = (float)((screensize.x / 1200.0F));
    float y = (x > 0) ? x : 1.0F;
    return { y, y };
}

TagDataManager::TagDataManager()
    : _tag()
    , _guimanager(new olc::QuickGUI::Manager(false))
    , _layer2manager(new olc::QuickGUI::Manager(false))
    , _job()
    , _string_input(" ")
    , _j()
{
}

void TagDataManager::reset()
{
    render = false;
    _has_json_data = false;
    _is_self_tag_set = false;
    _has_tag_select_key_been_made = false;
    _has_tag_enquiry_been_made = false;
    _has_tag_which_key_selected_been_made = false;
    _is_server_tag_same = false;
    _update_gui_elements = true;
    _is_text_input = false;
    _json_data_recieve_ticket = 0;
    _is_server_tag_same_ticket= 0;
    _tag.clear();
    _shopping_list_ticket = 0;
    _is_shopping_list_requested = false;
    _has_made_image_request = false;
    _has_image_name = false;
    _image_ticket = 0;
    if (_has_image)
    {
        delete image;
        delete image_decal;
    }
    _has_image = false;
}

bool TagDataManager::setTag(const std::string tag)
{
    // posFinder3d({10, 10}, {10, 10});

    if (_job == nullptr)
    {
        throw std::runtime_error("TagDataManager : run giveJobPointer() to give pointer to job manager ");
    }
    else if (tag.size() > 0)
    {
        reset();
        _is_self_tag_set = true;
        _has_json_data = false;
        _json_data_recieve_ticket = _job->sendNetworkRequest(crbn::op::ID_ENQUIRY, tag);
        _job->sendNetworkRequest(crbn::op::SELECT_KEY, tag);
        _job->activeTag = "";
        _has_tag_enquiry_been_made = true;
        _has_tag_select_key_been_made = true;
        _has_tag_which_key_selected_been_made = false;
    }
    else
    {
        throw std::runtime_error("TagDataManager : Cannot have a 0 lenght tag ");
    }
    return true;
}

void TagDataManager::giveJobPointer(std::shared_ptr<crbn::Client_Jobs> jobManager)
{
    if (jobManager == nullptr)
    {
        throw std::runtime_error("crbn::scr::TagDataManager::giveJobPointer() : given nullptr, bad ");
    }
    else
    {
        _job = jobManager;
    }
}

void TagDataManager::giveString(const std::string textinput)
{
    if (_is_text_input)
    {
        _string_input = textinput;
    }
    else
    {
        _string_input = " ";
    }
}

void TagDataManager::run(float elapsedTime, olc::PixelGameEngine* pge)
{
    if (!_is_self_tag_set) // if the tag that this manages isnt set
    {
        crbn::warn("TagDataManager being run with no tag set");
        return;
    }
    if (!_has_tag_which_key_selected_been_made)
    {
        _is_server_tag_same_ticket = _job->sendNetworkRequest(crbn::op::WHICH_KEY_SELECTED, _tag);
        _has_tag_which_key_selected_been_made = true;
    }
    else if (!_has_json_data) // if dont have the json data
    {
        if (_job->isDataAvailable(_json_data_recieve_ticket))
        {
            _j = _job->getJsonData(_json_data_recieve_ticket);
            _has_json_data = true;
        }
        else
        {
            crbn::log(" still watiing from json data");
        }
    }
    else if (!_is_server_tag_same) // if the server side actively editing tag isnt the same
    {
        // if ( !(_tag.size() > 0))
        // throw std::runtime_error("TagDataManager : Cannot have a 0 lenght tag ");
        if (_job->activeTag().size() > 0)
        {
            if (_job->activeTag() == _tag)
            {
                _is_server_tag_same = true;
            }
        }
        else
        {
            crbn::log(
                "crbn::scr::TagDataManager::run() waiting for server to answer about active tag");
            return;
        }
    }

    if (render)
    {
        if (!_has_json_data)
        {
            // render loading image
        }
        else // has the json data
        {
            auto temp = posFinder3d({ 10, 10 }, { 1, 2 });
            if (temp != posFinder3d({ 10, 10 }, { 1, 2 }, std::optional(pge->GetScreenSize())))
                _update_gui_elements = true;

            if (_update_gui_elements)
            // if (true)
            {
                if (labels.size() > 0)
                    labels.erase(labels.begin(), labels.end());

                labels.reserve(labelNames::END_SIZE);
                std::cout << +(u_int8_t)labelNames::END_SIZE << std::endl;

                for (int i = 0; i < +(u_int8_t)labelNames::END_SIZE; i++) // Reserve const values of
                                                                          // label before adding the
                                                                          // variable stuff
                {
                    labels.emplace_back(std::unique_ptr<olc::QuickGUI::Label>());
                }

                _guimanager.reset(new olc::QuickGUI::Manager(false));
                _layer2manager.reset(new olc::QuickGUI::Manager(false));

                int i = 1;

                json jsn = json::parse(crbn::jsn::jsonLiterals::data);

                auto iterator = jsn.begin();

                _guimanager->textScale = textScale(pge->GetScreenSize().x);
                _layer2manager->textScale = textScale(pge->GetScreenSize().x);

                posFinder3d({ 1, 1 }, { 0, 0 }, std::optional(pge->GetWindowSize()));

                int labels_x_position = 3;
                size_t longestText = 0;

                std::string tagdata;
                for (auto x : *_j.get())
                {
                    bool valid = false;
                    if (x.is_string())
                    {
                        tagdata = x.get<std::string>();
                        valid = true;
                    }
                    else if (x.is_number())
                    {
                        tagdata = std::to_string(x.get<double>());
                        valid = true;
                    }
                    else if (x.is_boolean())
                    {
                        tagdata = (x.get<bool>() ? "true" : "false");
                        valid = true;
                    }

                    if (valid)
                    {
                        std::string name = iterator.key();
                        std::replace(
                            name.begin(),
                            name.end(),
                            '_',
                            ' '); //                                              position size
                        //
                        labels.emplace_back(new olc::QuickGUI::Label(
                            *_guimanager.get(),
                            tagdata,
                            static_cast<olc::vf2d>(posFinder3d({ 8, 16 }, { 3, i })),
                            (posFinder3d({ 8, 16 }, { labels_x_position, i })
                             - posFinder3d({ 8, 16 }, { labels_x_position, i - 1 }))));

                        labels.back()->nAlign = olc::QuickGUI::Label::Alignment::Left;
                        if (tagdata.size() > longestText)
                            longestText = tagdata.size();
                        labels.emplace_back(new olc::QuickGUI::Label(
                            *_guimanager.get(),
                            name,
                            static_cast<olc::vf2d>(posFinder3d({ 8, 16 }, { 1, i })),
                            (posFinder3d({ 8, 16 }, { labels_x_position, i })
                             - posFinder3d({ 8, 16 }, { labels_x_position, i - 1 }))));
                        labels.back()->nAlign = olc::QuickGUI::Label::Alignment::Left;
                        i++;
                    }

                    iterator++;
                }

                // Background box behind text
                // std::cout << _guimanager->textScale << " " << longestText << std::endl;
                labels[labelNames::BACKGROUND].reset(new olc::QuickGUI::Label(
                    *_layer2manager.get(),
                    "",
                    posFinder3d({ 16, 16 }, { 1, 1 }),
                    labels[labels.size() - 2]->vPos
                        + olc::vf2d(
                            (float)longestText * 6.0f * _layer2manager->textScale,
                            labels[labels.size() - 2]->vSize.y)));

                labels[labelNames::BACKGROUND]->bHasBackground = true;
                labels[labelNames::BACKGROUND]->bHasBorder = true;

                // create buttonss
                // if (buttons.size() > 0)
                // buttons.erase(buttons.begin(), buttons.end());
                // buttons.reserve(10);

                olc::vf2d buttonSize = (pge->GetScreenSize() / 8.0F) - 10.0F;
                olc::vf2d shortbutton = { buttonSize.x, buttonSize.y / 2 };
                olc::vf2d thinbutton = { buttonSize.x / 2, buttonSize.y };

                using namespace buttonNames;
                // buttons[CONSUME].reset(new olc::QuickGUI::Button(*_guimanager.get(), "button",
                // posFinder3d({8, 16}, {5, 5}), buttonSize ) );
                olc::vf2d labelbox_rightbottomcorner
                    = labels[labelNames::BACKGROUND]->vPos + labels[labelNames::BACKGROUND]->vSize;
                olc::vf2d labelBoxOffset
                    = { 0, labelbox_rightbottomcorner.y + posFinder3d({ 16, 16 }, { 0, 1 }).y };
                _modByLabel.reset(new olc::QuickGUI::Label(
                    *_guimanager.get(),
                    "",
                    posFinder3d({ 16, 16 }, { 2, 0 }) + labelBoxOffset,
                    buttonSize / 2 - 10));
                _modByLabel->bHasBackground = true;
                _modByLabel->bHasBorder = true;
                buttons[ADD_TO_X].reset(new olc::QuickGUI::Button(
                    *_guimanager.get(),
                    " + ",
                    posFinder3d({ 16, 16 }, { 3, 0 }) + labelBoxOffset,
                    buttonSize / 2 - 10));
                buttons[MINUS_TO_X].reset(new olc::QuickGUI::Button(
                    *_guimanager.get(),
                    " - ",
                    posFinder3d({ 16, 16 }, { 1, 0 }) + labelBoxOffset,
                    buttonSize / 2 - 10));
                buttons[SET_ONE_X].reset(new olc::QuickGUI::Button(
                    *_guimanager.get(),
                    "Reset",
                    posFinder3d({ 16, 16 }, { 1, 0 }) + labelBoxOffset
                        + olc::vf2d(0, shortbutton.y),
                    { buttons[ADD_TO_X]->vPos.x + buttons[ADD_TO_X]->vSize.x
                          - olc::vf2d(posFinder3d({ 16, 16 }, { 1, 0 })).x,
                      shortbutton.y }));
                buttons[SAVE].reset(new olc::QuickGUI::Button(
                    *_guimanager.get(), "Save \n", posFinder3d({ 8, 8 }, { 2, 7 }), buttonSize));
                buttons[CANCEL].reset(new olc::QuickGUI::Button(
                    *_guimanager.get(), "CANCEL\n", posFinder3d({ 8, 8 }, { 1, 7 }), buttonSize));

                labels[labelNames::TEXT_INPUT].reset(new olc::QuickGUI::Label(
                    *_guimanager.get(),
                    "",
                    olc::vf2d(
                        labels[labelNames::BACKGROUND]->vPos.x
                            + labels[labelNames::BACKGROUND]->vSize.x,
                        labels[labelNames::BACKGROUND]->vPos.y)
                        + olc::vf2d { labels[labelNames::BACKGROUND]->vPos.x / 2, 0 },
                    posFinder3d({ 16, 16 }, { 15, 3 })
                        - olc::vf2d(
                            labels[labelNames::BACKGROUND]->vPos.x
                                + labels[labelNames::BACKGROUND]->vSize.x,
                            labels[labelNames::BACKGROUND]->vPos.y)));

                labels[labelNames::TEXT_INPUT]->bHasBackground = true;
                labels[labelNames::TEXT_INPUT]->bHasBorder = true;
                buttons[buttonNames::ADD_ONE_INDIVIDUAL].reset(new olc::QuickGUI::Button(
                    *_guimanager.get(),
                    "Add",
                    posFinder3d({ 8, 8 }, { 2, 0 }) + labelBoxOffset,
                    buttonSize));
                buttons[buttonNames::CONSUME].reset(new olc::QuickGUI::Button(
                    *_guimanager.get(),
                    "Consume",
                    posFinder3d({ 8, 8 }, { 3, 0 }) + labelBoxOffset,
                    buttonSize));
                buttons[buttonNames::SET_NAME].reset(new olc::QuickGUI::Button(
                    *_guimanager.get(),
                    "Set Name",
                    labels[labelNames::TEXT_INPUT]->vPos
                        + olc::vf2d(0, labels[labelNames::TEXT_INPUT]->vSize.y)
                        + olc::vf2d(0, shortbutton.y / 2),
                    buttonSize));
                buttons[buttonNames::REQUEST_SHOPPING_LIST].reset(new olc::QuickGUI::Button(
                    *_guimanager.get(),
                    "Generate\nShopping\nList",
                    buttons[SET_NAME]->vPos + posFinder3d({ 8, 8 }, { 0, 1 }),
                    buttonSize));
                _update_gui_elements = false;
            }

            // image stuff
            if (!_has_image && !_job->activeTag().empty())
            {
                if (!_has_made_image_request)
                {
                    _image_ticket = _job->sendNetworkRequest(
                        crbn::op::IMAGE_REQUEST,
                        (u_char*)_job->activeTag().c_str(),
                        _job->activeTag().size());
                    _has_made_image_request = true;
                }
                else if (!_has_image_name)
                {
                    if (_job->isDataAvailable(_image_ticket))
                    {
                        // _image_name =
                        // _job->getData<crbn::serialiser>(_image_ticket)->bodyAsString(); //
                        // function not implemented
                        _image_name = std::get<std::string>(_job->dataMap().at(_image_ticket));
                        _has_image_name = true;
                    }
                }
                else if (_has_image_name)
                {
                    if (std::filesystem::exists(_image_name))
                    {
                        // image is downloaded
                        _has_image = true;
                        image = new olc::Sprite(_image_name);
                        image_decal = new olc::Decal(image);
                    }
                    else
                    {
                        _has_image = false;
                    }
                }
            }

            using namespace buttonNames;

            _modByLabel->sText = std::to_string(_gui_mod_by_val);

            if (buttons[ADD_TO_X]->bPressed)
                _gui_mod_by_val++;
            if (buttons[MINUS_TO_X]->bPressed)
                _gui_mod_by_val--;
            if (buttons[SET_ONE_X]->bPressed)
                _gui_mod_by_val = 1;
            if (buttons[CANCEL]->bPressed) // seg faults because not init
                this->reset();
            if (buttons[CONSUME]->bPressed)
            {
                (*_j)["number_of"] = (*_j)["number_of"].get<int>() - _gui_mod_by_val;
                _update_gui_elements = true;
            }
            if (buttons[ADD_ONE_INDIVIDUAL]->bPressed)
            {
                (*_j)["number_of"] = _gui_mod_by_val + (*_j)["number_of"].get<int>();
                _update_gui_elements = true;
            }
            if (buttons[SET_NAME]->bPressed)
            {
                _is_text_input = true;
                _active_input = name_change;
            }
            if (pge->GetKey(olc::ENTER).bPressed)
            {
                if (_active_input == name_change)
                {
                    crbn::warn("Missing string validation");
                    (*_j.get())["name"] = _string_input;
                    _update_gui_elements = true;
                    _string_input.clear();
                }
                _active_input = 0;
                _is_text_input = false;
            }
            if (buttons[SAVE]->bPressed)
            {
                _job->sendNetworkRequest(
                    crbn::op::SET_JSON, (u_char*)_j->dump().c_str(), _j->dump().size());
            }
            if (buttons[REQUEST_SHOPPING_LIST]->bPressed)
            {
                _shopping_list_ticket
                    = _job->sendNetworkRequest(crbn::op::GENERATE_SHOPPING_LIST, nullptr, 0);
            }

            if (_job->isDataAvailable(_shopping_list_ticket))
            {
                _shopping_list = _job->getJsonData(_shopping_list_ticket)->dump();
            }

            pge->DrawStringDecal({ 0, 0 }, _shopping_list);
            _layer2manager->Update(pge);
            _layer2manager->DrawDecal(pge);

            _guimanager->Update(pge);
            _guimanager->DrawDecal(pge);
            pge->DrawStringDecal(
                labels[labelNames::TEXT_INPUT]->vPos
                    + olc::vf2d(0, labels[labelNames::TEXT_INPUT]->vSize.y * (1.0F / 3.0F)),
                _string_input,
                olc::WHITE,
                olc::vf2d(_guimanager->textScale, _guimanager->textScale));

            // if (_has_image)
            if (_has_image)
            {
                olc::vf2d buttonSize = (pge->GetScreenSize() / 8.0F) - 10.0F;
                pge->DrawDecal(
                    buttons[buttonNames::REQUEST_SHOPPING_LIST]->vPos
                        + olc::vf2d(0, buttonSize.y * 2),
                    image_decal,
                    olc::vf2d(
                        (posFinder3d({ 8, 8 }, { 7, 7 })
                         - buttons[buttonNames::REQUEST_SHOPPING_LIST]->vPos
                         + olc::vf2d(0, buttonSize.y * 2)))
                        / image->Size());
            }
        }
    } // if (render)

    // checks if there are any requests which need requesting
    // checks datamap for tag
    // render buttons
    // check button input and send to server
    // image??
}


void ShoppingList::reset()
{
    buttonSize.x = widthScreen / 8.0f - 15.0f;
    buttonSize.y = heightScreen / 8.0f - 15.0f;

    _guiManager = olc::QuickGUI::Manager(true); // should destruct old pointers in theory

    _guiManager.textScale = textScale(widthScreen);
    _back = new olc::QuickGUI::Button(
        _guiManager,
        "Back",
        posFinder3d(
            { 8, 8 }, { 1, 7 }, std::optional(olc::vi2d { (int)widthScreen, (int)heightScreen })),
        buttonSize);
    _dataList = new olc::QuickGUI::ListBox(
        _guiManager,
        _list_text,
        posFinder3d({ 8, 8 }, { 0, 0 }) + olc::vf2d { 20, 20 },
        posFinder3d({ 8, 8 }, { 7, 6 }) - posFinder3d({ 8, 8 }, { 0, 0 }));
}

ShoppingList::ShoppingList(olc::PixelGameEngine* pge, std::shared_ptr<crbn::Client_Jobs> jobs)
    : _guiManager(true)
{
    _has_all_data = false;
    _has_made_shopping_list_request = false;
    _has_shopping_list = false;
    _shopping_list_request_ticket = 0;
    _requested_all_data = false;

    pge->Clear(olc::DARK_GREEN);
    heightScreen = pge->GetWindowSize().y;
    widthScreen = pge->GetWindowSize().x;
    reset();
}

int ShoppingList::run(
    float* felapsedTime, olc::PixelGameEngine* pge, std::shared_ptr<crbn::Client_Jobs> jobs)
{

    if ((pge->GetScreenSize() > pge->GetWindowSize() + olc::vi2d { 10, 10 })
        or (pge->GetScreenSize() < pge->GetWindowSize() - olc::vi2d { 10, 10 }))
    {
        pge->SetScreenSize(pge->GetWindowSize().x, pge->GetWindowSize().y);
        heightScreen = pge->GetWindowSize().y;
        widthScreen = pge->GetWindowSize().x;
        reset();
    }
    if (!_has_shopping_list)
    {
        if (!_has_made_shopping_list_request)
        {
            crbn::warn(" Making shoppinglist request");
            _shopping_list_request_ticket
                = jobs->sendNetworkRequest(crbn::op::GENERATE_SHOPPING_LIST);
            _has_made_shopping_list_request = true;
        }
        else if (_has_made_shopping_list_request)
        {
            if (jobs->isDataAvailable(_shopping_list_request_ticket))
                _has_shopping_list = true;
        }
    }
    else if (_has_shopping_list)
    {
        // request made, so check for data
        if (!_requested_all_data)
        {
            _shopping_list_tags = *jobs->getJsonData(_shopping_list_request_ticket);
            crbn::warn("making All of id_enquiry requests ;" );
            // request the data that goes along with each tag
            for (auto& x : _shopping_list_tags[jsn::keys::known_have_none])
            {
                _json_data_tickets.push_back(
                    { jobs->sendNetworkRequest(crbn::op::ID_ENQUIRY, x.get<std::string>()),
                      false });
            }
            for (auto& x : _shopping_list_tags[jsn::keys::predicted])
            {
                _json_data_tickets.push_back(
                    { jobs->sendNetworkRequest(crbn::op::ID_ENQUIRY, x.get<std::string>()),
                      false });
            }
            // all the data has been requested;
            _requested_all_data = true;
        }
        if (!_has_all_data)
        {
            crbn::warn("Checking for recieved data");
            // check on status of data requests
            bool all_data_recieved = true;
            for (auto& x : _json_data_tickets)
            {
                if (std::get<bool>(x) == false)
                {
                    all_data_recieved = false;
                    if (jobs->isDataAvailable(std::get<serc::TICKET_TYPE>(x)))
                    // if data available and if not already obtained
                    {
                        _list_text.push_back(
                            (*jobs->getJsonData(
                                std::get<crbn::serc::TICKET_TYPE>(x)))[jsn::keys::name]
                                .get<std::string>()
                            + " | remaining = "
                            + std::to_string((*jobs->getJsonData(
                                std::get<crbn::serc::TICKET_TYPE>(x)))[jsn::keys::number_of]
                                                 .get<int>()));
                        std::get<bool>(x) = true;
                        reset();
                    }
                }
            }
            if (all_data_recieved)
                _has_all_data = true;
        }
    }

    if (_back->bPressed)
    {
        return crbn::scr::active_screen::MAIN_MENU;
    }
    _guiManager.Update(pge);
    _guiManager.DrawDecal(pge);
    return crbn::scr::no_screen_change;
}
void MainMenu::resetGuiElements(olc::PixelGameEngine* pge)
{
    WidthScreen = pge->GetWindowSize().x;
    HeightScreen = pge->GetWindowSize().y;

    screenDims = { (int)WidthScreen, (int)HeightScreen };

    buttonSize.x = WidthScreen / howManyItemsVertiacal - 15;
    buttonSize.y = HeightScreen / howManyItemsHorisontal - 15;

    if (manager.operator bool())
        manager = (std::make_unique<olc::QuickGUI::Manager>(false));
    else
        manager.reset(new olc::QuickGUI::Manager(false));

    // std::cout << "HI" << std::endl;

    manager->textScale = textScale((int)WidthScreen);

    buttonToInpScr.reset(new olc::QuickGUI::Button(*manager.get(), "Add items", posFinder3d(elements, { 1, 1 }, screenDims), buttonSize));
    buttonToOptns.reset(new olc::QuickGUI::Button(*manager.get(), "Options", posFinder3d(elements, { 1, 3 }, screenDims), buttonSize));
    buttonToShoppingList.reset(new olc::QuickGUI::Button(*manager.get(), "Shopping List", posFinder3d(elements, { 1, 2 }, screenDims), buttonSize));
    buttonQuit.reset(new olc::QuickGUI::Button(*manager.get(), "QUIT", posFinder3d(elements, { 1, 7 }, screenDims), buttonSize));

    manager->Update(pge);
}

MainMenu::MainMenu(olc::PixelGameEngine* pge, crbn::Client_Jobs* jobs)
    : howManyItemsVertiacal(8.0F)
    , howManyItemsHorisontal(8.0F)
{

    elements = olc::vi2d(howManyItemsHorisontal, howManyItemsVertiacal);
    manager.reset(new olc::QuickGUI::Manager(false));
    manager->Update(pge);

    // howManyItemsVertiacal = 7;
    // elements = {}
    // buttonToInpScr.reset(new olc::QuickGUI::Button(*manager.get(), "Add items", {10, 10}, buttonSize));
    // buttonToOptns.reset(new olc::QuickGUI::Button(*manager.get(), "Options", {10, 10}, buttonSize));
    // buttonQuit.reset(new olc::QuickGUI::Button(*manager.get(), "QUIT", {10, 10}, buttonSize));

    resetGuiElements(pge);
    std::cout << "ihih" << std::endl;
    jobs->checkServerConnection();
    pge->Clear(olc::BLUE);
}

int MainMenu::run(float* felapsedTime, olc::PixelGameEngine* pge, crbn::Client_Jobs* jobs)
{
    pge->Clear(olc::BLUE);

    if (jobs->isServerConnectable)
    {
        pge->DrawString(posFinder3d({ 7, 7 }, { 5, 1 }, { WidthScreen, HeightScreen }), "CONNECTED", olc::WHITE, 3);
    }

    if (buttonQuit->bPressed)
    {
        return active_screen::QUIT_PROGRAM;
    }
    if (buttonToInpScr->bPressed)
    {
        return active_screen::SCANN_INPUT;
    }
    if (buttonToOptns->bPressed)
    {
        return active_screen::SETTINGS;
    }
    if (buttonToShoppingList->bPressed)
    {
        return active_screen::SHOPPING_LIST;
    }

    if ((pge->GetScreenSize() > pge->GetWindowSize() + olc::vi2d { 10, 10 }) or (pge->GetScreenSize() < pge->GetWindowSize() - olc::vi2d { 10, 10 }))
    {
        pge->SetScreenSize(pge->GetWindowSize().x, pge->GetWindowSize().y);
        resetGuiElements(pge);
    }
    manager->Update(pge);
    manager->Draw(pge);

    return crbn::scr::no_screen_change;
}

void ScannInput::tbInit()
{
    // tb = new olc::QuickGUI::TextBox(guiManager, "", posFinder3d(elements, {1, 1}, screenDims), {buttonSize.x * 5, buttonSize.y});
}

void ScannInput::resetGuiElements(olc::PixelGameEngine* pge)
{
    widthScreen = pge->GetWindowSize().x;
    heightScreen = pge->GetWindowSize().y;

    screenDims = { widthScreen, heightScreen };

    buttonSize.x = widthScreen / howManyItemsVertiacal - 15.0f;
    buttonSize.y = heightScreen / howManyItemsVertiacal - 15.0f;

    guiManager.reset(new olc::QuickGUI::Manager(false));

    guiManager->textScale = textScale(screenDims.x);

    clearTextEntry.reset(
        new olc::QuickGUI::Button(*guiManager.get(), "CLEAR", posFinder3d(elements, { 1, 2 }, std::optional(screenDims)), buttonSize));
    confirmTextInput.reset(new olc::QuickGUI::Button(*guiManager.get(), "Confirm", posFinder3d(elements, { 1, 3 }), buttonSize));
    returnToMainMenu.reset(new olc::QuickGUI::Button(*guiManager.get(), "Exit", posFinder3d(elements, { 1, 7 }), buttonSize));
    guiManager->Update(pge);
    _tagManager._update_gui_elements = true;
}

ScannInput::ScannInput(olc::PixelGameEngine* pge, std::shared_ptr<crbn::Client_Jobs> jobs)
    : _tagManager()
    , howManyItemsVertiacal(8.0f)
    , howManyItemsHorisontal(8.0f)
{
    pge->TextEntryEnable(true, "");

    elements = { howManyItemsHorisontal, howManyItemsVertiacal };

    resetGuiElements(pge);

    jobs->async_clientStart();
    jobs->enqueOUT(std::make_shared<crbn::serialiser>(nullptr, 0, crbn::op::WHICH_KEY_SELECTED, jobs->getTicket()));

    guiManager->Update(pge);
}

void ScannInput::rawTextInputUpdate(olc::PixelGameEngine* pge)
{
    const auto& c = pge->GetKeyPressCache();
    if (c.size() > 0)
    {
        const auto* vector = pge->GetKeyboardMap();
        for (const auto& key : c)
            for (const auto& x : *vector)
            {
                if (std::get<0>(x) == key)
                {
                    crbn::log("crbn::scr::ScannInput::rawTextInputUpdate key equals : " + std::to_string(key) + "\n");
                    // if (pge->GetKey(olc::BACK).bPressed)
                    // {
                    //     std::cout << "erasing char \n";
                    //     selfTextInputString.erase(selfTextInputString.size() - 1, 1);
                    // }
                    // else
                    selfTextInputString.append(std::get<1>(x));
                }
                else
                    continue;
            }
    }
}

int ScannInput::run(float* felapsedTime, olc::PixelGameEngine* pge, std::shared_ptr<crbn::Client_Jobs> jobs)
{
    toStopLag += *felapsedTime;
    // selfTextInputString = tb->sText;
    pge->Clear(olc::DARK_CYAN);

    rawTextInputUpdate(pge);
    // crbn::log(" self text input string == " + selfTextInputString);
    if (!_tagManager._is_self_tag_set)
        pge->DrawString(posFinder3d(elements, { 1, 1 }, screenDims), selfTextInputString, olc::WHITE, 3);

    for (int i = 0; i < 8; i++)
    {
        // code to draw guide lines for posFinder3d() positions  :
        //posFinder3d({ 1, 1 }, { 1, 1 }, std::optional(screenDims));
        //pge->DrawLine(posFinder3d(elements, { 0, i }), posFinder3d(elements, { 7, i }));
        //pge->DrawLine(posFinder3d(elements, { i, 0 }), posFinder3d(elements, { i, howManyItemsVertiacal }));
    }

    // tb->sText = selfTextInputString;

    if (image != nullptr)
    {
        pge->DrawDecal(
            posFinder3d(elements, { 3, 4 }, screenDims),
            decalImage,
            olc::vf2d(float(200.0F / float(image->Size().x)), float(200.0F / float(image->Size().y))));
    }

    if (returnToMainMenu->bPressed)
    {
        return crbn::scr::active_screen::MAIN_MENU;
    }
    if (clearTextEntry->bPressed)
    {
        // tb->sText = "";
        selfTextInputString = "";
    }

    if (pge->GetKey(olc::ENTER).bPressed or confirmTextInput->bPressed)
    {
        _tagManager.giveJobPointer(jobs);
        if (!_tagManager._is_self_tag_set)
        {
            _tagManager.setTag(selfTextInputString);
        }
        else 
        {
            _tagManager.giveString(selfTextInputString);
        }
        selfTextInputString.clear();
    }
    if (pge->GetKey(olc::BACK).bPressed)
        if (selfTextInputString.size())
            selfTextInputString.pop_back();

    if (toStopLag > .1F) // runs every time .02 seconds have passed
    {
        toStopLag = 0.0F;

        if ((pge->GetScreenSize() > pge->GetWindowSize() + olc::vi2d { 10, 10 })
            or (pge->GetScreenSize() < pge->GetWindowSize() - olc::vi2d { 10, 10 }))
        {
            pge->SetScreenSize(pge->GetWindowSize().x, pge->GetWindowSize().y);
            resetGuiElements(pge);
        }
    }

    if (_tagManager._is_self_tag_set)
    {
        _tagManager.render = true;
        _tagManager.run(*felapsedTime, pge);
        if (_tagManager._is_text_input)
            _tagManager.giveString(selfTextInputString);
        else
            selfTextInputString.clear();
    }
    else
    {
        guiManager->DrawDecal(pge);
        guiManager->Update(pge);
    }

    return no_screen_change;
};

} // namespace scr
} // namespace crbn

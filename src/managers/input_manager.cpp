#include "input_manager.hpp"
#include "backend_manager.hpp"
#include "theatre_manager.hpp"
#include "input/binding.hpp"
#include "input/event_queue.hpp"
#include "input/demo_controller.hpp"

#include <thread>

// I'm doing this bc I'm lazy and don't want to change the binding IDs bc they're in a specific order
static std::map<ID, ID> sBindingNameIDLookups
{
    // Keys
    { ID{"LShift"},    BindingIDs::KeyLEFTSHIFT    },
    { ID{"RShift"},    BindingIDs::KeyRIGHTSHIFT   },
    { ID{"LCtrl"},     BindingIDs::KeyLEFTCONTROL  },
    { ID{"RCtrl"},     BindingIDs::KeyRIGHTCONTROL },
    { ID{"LAlt"},      BindingIDs::KeyLEFTALT      },
    { ID{"RAlt"},      BindingIDs::KeyRIGHTALT     },
    { ID{"Fn"},        BindingIDs::KeyFUNCTION     },
    { ID{"LSuper"},    BindingIDs::KeyLEFTSUPER    },
    { ID{"RSuper"},    BindingIDs::KeyRIGHTSUPER   },
    { ID{"Enter"},     BindingIDs::KeyENTER        },
    { ID{"Backspace"}, BindingIDs::KeyBACKSPACE    },
    { ID{"Tab"},       BindingIDs::KeyTAB          },
    { ID{"Space"},     BindingIDs::KeySPACE        },
    { ID{"Escape"},    BindingIDs::KeyESC          },
    // Mouse Buttons
    { ID{"LeftMouse"},   BindingIDs::MouseLEFT   },
    { ID{"RightMouse"},  BindingIDs::MouseRIGHT  },
    { ID{"MiddleMouse"}, BindingIDs::MouseMIDDLE },
    // Mouse Motion
    { ID{"MouseX"}, BindingIDs::MouseMotionX },
    { ID{"MouseY"}, BindingIDs::MouseMotionY },
    // Numbers
    { ID{"0"}, BindingIDs::KeyZERO  },
    { ID{"1"}, BindingIDs::KeyONE   },
    { ID{"2"}, BindingIDs::KeyTWO   },
    { ID{"3"}, BindingIDs::KeyTHREE },
    { ID{"4"}, BindingIDs::KeyFOUR  },
    { ID{"5"}, BindingIDs::KeyFIVE  },
    { ID{"6"}, BindingIDs::KeySIX   },
    { ID{"7"}, BindingIDs::KeySEVEN },
    { ID{"8"}, BindingIDs::KeyEIGHT },
    { ID{"9"}, BindingIDs::KeyNINE  },
    // Letters
    { ID{"A"}, BindingIDs::KeyA },
    { ID{"B"}, BindingIDs::KeyB },
    { ID{"C"}, BindingIDs::KeyC },
    { ID{"D"}, BindingIDs::KeyD },
    { ID{"E"}, BindingIDs::KeyE },
    { ID{"F"}, BindingIDs::KeyF },
    { ID{"G"}, BindingIDs::KeyG },
    { ID{"H"}, BindingIDs::KeyH },
    { ID{"I"}, BindingIDs::KeyI },
    { ID{"J"}, BindingIDs::KeyJ },
    { ID{"K"}, BindingIDs::KeyK },
    { ID{"L"}, BindingIDs::KeyL },
    { ID{"M"}, BindingIDs::KeyM },
    { ID{"N"}, BindingIDs::KeyN },
    { ID{"O"}, BindingIDs::KeyO },
    { ID{"P"}, BindingIDs::KeyP },
    { ID{"Q"}, BindingIDs::KeyQ },
    { ID{"R"}, BindingIDs::KeyR },
    { ID{"S"}, BindingIDs::KeyS },
    { ID{"T"}, BindingIDs::KeyT },
    { ID{"U"}, BindingIDs::KeyU },
    { ID{"V"}, BindingIDs::KeyV },
    { ID{"W"}, BindingIDs::KeyW },
    { ID{"X"}, BindingIDs::KeyX },
    { ID{"Y"}, BindingIDs::KeyY },
    { ID{"Z"}, BindingIDs::KeyZ },
};

using namespace ManagerEnums;

InputEventCallbackFunction InputManager::m_sInputEventCallback{nullptr};
std::map<ID, InputBinding> InputManager::m_sInputBindings
{
    // Keys
    { BindingIDs::KeyLEFTSHIFT,     "LShift"    },
    { BindingIDs::KeyRIGHTSHIFT,    "RShift"    },
    { BindingIDs::KeyLEFTCONTROL,   "LCtrl"     },
    { BindingIDs::KeyRIGHTCONTROL,  "RCtrl"     },
    { BindingIDs::KeyLEFTALT,       "LAlt"      },
    { BindingIDs::KeyRIGHTALT,      "RAlt"      },
    { BindingIDs::KeyFUNCTION,      "Fn"        },
    { BindingIDs::KeyLEFTSUPER,     "LSuper"    },
    { BindingIDs::KeyRIGHTSUPER,    "RSuper"    },
    { BindingIDs::KeyENTER,         "Enter"     },
    { BindingIDs::KeyBACKSPACE,     "Backspace" },
    { BindingIDs::KeyTAB,           "Tab"       },
    { BindingIDs::KeySPACE,         "Space"     },
    { BindingIDs::KeyESC,           "Escape"    },
    // Mouse Buttons
    { BindingIDs::MouseLEFT,    "LeftMouse"   },
    { BindingIDs::MouseRIGHT,   "RightMouse"  },
    { BindingIDs::MouseMIDDLE,  "MiddleMouse" },
    // Mouse Motion
    { BindingIDs::MouseMotionX, "MouseX" },
    { BindingIDs::MouseMotionY, "MouseY" },
    // Numbers
    { BindingIDs::KeyZERO,  "0" },
    { BindingIDs::KeyONE,   "1" },
    { BindingIDs::KeyTWO,   "2" },
    { BindingIDs::KeyTHREE, "3" },
    { BindingIDs::KeyFOUR,  "4" },
    { BindingIDs::KeyFIVE,  "5" },
    { BindingIDs::KeySIX,   "6" },
    { BindingIDs::KeySEVEN, "7" },
    { BindingIDs::KeyEIGHT, "8" },
    { BindingIDs::KeyNINE,  "9" },
    // Letters
    { BindingIDs::KeyA, "A" },
    { BindingIDs::KeyB, "B" },
    { BindingIDs::KeyC, "C" },
    { BindingIDs::KeyD, "D" },
    { BindingIDs::KeyE, "E" },
    { BindingIDs::KeyF, "F" },
    { BindingIDs::KeyG, "G" },
    { BindingIDs::KeyH, "H" },
    { BindingIDs::KeyI, "I" },
    { BindingIDs::KeyJ, "J" },
    { BindingIDs::KeyK, "K" },
    { BindingIDs::KeyL, "L" },
    { BindingIDs::KeyM, "M" },
    { BindingIDs::KeyN, "N" },
    { BindingIDs::KeyO, "O" },
    { BindingIDs::KeyP, "P" },
    { BindingIDs::KeyQ, "Q" },
    { BindingIDs::KeyR, "R" },
    { BindingIDs::KeyS, "S" },
    { BindingIDs::KeyT, "T" },
    { BindingIDs::KeyU, "U" },
    { BindingIDs::KeyV, "V" },
    { BindingIDs::KeyW, "W" },
    { BindingIDs::KeyX, "X" },
    { BindingIDs::KeyY, "Y" },
    { BindingIDs::KeyZ, "Z" },
};

static InputBinding sInvalidBinding{};
static std::set<std::string> sEmptyActions{};

static std::map<ID, std::set<std::string>> sBindingActionsLookup{};
static std::set<std::string> sActions{};

static InputManager sInputManager;
InputManager* g_pInputManager = &sInputManager;

void InputManager::HandleInputEvent(const InputEvent& event)
{
    if(!m_sInputEventCallback) { return; }
    m_sInputEventCallback(event);
}

bool InputManager::Init()
{ return true; }

void InputManager::Tick()
{
    EventQueue event_queue;
    PollInputs(event_queue);
    g_pDemoController->ProcessQueue(event_queue);
    InputEvent temp_event;
    while(event_queue.GetNextEvent(temp_event))
    {
        std::thread l_InputEventCallbackThread(HandleInputEvent, temp_event);
        g_pTheatreManager->DelegateInputEvent(temp_event);
        l_InputEventCallbackThread.join();
    }
}

void InputManager::PollInputs(EventQueue& queue)
{
    glm::vec2 last_mouse_position = mMousePosition;
    auto window = g_pBackendManager->Windowing();
    window->GetMousePosition(mMousePosition);
    window->PollEvents();
    for(auto& [id, binding] : m_sInputBindings)
    {
        if(binding.locked())
            { continue; }
        else if(window->GetKey(id, binding))
            { queue.QueueEvent({id}); }
        else if(window->GetMotion(id, binding, mMousePosition - last_mouse_position))
            { queue.QueueEvent({id, mMousePosition, last_mouse_position}); }
    }
}

InputEventCallbackFunction InputManager::SetInputEventCallback(InputEventCallbackFunction callback)
{ return (m_sInputEventCallback = callback); }

const std::set<std::string>& InputManager::GetActions(ID id)
{
    if(!sBindingActionsLookup.contains(id))
        { return sEmptyActions; }
    return sBindingActionsLookup.at(id);
}

bool InputManager::BindingExists(ID id)
{ return m_sInputBindings.contains(id); }

bool InputManager::BindingExists(const std::string& name)
{
    if(hash_t name_hash = ConstexprHash(name); sBindingNameIDLookups.contains(name_hash))
        { return BindingExists(sBindingNameIDLookups.at(name_hash)); }
    return false;
}

InputBinding& InputManager::GetBinding(ID id)
{
    return (m_sInputBindings.contains(id))
        ? m_sInputBindings.at(id)
        : sInvalidBinding;
}

ID InputManager::GetBindingID(const std::string& name)
{
    return (sBindingNameIDLookups.contains(ID{name}))
        ? sBindingNameIDLookups.at(ID{name})
        : ID{ID::Invalid};
}

const std::set<std::string>& InputManager::GetActions(const std::string& name)
{ return GetActions(GetBindingID(name)); }

bool InputManager::AddAction(const std::string& action)
{ return sActions.emplace(action).second; }

bool InputManager::AddAction(const std::string& action, const std::string& input_name)
{ return AddAction(action, GetBindingID(input_name)); }

bool InputManager::AddAction(const std::string& action, ID input_id)
{
    if(sActions.contains(action))
        { return print_error("InputManager::AddAction - Action '{}' already exists", action); }
    else if(!m_sInputBindings.contains(input_id))
        { return print_error("InputManager::AddAction - Invalid binding ID {}", input_id); }
    if(action.at(0) == '+' && action.size() > 1)
        { AddAction("-" + action.substr(1)); }
    sActions.emplace(action);
    return sBindingActionsLookup[input_id].emplace(action).second;
}

bool InputManager::AssignToAction(const std::string& action, const std::string& input_name)
{ return AssignToAction(action, GetBindingID(input_name)); }

bool InputManager::AssignToAction(const std::string& action, ID input_id)
{
    if(!sActions.contains(action))
        { return print_error("InputManager::AssignToAction - Action '{}' doesn't exist", action); }
    else if(!m_sInputBindings.contains(input_id))
        { return print_error("InputManager::AssignToAction - Invalid binding ID {}", input_id); }
    sBindingActionsLookup[input_id].emplace(action);
    return true;
}

bool InputManager::DeleteAction(const std::string& action)
{
    if(!sActions.contains(action))
        { return false; }
    for(auto& [id, actions] : sBindingActionsLookup)
        { actions.erase(action); }
    sActions.erase(action);
    return true;
}

bool InputManager::ClearActions(const std::string& input_name)
{ return ClearActions(GetBindingID(input_name)); }

bool InputManager::ClearActions(ID input_id)
{ return (sBindingActionsLookup.erase(input_id) > 0); }

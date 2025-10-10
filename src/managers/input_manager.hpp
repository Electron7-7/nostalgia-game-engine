#ifndef INPUT_MANAGER_H
#define INPUT_MANAGER_H

#include "input/fwd.hpp"

#include "manager.hpp"
#include "safe_return.hpp"
#include "ids.hpp"

#include <glm/vec2.hpp>
#include <string>
#include <set>

typedef SafeStatus (*InputEventCallbackFunction)(const InputEvent&);

class InputManager : public Manager
{
public:
    bool Init();
    void Tick();

    static bool Pressed(ID InputID);
    static bool Released(ID InputID);
    static bool Active(ID InputID);
    static bool Inactive(ID InputID);
    static bool JustPressed(ID InputID);
    static bool JustActive(ID InputID);
    static bool Pressed(const std::string& InputName);
    static bool Released(const std::string& InputName);
    static bool Active(const std::string& InputName);
    static bool Inactive(const std::string& InputName);
    static bool JustPressed(const std::string& InputName);
    static bool JustActive(const std::string& InputName);

    InputEventCallbackFunction SetInputEventCallback(SafeStatus (*Callback)(const InputEvent&));

    static const std::set<std::string>& GetActions(ID InputID);
    static const std::set<std::string>& GetActions(const std::string& InputName);

    static bool AddAction(const std::string& Action);
    static bool AddAction(const std::string& Action, const std::string& InputName);
    static bool AddAction(const std::string& Action, ID InputID);

    static bool AssignToAction(const std::string& Action, const std::string& InputName);
    static bool AssignToAction(const std::string& Action, ID InputID);

    static bool DeleteAction(const std::string& ActionName);
    static bool ClearActions(const std::string& InputName);
    static bool ClearActions(ID InputID);

    static InputBinding& GetBinding(ID);

private:
    static std::vector<InputBinding> s_Bindings;
    static InputEventCallbackFunction m_sInputEventCallback;
    static glm::vec2 m_sMousePosition;
    static EventQueue m_sInputEventQueue;
    static EventQueue m_sDemoEventQueue;

    static void m_sHandleInputEvent(const InputEvent&);
    void mPollInputs(EventQueue&);
};

extern InputManager* g_pInputManager;

#endif // INPUT_MANAGER_H

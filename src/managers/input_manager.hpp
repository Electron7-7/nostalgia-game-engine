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
    constexpr const char* DebugName() { return "InputManager"; }
    bool Init();
    void Tick();

    InputEventCallbackFunction SetInputEventCallback(SafeStatus (*Callback)(const InputEvent&));

    const std::set<std::string>& GetActions(ID BindingID);
    const std::set<std::string>& GetActions(const std::string& BindingName);

    bool AddAction(const std::string& Action);
    bool AddAction(const std::string& Action, const std::string& BindingName);
    bool AddAction(const std::string& Action, ID BindingID);

    bool AssignToAction(const std::string& Action, const std::string& BindingName);
    bool AssignToAction(const std::string& Action, ID BindingID);

    bool DeleteAction(const std::string& ActionName);
    bool ClearActions(ID BindingID);
    bool ClearActions(const std::string& BindingName);

    InputBinding& GetBinding(ID BindingID);
    InputBinding& GetBinding(const std::string& BindingName);
    ID GetBindingID(const std::string& BindingName);

    bool BindingExists(ID BindingID);
    bool BindingExists(const std::string& BindingName);

private:
    glm::vec2 mMousePosition{0.0f};
    EventQueue* mInputEventQueue{nullptr};
    EventQueue* mDemoEventQueue{nullptr};

    void PollInputs(EventQueue&);

    static std::map<ID, InputBinding> m_sInputBindings;
    static InputEventCallbackFunction m_sInputEventCallback;
    static void HandleInputEvent(const InputEvent&);
};

extern InputManager* g_pInputManager;

#endif // INPUT_MANAGER_H

#ifndef INPUT_MANAGER_H
#define INPUT_MANAGER_H

#include "input/fwd.hpp"

#include "manager.hpp"
#include "input/binding.hpp"
#include "input/actions.hpp"
#include "core/safe_return.hpp"
#include "core/id.hpp"

#include <glm/vec2.hpp>
#include <string>
#include <map>

typedef SafeStatus (*InputEventCallbackFunction)(const InputEvent&);

class InputManager : public Manager
{
public:
    consteval const char* DebugName() { return "InputManager"; }
    bool Init();
    void Tick();

    InputEventCallbackFunction SetInputEventCallback(SafeStatus (*Callback)(const InputEvent&));

    bool BindingExists(const ID& BindingID);
    bool BindingExists(const std::string& BindingName);

    InputBinding& GetBinding(const ID& BindingID);
    InputBinding& GetBinding(const std::string& BindingName);

    bool NewAction(const std::string& Action);
    bool AssignAction(const std::string& Action, const std::string& BindingName);
    bool AssignAction(const std::string& Action, const ID& BindingID);
    bool DeleteAction(const std::string& Action);

    bool ClearActions(const ID& BindingID);
    bool ClearActions(const std::string& BindingName);

private:
    glm::vec2 mMousePosition{0.0f};
    std::map<ID, InputBinding> mBindings{};
    InputActions mAllActions{};

    void PollInputs(InputEvent&);

    InputEventCallbackFunction m_pInputEventCallback{nullptr};
    static bool sHandlingInputEvent;
    static void m_sHandleInputEvent(const InputEvent&, InputEventCallbackFunction);
};

extern InputManager* g_pInputManager;

#endif // INPUT_MANAGER_H

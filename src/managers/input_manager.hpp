#ifndef INPUT_MANAGER_H
#define INPUT_MANAGER_H

#include "manager.hpp"
#include "input/fwd.hpp"
#include "safe_return.hpp"

#include <glm/vec2.hpp>
#include <string>

typedef SafeStatus (*InputEventCallbackFunction)(const InputEvent&);

class InputManager : public Manager
{
public:
    bool Init();
    void Update();

    void PollInputs();

    InputEventCallbackFunction SetInputEventCallback(SafeStatus (*Callback)(const InputEvent&));

    static bool AddAction(const std::string& Action);
    static bool AddAction(const std::string& Action, const std::string& InputName);
    static bool AddAction(const std::string& Action, id_t InputID);

    static bool AssignToAction(const std::string& Action, const std::string& InputName);
    static bool AssignToAction(const std::string& Action, id_t InputID);

    static bool DeleteAction(const std::string& ActionName);
    static bool ClearActions(const std::string& InputName);
    static bool ClearActions(id_t InputID);

private:
    static std::vector<binding_t> s_Bindings;
    static InputEventCallbackFunction m_sInputEventCallback;
    static glm::vec2 m_sCurrentMousePosition;
    static glm::vec2 m_sLastMousePosition;

    static void m_sHandleInputEvent(const InputEvent& event);
};

extern InputManager* g_pInputManager;

#endif // INPUT_MANAGER_H

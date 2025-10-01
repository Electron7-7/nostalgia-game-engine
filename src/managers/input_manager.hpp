#ifndef INPUT_MANAGER_H
#define INPUT_MANAGER_H

#include "manager.hpp"
#include "input/fwd.hpp"
#include "safe_return.hpp"

#include <glm/vec2.hpp>
#include <string>

typedef SafeStatus (*InputEventCallbackFunction)(const InputEvent&);
typedef __gnu_cxx::__normal_iterator<const binding_t*, std::vector<binding_t>> BindingIterator;

class InputManager : public Manager
{
public:
    bool Init();
    void Update();
    void Shutdown();

    void PollInputs();
    static const BindingIterator QueryBinding(id_t InputID);
    static const BindingIterator QueryBinding(const std::string& InputName);
    static bool Pressed(id_t InputID);
    static bool Released(id_t InputID);
    static bool Active(id_t InputID);
    static bool Inactive(id_t InputID);
    static bool JustPressed(id_t InputID);
    static bool JustActive(id_t InputID);
    static bool Pressed(const std::string& InputName);
    static bool Released(const std::string& InputName);
    static bool Active(const std::string& InputName);
    static bool Inactive(const std::string& InputName);
    static bool JustPressed(const std::string& InputName);
    static bool JustActive(const std::string& InputName);

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

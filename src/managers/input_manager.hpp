#ifndef INPUT_MANAGER_H
#define INPUT_MANAGER_H

#include "input/fwd.hpp"

#include "manager.hpp"
#include "demo/demo.hpp"
#include "safe_return.hpp"

#include <glm/vec2.hpp>
#include <string>
#include <set>

typedef SafeStatus (*InputEventCallbackFunction)(const InputEvent&);

class InputManager : public Manager
{
public:
    bool Init();
    void Tick();

    static bool StartRecordingDemo(const std::string& DemoName = "");
    static bool StopRecordingDemo();
    static bool StartPlayingDemo(const std::string& FilePath_Or_DemoData);
    static bool StopPlayingDemo();

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

private:
    static Demo m_sDemo;

    static std::vector<InputBinding> s_Bindings;
    static InputEventCallbackFunction m_sInputEventCallback;
    static glm::vec2 m_sMousePosition;
    static EventQueue m_sInputEventQueue;
    static EventQueue m_sDemoEventQueue;

    static bool m_sRecordingDemo;
    static bool m_sPlayingDemo;
    static bool m_sProcessingQueue;

    static void m_sHandleInputEvent(const InputEvent&);
    void mPollInputs(EventQueue&);

    friend class DemoParser;
    static InputBinding& m_sGetBinding(ID);
};

extern InputManager* g_pInputManager;

#endif // INPUT_MANAGER_H

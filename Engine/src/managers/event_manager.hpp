#ifndef INPUT_MANAGER_H
#define INPUT_MANAGER_H

#include "events/fwd.hpp"

#include "manager.hpp"
#include "events/action.hpp"
#include "core/type_helpers.hpp"
#include "core/error.hpp"

#include <glm/vec2.hpp>
#include <unordered_map>
#include <string>

class EventManager : public Manager
{
public:
    consteval const char* DebugName() final { return "EventManager"; }
    bool Init()   override;
    void Update() override;

    bool UpdateKeyState(FARG(KeyID) inKeyID, bool inCurrentState);
    InputEventCallback SetInputEventCallback(InputEventCallback);

    InputEventQueue* GetListeningInputEventQueue();
    void PushAppEvent(FARG(AppEvent) inAppEvent);

    void SetAction(FARG(InputAction) inAction);
    Error AddAction(FARG(InputAction) inAction);
    Error DeleteAction(FARG(std::string) inActionName);
    void ClearAllActions();

    bool mDebugPrintEverySingleEventToTheConsole{false};

private:
    InputEventCallback m_pInputEventCallback{nullptr};
    std::unordered_map<std::string, InputAction> mInputActions{};
    ushort mCurrentInputEventQueueIndex{0};
    ushort mPreviousInputEventQueueIndex{1};

    static std::unordered_map<KeyID, bool> sInputStateBuffer;
    static std::unordered_map<KeyID, std::vector<std::string>> sInputActionBindingsLookup;
    static std::array<InputEventQueue, 2> sInputEventQueueBuffers;
    static AppEventQueue sAppEventQueue;
};

extern EventManager* g_pEventManager;

#endif // INPUT_MANAGER_H

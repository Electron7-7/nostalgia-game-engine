#ifndef INPUT_MANAGER_H
#define INPUT_MANAGER_H

#include "events/fwd.hpp"

#include "manager.hpp"
#include "core/id.hpp"
#include "core/type_helpers.hpp"

#include <glm/vec2.hpp>
#include <string>

class EventManager : public Manager
{
public:
    consteval const char* DebugName() final { return "EventManager"; }
    bool Init()   override;
    void Update() override;

    InputEventCallback SetInputEventCallback(InputEventCallback);

    InputEventQueue* GetListeningInputEventQueue();

    void AddAction(FARG(InputAction) inAction);
    void DeleteAction(FARG(std::string) inActionName);

    bool ClearActions(FARG(ID) BindingID);
    bool ClearActions(FARG(std::string) BindingName);
    void ClearAllActions();

    bool mDebugPrintEverySingleEventToTheConsole{false};

private:
    InputEventCallback m_pInputEventCallback{nullptr};

    static std::array<EventQueue, 2> sEventQueueBuffers;
    inline static ushort sCurrentInputQueueIndex{0};
    inline static ushort sPreviousInputQueueIndex{1};

    static std::array<InputEventQueue, 2> sInputQueueBuffers;
    inline static ushort sCurrentInputQueueIndex{0};
    inline static ushort sPreviousInputQueueIndex{1};
};

extern EventManager* g_pEventManager;

#endif // INPUT_MANAGER_H

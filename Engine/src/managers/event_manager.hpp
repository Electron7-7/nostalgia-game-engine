#ifndef EVENT_MANAGER_H
#define EVENT_MANAGER_H

#include "fwd/events.hpp"
#include "managers/manager.hpp"

class EventManager : public Manager
{
public:
    consteval const char* DebugName() final { return "EventManager"; }
    bool Init()   override;
    void Update() override;

    bool mDebugPrintEverySingleEventToTheConsole{false};

    static EventQueue* Queue();
};

extern EventManager* g_pEventManager;

#endif // EVENT_MANAGER_H

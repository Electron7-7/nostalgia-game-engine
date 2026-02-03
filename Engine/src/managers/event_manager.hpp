#ifndef EVENT_MANAGER_H
#define EVENT_MANAGER_H

#include <Nostalgia/fwd/events.hpp>
#include <Nostalgia/managers/manager.hpp>

class EventManager : public Manager
{
public:
    consteval const char* DebugName() final { return "EventManager"; }
    bool Init()   override;
    void Update() override;

    static EventQueue* Queue();
};

extern bool gPrintEventLogs;
extern EventManager* g_pEventManager;

#endif // EVENT_MANAGER_H

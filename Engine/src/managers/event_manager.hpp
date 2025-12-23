#ifdef FWD_DCL
    class EventManager;
    extern EventManager* g_pEventManager;
#elif !defined EVENT_MANAGER_H
#define EVENT_MANAGER_H

#define FWD_DCL
#   include "events/event_queue.hpp"
#undef  FWD_DCL

#include "manager.hpp"

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

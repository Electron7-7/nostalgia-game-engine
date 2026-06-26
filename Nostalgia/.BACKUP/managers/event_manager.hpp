#ifndef EVENT_MANAGER_H
#define EVENT_MANAGER_H

#include <Nostalgia/events/event_queue.hpp>
#include <Nostalgia/managers/manager.hpp>

class EventManager : public Manager
{
public:
    constexpr const char* DebugName() final { return "EventManager"; }
    void Update() override;

    static EventQueue* Queue();

private:
    inline static RMutex m_sActiveQueueMutex{}, m_sPassiveQueueMutex{};
};

extern bool gPrintEventLogs;
extern EventManager* g_pEventManager;

#endif // EVENT_MANAGER_H

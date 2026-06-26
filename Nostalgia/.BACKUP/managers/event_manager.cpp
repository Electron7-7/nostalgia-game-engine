#include "event_manager.hpp"
#include "events/event_queue.hpp"
#include "application/application.hpp"

#define LOCK_ACTIVE  LockGuard<RMutex> _lock{m_sActiveQueueMutex};
#define LOCK_PASSIVE LockGuard<RMutex> _lock{m_sPassiveQueueMutex};

static EventQueue sActiveQueue{}, sPassiveQueue{};
static EventManager sEventManager{};
EventManager* g_pEventManager{&sEventManager};
bool gPrintEventLogs{false};

void EventManager::Update()
{
    LOCK_PASSIVE
    {
        LOCK_ACTIVE
        sPassiveQueue = sActiveQueue;
        sActiveQueue.clear();
    }

    for(auto iter{sPassiveQueue.get().begin()}; iter != sPassiveQueue.get().end();)
    {
        Application()->Event(iter->get());
        Manager::InvokeEvent(iter->get());
        if(Console::GetVariable("print_event_logs").int_value)
        {
            __print_verbose(true,
                VERBOSE0,
                "{}",
                std::source_location::current(),
                {"<EVENT>",{ANSI::cyan,true,true}},
                iter->get()->DebugLog());
        }
        iter = sPassiveQueue.get().erase(iter);
    }
}

EventQueue* EventManager::Queue()
{
    LOCK_ACTIVE
    return &sActiveQueue;
}

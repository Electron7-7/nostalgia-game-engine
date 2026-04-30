#include "event_manager.hpp"
#include "events/event_queue.hpp"
#include "application/application.hpp"

static EventQueue sEventQueue{};
static EventManager sEventManager{};
EventManager* g_pEventManager{&sEventManager};
bool gPrintEventLogs{false};

bool EventManager::Init()
{ return true; }

void EventManager::Update()
{
    for(auto iter{sEventQueue.get().begin()}; iter != sEventQueue.get().end();)
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
        iter = sEventQueue.get().erase(iter);
    }
}

EventQueue* EventManager::Queue()
{ return &sEventQueue; }

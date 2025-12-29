#include "event_manager.hpp"
#include "core/printing.hpp"
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
    event_queue_t events{sEventQueue.get()};
    for(auto event : events)
    {
        if(auto app_event{DCast<AppEvent>(event)})
            { Application()->Event(app_event.get()); }
        else if(auto engine_event{DCast<EngineEvent>(event)})
            { IManager::InvokeEvent(engine_event.get()); }
        if(gPrintEventLogs)
        {
            __print_verbose(true,
                VERBOSE0,
                "{}",
                std::source_location::current(),
                {"<EVENT>",{ANSI::cyan,true,true}},
                event->DebugLog());
        }
    }
}

EventQueue* EventManager::Queue()
{ return &sEventQueue; }

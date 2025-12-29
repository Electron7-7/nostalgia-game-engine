#include "event_manager.hpp"
#include "events/event_queue.hpp"
#include "components/event_handling.hpp"
#include "application/application.hpp"

static EventQueue sEventQueue{};
static EventManager sEventManager{};
EventManager* g_pEventManager{&sEventManager};

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
            { IManager::PassEngineEvent(engine_event.get()); }
    }
}

EventQueue* EventManager::Queue()
{ return &sEventQueue; }

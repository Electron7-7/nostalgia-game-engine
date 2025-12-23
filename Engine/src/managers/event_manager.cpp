#include "event_manager.hpp"
#include "events/event_queue.hpp"

static EventQueue sEventQueue{};

static EventManager sEventManager{};
EventManager* g_pEventManager{&sEventManager};

bool EventManager::Init()
{ return true; }

void EventManager::Update()
{ sEventQueue.DispatchEvents(mDebugPrintEverySingleEventToTheConsole); }

EventQueue* EventManager::Queue()
{ return &sEventQueue; }

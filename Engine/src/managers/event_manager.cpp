#include "event_manager.hpp"
#include "events/event_queue.hpp"

EventQueue EventManager::m_sEventQueue{};

static EventManager sEventManager{};
EventManager* g_pEventManager{&sEventManager};

bool EventManager::Init()
{ return true; }

void EventManager::Update()
{ m_sEventQueue.DispatchEvents(mDebugPrintEverySingleEventToTheConsole); }

EventQueue* EventManager::Queue()
{ return &m_sEventQueue; }

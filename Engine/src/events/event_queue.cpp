#include "event_queue.hpp"
#include "core/printing.hpp"
#include "components/event_handling.hpp"

Farg<std::vector<Shared<IEvent>>> EventQueue::get() const noexcept
{ return mEvents; }

bool EventQueue::empty() const noexcept
{ return mEvents.empty(); }

void EventQueue::clear()
{
    do{} while(mProcessingQueue || mQueueingEvents);
    mEvents.clear();
}

void EventQueue::DispatchEvents(bool debugPrintEventLog)
{
    if(mProcessingQueue)
        { return; }
    do{} while(mQueueingEvents);
    mProcessingQueue = true;
    for(auto it{mEvents.begin()}; it != mEvents.end();)
    {
        Shared<IEvent> event{*it};
        switch(event->Type())
        {
        case EventType::InputEvent:
            EventHandler::Instance()->HandleEvent<InputEvent>(event);
            break;
        case EventType::AppEvent:
            EventHandler::Instance()->HandleEvent<AppEvent>(event);
            break;
        case EventType::EngineEvent:
            EventHandler::Instance()->HandleEvent<EngineEvent>(event);
            break;
        default:
            print_warning("Invalid event type");
            break;
        }
        if(debugPrintEventLog)
        {
            __print_verbose(true,
                VERBOSE0,
                "{}",
                std::source_location::current(),
                MessageLabel{"[EVENT]", {ANSI::green, ANSI::foreground}, "EventLabel"},
                event->DebugLog().data());
        }
        it = mEvents.erase(it);
    }
    mProcessingQueue = false;
}

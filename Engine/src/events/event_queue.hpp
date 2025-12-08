#ifndef EVENT_QUEUE_H
#define EVENT_QUEUE_H

#include "event.hpp"
#include "core/type_helpers.hpp"
#include "components/event_handling.hpp"

class EventQueue
{
public:
    FARG(std::vector<Shared<IEvent>>) get() const noexcept
    { return mEvents; }

    bool empty() const noexcept
    { return mEvents.empty(); }

    template<is_event T, class... Args> requires std::is_constructible_v<T, Args...>
        void add(Args... inArgs)
        {
            do{} while(mQueueingEvents || mProcessingQueue);
            mQueueingEvents = true;
            if(!mEvents.empty())
            {
                for(auto it{mEvents.cbegin()}; it != mEvents.cend(); ++it)
                {
                    if(it->get()->Priority() >= T::Priority())
                        { mEvents.insert(it, MakeShared<T>(std::forward<Args>(inArgs)...)); break; }
                }
            }
            else
                { mEvents.push_back(MakeShared<T>(std::forward<Args>(inArgs)...)); }
            mQueueingEvents = false;
        }

    template<is_event T>
        void erase(FARG(T) inEvent, bool beThoroughButAlsoSlow = false) noexcept
        {
            do{} while(mQueueingEvents || mProcessingQueue);
            mQueueingEvents = true;
            for(auto it{mEvents.begin()}; it != mEvents.end();)
            {
                if(auto event_ptr{dynamic_cast<T>(it->get())}; event_ptr && *event_ptr == inEvent)
                {
                    it = mEvents.erase(it);
                    if(beThoroughButAlsoSlow)
                        { continue; }
                    break;
                }
                ++it;
            }
            mQueueingEvents = false;
        }

    void clear()
    {
        do{} while(mProcessingQueue || mQueueingEvents);
        mEvents.clear();
    }

    void DispatchEvents(bool debugPrintEventLog = false)
    {
        if(mProcessingQueue)
            { return; }
        do{} while(mQueueingEvents);
        mProcessingQueue = true;
        for(auto it{mEvents.begin()}; it != mEvents.end();)
        {
            Shared<IEvent>& event{*it};
            switch(event->Type())
            {
            case EventType::AppEvent:
                OnAppEvent::HandleEvent(event);
                break;
            case EventType::EngineEvent:
                OnEngineEvent::HandleEvent(event);
                break;
            case EventType::InputEvent:
                OnInput::HandleEvent(event);
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
                    "\x1b[32m[EVENT] ",
                    event->DebugLog().data());
            }
            it = mEvents.erase(it);
        }
        mProcessingQueue = false;
    }

private:
    bool mProcessingQueue{false};
    bool mQueueingEvents{false};

    std::vector<Shared<IEvent>> mEvents{};
};

#endif // EVENT_QUEUE_H

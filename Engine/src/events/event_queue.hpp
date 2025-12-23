#ifdef FWD_DCL
    class EventQueue;
#elif !defined EVENT_QUEUE_H
#define EVENT_QUEUE_H

#include "event.hpp"
#include "core/smart_pointers.hpp"

class EventQueue
{
public:
    Farg<std::vector<Shared<IEvent>>> get() const noexcept;
    bool empty() const noexcept;
    void clear();
    void DispatchEvents(bool debugPrintEventLog = false);

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
        void erase(Farg<T> inEvent, bool beThoroughButAlsoSlow = false) noexcept
        {
            do{} while(mQueueingEvents || mProcessingQueue);
            mQueueingEvents = true;
            for(auto it{mEvents.begin()}; it != mEvents.end();)
            {
                if(auto event_ptr{DCast<T>(it->get())}; event_ptr && *event_ptr == inEvent)
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

private:
    bool mProcessingQueue{false};
    bool mQueueingEvents{false};

    std::vector<Shared<IEvent>> mEvents{};
};

#endif // EVENT_QUEUE_H

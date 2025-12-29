#ifndef EVENT_QUEUE_H
#define EVENT_QUEUE_H

#include "event.hpp"
#include "core/smart_pointers.hpp"
#include <mutex>

using event_queue_t = std::vector<Shared<IEvent>>;

class EventQueue
{
public:
    event_queue_t& get() noexcept;
    std::recursive_mutex& get_mutex() noexcept;
    bool empty() noexcept;
    void clear() noexcept;

    template<is_event T, class... Args> requires std::is_constructible_v<T, Args...>
        void add(Args... inArgs)
        {
            const std::lock_guard<std::recursive_mutex> lock{mEventsMutex};
            if(!mEvents.empty())
            {
                for(auto it{mEvents.cbegin()}; it != mEvents.cend(); ++it)
                {
                    if(it->get()->Priority() >= T::Priority())
                        { mEvents.insert(it, MakeShared<T>(std::forward<Args>(inArgs)...)); break; }
                }
            }
            else
                { mEvents.emplace_back(MakeShared<T>(std::forward<Args>(inArgs)...)); }
        }

    template<is_event T>
        void erase(Farg<T> inEvent, bool beThoroughButAlsoSlow = false) noexcept
        {
            const std::lock_guard<std::recursive_mutex> lock{mEventsMutex};
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
        }

private:
    event_queue_t mEvents{};
    std::recursive_mutex mEventsMutex{};
};

#endif // EVENT_QUEUE_H

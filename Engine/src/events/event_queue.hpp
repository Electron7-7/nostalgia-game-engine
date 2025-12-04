#ifndef EVENT_QUEUE_H
#define EVENT_QUEUE_H

#include "event.hpp"
#include "core/type_helpers.hpp"

class InputEventQueue
{
public:
    template<typename T, class... Args>
        requires std::derived_from<T, InputEvent> && std::is_constructible_v<T, Args...>
            void add(Args... inArgs)
            { mEvents.push_back(MakeUnique<T>(std::forward<Args>(inArgs)...)); }

    void clear();
    void DispatchEvents();

private:
    bool mQueueIsBusy{false};

    std::vector<Unique<InputEvent>> mEvents{};
};

class AppEventQueue
{
public:
    void add(FARG(AppEvent) inEvent);
    void clear();
    void DispatchEvents();

private:
    bool mQueueIsBusy{false};
    std::vector<AppEvent> mEvents{};
};

#endif // EVENT_QUEUE_H

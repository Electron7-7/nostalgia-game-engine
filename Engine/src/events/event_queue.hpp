#ifndef EVENT_QUEUE_H
#define EVENT_QUEUE_H

#include "fwd.hpp"

#include "core/type_helpers.hpp"

#include <vector>

class InputEventQueue
{
public:
    void PushInputEvents();

    template<typename T, class... Args>
        requires std::derived_from<T,InputEvent> && std::is_constructible_v<T,Args...>
            void add(Args&&...);
    void clear();

private:
    bool mQueueIsBusy{false};
    std::vector<Unique<InputEvent>> mEvents{};
};

#endif // EVENT_QUEUE_H

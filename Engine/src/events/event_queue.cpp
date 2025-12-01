#include "event_queue.hpp"
#include "event.hpp"
#include "components/input_handling.hpp"

void InputEventQueue::PushInputEvents()
{
    do {} while(mQueueIsBusy);

    mQueueIsBusy = true;
    for(auto& event : mEvents)
    {
        OnInputUI::HandleInput(event.get());
        OnInput::HandleInput(event.get());
    }
    mQueueIsBusy = false;
}

void InputEventQueue::clear()
{
    do {} while(mQueueIsBusy);

    mQueueIsBusy = true;
    mEvents.clear();
    mQueueIsBusy = false;
}

template<typename T,class... Args> requires std::derived_from<T,InputEvent> && std::is_constructible_v<T,Args...>
void InputEventQueue::add(Args&&... inArgs)
{ mEvents.push_back(MakeUnique<T>(std::forward<Args>(inArgs)...)); }

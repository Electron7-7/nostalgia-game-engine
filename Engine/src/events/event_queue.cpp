#include "event_queue.hpp"
#include "components/input_handling.hpp"
#include "components/event_handling.hpp"

void InputEventQueue::DispatchEvents()
{
    do {} while(mQueueIsBusy);

    mQueueIsBusy = true;
    for(auto& event : mEvents)
    {
        print_debug("InputEvent: {}", event->GetDebugLog());
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

void AppEventQueue::add(FARG(AppEvent) inEvent)
{ mEvents.push_back(inEvent); }

void AppEventQueue::clear()
{ mEvents.clear(); }

void AppEventQueue::DispatchEvents()
{
    do {} while(mQueueIsBusy);
    mQueueIsBusy = true;
    for(auto& event : mEvents)
        { OnAppEvent::HandleEvent(&event); }
    mQueueIsBusy = false;
}

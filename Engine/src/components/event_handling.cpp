#include "event_handling.hpp"
#include "core/printing.hpp"
#include "events/event.hpp"

#include <vector>

Unique<EventHandler> EventHandler::m_sInstance{nullptr};

EventHandler::EventHandler() noexcept
{ assert(!m_sInstance && "Only one EventHandler instance allowed!"); }

EventHandler::~EventHandler() noexcept
{
    mInputEventInstances.clear();
    mAppEventInstances.clear();
    mEngineEventInstances.clear();
}

Unique<EventHandler>& EventHandler::Instance()
{
    if(!m_sInstance)
        { m_sInstance = Unique<EventHandler>(new EventHandler{}); }
    return m_sInstance;
}

void EventHandler::Add(OnInput* inInstancePtr)
{ mInputEventInstances.emplace_back(inInstancePtr); }

void EventHandler::Add(OnAppEvent* inInstancePtr)
{ mAppEventInstances.emplace_back(inInstancePtr); }

void EventHandler::Add(OnEngineEvent* inInstancePtr)
{ mEngineEventInstances.emplace_back(inInstancePtr); }

void EventHandler::Erase(OnInput* inInstancePtr)
{
    if(auto found_it{std::find(mInputEventInstances.begin(), mInputEventInstances.end(), inInstancePtr)};
        found_it != mInputEventInstances.end())
        { mInputEventInstances.erase(found_it); }
}

void EventHandler::Erase(OnAppEvent* inInstancePtr)
{
    if(auto found_it{std::find(mAppEventInstances.begin(), mAppEventInstances.end(), inInstancePtr)};
        found_it != mAppEventInstances.end())
        { mAppEventInstances.erase(found_it); }
}

void EventHandler::Erase(OnEngineEvent* inInstancePtr)
{
    if(auto found_it{std::find(mEngineEventInstances.begin(), mEngineEventInstances.end(), inInstancePtr)};
        found_it != mEngineEventInstances.end())
        { mEngineEventInstances.erase(found_it); }
}


template<>
    void EventHandler::HandleEvent<InputEvent>(Shared<IEvent> inEvent)
    {
        for(auto instance : mInputEventInstances)
            { instance->Input(dynamic_pointer_cast<InputEvent>(inEvent).get()); }
    }

template<>
    void EventHandler::HandleEvent<AppEvent>(Shared<IEvent> inEvent)
    {
        for(auto instance : mAppEventInstances)
            { instance->Event(dynamic_pointer_cast<AppEvent>(inEvent).get()); }
    }

template<>
    void EventHandler::HandleEvent<EngineEvent>(Shared<IEvent> inEvent)
    {
        for(auto instance : mEngineEventInstances)
            { instance->Event(dynamic_pointer_cast<EngineEvent>(inEvent).get()); }
    }

OnInput::OnInput() noexcept
{ EventHandler::Instance()->Add(this); }

OnInput::~OnInput() noexcept
{ EventHandler::Instance()->Erase(this); }

OnAppEvent::OnAppEvent() noexcept
{ EventHandler::Instance()->Add(this); }

OnAppEvent::~OnAppEvent() noexcept
{ EventHandler::Instance()->Erase(this); }

OnEngineEvent::OnEngineEvent() noexcept
{ EventHandler::Instance()->Add(this); }

OnEngineEvent::~OnEngineEvent() noexcept
{ EventHandler::Instance()->Erase(this); }

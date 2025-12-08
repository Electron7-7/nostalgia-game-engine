#ifndef EVENT_HANDLING_COMPONENTS_H
#define EVENT_HANDLING_COMPONENTS_H

#include "events/event.hpp"

#include "core/type_helpers.hpp"

#include <vector>
#include <algorithm>

class IGetEvents
{
public:
    inline static std::vector<IGetEvents*> m_sInstances{};

    IGetEvents() noexcept
    { PRINT_PRETTY_FUNCTION; m_sInstances.push_back(this); }

    virtual ~IGetEvents() noexcept
    {
        if(const auto& found_it{std::find(m_sInstances.begin(), m_sInstances.end(), this)};
            found_it != m_sInstances.end())
        { m_sInstances.erase(found_it); }
    }

    virtual void Event(AppEvent*) = 0;
    virtual void Event(EngineEvent*) = 0;
    virtual void Input(InputEvent*) = 0;
};

class OnInput
{
public:
    inline static std::vector<OnInput*> m_sInstances{};

    OnInput() noexcept
    { PRINT_PRETTY_FUNCTION; m_sInstances.push_back(this); }

    virtual ~OnInput() noexcept
    {
        if(const auto& found_it{std::find(m_sInstances.begin(), m_sInstances.end(), this)};
            found_it != m_sInstances.end())
        { m_sInstances.erase(found_it); }
    }

    static void HandleEvent(Shared<IEvent>& inEvent)
    { for(auto instance : m_sInstances) { instance->Input(dynamic_cast<InputEvent*>(inEvent.get())); } }

    virtual void Input(InputEvent*) = 0;
};

class OnAppEvent
{
public:
    inline static std::vector<OnAppEvent*> m_sInstances{};

    OnAppEvent() noexcept
    { PRINT_PRETTY_FUNCTION; m_sInstances.push_back(this); }

    virtual ~OnAppEvent() noexcept
    {
        if(const auto& found_it{std::find(m_sInstances.begin(), m_sInstances.end(), this)};
            found_it != m_sInstances.end())
        { m_sInstances.erase(found_it); }
    }

    static void HandleEvent(Shared<IEvent>& inEvent)
    { for(auto instance : m_sInstances) { instance->Event(dynamic_cast<AppEvent*>(inEvent.get())); } }

    virtual void Event(AppEvent*) = 0;
};

class OnEngineEvent
{
public:
    inline static std::vector<OnEngineEvent*> m_sInstances{};

    OnEngineEvent() noexcept
    { PRINT_PRETTY_FUNCTION; m_sInstances.push_back(this); }

    virtual ~OnEngineEvent() noexcept
    {
        if(const auto& found_it{std::find(m_sInstances.begin(), m_sInstances.end(), this)};
            found_it != m_sInstances.end())
        { m_sInstances.erase(found_it); }
    }

    static void HandleEvent(Shared<IEvent>& inEvent)
    { for(auto instance : m_sInstances) { instance->Event(dynamic_cast<EngineEvent*>(inEvent.get())); } }

    virtual void Event(EngineEvent*) = 0;
};

#endif // EVENT_HANDLING_COMPONENTS_H

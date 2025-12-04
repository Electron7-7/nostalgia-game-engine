#ifndef EVENT_HANDLING_COMPONENT_H
#define EVENT_HANDLING_COMPONENT_H

#include "events/fwd.hpp"

#include <vector>
#include <algorithm>

class OnAppEvent
{
public:
    OnAppEvent() noexcept
    { m_sInstances.push_back(this); }

    virtual ~OnAppEvent() noexcept
    {
        if(const auto& found_it{std::find(m_sInstances.begin(), m_sInstances.end(), this)};
            found_it != m_sInstances.end())
        { m_sInstances.erase(found_it); }
    }

    virtual void Event(AppEvent*) = 0;

    inline static void HandleEvent(AppEvent* inEvent)
    { for(auto& instance : m_sInstances) { instance->Event(inEvent); } }

private:
    inline static std::vector<OnAppEvent*> m_sInstances{};
};

#endif // EVENT_HANDLING_COMPONENT_H

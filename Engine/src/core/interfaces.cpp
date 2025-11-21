#include "interfaces.hpp"

#include <algorithm>

std::vector<HandlesInput*> HandlesInput::m_sInstances{};

HandlesInput::HandlesInput()
{ m_sInstances.push_back(this); }

HandlesInput::~HandlesInput()
{
    if(auto found_it{std::find(m_sInstances.begin(), m_sInstances.end(), this)};
        found_it != m_sInstances.end())
    { m_sInstances.erase(found_it); }
}

void HandlesInput::DelegateInputEvent(const InputEvent& inEvent)
{
    for(auto instance : m_sInstances)
        { instance->Input(inEvent); }
}

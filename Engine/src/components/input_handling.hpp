#ifndef INPUT_HANDLING_COMPONENT_H
#define INPUT_HANDLING_COMPONENT_H

#include "events/fwd.hpp"

#include <vector>
#include <algorithm>

namespace
{
    template<bool isUI>
        class IGetInputEvents
        {
        public:
            IGetInputEvents() noexcept
            { m_sInstances.push_back(this); }

            virtual ~IGetInputEvents() noexcept
            {
                if(const auto& found_it{std::find(m_sInstances.begin(), m_sInstances.end(), this)};
                    found_it != m_sInstances.end())
                { m_sInstances.erase(found_it); }
            }

            virtual void Input(InputEvent*) = 0;

            inline static void HandleInput(InputEvent* inEvent)
            { for(auto& instance : m_sInstances) { instance->Input(inEvent); } }

        private:
            inline static std::vector<IGetInputEvents<isUI>*> m_sInstances{};
        };
}

class OnInput   : public IGetInputEvents<false> {}; // isUI == false
class OnInputUI : public IGetInputEvents<true>  {}; // isUI == true

#endif // INPUT_HANDLING_COMPONENT_H

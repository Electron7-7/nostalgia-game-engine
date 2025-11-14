#include "event.hpp"
#include "binding.hpp"
#include "managers/backend_manager.hpp"

#include <math/glm_format.hpp>
#include <format>

void InputEvent::UpdateMouseMotion(const glm::vec2& current, const glm::vec2& last)
{
    mCurMousePos  = current;
    mLastMousePos = last;
}

const glm::vec2& InputEvent::CurrentMousePosition() const
{ return mCurMousePos; }

const glm::vec2& InputEvent::LastMousePosition() const
{ return mLastMousePos; }

glm::vec2 InputEvent::MouseMotion() const
{ return mCurMousePos - mLastMousePos; }

InputStatus InputEvent::StateOf(const ID& id) const
{
    return (mInputs.contains(id))
        ? mInputs.find(id)->status()
        : InputStatus::Inactive;
}

bool InputEvent::IsKeyDown(const ID& id) const
{ return QueryInput(id, &InputBinding::Active); }

bool InputEvent::IsKeyUp(const ID& id) const
{ return QueryInput(id, &InputBinding::Inactive); }

bool InputEvent::IsKeyPressed(const ID& id) const
{ return QueryInput(id, &InputBinding::JustActivated); }

bool InputEvent::IsKeyReleased(const ID& id) const
{ return QueryInput(id, &InputBinding::JustDeactivated); }

bool InputEvent::IsMouseCaptured() const
{ return g_pBackendManager->Windowing()->GetMouseMode() == MouseMode::Disabled; }

bool InputEvent::empty() const
{ return mInputs.empty(); }

bool InputEvent::add(const InputBinding& binding)
{ return mInputs.insert(binding).second; }

bool InputEvent::erase(const InputBinding& binding)
{ return mInputs.erase(binding) != 0; }

void InputEvent::clear()
{ mInputs.clear(); }

std::string InputEvent::Log() const
{
    std::string log{std::format("InputEvent Log [Tick#{}] (CurrentMousePosition:{}) (LastMousePosition:{})",
        mTick,
        mCurMousePos,
        mLastMousePos)};
    for(auto input : mInputs)
    {
        log += std::format("\n\t<Input{{{}}}:State{{{}}}:Timing{{{}}}:Actions{{{}}}>",
            input.id().name(),
            ((bool)input.status()) ? "Active" : "Inactive",
            ((bool)input.just_changed()) ? "Just Changed" : "Stagnant",
            input.mActions);
    }
    return log;
}

std::string InputEvent::DemoString() const
{
    std::string output{std::format("({:0.3f},{:0.3f}) ({:0.3f},{:0.3f})",
        mCurMousePos.x,
        mCurMousePos.y,
        mLastMousePos.x,
        mLastMousePos.y)};
    for(const auto& input : mInputs)
    {
        output +=
            std::format("\n\t<{}:{}:{}:{}>",
                input.id().name(),
                static_cast<int>(input.status()),
                static_cast<int>(input.just_changed()),
                input.mActions
            );
    }
    return output;
}

bool InputEvent::QueryInput(const ID& id, InputQueryFunction function) const
{
    return (mInputs.contains(id))
        ? ((*mInputs.find(id)).*function)()
        : false;
}

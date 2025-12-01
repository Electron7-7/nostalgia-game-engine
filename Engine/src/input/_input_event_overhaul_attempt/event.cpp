#include "event_trough.hpp"

#include <math/glm_format.hpp>
#include <format>


std::string EventTrough::Log() const
{
    std::string log{std::format("EventTrough Log [Tick#{}] (CurrentMousePosition:{}) (LastMousePosition:{})",
        mTick,
        mMouseMotion.GetCurrentMousePosition(),
        mMouseMotion.GetLastMousePosition())};
    for(auto input : mKeys)
    {
        log += std::format("\n\t<Input{{{}}}:State{{{}}}:Timing{{{}}}:Actions{{{}}}>",
            input.id().name(),
            ((bool)input.status()) ? "Active" : "Inactive",
            ((bool)input.just_changed()) ? "Just Changed" : "Stagnant",
            input.mActions);
    }
    return log;
}

std::string EventTrough::DemoString() const
{
    std::string output{std::format("({:0.3f},{:0.3f}) ({:0.3f},{:0.3f})",
        mCurMousePos.x,
        mCurMousePos.y,
        mLastMousePos.x,
        mLastMousePos.y)};
    for(const auto& input : mBindings)
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

#define DO_NOT_COMPILE
#ifndef DO_NOT_COMPILE

void EventTrough::UpdateMouseMotion(const glm::vec2& current, const glm::vec2& last)
{
    mCurMousePos  = current;
    mLastMousePos = last;
}

const glm::vec2& EventTrough::CurrentMousePosition() const
{ return mCurMousePos; }

const glm::vec2& EventTrough::LastMousePosition() const
{ return mLastMousePos; }

glm::vec2 EventTrough::MouseMotion() const
{ return mCurMousePos - mLastMousePos; }

InputStatus EventTrough::StateOf(const ID& id) const
{
    return (mBindings.contains(id))
        ? mBindings.find(id)->status()
        : InputStatus::Inactive;
}

bool EventTrough::IsKeyDown(const ID& id) const
{ return QueryInput(id, &InputBinding::Active); }

bool EventTrough::IsKeyUp(const ID& id) const
{ return QueryInput(id, &InputBinding::Inactive); }

bool EventTrough::IsKeyPressed(const ID& id) const
{ return QueryInput(id, &InputBinding::JustActivated); }

bool EventTrough::IsKeyReleased(const ID& id) const
{ return QueryInput(id, &InputBinding::JustDeactivated); }

#pragma message("FIXME: This should probably not be such an engine-level check(?)")
bool EventTrough::IsMouseCaptured() const
{ return Application()->GetWindow().GetMouseMode() == IWindow::MOUSE_MODE_CAPTURED; }

bool EventTrough::empty() const
{ return mBindings.empty(); }

bool EventTrough::add(const InputBinding& binding)
{ return mBindings.insert(binding).second; }

bool EventTrough::erase(const InputBinding& binding)
{ return mBindings.erase(binding) != 0; }

void EventTrough::clear()
{ mBindings.clear(); }

bool EventTrough::QueryInput(const ID& id, InputQueryFunction function) const
{
    return (mBindings.contains(id))
        ? ((*mBindings.find(id)).*function)()
        : false;
}

#endif // DO_NOT_COMPILE

#include "event.hpp"
#include "binding.hpp"
#include "common/printing.hpp"
#include "managers/input_manager.hpp"

#include <format>

#pragma message("TODO: Decide whether or not to keep `InputEvent::mTime` and `InputEvent::mTick`")

InputEvent::InputEvent(const InputBinding& binding, const glm::vec2& current, const glm::vec2& last):
    InputBinding(binding),
    mCurMousePos{current},
    mLastMousePos{last} {}

glm::vec2 InputEvent::CurrentMousePosition() const
{ return mCurMousePos; }

glm::vec2 InputEvent::LastMousePosition() const
{ return mLastMousePos; }

glm::vec2 InputEvent::MouseMotion() const
{
    if(!IsMouseMotion())
        { print_warning("InputEvent::MouseMotion called on an event that isn't mouse motion"); }
    return mCurMousePos - mLastMousePos;
}

InputEventType InputEvent::Type() const
{
    if(BindingIDs::IsKey(mID))
        { return InputEventType::Key; }
    else if(BindingIDs::IsMouseButton(mID))
        { return InputEventType::MouseButton; }
    else if(BindingIDs::IsMouseMotion(mID))
        { return InputEventType::MouseMotion; }
    return InputEventType::Invalid;
}

bool InputEvent::IsType(InputEventType EventType) const
{ return (Type() == EventType); }

std::string InputEvent::Log() const
{
    return std::format(
        "InputEvent Log - Creation Time: {:0.3f}, Tick: {} Event Type: {:11}, Input ID: {}, InputName: '{}', Actions: {}",
        mTime,
        mTick,
        gInputEventType[static_cast<size_t>(Type())],
        mID,
        mID.name(),
        mActions);
}

std::string InputEvent::DemoString() const
{
    std::string output{
        std::format("<{}:{}:{}:{}>",
            mID.name(),
            static_cast<int>(mStatus),
            static_cast<int>(mJustChanged),
            mActions
        )
    };
    if(IsMouseMotion())
    {
        output += std::format(" ({:0.3f},{:0.3f}) ({:0.3f},{:0.3f})",
            mCurMousePos.x,
            mCurMousePos.y,
            mLastMousePos.x,
            mLastMousePos.y);
    }
    return output;
}

bool InputEvent::Valid() const
{ return g_pInputManager->BindingExists(mID); }

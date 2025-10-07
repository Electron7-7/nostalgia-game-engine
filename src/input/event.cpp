#include "event.hpp"
#include "printing.hpp"

#include <format>

InputEvent::InputEvent() = default;

InputEvent::InputEvent(const InputEvent& Copy):
    mTime(Time::Elapsed()),
    mTick(_Manager::TickNumber()),
    mBinding(Copy.mBinding),
    mActions(Copy.mActions),
    mCurMousePos(Copy.mCurMousePos),
    mLastMousePos(Copy.mLastMousePos)
{}

InputEvent::InputEvent(const std::string& InputName, InputStatus Status, const std::set<std::string>& Actions):
    mBinding(InputName, Status),
    mActions(Actions)
{}

InputEvent::InputEvent(const InputBinding& InputBinding, const std::set<std::string>& Actions):
    mBinding(InputBinding),
    mActions(Actions)
{}

InputEvent::InputEvent(const InputBinding& InputBinding, const glm::vec2& CurMousePos, const glm::vec2& LastMousePos, const std::set<std::string>& Actions):
    mBinding(InputBinding),
    mActions(Actions),
    mCurMousePos(CurMousePos),
    mLastMousePos(LastMousePos)
{}

const glm::vec2& InputEvent::CurrentMousePosition() const
{ return mCurMousePos; }

const glm::vec2& InputEvent::LastMousePosition() const
{ return mLastMousePos; }

glm::vec2 InputEvent::MouseMotion() const
{
    if(!IsMouseMotion())
        { PRINT_WARNING("InputEvent::MouseMotion called on an event that isn't mouse motion") }
    return mCurMousePos - mLastMousePos;
}

const std::set<std::string>& InputEvent::Actions() const
{ return mActions; }

const InputBinding& InputEvent::Binding() const
{ return mBinding; }

InputStatus InputEvent::Status() const
{ return mBinding.status; }

bool InputEvent::JustPressed() const
{ return (Pressed() && mBinding.just_changed); }

bool InputEvent::JustReleased() const
{ return Released(); }

bool InputEvent::Pressed() const
{ return (Status() == InputStatus::Pressed); }

bool InputEvent::Released() const
{ return (Status() == InputStatus::Released); }

bool InputEvent::JustActive() const
{ return JustPressed(); }

bool InputEvent::Active() const
{ return Pressed(); }

bool InputEvent::Inactive() const
{ return Released(); }

InputEventType InputEvent::Type() const
{
    if(mBinding > BindingIDs::end)
        { return InputEventType::Invalid; }
    else if(mBinding < BindingIDs::MouseButtonsEnd)
        { return InputEventType::Key; }
    else if(mBinding < BindingIDs::MouseMotionFront)
        { return InputEventType::MouseButton; }
    return InputEventType::MouseMotion;
}

bool InputEvent::IsMouseMotion() const
{ return (mBinding >= BindingIDs::MouseMotionFront && mBinding <= BindingIDs::MouseMotionEnd); }

bool InputEvent::IsType(InputEventType EventType) const
{ return (Type() == EventType); }

bool InputEvent::IsAction(const std::string& Action) const
{
    if(!mActions.contains(Action))
        { return false; }
    else if(Action.at(0) == '-')
        { return Released(); }
    return Pressed();
}

bool InputEvent::IsInput(const std::string& InputName) const
{ return (BindingIDs::GetID(InputName) == mBinding); }

bool InputEvent::IsInput(const InputBinding& InputBinding) const
{ return (mBinding == InputBinding); }

std::string InputEvent::Log() const
{
    return std::format(
        "InputEvent Log - Creation Time: {:0.3f}, Tick: {} Event Type: {:11}, Input ID: {}, InputName: '{}', Actions: {}",
        mTime,
        mTick,
        gInputEventType[static_cast<size_t>(Type())],
        static_cast<id_t>(mBinding),
        BindingIDs::GetName(mBinding),
        (mActions.empty()) ? std::set<std::string>{"none"} : mActions);
}

std::string InputEvent::DemoString() const
{
    return std::format("[{}:{}:{}] ({:0.3f},{:0.3f}) ({:0.3f},{:0.3f})",
        static_cast<id_t>(mBinding),
        static_cast<int>(mBinding.status),
        static_cast<int>(mBinding.just_changed),
        mCurMousePos.x,
        mCurMousePos.y,
        mLastMousePos.x,
        mLastMousePos.y);
}

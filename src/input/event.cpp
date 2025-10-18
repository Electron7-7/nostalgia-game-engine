#include "event.hpp"
#include "binding.hpp"
#include "printing.hpp"
#include "managers/input_manager.hpp"

#include <format>

InputEvent::InputEvent(ID id):
    mBindingID(id) {}

InputEvent::InputEvent(ID id, const glm::vec2& current_mouse, const glm::vec2& last_mouse):
    mBindingID(id),
    mCurMousePos(current_mouse),
    mLastMousePos(last_mouse) {}

const glm::vec2& InputEvent::CurrentMousePosition() const
{ return mCurMousePos; }

const glm::vec2& InputEvent::LastMousePosition() const
{ return mLastMousePos; }

glm::vec2 InputEvent::MouseMotion() const
{
    if(!IsMouseMotion())
        { print_warning("InputEvent::MouseMotion called on an event that isn't mouse motion"); }
    return mCurMousePos - mLastMousePos;
}

const std::set<std::string>& InputEvent::Actions() const
{ return g_pInputManager->GetActions(mBindingID); }

const InputBinding& InputEvent::Binding() const
{ return g_pInputManager->GetBinding(mBindingID); }

InputStatus InputEvent::Status() const
{ return Binding().status(); }

bool InputEvent::JustPressed() const
{ return Binding().JustPressed(); }

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
    if(mBindingID > BindingIDs::end)
        { return InputEventType::Invalid; }
    else if(mBindingID < BindingIDs::MouseButtonsEnd)
        { return InputEventType::Key; }
    else if(mBindingID < BindingIDs::MouseMotionFront)
        { return InputEventType::MouseButton; }
    return InputEventType::MouseMotion;
}

bool InputEvent::IsMouseMotion() const
{
    return (mBindingID >= BindingIDs::MouseMotionFront &&
        mBindingID < BindingIDs::MouseMotionEnd);
}

bool InputEvent::IsType(InputEventType EventType) const
{ return (Type() == EventType); }

bool InputEvent::IsAction(const std::string& Action) const
{
    if(!Actions().contains(Action))
        { return false; }
    else if(Action.at(0) == '-')
        { return Released(); }
    return Pressed();
}

bool InputEvent::IsInput(const std::string& BindingName) const
{ return (Binding() == BindingName); }

bool InputEvent::IsInput(ID BindingID) const
{ return (mBindingID == BindingID); }

std::string InputEvent::Log() const
{
    return std::format(
        "InputEvent Log - Creation Time: {:0.3f}, Tick: {} Event Type: {:11}, Input ID: {}, InputName: '{}', Actions: {}",
        mTime,
        mTick,
        gInputEventType[static_cast<size_t>(Type())],
        mBindingID,
        Binding().name(),
        (Actions().empty()) ? std::set<std::string>{"none"} : Actions());
}

std::string InputEvent::DemoString() const
{
    std::string output{
        std::format("<{}:{}:{}>",
            mBindingID,
            static_cast<int>(Binding().status()),
            static_cast<int>(Binding().just_changed())
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
{ return g_pInputManager->BindingExists(mBindingID); }

#include "event.hpp"
#include "bindings.hpp"

///////////////////////////
// InputEventMouseMotion //
///////////////////////////
constexpr InputEventMouseMotion::InputEventMouseMotion() = default;
constexpr InputEventMouseMotion::InputEventMouseMotion(FARG(Position) inCurrentPos, FARG(Position) inLastPos):
    mMousePosition{inCurrentPos},
    mLastMousePosition{inLastPos} {}

constexpr bool InputEventMouseMotion::IsMouseMotion() const
{ return true; }

FARG(Position) InputEventMouseMotion::MousePosition() const
{ return mMousePosition; }

FARG(Position) InputEventMouseMotion::LastMousePosition() const
{ return mLastMousePosition; }

FARG(Motion) InputEventMouseMotion::MouseMotion() const
{ return mMouseMotion; }

//////////////////////
// InputEventAction //
//////////////////////
constexpr InputEventAction::InputEventAction(FARG(std::string) inAction, bool isActive, bool isJustChanged):
    mAction{inAction},
    mActive{isActive},
    mJustChanged{isJustChanged} {}

constexpr bool InputEventAction::IsAction(FARG(std::string) inAction) const
{ return !mAction.compare(inAction); }

constexpr bool InputEventAction::IsActive(FARG(std::string) inAction) const
{ return mActive && IsAction(inAction); }

constexpr bool InputEventAction::IsJustChanged(FARG(std::string) inAction) const
{ return mJustChanged && IsAction(inAction); }

///////////////////////
// InputEventBinding //
///////////////////////
constexpr InputEventBinding::InputEventBinding(KeyArg inBindingID, FARG(Key::Modifiers) inModifiers, bool isPressed, bool isRepeated, bool isJustChanged):
    mID{inBindingID},
    mModifiers{inModifiers},
    mPressed{isPressed},
    mRepeated{isRepeated},
    mJustChanged{isJustChanged} {}

constexpr const Key::Modifiers& InputEventBinding::GetModifiers() const
{ return mModifiers; }

constexpr bool InputEventBinding::IsBinding(KeyArg inID) const
{ return mID == inID; }

constexpr bool InputEventBinding::IsPressed(KeyArg inID) const
{ return mPressed && IsBinding(inID); }

constexpr bool InputEventBinding::IsRepeated(KeyArg inID) const
{ return mRepeated && IsBinding(inID); }

constexpr bool InputEventBinding::IsReleased(KeyArg inID) const
{ return !mPressed && IsBinding(inID); }

constexpr bool InputEventBinding::IsJustPressed(KeyArg inID) const
{ return mJustChanged && IsPressed(inID); }

constexpr bool InputEventBinding::IsJustReleased(KeyArg inID) const
{ return mJustChanged && IsReleased(inID); }


#define DO_NOT_COMPILE
#ifndef DO_NOT_COMPILE

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

#endif // DO_NOT_COMPILE

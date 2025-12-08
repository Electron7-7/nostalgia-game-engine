#include "event.hpp"
#include "action.hpp"
#include "bindings.hpp"

///////////////////////////
// InputEventMouseMotion //
///////////////////////////
InputEventMouseMotion::InputEventMouseMotion() = default;
InputEventMouseMotion::InputEventMouseMotion(FARG(Position2D) inCurrentPos, FARG(Position2D) inLastPos):
    mMousePosition{inCurrentPos},
    mLastMousePosition{inLastPos} {}

size_t InputEventMouseMotion::GetHash() const
{ return ConstexprHash("InputEventMouseMotion"); }

bool InputEventMouseMotion::IsMouseMotion() const
{ return true; }

FARG(Position2D) InputEventMouseMotion::MousePosition() const
{ return mMousePosition; }

FARG(Position2D) InputEventMouseMotion::LastMousePosition() const
{ return mLastMousePosition; }

FARG(Motion2D) InputEventMouseMotion::MouseMotion() const
{ return mMouseMotion; }

//////////////////////
// InputEventAction //
//////////////////////
InputEventAction::InputEventAction(FARG(InputAction) inAction):
    mAction{inAction.Name()},
    mActive{inAction.State()},
    mJustChanged{inAction.StateJustChanged()} {}

size_t InputEventAction::GetHash() const
{ return ConstexprHash(mAction); }

bool InputEventAction::IsAction(FARG(std::string) inAction) const
{ return !mAction.compare(inAction); }

bool InputEventAction::IsActive(FARG(std::string) inAction) const
{ return mActive && IsAction(inAction); }

bool InputEventAction::IsJustChanged(FARG(std::string) inAction) const
{ return mJustChanged && IsAction(inAction); }

///////////////////////
// InputEventBinding //
///////////////////////
InputEventBinding::InputEventBinding(uint inBindingID, Key::Modifiers inModifiers, bool isPressed, bool isRepeated, bool isJustChanged):
    mID{inBindingID},
    mModifiers{inModifiers},
    mPressed{isPressed},
    mRepeated{isRepeated},
    mJustChanged{isJustChanged} {}

size_t InputEventBinding::GetHash() const
{ return static_cast<size_t>(mID()); }

bool InputEventBinding::IsModifierActive(Key::Modifier inMod) const
{ return mModifiers.has(inMod); }

Key::Modifiers InputEventBinding::GetModifiers() const
{ return mModifiers; }

bool InputEventBinding::IsBinding(KeyArg inID) const
{ return mID == inID; }

bool InputEventBinding::IsPressed(KeyArg inID) const
{ return mPressed && IsBinding(inID); }

bool InputEventBinding::IsRepeated(KeyArg inID) const
{ return mRepeated && IsBinding(inID); }

bool InputEventBinding::IsReleased(KeyArg inID) const
{ return !mPressed && IsBinding(inID); }

bool InputEventBinding::IsJustPressed(KeyArg inID) const
{ return mJustChanged && IsPressed(inID); }

bool InputEventBinding::IsJustReleased(KeyArg inID) const
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

#include "event.hpp"
#include "action.hpp"
#include "core/printing.hpp"

////////////////
// InputEvent //
////////////////
size_t           InputEvent::GetHash()                        const { return ID::Invalid;             }
std::string      InputEvent::GetDebugLog()                    const { return "InputEvent Base Class"; }
bool             InputEvent::IsMouseMotion()                  const { return false;                   }
bool             InputEvent::IsStoppedMouseMotion()           const { return false;                   }
Farg<Position2D> InputEvent::MousePosition()                  const { return empty_position;          }
Farg<Position2D> InputEvent::LastMousePosition()              const { return empty_position;          }
Farg<Motion2D>   InputEvent::MouseMotion()                    const { return empty_motion;            }
bool             InputEvent::IsInputAction()                  const { return false;                   }
bool             InputEvent::IsAction(Farg<std::string>)      const { return false;                   }
bool             InputEvent::IsActive(Farg<std::string>)      const { return false;                   }
bool             InputEvent::IsJustChanged(Farg<std::string>) const { return false;                   }
bool             InputEvent::IsInputBinding()                 const { return false;                   }
bool             InputEvent::IsBinding(KeyID)                 const { return false;                   }
bool             InputEvent::IsRepeated(KeyID)                const { return false;                   }
bool             InputEvent::IsPressed(KeyID)                 const { return false;                   }
bool             InputEvent::IsReleased(KeyID)                const { return false;                   }
bool             InputEvent::IsJustPressed(KeyID)             const { return false;                   }
bool             InputEvent::IsJustReleased(KeyID)            const { return false;                   }
bool             InputEvent::IsModifierActive(Key::Modifier)  const { return false;                   }
Key::Modifiers   InputEvent::GetModifiers()                   const { return Key::Modifiers{};        }

void InputEvent::sPrintMouseWarning(const char* inFunction)
{
    print_warningv(VERBOSE0,
        "\x1b[1m`InputEvent::{}`:\x1b[22m this \x1b[1mInputEvent\x1b[22m is \x1b[1mNOT\x1b[22m mouse motion!",
        inFunction);
}

Position2D InputEvent::empty_position{};
Motion2D   InputEvent::empty_motion{};

///////////////////////////
// InputEventMouseMotion //
///////////////////////////
InputEventMouseMotion::InputEventMouseMotion() = default;
InputEventMouseMotion::InputEventMouseMotion(Farg<Position2D> inCurrentPos, Farg<Position2D> inLastPos):
    mMousePosition{inCurrentPos},
    mLastMousePosition{inLastPos},
    mMouseMotion{mMousePosition - mLastMousePosition} {}

size_t InputEventMouseMotion::GetHash() const
{ return ConstexprHash("InputEventMouseMotion"); }

bool InputEventMouseMotion::IsMouseMotion() const
{ return true; }

bool InputEventMouseMotion::IsStoppedMouseMotion() const
{ return mMouseMotion.is_zero_approx(); }

Farg<Position2D> InputEventMouseMotion::MousePosition() const
{ return mMousePosition; }

Farg<Position2D> InputEventMouseMotion::LastMousePosition() const
{ return mLastMousePosition; }

Farg<Motion2D> InputEventMouseMotion::MouseMotion() const
{ return mMouseMotion; }

//////////////////////
// InputEventAction //
//////////////////////
InputEventAction::InputEventAction(Farg<InputAction> inAction):
    mAction{inAction.Name()},
    mActive{inAction.State()},
    mJustChanged{inAction.StateJustChanged()} {}

size_t InputEventAction::GetHash() const
{ return ConstexprHash(mAction); }

bool InputEventAction::IsAction(Farg<std::string> inAction) const
{ return !mAction.compare(inAction); }

bool InputEventAction::IsActive(Farg<std::string> inAction) const
{ return mActive && IsAction(inAction); }

bool InputEventAction::IsJustChanged(Farg<std::string> inAction) const
{ return mJustChanged && IsAction(inAction); }

///////////////////////
// InputEventBinding //
///////////////////////
InputEventBinding::InputEventBinding(KeyID inBindingID,
    Key::Modifiers inModifiers,
    bool isPressed,
    bool isRepeated,
    bool isJustChanged):
        mID{inBindingID},
        mModifiers{inModifiers},
        mPressed{isPressed},
        mRepeated{isRepeated},
        mJustChanged{isJustChanged} {}

size_t InputEventBinding::GetHash() const
{ return static_cast<size_t>(mID[]); }

bool InputEventBinding::IsModifierActive(Key::Modifier inMod) const
{ return mModifiers.has(inMod); }

Key::Modifiers InputEventBinding::GetModifiers() const
{ return mModifiers; }

bool InputEventBinding::IsBinding(KeyID inID) const
{ return mID == inID; }

bool InputEventBinding::IsPressed(KeyID inID) const
{ return mPressed && IsBinding(inID); }

bool InputEventBinding::IsRepeated(KeyID inID) const
{ return mRepeated && IsBinding(inID); }

bool InputEventBinding::IsReleased(KeyID inID) const
{ return !mPressed && IsBinding(inID); }

bool InputEventBinding::IsJustPressed(KeyID inID) const
{ return mJustChanged && IsPressed(inID); }

bool InputEventBinding::IsJustReleased(KeyID inID) const
{ return mJustChanged && IsReleased(inID); }

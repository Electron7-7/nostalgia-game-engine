#ifndef INPUT_EVENT_H
#define INPUT_EVENT_H

#include "fwd.hpp"

#include "bindings.hpp"
#include "core/printing.hpp"
#include "core/type_helpers.hpp"
#include "math/containers.hpp"

struct MouseMotionData
{
    Position mouse_current{};
    Position mouse_last{};
    Motion   mouse_motion{};
};

class InputEvent
{
public:
    virtual ~InputEvent() = default;

    // InputEventMouseMotion
    virtual constexpr bool IsMouseMotion()                   const { return false; }
    virtual FARG(Position) MousePosition()                   const { sPrintMouseWarning("MousePosition");     return sDefaultMouseData.mouse_current; }
    virtual FARG(Position) LastMousePosition()               const { sPrintMouseWarning("LastMousePosition"); return sDefaultMouseData.mouse_last;    }
    virtual FARG(Motion)   MouseMotion()                     const { sPrintMouseWarning("MouseMotion");       return sDefaultMouseData.mouse_motion;  }
    // InputEventAction
    virtual constexpr bool IsAction(FARG(std::string))       const { return false; }
    virtual constexpr bool IsActive(FARG(std::string))       const { return false; }
    virtual constexpr bool IsJustChanged(FARG(std::string))  const { return false; }
    // InputEventBinding
    virtual constexpr bool IsBinding(KeyArg)                 const { return false; }
    virtual constexpr bool IsRepeated(KeyArg)                const { return false; }
    virtual constexpr bool IsPressed(KeyArg)                 const { return false; }
    virtual constexpr bool IsReleased(KeyArg)                const { return false; }
    virtual constexpr bool IsJustPressed(KeyArg)             const { return false; }
    virtual constexpr bool IsJustReleased(KeyArg)            const { return false; }
    virtual constexpr const Key::Modifiers& GetModifiers()   const { return EmptyModifiers; }

protected:
    inline static MouseMotionData sDefaultMouseData{};
    inline static void sPrintMouseWarning(const char* inFunction)
    { print_warningv(VERBOSE0, "\x1b[1m`InputEvent::{}`:\x1b[22m this \x1b[1mInputEvent\x1b[22m is \x1b[1mNOT\x1b[22m mouse motion!", inFunction); }

private:
    inline static const Key::Modifiers EmptyModifiers{};
};

class InputEventMouseMotion : InputEvent
{
public:
    constexpr InputEventMouseMotion();
    constexpr InputEventMouseMotion(FARG(Position) inCurrentPos, FARG(Position) inLastPos);

    constexpr bool IsMouseMotion() const final;
    FARG(Position) MousePosition() const final;
    FARG(Position) LastMousePosition() const final;
    FARG(Motion) MouseMotion() const final;

private:
    Position mMousePosition{};
    Position mLastMousePosition{};
    Motion   mMouseMotion{mMousePosition - mLastMousePosition};
};

class InputEventAction : public InputEvent
{
public:
    constexpr InputEventAction(FARG(std::string), bool isActive, bool isJustChanged = false);

    constexpr bool IsAction(FARG(std::string)) const final;
    constexpr bool IsActive(FARG(std::string)) const final;
    constexpr bool IsJustChanged(FARG(std::string)) const final;

private:
    std::string mAction{""};
    bool mActive{true};
    bool mJustChanged{false};
};

class InputEventBinding : public InputEvent
{
public:
    constexpr InputEventBinding(KeyArg inBindingID, FARG(Key::Modifiers) inModifiers, bool isPressed, bool isRepeated = false, bool isJustChanged = false);

    constexpr bool IsBinding(KeyArg)      const final;
    constexpr bool IsPressed(KeyArg)      const final;
    constexpr bool IsRepeated(KeyArg)     const final;
    constexpr bool IsReleased(KeyArg)     const final;
    constexpr bool IsJustPressed(KeyArg)  const final;
    constexpr bool IsJustReleased(KeyArg) const final;

    constexpr const Key::Modifiers& GetModifiers() const final;

private:
    friend class InputEventQueue;
    KeyID mID{};
    Key::Modifiers mModifiers{};
    bool mPressed{true};
    bool mRepeated{false};
    bool mJustChanged{false};
};

#endif // INPUT_EVENT_H

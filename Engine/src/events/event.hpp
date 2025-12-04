#ifndef INPUT_EVENT_H
#define INPUT_EVENT_H

#include "fwd.hpp"

#include "bindings.hpp"
#include "core/printing.hpp"
#include "core/type_helpers.hpp"
#include "math/containers.hpp"

class IEvent
{
public:
    virtual ~IEvent() noexcept = default;
};

namespace EventNames
{
    inline constinit const char* WindowClose{"window_close"};
}

class AppEvent : public IEvent
{
public:
    constexpr AppEvent(FARG(std::string) inName): mName{inName} {}

protected:
    std::string mName{"Untitled AppEvent"};
};

class InputEvent : public IEvent
{
public:
    // All
    virtual size_t GetHash() const { return 0; }
    virtual std::string GetDebugLog() const { return "InputEvent Base Class"; }

    // InputEventMouseMotion
    virtual bool IsMouseMotion()                  const { return false; }
    virtual FARG(Position2D) MousePosition()      const { sPrintMouseWarning("MousePosition");     return empty_position; }
    virtual FARG(Position2D) LastMousePosition()  const { sPrintMouseWarning("LastMousePosition"); return empty_position; }
    virtual FARG(Motion2D)   MouseMotion()        const { sPrintMouseWarning("MouseMotion");       return empty_motion;   }
    // InputEventAction
    virtual bool IsAction(FARG(std::string))      const { return false; }
    virtual bool IsActive(FARG(std::string))      const { return false; }
    virtual bool IsJustChanged(FARG(std::string)) const { return false; }
    // InputEventBinding
    virtual bool IsBinding(KeyArg)                const { return false; }
    virtual bool IsRepeated(KeyArg)               const { return false; }
    virtual bool IsPressed(KeyArg)                const { return false; }
    virtual bool IsReleased(KeyArg)               const { return false; }
    virtual bool IsJustPressed(KeyArg)            const { return false; }
    virtual bool IsJustReleased(KeyArg)           const { return false; }
    virtual bool IsModifierActive(Key::Modifier)  const { return false; }
    virtual const Key::Modifiers& GetModifiers()  const { return Key::Modifiers::empty; }

protected:
    inline static void sPrintMouseWarning(const char* inFunction)
    { print_warningv(VERBOSE0, "\x1b[1m`InputEvent::{}`:\x1b[22m this \x1b[1mInputEvent\x1b[22m is \x1b[1mNOT\x1b[22m mouse motion!", inFunction); }

    inline static Position2D empty_position{};
    inline static Motion2D   empty_motion{};
};

class InputEventMouseMotion final : InputEvent
{
public:
    InputEventMouseMotion();
    InputEventMouseMotion(FARG(Position2D) inCurrentPos, FARG(Position2D) inLastPos);

    size_t GetHash() const final;
    std::string GetDebugLog() const final { return std::format("InputEventMouseMotion - mouse pos: [{}, {}], last pos: [{}, {}], motion: [{}, {}]", mMousePosition.x(), mMousePosition.y(), mLastMousePosition.x(), mLastMousePosition.y(), mMouseMotion.x(), mMouseMotion.y()); }
    bool IsMouseMotion() const final;
    FARG(Position2D) MousePosition() const final;
    FARG(Position2D) LastMousePosition() const final;
    FARG(Motion2D) MouseMotion() const final;

private:
    Position2D mMousePosition{};
    Position2D mLastMousePosition{};
    Motion2D   mMouseMotion{};
};

class InputEventAction final : public InputEvent
{
public:
    InputEventAction(FARG(InputAction) inAction);

    size_t GetHash() const final;
    std::string GetDebugLog() const final { return std::format("InputEventAction - action: {}, active: {}, changed: {}", mAction, mActive, mJustChanged); }
    bool IsAction(FARG(std::string)) const final;
    bool IsActive(FARG(std::string)) const final;
    bool IsJustChanged(FARG(std::string)) const final;

private:
    friend class InputEventQueue;
    std::string mAction{""};
    bool mActive{true};
    bool mJustChanged{false};
};

class InputEventBinding final : public InputEvent
{
public:
    InputEventBinding(KeyArg inBindingID, FARG(Key::Modifiers) inModifiers, bool isPressed, bool isRepeated = false, bool isJustChanged = false);

    size_t GetHash()            const final;
    std::string GetDebugLog()   const final { return std::format("InputEventBinding - binding: {}, pressed: {}, changed: {}", mID(), mPressed, mJustChanged); }
    bool IsBinding(KeyArg)      const final;
    bool IsPressed(KeyArg)      const final;
    bool IsRepeated(KeyArg)     const final;
    bool IsReleased(KeyArg)     const final;
    bool IsJustPressed(KeyArg)  const final;
    bool IsJustReleased(KeyArg) const final;

    bool IsModifierActive(Key::Modifier) const final;
    const Key::Modifiers& GetModifiers() const final;

private:
    friend class InputEventQueue;
    KeyID mID{};
    Key::Modifiers mModifiers{};
    bool mPressed{true};
    bool mRepeated{false};
    bool mJustChanged{false};
};

#endif // INPUT_EVENT_H

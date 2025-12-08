#ifndef INPUT_EVENT_H
#define INPUT_EVENT_H

#include "fwd.hpp"

#include "bindings.hpp"
#include "core/printing.hpp"
#include "core/type_helpers.hpp"
#include "math/containers.hpp"

/**
 * `Highest, Lowest`: What they say on the tins.
 *
 * `P0, P1, P2, ...`: The higher the number, the lower the priority (`P0` is one step below `Highest`).
**/
enum class EventPriority : unsigned int
{
    Highest = 0,
    P0, P1, P2,
    Lowest
};

enum class EventType : unsigned int
{
    InputEvent,
    AppEvent,
    EngineEvent,
};

#ifdef DEBUGGING
constexpr const char* gDebugEventTypeStr(EventType inType) noexcept
{
    switch(inType)
    {
    case EventType::InputEvent:
        return "InputEvent";
    case EventType::AppEvent:
        return "AppEvent";
    case EventType::EngineEvent:
        return "EngineEvent";
    }
}
#else
constexpr const char* gDebugEventTypeStr(EventType) noexcept { return ""; }
#endif // DEBUGGING

#define EVENT_TYPE(TYPE) constexpr EventType Type() const noexcept final { return TYPE; }
#define EVENT_LOG  constexpr std::string DebugLog() const noexcept

class IEvent
{
public:
    virtual ~IEvent() noexcept = default;

    static constexpr EventPriority Priority() noexcept { return EventPriority::Lowest; }
    virtual constexpr EventType Type() const noexcept = 0;
    virtual constexpr std::string DebugLog() const noexcept { return "[NO LOG]"; }
};

template<EventPriority _Priority = EventPriority::Lowest>
    class CEvent : public IEvent
    {
    public:
        static constexpr EventPriority Priority() noexcept { return _Priority; }
    };

class AppEvent : public CEvent<EventPriority::Highest>
{
public:
    constexpr AppEvent(FARG(std::string) inName): mName{inName} {}
    EVENT_TYPE(EventType::AppEvent)
    EVENT_LOG final { return "AppEvent: " + mName; }

    constexpr FARG(std::string) Name() const noexcept
    { return mName; }

    constexpr bool IsEvent(FARG(std::string) inEventName) const noexcept
    { return !mName.compare(inEventName); }

protected:
    std::string mName{"Untitled AppEvent"};
};

class EngineEvent : public CEvent<EventPriority::P1>
{
public:
    EVENT_TYPE(EventType::EngineEvent)
};

class InputEvent : public CEvent<EventPriority::P0>
{
public:
    EVENT_TYPE(EventType::InputEvent)
    EVENT_LOG override { return GetDebugLog(); }

    // All
    virtual size_t GetHash() const { return 0; }
    virtual std::string GetDebugLog() const { return "InputEvent Base Class"; }

    // InputEventMouseMotion
    virtual bool IsMouseMotion()                  const { return false; }
    virtual FARG(Position2D) MousePosition()      const { return empty_position; }
    virtual FARG(Position2D) LastMousePosition()  const { return empty_position; }
    virtual FARG(Motion2D)   MouseMotion()        const { return empty_motion;   }
    // InputEventAction
    virtual bool IsInputAction()                  const { return false; }
    virtual bool IsAction(FARG(std::string))      const { return false; }
    virtual bool IsActive(FARG(std::string))      const { return false; }
    virtual bool IsJustChanged(FARG(std::string)) const { return false; }
    // InputEventBinding
    virtual bool IsInputBinding()                 const { return false; }
    virtual bool IsBinding(KeyArg)                const { return false; }
    virtual bool IsRepeated(KeyArg)               const { return false; }
    virtual bool IsPressed(KeyArg)                const { return false; }
    virtual bool IsReleased(KeyArg)               const { return false; }
    virtual bool IsJustPressed(KeyArg)            const { return false; }
    virtual bool IsJustReleased(KeyArg)           const { return false; }
    virtual bool IsModifierActive(Key::Modifier)  const { return false; }
    virtual Key::Modifiers GetModifiers()         const { return Key::Modifiers{}; }

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
    bool IsInputAction() const final { return true; }
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
    InputEventBinding(uint inBindingID, Key::Modifiers inModifiers, bool isPressed, bool isRepeated = false, bool isJustChanged = false);

    size_t GetHash()            const final;
    std::string GetDebugLog()   const final { return std::format("InputEventBinding - binding: {}, pressed: {}, changed: {}", mID(), mPressed, mJustChanged); }
    bool IsInputBinding()       const final { return true; }
    bool IsBinding(KeyArg)      const final;
    bool IsPressed(KeyArg)      const final;
    bool IsRepeated(KeyArg)     const final;
    bool IsReleased(KeyArg)     const final;
    bool IsJustPressed(KeyArg)  const final;
    bool IsJustReleased(KeyArg) const final;

    bool IsModifierActive(Key::Modifier) const final;
    virtual Key::Modifiers GetModifiers() const final;

private:
    friend class InputEventQueue;
    KeyID mID{};
    Key::Modifiers mModifiers{};
    bool mPressed{true};
    bool mRepeated{false};
    bool mJustChanged{false};
};

#define APP_EVENT(NAME) \
    inline constinit const char* NAME{#NAME};

namespace AppEvents
{
    APP_EVENT(WindowClose)
    APP_EVENT(WindowResize)
}

template<typename T>
    concept is_event = std::derived_from<T,IEvent> && !(std::is_same_v<T,IEvent>);

#undef APP_EVENT
#undef EVENT_TYPE
#undef EVENT_LOG
#endif // INPUT_EVENT_H

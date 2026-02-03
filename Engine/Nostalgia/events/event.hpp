#ifndef INPUT_EVENT_H
#define INPUT_EVENT_H

#include <Nostalgia/fwd/events.hpp>
#include <Nostalgia/events/bindings.hpp>

#define APP_EVENT(NAME) inline static constinit const std::string NAME{#NAME};
#define EVENT_TYPE(TYPE) constexpr EventType Type() const noexcept final { return TYPE; }
#define EVENT_LOG constexpr std::string DebugLog() const noexcept

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
    APP_EVENT(WindowClose)
    APP_EVENT(WindowResize)
    APP_EVENT(WindowMoved)

    constexpr AppEvent(Sarg inName): mName{inName} {}

    EVENT_TYPE(EventType::AppEvent)
    EVENT_LOG final { return "AppEvent: " + mName; }

    constexpr Farg<std::string> Name() const noexcept
    { return mName; }

    constexpr bool IsEvent(Farg<std::string> inEventName) const noexcept
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
    virtual size_t GetHash() const;
    virtual std::string GetDebugLog() const;

    // InputEventMouseMotion
    virtual bool IsMouseMotion() const;
    virtual bool IsStoppedMouseMotion() const;
    virtual Farg<Position2D> MousePosition() const;
    virtual Farg<Position2D> LastMousePosition() const;
    virtual Farg<Motion2D>   MouseMotion() const;

    // InputEventAction
    virtual bool IsInputAction() const;
    virtual bool IsAction(Farg<std::string>) const;
    virtual bool IsActive(Farg<std::string>) const;
    virtual bool IsJustChanged(Farg<std::string>) const;

    // InputEventBinding
    virtual bool IsInputBinding() const;
    virtual bool IsBinding(KeyID) const;
    virtual bool IsRepeated(KeyID) const;
    virtual bool IsPressed(KeyID) const;
    virtual bool IsReleased(KeyID) const;
    virtual bool IsJustPressed(KeyID) const;
    virtual bool IsJustReleased(KeyID) const;
    virtual bool IsModifierActive(Key::Modifier) const;
    virtual Key::Modifiers GetModifiers() const;

protected:
    static void sPrintMouseWarning(const char* inFunction);
    static Position2D empty_position;
    static Motion2D   empty_motion;
};

class InputEventMouseMotion final : public InputEvent
{
public:
    InputEventMouseMotion();
    InputEventMouseMotion(Farg<Position2D> inCurrentPos, Farg<Position2D> inLastPos);

    size_t GetHash() const final;
    std::string GetDebugLog() const final { return std::format("InputEventMouseMotion - mouse pos: [{}, {}], last pos: [{}, {}], motion: [{}, {}]", mMousePosition.x(), mMousePosition.y(), mLastMousePosition.x(), mLastMousePosition.y(), mMouseMotion.x(), mMouseMotion.y()); }
    bool IsMouseMotion() const final;
    bool IsStoppedMouseMotion() const final;
    Farg<Position2D> MousePosition() const final;
    Farg<Position2D> LastMousePosition() const final;
    Farg<Motion2D> MouseMotion() const final;

private:
    Position2D mMousePosition{};
    Position2D mLastMousePosition{};
    Motion2D   mMouseMotion{};
};

class InputEventAction final : public InputEvent
{
public:
    InputEventAction(Farg<InputAction> inAction);

    size_t GetHash() const final;
    std::string GetDebugLog() const final { return std::format("InputEventAction - action: {}, active: {}, changed: {}", mAction, mActive, mJustChanged); }
    bool IsInputAction() const final { return true; }
    bool IsAction(Farg<std::string>) const final;
    bool IsActive(Farg<std::string>) const final;
    bool IsJustChanged(Farg<std::string>) const final;

private:
    std::string mAction{""};
    bool mActive{true};
    bool mJustChanged{false};
};

class InputEventBinding final : public InputEvent
{
public:
    InputEventBinding(KeyID inBindingID, Key::Modifiers inModifiers, bool isPressed, bool isRepeated = false, bool isJustChanged = false);

    size_t GetHash()                 const final;
    std::string GetDebugLog()        const final { return std::format("InputEventBinding - key: {}, pressed: {}, changed: {}", debug_GetKeyName(mID), mPressed, mJustChanged); }
    bool IsInputBinding()            const final { return true; }
    bool IsBinding(KeyID)      const final;
    bool IsPressed(KeyID)      const final;
    bool IsRepeated(KeyID)     const final;
    bool IsReleased(KeyID)     const final;
    bool IsJustPressed(KeyID)  const final;
    bool IsJustReleased(KeyID) const final;

    bool IsModifierActive(Key::Modifier) const final;
    virtual Key::Modifiers GetModifiers() const final;

private:
    KeyID mID{};
    Key::Modifiers mModifiers{};
    bool mPressed{true};
    bool mRepeated{false};
    bool mJustChanged{false};
};

template<typename T>
    concept is_event = std::derived_from<T,IEvent> && !(std::is_same_v<T,IEvent>);

#undef APP_EVENT
#undef EVENT_TYPE
#undef EVENT_LOG
#endif // INPUT_EVENT_H

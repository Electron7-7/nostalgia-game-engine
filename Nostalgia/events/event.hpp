#ifndef INPUT_EVENT_H
#define INPUT_EVENT_H

#define NEW_EVENT(NAME) inline static constinit const char* NAME{#NAME};
#define EVENT_TYPE(TYPE) constexpr EventType Type() const noexcept final { return TYPE; }
#define EVENT_LOG std::string DebugLog() const noexcept

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
    WindowEvent,
};

class IEvent
{
public:
    virtual ~IEvent() noexcept = default;

    static constexpr EventPriority Priority() noexcept { return EventPriority::Lowest; }
    virtual constexpr EventType Type() const noexcept = 0;
    virtual constexpr bool IsEvent(Sarg inEventName) const noexcept { return false; }
    virtual std::string DebugLog() const noexcept { return "[NO LOG]"; }
};

template<EventPriority _Priority = EventPriority::Lowest>
    class CEvent : public IEvent
    {
    public:
        static constexpr EventPriority Priority() noexcept { return _Priority; }
    };

class WindowEvent : public CEvent<EventPriority::Highest>
{
public:
    NEW_EVENT(WindowClose)
    NEW_EVENT(WindowResize)
    NEW_EVENT(WindowMove)

    EVENT_TYPE(EventType::WindowEvent)
    EVENT_LOG final { return (mIsMainWindow) ? "(MainWindow) <WindowEvent> " : "<WindowEvent> " + mName; }

    constexpr WindowEvent(Sarg inName, bool inIsMainWindow = false):
        mName{inName}, mIsMainWindow{inIsMainWindow} {}

    constexpr bool IsMainWindow() const noexcept
    { return mIsMainWindow; }

    constexpr bool IsEvent(Sarg inEventName) const noexcept final
    { return mName == inEventName; }

    constexpr Sarg Name() const noexcept
    { return mName; }

protected:
    std::string mName{};
    bool mIsMainWindow{false};
};

class InputEvent : public CEvent<EventPriority::P0>
{
public:
    EVENT_TYPE(EventType::InputEvent)
    EVENT_LOG override { return GetDebugLog(); }

    // All
    virtual size_t GetHash() const;
    virtual std::string GetDebugLog() const;

    // InputEventMouseScroll
    virtual bool IsMouseScroll() const;
    virtual Farg<Position2D<double>> ScrollOffset() const;

    // InputEventMouseMotion
    virtual bool IsMouseMotion() const;
    virtual bool IsStoppedMouseMotion() const;
    virtual Farg<Position2D<double>> MousePosition() const;
    virtual Farg<Position2D<double>> LastMousePosition() const;
    virtual Farg<Motion2D<double>>   MouseMotion() const;

    // InputEventAction
    virtual bool IsInputAction() const;
    virtual bool IsAction(Sarg) const;
    virtual bool IsActive(Sarg) const;
    virtual bool IsJustChanged(Sarg) const;

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
    static Position2D<double> empty_position;
    static Motion2D<double>   empty_motion;
};

class InputEventMouseScroll final : public InputEvent
{
public:
    InputEventMouseScroll();
    InputEventMouseScroll(Farg<Position2D<double>> inScrollOffset);

    std::string GetDebugLog() const final
    { return std::format("InputEventMouseScroll - scroll offset: [{}, {}]", mScrollOffset.x(), mScrollOffset.y()); }

    Farg<Position2D<double>> ScrollOffset() const final;

private:
    Position2D<double> mScrollOffset{};
};


class InputEventMouseMotion final : public InputEvent
{
public:
    InputEventMouseMotion();
    InputEventMouseMotion(Farg<Position2D<double>> inCurrentPos, Farg<Position2D<double>> inLastPos);

    std::string GetDebugLog() const final
    { return std::format("InputEventMouseMotion - mouse pos: [{}, {}], last pos: [{}, {}], motion: [{}, {}]", mMousePosition.x(), mMousePosition.y(), mLastMousePosition.x(), mLastMousePosition.y(), mMouseMotion.x(), mMouseMotion.y()); }

    size_t GetHash() const final;
    bool IsMouseMotion() const final;
    bool IsStoppedMouseMotion() const final;
    Farg<Position2D<double>> MousePosition() const final;
    Farg<Position2D<double>> LastMousePosition() const final;
    Farg<Motion2D<double>> MouseMotion() const final;

private:
    Position2D<double> mMousePosition{};
    Position2D<double> mLastMousePosition{};
    Motion2D<double>   mMouseMotion{};
};

class InputEventAction final : public InputEvent
{
public:
    InputEventAction(Farg<InputAction> inAction);

    std::string GetDebugLog() const final
    { return std::format("InputEventAction - action: {}, active: {}, changed: {}", mAction, mActive, mJustChanged); }

    size_t GetHash() const final;
    bool IsInputAction() const final { return true; }
    bool IsAction(Sarg) const final;
    bool IsActive(Sarg) const final;
    bool IsJustChanged(Sarg) const final;

private:
    std::string mAction{""};
    bool mActive{true};
    bool mJustChanged{false};
};

class InputEventBinding final : public InputEvent
{
public:
    InputEventBinding(KeyID inBindingID, Key::Modifiers inModifiers, bool isPressed, bool isRepeated = false, bool isJustChanged = false);

    std::string GetDebugLog() const final
    {
        return std::format("InputEventBinding - key: {}, pressed: {}, changed: {}",
            debug_GetKeyName(mID), mPressed, mJustChanged);
    }

    bool IsInputBinding()      const final { return true; }
    size_t GetHash()           const final;
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

#undef NEW_EVENT
#undef EVENT_TYPE
#undef EVENT_LOG
#endif // INPUT_EVENT_H

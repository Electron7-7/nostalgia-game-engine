#ifndef EVENT_H
#define EVENT_H

#include "input/fwd.hpp"

#include "event_components.hpp"
#include "core/type_helpers.hpp"

#include <glm/vec2.hpp>

enum class EventType : ushort {
    None = 0,
    Key,
    MouseButton,
    MouseMotion,
    Action,
    // Gamepad, // unimplemented
};

class IEvent
{
public:
    virtual ~IEvent() = default;

    IEvent(FARG(std::string) inName): mID{inName} {}

    FARG(std::string) GetName() const { return mID.name(); }
    FARG(ID)          GetID()   const { return mID;        }
    bool IsHandled()            const { return mHandled;   }
    void SetHandled()                 { mHandled = true;   }

protected:
    ID mID{ID::Invalid};
    bool mHandled{false};
};

template<EventType mType = EventType::None>
    class Event : public IEvent
    {
    public:
        typedef EventType Type;

        virtual ~Event() = default;

        Event(FARG(std::string) inName): IEvent{inName} {}

        constexpr Type GetType()           const { return mType;           }
        constexpr bool IsType(Type inType) const { return mType == inType; }
    };

#define EVENT_DEFAULT_CONSTRUCTOR(NAME, TYPE, ARGS...) \
    NAME(ARGS): Event<EventType::TYPE>{#NAME}

class KeyEvent final : public Event<EventType::Key>, public CUsesKey
{
public:
    EVENT_DEFAULT_CONSTRUCTOR(KeyEvent, Key, ConstID inKeyID),
        CUsesKey{inKeyID} {}
};

class MouseButtonEvent final : public Event<EventType::MouseButton>, public CUsesMouseButton
{
public:
    EVENT_DEFAULT_CONSTRUCTOR(MouseButtonEvent, MouseButton, ConstID inButtonID),
        CUsesMouseButton{inButtonID} {}
};

class MouseMotionEvent final : public Event<EventType::MouseMotion>
{
public:
    EVENT_DEFAULT_CONSTRUCTOR(MouseMotionEvent, MouseMotion,
        FARG(glm::vec2) inCurrentMousePosition = {0.0f, 0.0f},
        FARG(glm::vec2) inLastMousePosition = {0.0f, 0.0f}),
            mCurrentPosition{inCurrentMousePosition},
            mLastPosition{inLastMousePosition},
            mDistanceTravelled{inCurrentMousePosition - inLastMousePosition} {}

    FARG(glm::vec2) GetCurrentMousePosition() const { return mCurrentPosition;   }
    FARG(glm::vec2) GetLastMousePosition()    const { return mLastPosition;      }
    FARG(glm::vec2) GetDistanceTravelled()    const { return mDistanceTravelled; }

private:
    glm::vec2 mCurrentPosition{0.0f};
    glm::vec2 mLastPosition{0.0f};
    glm::vec2 mDistanceTravelled{0.0f};
};

class ActionEvent final : public Event<EventType::Action>, public CUsesAction
{
public:
    EVENT_DEFAULT_CONSTRUCTOR(ActionEvent, Action, FARG(InputActions::Action) inAction),
        CUsesAction{inAction} {}
};

#undef EVENT_DEFAULT_CONSTRUCTOR

#endif // EVENT_H

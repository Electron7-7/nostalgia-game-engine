#ifndef EVENT_H
#define EVENT_H

#include "input/fwd.hpp"

#include "binding.hpp"
#include "managers/manager.hpp"
#include "common/time.hpp"

#include <glm/vec2.hpp>

inline bool gMouseMotionMatches(const ID& id, const glm::vec2& motion)
{
    if(!BindingIDs::IsMouseMotion(id))
        { return false; }
    return (id == BindingIDs::MouseMotionX && motion.x != 0.0f) || (id == BindingIDs::MouseMotionY && motion.y != 0.0f);
}

class InputEvent : public InputBinding
{
public:
    InputEvent() = default;
    InputEvent(const InputBinding& Binding, const glm::vec2& CurrentMouse = glm::vec2{0.0f}, const glm::vec2& LastMouse = glm::vec2{0.0f});

    glm::vec2 CurrentMousePosition() const;
    glm::vec2 LastMousePosition() const;
    glm::vec2 MouseMotion() const;

    InputEventType Type() const;
    bool IsType(InputEventType EventType) const;

    std::string Log() const;
    std::string DemoString() const;
    bool Valid() const;

private:
    double    mTime{Time::Elapsed()};
    long      mTick{_Manager::TickNumber()};
    glm::vec2 mCurMousePos{0.0f};
    glm::vec2 mLastMousePos{0.0f};
};

#endif // EVENT_H

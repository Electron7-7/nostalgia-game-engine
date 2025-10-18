#ifndef EVENT_H
#define EVENT_H

#include "fwd.hpp"
#include "enums.hpp"
#include "ids.hpp"
#include "time.hpp"
#include "managers/manager.hpp"

#include <glm/vec2.hpp>
#include <set>

struct InputEvent
{
public:
    InputEvent() = default;
    InputEvent(ID BindingID);
    InputEvent(ID BindingID, const glm::vec2& CurrentMousePosition, const glm::vec2& LastMousePosition);

    const glm::vec2& CurrentMousePosition() const;
    const glm::vec2& LastMousePosition() const;
    glm::vec2 MouseMotion() const;
    const InputBinding& Binding() const;
    const std::set<std::string>& Actions() const;
    InputStatus Status() const;
    bool JustPressed() const;
    bool JustReleased() const;
    bool Pressed() const;
    bool Released() const;
    bool JustActive() const;
    bool Active() const;
    bool Inactive() const;
    InputEventType Type() const;
    bool IsMouseMotion() const;
    bool IsType(InputEventType EventType) const;
    bool IsAction(const std::string& Action) const;
    bool IsInput(const std::string& BindingName) const;
    bool IsInput(ID BindingID) const;
    std::string Log() const;
    std::string DemoString() const;
    bool Valid() const;

private:
    double mTime{Time::Elapsed()};
    long   mTick{_Manager::TickNumber()};
    ID mBindingID{};
    glm::vec2 mCurMousePos{0.0f, 0.0f};
    glm::vec2 mLastMousePos{0.0f, 0.0f};
};

#endif // EVENT_H

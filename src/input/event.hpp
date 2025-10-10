#ifndef EVENT_H
#define EVENT_H

#include "fwd.hpp"
#include "binding.hpp"
#include "time.hpp"
#include "managers/manager.hpp"

#include <glm/vec2.hpp>
#include <set>

struct InputEvent
{
public:
    InputEvent();
    InputEvent(const InputEvent& Copy);
    InputEvent(const std::string& InputName, InputStatus Status, const std::set<std::string>& Actions = {});
    InputEvent(const InputBinding& InputBinding, const std::set<std::string>& Actions = {});
    InputEvent(const InputBinding& InputBinding, const glm::vec2& CurMousePos, const glm::vec2& LastMousePos, const std::set<std::string>& Actions = {});

    const glm::vec2& CurrentMousePosition() const;
    const glm::vec2& LastMousePosition() const;
    glm::vec2 MouseMotion() const;
    const std::set<std::string>& Actions() const;
    const InputBinding& Binding() const;
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
    bool IsInput(const std::string& InputName) const;
    bool IsInput(const InputBinding& InputBinding) const;
    std::string Log() const;
    std::string DemoString() const;

    constexpr bool Valid() const
    { return BindingIDs::Exists(mBinding); }

private:
    friend class gen1_demo_controller;

    double mTime = Time::Elapsed();
    long   mTick = _Manager::TickNumber();
    InputBinding mBinding;
    std::set<std::string> mActions = {};
    glm::vec2 mCurMousePos  = {0.0f, 0.0f};
    glm::vec2 mLastMousePos = {0.0f, 0.0f};
};

#endif // EVENT_H

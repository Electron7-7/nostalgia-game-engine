#ifndef EVENT_H
#define EVENT_H

#include "id.hpp"
#include "binding.hpp"
#include "enums.hpp"
#include "printing.hpp"
#include "time.hpp"
#include "managers/manager.hpp"

#include <set>
#include <format>
#include <glm/vec2.hpp>

struct InputEvent
{
public:
    InputEvent() = default;
    InputEvent(const std::string& InputName, InputStatus Status, const std::set<std::string>& Actions = {}):
        m_Binding(InputName, Status),
        m_Actions(Actions)
    {}
    InputEvent(const binding_t& InputBinding, const std::set<std::string>& Actions = {}):
        m_Binding(InputBinding),
        m_Actions(Actions)
    {}
    InputEvent(const binding_t& InputBinding, const glm::vec2& CurMousePos, const glm::vec2& LastMousePos, const std::set<std::string>& Actions = {}):
        m_Binding(InputBinding),
        m_Actions(Actions),
        m_CurrentMousePosition(CurMousePos),
        m_LastMousePosition(LastMousePos)
    {}

    double CreationTime() const
    { return m_Time; }

    long TickNumber() const
    { return m_Tick; }

    const glm::vec2& CurrentMousePosition() const
    { return m_CurrentMousePosition; }

    const glm::vec2& LastMousePosition() const
    { return m_LastMousePosition; }

    glm::vec2 MouseMotion() const
    {
        if(!IsMouseMotion())
            { PRINT_WARNING("InputEvent::MouseMotion called on an event that isn't mouse motion") }
        return m_CurrentMousePosition - m_LastMousePosition;
    }

    const std::set<std::string>& Actions() const
    { return m_Actions; }

    const binding_t& Binding() const
    { return m_Binding; }

    InputStatus Status() const
    { return m_Binding.Status; }

    bool JustPressed() const
    { return (Pressed() && m_Binding.JustChanged); }

    bool JustReleased() const
    { return Released(); }

    bool Pressed() const
    { return (Status() == InputStatus::Pressed); }

    bool Released() const
    { return (Status() == InputStatus::Released); }

    bool JustActive() const
    { return JustPressed(); }

    bool Active() const
    { return Pressed(); }

    bool Inactive() const
    { return Released(); }

    InputEventType Type() const
    {
        if(m_Binding > InputID::End)
            { return InputEventType::Invalid; }
        else if(m_Binding < InputID::MouseButtonsBegin)
            { return InputEventType::Key; }
        else if(m_Binding < InputID::MouseMotionBegin)
            { return InputEventType::MouseButton; }
        return InputEventType::MouseMotion;
    }

    bool IsMouseMotion() const
    { return (m_Binding >= InputID::MouseMotionBegin && m_Binding <= InputID::MouseMotionEnd); }

    bool IsType(InputEventType EventType) const
    { return (Type() == EventType); }

    bool IsAction(const std::string& Action) const
    {
        if(!m_Actions.contains(Action))
            { return false; }
        else if(Action.at(0) == '-')
            { return Released(); }
        return Pressed();
    }

    bool IsInput(const std::string& InputName) const
    { return (InputID::GetID(InputName) == m_Binding); }

    bool IsInput(const binding_t& InputBinding) const
    { return (m_Binding == InputBinding); }

    bool IsFirstAction(const std::string& Action) const
    { return (IsAction(Action) && JustActive()); }

    bool IsFirstInput(const std::string& InputName) const
    { return (IsInput(InputName) && JustActive()); }

    bool IsFirstInput(const binding_t& InputBinding) const
    { return (IsInput(InputBinding) && JustActive()); }

    std::string Log()
    {
        std::string type_name = "";
        InputEventType type = Type();
        switch(type)
        {
        case InputEventType::Key:
            type_name = "Key";
            break;
        case InputEventType::MouseButton:
            type_name = "MouseButton";
            break;
        case InputEventType::MouseMotion:
            type_name = "MouseMotion";
            break;
        case InputEventType::Invalid:
        default:
            type_name = "Invalid";
            break;
        }
        return std::format("InputEvent Log - Creation Time: {}, Event Type: {} ({}), Input ID: {}", m_Time, static_cast<int>(type), type_name, static_cast<id_t>(m_Binding));
    }

protected:
    friend class InputManager;
    friend class EventQueue;
    binding_t   m_Binding = binding_t();
    double      m_Time    = Time::Elapsed();
    long        m_Tick    = _Manager::TickNumber();
    std::set<std::string> m_Actions = {};
    glm::vec2 m_CurrentMousePosition = {0.0f, 0.0f};
    glm::vec2 m_LastMousePosition = {0.0f, 0.0f};

    constexpr bool Valid() const
    { return (m_Binding != INVALID_ID); }
};

#endif // EVENT_H

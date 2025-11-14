#ifndef EVENT_H
#define EVENT_H

#include "input/fwd.hpp"

#include "binding.hpp"
#include "managers/manager.hpp"

#include <glm/vec2.hpp>

class InputEvent
{
public:
    void UpdateMouseMotion(const glm::vec2& CurrentMouse, const glm::vec2& LastMouse);

    const glm::vec2& CurrentMousePosition() const;
    const glm::vec2& LastMousePosition() const;
    glm::vec2 MouseMotion() const;

    InputStatus StateOf(const ID&) const;
    bool IsKeyDown(const ID&) const;
    bool IsKeyUp(const ID&) const;
    bool IsKeyPressed(const ID&) const;
    bool IsKeyReleased(const ID&) const;
    bool IsMouseCaptured() const;

    template<IDType... IDArgs>
        bool AreKeysDown(IDArgs... KeyIDs) const
        {
            for(auto id : {KeyIDs...})
                { if(!IsKeyDown(id)) { return false; } }
            return true;
        }
    template<IDType... IDArgs>
        bool AreKeysUp(IDArgs... KeyIDs) const
        {
            for(auto id : {KeyIDs...})
                { if(!IsKeyUp(id)) { return false; } }
            return true;
        }
    template<IDType... IDArgs>
        bool AreKeysPressed(IDArgs... KeyIDs) const
        {
            for(auto id : {KeyIDs...})
                { if(!IsKeyPressed(id)) { return false; } }
            return true;
        }
    template<IDType... IDArgs>
        bool AreKeysReleased(IDArgs... KeyIDs) const
        {
            for(auto id : {KeyIDs...})
                { if(!IsKeyReleased(id)) { return false; } }
            return true;
        }

    template<StringType... StringArgs>
        bool IsActionDown(StringArgs... Actions) const
        {
            for(const auto& input : mInputs)
                { if(input.IsAction(std::string{Actions}...) && input.Active()) { return true; } }
            return false;
        }
    template<StringType... StringArgs>
        bool IsActionUp(StringArgs... Actions) const
        {
            for(const auto& input : mInputs)
                { if(input.IsAction(std::string{Actions}...) && input.Inactive()) { return true; } }
            return false;
        }
    template<StringType... StringArgs>
        bool IsActionPressed(StringArgs... Actions) const
        {
            for(const auto& input : mInputs)
                { if(input.IsAction(std::string{Actions}...) && input.JustActivated()) { return true; } }
            return false;
        }
    template<StringType... StringArgs>
        bool IsActionReleased(StringArgs... Actions) const
        {
            for(const auto& input : mInputs)
                { if(input.IsAction(std::string{Actions}...) && input.JustDeactivated()) { return true; } }
            return false;
        }

    bool empty() const;
    bool add(const InputBinding&);
    bool erase(const InputBinding&);
    void clear();

    std::string Log() const;
    std::string DemoString() const;

private:
    long mTick{IManager::TickNumber()};
    std::set<InputBinding> mInputs{};
    glm::vec2 mCurMousePos{0.0f};
    glm::vec2 mLastMousePos{0.0f};

    typedef bool (InputBinding::*InputQueryFunction)() const;
    bool QueryInput(const ID&, InputQueryFunction) const;
};

#endif // EVENT_H

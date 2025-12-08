#ifndef INPUT_MANAGER_H
#define INPUT_MANAGER_H

#include "events/fwd.hpp"
#include "math/fwd.hpp"

#include "manager.hpp"
#include "events/action.hpp"
#include "core/type_helpers.hpp"
#include "core/error.hpp"

#include <glm/vec2.hpp>
#include <unordered_map>
#include <string>

class InputManager : public Manager
{
public:
    consteval const char* DebugName() final { return "InputManager"; }
    bool Init()   override;
    void Update() override;

    bool mDebugPrintEverySingleEventToTheConsole{false};

    EventQueue* Queue();

    bool UpdateKeyState(FARG(KeyID) inKeyID, bool inCurrentState);
    void SetAction(FARG(InputAction) inAction);
    Error AddAction(FARG(InputAction) inAction);
    Error DeleteAction(FARG(std::string) inActionName);
    void ClearAllActions();

    static bool IsKeyDown(const KeyID& inKey) noexcept;
    static bool IsKeyUp(const KeyID& inKey) noexcept;
    static bool IsKeyPressed(const KeyID& inKey) noexcept;
    static bool IsKeyReleased(const KeyID& inKey) noexcept;

    static bool IsActionDown(const std::string& inName) noexcept;
    static bool IsActionUp(const std::string& inName) noexcept;
    static bool IsActionPressed(const std::string& inName) noexcept;
    static bool IsActionReleased(const std::string& inName) noexcept;

    static Position2D MousePosition() noexcept;
    static Position2D LastMousePosition() noexcept;
    static Motion2D MouseMotion() noexcept;

private:
    struct InputState
    {
        bool active{false};
        bool just_changed{false};

        void set(bool isActive) noexcept
        {
            just_changed = active != isActive;
            active = isActive;
        }

        bool pressed() const noexcept
        { return active && just_changed; }

        bool released() const noexcept
        { return !active && just_changed; }
    };

    static std::unordered_map<std::string, InputAction> m_sInputActions;
    static EventQueue m_sInputEventQueue;
    static std::unordered_map<uint, InputState> m_sInputStateBuffer;
    static std::unordered_map<uint, std::vector<std::string>> m_sInputActionBindingsLookup;
};

extern InputManager* g_pInputManager;

#endif // INPUT_MANAGER_H

#ifdef FWD_DCL
class InputManager;
extern InputManager* g_pInputManager;
#elif !defined INPUT_MANAGER_H
#define INPUT_MANAGER_H

#define FWD_DCL
#   include "events/event.hpp"
#   include "events/event_queue.hpp"
#   include "math/containers.hpp"
#   include "events/action.hpp"
#   include "events/bindings.hpp"
#undef FWD_DCL

#include "manager.hpp"
#include "core/farg.hpp"
#include "core/error.hpp"

#include <unordered_map>
#include <mutex>

typedef void (*pInputCallback_f)(InputEvent*);

class InputManager : public Manager
{
public:
    consteval const char* DebugName() final { return "InputManager"; }
    bool Init()   override;
    void Update() override;

    bool mDebugPrintEverySingleEventToTheConsole{false};

    EventQueue* Queue();

    bool UpdateKeyState(KeyID inKeyID, bool inCurrentState);
    void SetAction(Farg<InputAction> inAction);
    Error AddAction(Farg<InputAction> inAction);
    Error DeleteAction(Farg<std::string> inActionName);
    void ClearAllActions();
    void AddCallback(pInputCallback_f);
    void EraseCallback(pInputCallback_f);

    static bool IsKeyDown(KeyID inKeyID) noexcept;
    static bool IsKeyUp(KeyID inKeyID) noexcept;
    static bool IsKeyPressed(KeyID inKeyID) noexcept;
    static bool IsKeyReleased(KeyID inKeyID) noexcept;

    static bool IsActionDown(Farg<std::string> inName) noexcept;
    static bool IsActionUp(Farg<std::string> inName) noexcept;
    static bool IsActionPressed(Farg<std::string> inName) noexcept;
    static bool IsActionReleased(Farg<std::string> inName) noexcept;

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

    std::vector<pInputCallback_f> mCallbacks{};
    std::recursive_mutex mCallbacksMutex{};
    static std::unordered_map<uint, InputManager::InputState> m_sInputStateBuffer;
};

extern InputManager* g_pInputManager;

#endif // INPUT_MANAGER_H

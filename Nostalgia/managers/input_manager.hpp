#ifndef INPUT_MANAGER_H
#define INPUT_MANAGER_H

#include <Nostalgia/managers/manager.hpp>
#include <Nostalgia/fwd/events.hpp>

typedef void (*pInputCallback_f)(InputEvent*);

class InputManager : public Manager
{
public:
    constexpr const char* DebugName() final { return "InputManager"; }
    bool Init()   override;
    void Update() override;

    EventQueue* Queue();

    bool UpdateKeyState(KeyID inKeyID, bool inCurrentState);
    void UpdateScrollOffset(double inOffsetX, double inOffsetY);
    void SetAction(Farg<InputAction> inAction);
    Error AddAction(Farg<InputAction> inAction);
    Error DeleteAction(Farg<std::string> inActionName);
    void ClearAllActions();
    void AddCallback(pInputCallback_f);
    void EraseCallback(pInputCallback_f);

    static bool IsKeyDown(KeyID inKeyID) noexcept;
    static bool IsKeyUp(KeyID inKeyID) noexcept;
    static bool IsActionDown(Farg<std::string> inName) noexcept;
    static bool IsActionUp(Farg<std::string> inName) noexcept;

    static bool IsKeyJustDown(KeyID inKeyID) noexcept;
    static bool IsKeyJustUp(KeyID inKeyID) noexcept;
    static bool IsActionJustDown(Farg<std::string> inName) noexcept;
    static bool IsActionJustUp(Farg<std::string> inName) noexcept;

    static Position2D MousePosition() noexcept;
    static Position2D LastMousePosition() noexcept;
    static Motion2D MouseMotion() noexcept;

    static Position2D ScrollOffset() noexcept;

private:
    struct InputState
    {
    private:
        long time_changed_{0};
        bool active_{false};
        bool just_changed_{false};

        void update_()
        {
            if(!just_changed_)
                { return; }
            else if(m_sFrameNumber != time_changed_)
                { just_changed_ = false; }
        }
    public:
        void set(bool isActive) noexcept
        {
            just_changed_ = active_ != isActive;
            active_ = isActive;
            if(just_changed_)
                { time_changed_ = Manager::m_sFrameNumber; }
        }

        bool active() noexcept
        { return active_; }

        bool just_changed() noexcept
        { return just_changed_; }

        bool pressed() noexcept
        {
            update_();
            return active_ and just_changed_;
        }

        bool released() noexcept
        {
            update_();
            return !active_ and just_changed_;
        }
    };

    std::vector<pInputCallback_f> mCallbacks{};
    std::recursive_mutex mCallbacksMutex{};
    static Position2D m_sScrollOffset;
    static std::unordered_map<uint, InputManager::InputState> m_sInputStateBuffer;
    static std::unordered_map<uint, InputManager::InputState> m_sPreviousInputState;
};

extern bool gPrintInputLogs;
extern InputManager* g_pInputManager;

#endif // INPUT_MANAGER_H

#ifndef INPUT_BINDING_H
#define INPUT_BINDING_H

#include "enums.hpp"

#include <string>

struct InputBinding
{
public:
    constexpr InputBinding() = default;

    const char* name()   const { return name_; }
    InputStatus status() const { return status_; }
    bool just_changed()  const { return just_changed_; }
    bool locked()        const { return locked_; }

    bool Activate()
    {
        just_changed_ = (status_ != InputStatus::Active);
        status_ = InputStatus::Active;
        return just_changed_;
    }
    bool Press() { return Activate(); }

    bool Deactivate()
    {
        just_changed_ = (status_ != InputStatus::Inactive);
        status_ = InputStatus::Inactive;
        return just_changed_;
    }
    bool Release() { return Deactivate(); }

    bool Active()   const { return status_ == InputStatus::Active; }
    bool Pressed()  const { return Active();  }
    bool Inactive() const { return !Active(); }
    bool Released() const { return !Active(); }

    bool JustActivated()   const { return (Active() && just_changed_); }
    bool JustPressed()     const { return JustActivated(); }
    bool JustDeactivated() const { return (Inactive() && just_changed_); }
    bool JustReleased()    const { return JustDeactivated(); }

    constexpr bool operator==(const std::string& name) const
    { return !name.compare(name_); }

private:
    friend class InputManager;
    constexpr InputBinding(const char* name):
        name_(name) {}

    friend class gen1_demo_controller;
    const char* name_{""};
    InputStatus status_{InputStatus::Inactive};
    bool just_changed_{false};
    bool locked_{false};
};

#endif // INPUT_BINDING_H

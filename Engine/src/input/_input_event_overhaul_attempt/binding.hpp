#ifndef INPUT_BINDING_H
#define INPUT_BINDING_H

#include "enums.hpp"
#include "actions.hpp"
#include "core/id.hpp"
#include "core/type_helpers.hpp"

#include <string>

class InputBinding
{
public:
    InputBinding() = default;
    InputBinding(FARG(ID), FARG(InputActions), bool Locked = false, InputStatus = InputStatus::Inactive, bool JustChanged = false);
    InputBinding(FARG(ID), bool Locked = false, InputStatus = InputStatus::Inactive, bool JustChanged = false);

    InputActions mActions{};

    const ID& id() const;
    InputStatus status() const;
    bool just_changed() const;
    bool locked() const;

    bool Activate();
    bool Deactivate();
    bool Press();
    bool Release();

    bool Active() const;
    bool Inactive() const;
    bool JustActivated() const;
    bool JustDeactivated() const;
    bool Pressed() const;
    bool Released() const;
    bool JustPressed() const;
    bool JustReleased() const;
    bool IsMouseMotion() const;
    bool IsInput(const ID& BindingID) const;
    bool IsInput(const std::string& BindingName) const;
    bool IsValid() const;

    // Returns `true` if `mActions` contains any of the supplied actions.
    template<StringType... StringArgs>
    bool IsAction(StringArgs... Actions) const
    {
        for(const auto& action : {Actions...})
            { if(mActions.has(action)) { return (action.at(0) == '-') ? Released() : Pressed(); } }
        return false;
    }

    constexpr bool operator==(FARG(InputBinding) binding) const { return mID == binding.mID; }
    constexpr bool operator< (FARG(InputBinding) binding) const { return mID < binding.mID; }
    constexpr bool operator==(FARG(ID) id) const { return mID == id; }
    constexpr bool operator==(FARG(std::string) name) const { return !name.compare(mID.name()); }

protected:
    ID mID{};
    InputStatus mStatus{InputStatus::Inactive};
    bool mJustChanged{false};
    bool mLocked{false};
};

#endif // INPUT_BINDING_H

#include "binding.hpp"

InputBinding::InputBinding(const ID& id, const InputActions& actions, bool locked, InputStatus status, bool changed):
    mActions{actions},
    mID{id},
    mStatus{status},
    mJustChanged{changed},
    mLocked{locked} {}

InputBinding::InputBinding(const ID& id, bool locked, InputStatus status, bool changed):
    mID{id},
    mStatus{status},
    mJustChanged{changed},
    mLocked{locked} {}

InputActions mActions{};

const ID& InputBinding::id() const
{ return mID; }

bool InputBinding::Activate()
{
    if(Active())
        { return mJustChanged = false; }
    mStatus = InputStatus::Active;
    return mJustChanged = true;
}
bool InputBinding::Press() { return Activate(); }

bool InputBinding::Deactivate()
{
    if(Inactive())
        { return mJustChanged = false; }
    mStatus = InputStatus::Inactive;
    return mJustChanged = true;
}
bool InputBinding::Release() { return Deactivate(); }

bool InputBinding::Active()          const { return (mLocked) ? false : (mStatus == InputStatus::Active);   }
bool InputBinding::Inactive()        const { return (mLocked) ? false : (mStatus == InputStatus::Inactive); }
bool InputBinding::JustActivated()   const { return (Active()   && mJustChanged);  }
bool InputBinding::JustDeactivated() const { return (Inactive() && mJustChanged);  }
bool InputBinding::Pressed()         const { return Active();          }
bool InputBinding::Released()        const { return Inactive();        }
bool InputBinding::JustPressed()     const { return JustActivated();   }
bool InputBinding::JustReleased()    const { return JustDeactivated(); }

bool InputBinding::IsMouseMotion()                         const { return BindingIDs::IsMouseMotion(mID);      }
bool InputBinding::IsInput(const ID& BindingID)            const { return (mID == BindingID);                  }
bool InputBinding::IsInput(const std::string& BindingName) const { return IsInput(ConstexprHash(BindingName)); }
bool InputBinding::IsValid()                               const { return mID != ID::Invalid;                  }

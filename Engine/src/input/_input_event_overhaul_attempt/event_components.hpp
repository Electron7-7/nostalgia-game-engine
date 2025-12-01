#ifndef EVENT_COMPONENTS_H
#define EVENT_COMPONENTS_H

#include "binding.hpp"
#include "core/id.hpp"

class CUsesAction
{
public:
    CUsesAction(FARG(InputActions::Action) inAction): mAction{inAction} {}



    FARG(InputActions::Action) operator()() const { return mAction; }

protected:
    InputActions::Action mAction{};
};

class CUsesBinding
{
public:
    virtual ~CUsesBinding() = default;
    CUsesBinding(FARG(InputBinding) inBinding): mBinding{inBinding} {}

    bool IsDown()     const { return mBinding.Active();          }
    bool IsUp()       const { return mBinding.Inactive();        }
    bool IsPressed()  const { return mBinding.JustActivated();   }
    bool IsReleased() const { return mBinding.JustDeactivated(); }

    bool IsBinding(FARG(InputBinding) inBinding) const
    { return mBinding == inBinding; }

    FARG(InputBinding) operator()()                     const { return mBinding;          }
    constexpr bool operator==(FARG(InputBinding) other) const { return mBinding == other; }
    constexpr bool operator< (FARG(InputBinding) other) const { return mBinding < other;  }

protected:
    InputBinding mBinding{};
};

class CUsesKey : public CUsesBinding
{
public:
    CUsesKey(ConstID inKeyID):
        CUsesBinding{InputBinding{inKeyID}}
    { assert(BindingID::IsKey(inKeyID) && "CUsesKey is for keyboard keys only"); }
};

class CUsesMouseButton : public CUsesBinding
{
public:
    CUsesMouseButton(ConstID inButtonID): CUsesBinding{InputBinding{inButtonID}}
    { assert(BindingID::IsMouseButton(inButtonID) && "CUsesMouseButton is for mouse buttons only"); }
};

#endif // EVENT_COMPONENTS_H

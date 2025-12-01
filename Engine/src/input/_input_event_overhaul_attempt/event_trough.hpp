#ifndef EVENT_TROUGH_H
#define EVENT_TROUGH_H

#include "event.hpp"
#include "managers/manager.hpp"

#include <map>

class EventTrough
{
public:
    bool HasMouseMotion()                   const { return mMouseMotion != nullptr;      }
    bool HasKey(FARG(ConstID) inID)         const { return mKeys.contains(inID);         }
    bool HasMouseButton(FARG(ConstID) inID) const { return mMouseButtons.contains(inID); }

    FARG(Unique<MouseMotionEvent>) MouseMotion() const { return mMouseMotion; }

    bool IsDown(FARG(ID) inID) const
    {
        if(BindingID::IsBinding(inID))
        {
            if(mKeys.contains(inID))
                { return mKeys.at(inID).IsDown(); }
            else if(mMouseButtons.contains(inID))
                { return mMouseButtons.at(inID).IsDown(); }
        }
        return false;
    }

    bool IsUp(FARG(ID) inID) const
    {
        if(BindingID::IsBinding(inID))
        {
            if(mKeys.contains(inID))
                { return mKeys.at(inID).IsUp(); }
            else if(mMouseButtons.contains(inID))
                { return mMouseButtons.at(inID).IsUp(); }
        }
        return false;
    }

    bool IsPressed(FARG(ID) inID) const
    {
        if(BindingID::IsBinding(inID))
        {
            if(mKeys.contains(inID))
                { return mKeys.at(inID).IsPressed(); }
            else if(mMouseButtons.contains(inID))
                { return mMouseButtons.at(inID).IsPressed(); }
        }
        return false;
    }

    bool IsReleased(FARG(ID) inID) const
    {
        if(BindingID::IsBinding(inID))
        {
            if(mKeys.contains(inID))
                { return mKeys.at(inID).IsReleased(); }
            else if(mMouseButtons.contains(inID))
                { return mMouseButtons.at(inID).IsReleased(); }
        }
        return false;
    }

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
        bool IsActionActive(StringArgs... Actions) const
        { return mActions.has_any({Actions...}); }
    template<StringType... StringArgs>
        bool IsActionInactive(StringArgs... Actions) const
        { return !IsActionActive({Actions...}); }

    std::string Log() const;
    std::string DemoString() const;

    void add(FARG(InputBinding) inBinding)
    {
        FARG(ID) bindingID{inBinding.id()};
        if(!BindingID::IsBinding(bindingID))
            { return; }
        else if(BindingID::IsKey(bindingID))
            { mKeys[bindingID] = KeyEvent{bindingID}; }
        else if(BindingID::IsMouseButton(bindingID))
            { mMouseButtons[bindingID] = MouseButtonEvent{bindingID}; }
        if(!inBinding.mActions.empty())
            { mActions.add(inBinding.mActions); }
    }

    void add(FARG(MouseMotionEvent) inMouseMotion)
    { mMouseMotion = inMouseMotion; }

private:
    long mTick{IManager::TickNumber()};

    std::map<ConstID, KeyEvent> mKeys{};
    std::map<ConstID, MouseButtonEvent> mMouseButtons{};
    InputActions mActions{};
    MouseMotionEvent mMouseMotion{};

    typedef bool (InputBinding::*InputQueryFunction)() const;
    bool QueryInput(FARG(ID), InputQueryFunction) const;
};

#endif // EVENT_TROUGH_H

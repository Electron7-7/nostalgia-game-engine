#include "action.hpp"

FARG(std::string) InputAction::Name() const
{ return mName; }

void InputAction::UpdateStatus(FARG(KeyID) inKeyID, bool isActive)
{
    mLastStatus = mCurrentStatus;
    mCurrentStatus = isActive;
    for(auto [id, state] : mKeyIDs)
    {
        if(inKeyID == id)
            { state = isActive; }
        if(!state)
            { mCurrentStatus = false; }
    }
}

void InputAction::UpdateStatus()
{
    mLastStatus = mCurrentStatus;
    for(auto [id, state] : mKeyIDs)
        { if(!state) { mCurrentStatus = false; } }
}

bool InputAction::Status() const
{ return mCurrentStatus; }

bool InputAction::StatusChanged() const
{ return mCurrentStatus != mLastStatus; }

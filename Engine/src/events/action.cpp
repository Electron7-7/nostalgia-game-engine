#include "action.hpp"
#include "core/error.hpp"

bool InputAction::UpdateState(uint inKeyID, bool inKeyState)
{
    if(auto it{mBitMasks.find(inKeyID)}; it != mBitMasks.end())
    {
        mPreviousField = mBitField;
        if(inKeyState) { mBitField |=  it->second; return true; }
        else           { mBitField &= ~it->second; return mPreviousField != mBitField; }
    }
    return false;
}

Error InputAction::Erase(uint inKeyID)
{
    if(mBitMasks.empty())
        { return ERR_EMPTY; }
    else if(auto it{mBitMasks.find(inKeyID)}; it != mBitMasks.end())
    {
        mBitMaskIter = (it->second != 0b1)
            ? it->second >> 1
            : it->second;
        it = mBitMasks.erase(it);
        while(it != mBitMasks.end())
        {
            it->second = mBitMaskIter <<= 1;
            ++it;
        }
        return OK;
    }
    return ERR_NOT_FOUND;
}

Error InputAction::Add(uint inKeyID)
{
    if(Full())
        { return ERR_FULL; }
    else if(auto it{mBitMasks.find(inKeyID)}; it == mBitMasks.end())
        { mBitMasks[inKeyID] = mBitMaskIter <<= 1; return OK; }
    return ERR_ALREADY_EXISTS;
}

const std::string& InputAction::Name() const
{ return mName; }

bool InputAction::State() const
{ return mTrueState == mBitField; }

bool InputAction::StateJustChanged() const
{ return mBitField == mPreviousField; }

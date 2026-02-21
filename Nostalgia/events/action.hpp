#ifndef INPUT_ACTION_H
#define INPUT_ACTION_H

// can't be fucked to find the max width of `uint` and, tbh, you don't really need more than 20 keys in an action
inline constinit const ushort MaxActions{20};

class InputAction
{
private:
    std::string mName{"Untitled InputAction"};
    std::unordered_map<uint, uint> mBitMasks{}; // <KeyID, bitmask>
    uint mPreviousField{0}, mBitField{0}, mTrueState{0};
    uint mBitMaskIter{0b1}; // the last-used bitmask value

public:
    template<ID_t... KeyIDs> requires (sizeof...(KeyIDs) <= MaxActions)
        InputAction(Farg<std::string> inName, KeyIDs&... inKeyIDs):
            mName{inName}
        {
            mBitMasks.reserve(sizeof...(inKeyIDs));
            for(FARG(auto) id : {inKeyIDs...})
            {
                mTrueState |= mBitMasks[id()] = mBitMaskIter;
                mBitMaskIter <<= 1;
            }
            if(mBitMaskIter != 0b1) { mBitMaskIter >>= 1; }
        }

    constexpr bool Full() const noexcept
    { return mBitMasks.size() >= MaxActions; }

    bool UpdateState(uint inKeyID, bool inKeyState);
    Error Erase(uint inKeyID);
    Error Add(uint inKeyID);
    Farg<std::string> Name() const;
    bool State() const;
    bool StateJustChanged() const;

    constexpr bool operator==(Farg<std::string> inName) const noexcept
    { return !mName.compare(inName); }
    constexpr bool operator==(Farg<InputAction> inAction) const noexcept
    { return !mName.compare(inAction.mName); }
    constexpr bool operator< (Farg<InputAction> inAction) const noexcept
    { return mName < inAction.mName; }
};
#endif // INPUT_ACTION_H

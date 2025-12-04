#ifndef INPUT_ACTION_H
#define INPUT_ACTION_H

#include "bindings.hpp"
#include "core/type_helpers.hpp"

#include <unordered_map>

constinit const ushort cInputActionMaxKeys{20};

class InputAction
{
public:
    template<KeyID_t... KeyIDs> requires (sizeof...(KeyIDs) <= cInputActionMaxKeys)
        InputAction(FARG(std::string) inName, KeyIDs... inKeyIDs):
            mName{inName}
        {
            for(auto key : {inKeyIDs...})
                { mKeyIDs[key] = false; }
        }

    FARG(std::string) Name() const;
    void UpdateStatus(FARG(KeyID) inKeyID, bool isActive);
    void UpdateStatus();
    bool Status() const;
    bool StatusChanged() const;

    constexpr bool operator==(FARG(std::string) inName) const noexcept
    { return !mName.compare(inName); }

private:
    std::string mName{"Untitled InputAction"};
    std::unordered_map<KeyID, bool> mKeyIDs{};
    bool mLastStatus{false};
    bool mCurrentStatus{false};
};

#endif // INPUT_ACTION_H

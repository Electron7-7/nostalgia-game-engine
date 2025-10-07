#ifndef INPUT_BINDING_H
#define INPUT_BINDING_H

#include "ids.hpp"
#include "enums.hpp"

#include <string>

struct InputBinding
{
public:
    constexpr InputBinding() = default;
    constexpr InputBinding(ID binding_id): id_(binding_id) {}
    constexpr InputBinding(const std::string& Name, InputStatus Status):
        status(Status),
        id_(BindingIDs::GetID(Name))
    {}

    constexpr operator ID() const
    { return id_; }
    constexpr operator id_t() const
    { return id_; }

    InputStatus status = InputStatus::Inactive;
    bool just_changed = false;
    bool locked = false;

private:
    friend class DemoParser;
    ID id_ = ID::Invalid;
};

#endif // INPUT_BINDING_H

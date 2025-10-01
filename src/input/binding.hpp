#ifndef INPUT_BINDING_H
#define INPUT_BINDING_H

#include "fwd.hpp"
#include "id.hpp"
#include "enums.hpp"

#include <string>

struct binding_t
{
public:
    constexpr binding_t() = default;
    constexpr binding_t(id_t ID): m_ID(ID) {}
    constexpr binding_t(const std::string& Name, InputStatus Status):
        Status(Status),
        m_ID(InputID::GetID(Name))
    {}

    constexpr operator const id_t&() const
    { return m_ID; }

    InputStatus Status = InputStatus::Inactive;
    bool JustChanged = false;
    bool Locked = false;

private:
    friend class DemoParser;
    id_t m_ID = INVALID_ID;
};

#endif // INPUT_BINDING_H

#ifndef INPUT_ENUMS_H
#define INPUT_ENUMS_H

#include <array>

enum class InputEventType
{
    Key = 0,
    MouseButton = 1,
    MouseMotion = 2,
    Invalid = 3
};

constexpr std::array<const char*, 4>
gInputEventType =
{
    "Key",
    "MouseButton",
    "MouseMotion",
    "Invalid"
};

enum class InputStatus
{
    Pressed  = 1,
    Released = 0,
    Active   = Pressed,
    Inactive = Released
};

constexpr std::array<const char*, 2>
gInputStatus =
{
    "Pressed/Active",
    "Released/Inactive"
};

#endif // INPUT_ENUMS_H

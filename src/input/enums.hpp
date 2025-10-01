#ifndef INPUT_ENUMS_H
#define INPUT_ENUMS_H

enum class InputStatus
{
    Pressed  = 1,
    Released = 0,
    Active   = Pressed,
    Inactive = Released
};

#endif // INPUT_ENUMS_H

#ifndef DEVICE_H
#define DEVICE_H

#include "../thing.hpp"

class Device : public Thing
{
public:
    virtual void SetVariables(const ThingData&);
    virtual ThingData GetVariables() const;
};

#endif // DEVICE_H

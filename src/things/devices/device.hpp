#ifndef DEVICE_H
#define DEVICE_H

#include "../thing.hpp"

class Device : public Thing
{
public:
    virtual void SetupVariables(const ThingData&);
};

#endif // DEVICE_H

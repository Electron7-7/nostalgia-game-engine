#ifdef FWD_DCL
    class Device;
#elif !defined DEVICE_H
#define DEVICE_H

#include "things/thing.hpp"

class Device : public Thing
{
public:
    virtual void SetVariables(Farg<ThingData>) override;
    virtual Shared<ThingData> GetVariables() const override;
};

#endif // DEVICE_H

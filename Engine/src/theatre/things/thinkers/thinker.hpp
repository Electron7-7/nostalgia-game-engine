#ifndef THINKER_H
#define THINKER_H

#include "theatre/things/thing.hpp"

// Similar to Godot's `Node`
class Thinker : public Thing
{
public:
    virtual void Ready() override;
    virtual void SetVariables(Farg<ThingData>) override;
    virtual Shared<ThingData> GetVariables() const override;
};

#endif // THINKER_H

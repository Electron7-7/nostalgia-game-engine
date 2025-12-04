#ifndef THING_H
#define THING_H

#include "events/fwd.hpp"

#include "types.hpp"
#include "theatre/parser/fwd.hpp"
#include "components/game_loop.hpp"
#include "components/input_handling.hpp"

class Thing : public OnInput, public OnTick, public OnUpdate
{
public:
    virtual ~Thing();

    virtual void Ready() {}
    virtual void Tick() override {}
    virtual void Update() override {}
    virtual void Input(InputEvent*) override {}

    // Derived classes must call their base class' `::SetVariables` method at the start of their own implementation of `::SetVariables`. If done properly, this will result in a chain of function calls all the way to `Thing::SetVariables`.
    //
    // See `Material::SetVariables` for an example
    virtual void SetVariables(const ThingData& Data);
    // Derived classes must call their base class' `::GetVariables` method at the start of their own implementation of `::GetVariables`.
    //
    // See `Material::GetVariables` for an example.
    virtual ThingData GetVariables() const;

    const ID& uid() const;
    const ID& type() const;
    const std::string& name() const;
    const char* const c_name() const;

protected:
    ID mUID{ID::Invalid};
    ID mType{ThingType::Thing};
    std::string mName{""};
};

#endif // THING_H

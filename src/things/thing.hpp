#ifndef THING_H
#define THING_H

#include "input/fwd.hpp"
#include "theatre_parser/fwd.hpp"

#include "types.hpp"

class Thing
{
public:
    virtual ~Thing();

    virtual void Tick() {};
    virtual void Update() {};
    virtual void Input(const InputEvent&) {};

    // Derived classes must call their base class' `::SetVariables` method at the start of their own implementation of `::SetVariables`. If done properly, this will result in a chain of function calls all the way to `Thing::SetVariables`.
    //
    // See `Material::SetVariables` for an example
    virtual void SetVariables(const ThingData& Data);
    // Derived classes must call their base class' `::GetVariables` method at the start of their own implementation of `::GetVariables`.
    //
    // See `Material::GetVariables` for an example.
    virtual ThingData GetVariables() const;

    ID uid() const;
    ID type() const;
    const std::string& name() const;

protected:
    ID mUID{ID::None};
    ID mType{ThingType::Thing};
    std::string mName{""};
};

#endif // THING_H

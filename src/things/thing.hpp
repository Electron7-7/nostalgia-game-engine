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
    virtual void Input(const InputEvent& Event) {};

    // Derived classes must call their base class' `::SetupVariables` method at the start of their own implementation of `::SetupVariables`. If done properly, this will result in a chain of function calls all the way to `Thing::SetupVariables`.
    virtual void SetupVariables(const ThingData& Data);

    ID uid() const;
    ID type() const;
    const std::string& name() const;

protected:
    ID mUID = ID::None;
    ID mType = ThingType::Thing;
    std::string mName = "Untitled Thing";
};

#endif // THING_H

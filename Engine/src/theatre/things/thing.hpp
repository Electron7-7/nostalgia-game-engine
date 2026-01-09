#ifndef THING_H
#define THING_H

#include "fwd/core.hpp"
#include "fwd/theatre.hpp"
#include "core/smart_pointers.hpp"
#include "core/id.hpp"
#include "components/game_loop.hpp"
#include "components/event_handling.hpp"
#include "theatre/thing_type.hpp"

// Similar to Godot's `Object`
class Thing : public OnInput, public OnTick, public OnUpdate
{
public:
    Thing() noexcept;
    virtual ~Thing() noexcept;

    virtual void Free() {}
    virtual void Ready() {}
    virtual void Shutdown() {}
    virtual void Tick() override {}
    virtual void Update() override {}
    virtual void Input(InputEvent*) override {}

    // Derived classes must call their base class' `::SetVariables` method at the start of their own implementation of `::SetVariables`. If done properly, this will result in a chain of function calls all the way to `Thing::SetVariables`.
    //
    // See `Material::SetVariables` for an example
    virtual void SetVariables(Farg<ThingData>);
    // Derived classes must call their base class' `::GetVariables` method at the start of their own implementation of `::GetVariables`.
    //
    // See `Material::GetVariables` for an example.
    virtual Shared<ThingData> GetVariables() const;

    ThingData GetStartingVariables() const;

    ID uid() const;
    bool uid(ID inID);
    Sarg name() const;
    void name(Sarg inName);
    const char* const c_name() const;
    FPID type() const;
    Farg<ThingType> full_type() const;

protected:
    ID mUID{};
    // ID mParentTheatreID{};
    std::string mName{"Untitled Thing"};
    PID mType{};
    Unique<ThingData> mStartingData{nullptr};
};

template<typename T>
    concept ThingDerived = std::derived_from<T, Thing>;

#endif // THING_H

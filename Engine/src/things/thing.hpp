#ifndef THING_H
#define THING_H

#include "fwd/theatre.hpp"
#include "things/types.hpp"
#include "core/smart_pointers.hpp"
#include "components/game_loop.hpp"
#include "components/event_handling.hpp"

class Thing : public OnInput, public OnTick, public OnUpdate
{
public:
    Thing() noexcept;
    virtual ~Thing() noexcept;

    virtual void Ready() {}
    virtual void Tick() override {}
    virtual void Update() override {}
    virtual void Input(InputEvent*) override {}

    // Derived classes must call their base class' `::SetVariables` method at the start of their own implementation of `::SetVariables`. If done properly, this will result in a chain of function calls all the way to `Thing::SetVariables`.
    //
    // See `Material::SetVariables` for an example
    virtual void SetVariables(Farg<ThingData> Data);
    // Derived classes must call their base class' `::GetVariables` method at the start of their own implementation of `::GetVariables`.
    //
    // See `Material::GetVariables` for an example.
    virtual Shared<ThingData> GetVariables() const;

    ID uid() const;
    Farg<TTID> type() const;
    Sarg name() const;
    const char* const c_name() const;

protected:
    ID mUID{};
    std::string mName{"Untitled Thing"};
    TTID mType{"Thing"};
};

template<typename T>
    concept ThingDerived = std::derived_from<T, Thing>;

#endif // THING_H

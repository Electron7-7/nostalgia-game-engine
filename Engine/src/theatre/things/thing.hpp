#ifndef THING_H
#define THING_H

#include "fwd/core.hpp"
#include "fwd/theatre.hpp"
#include "components/game_loop.hpp"
#include "components/event_handling.hpp"
#include "theatre/thing_type.hpp"

// Similar to Godot's `Object`
class Thing : public OnInput, public OnTick, public OnUpdate
{
public:
    bool mIsHoveredInDebugger{false};

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
    virtual void SetVariables(Farg<TheatreFile::ThingData>);
    // Derived classes must call their base class' `::GetVariables` method at the start of their own implementation of `::GetVariables`.
    //
    // See `Material::GetVariables` for an example.
    virtual Shared<TheatreFile::ThingData> GetVariables() const;

    TheatreFile::ThingData GetStartingVariables() const;

    ID uid() const;
    Sarg name() const;
    void set_name(Sarg);
    const char* const c_name() const;
    FPID type() const;
    Farg<ThingType> full_type() const;

protected:
    friend class Theatre;
    ID mUID{};
    std::string mName{"Untitled Thing"};
    PID mType{};
    Unique<TheatreFile::ThingData> m_pStartingData{nullptr};
    Theatre* m_pRootTheatre{nullptr};
};

template<typename T>
    concept ThingDerived = std::derived_from<T, Thing>;

#endif // THING_H

#ifndef THING_H
#define THING_H

#include "fwd/theatre.hpp"
#include "fwd/core.hpp"
#include "things/types.hpp"
#include "core/smart_pointers.hpp"
#include "components/game_loop.hpp"
#include "components/event_handling.hpp"
#include <mutex>
#include <unordered_map>

struct ThingChildren;

class Thing : public OnInput, public OnTick, public OnUpdate
{
public:
    using Children = std::unordered_map<Shared<Thing>, std::string>;

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
    Sarg name() const;
    const char* const c_name() const;
    Children children() const;

    bool uid(ID inID);
    void name(Sarg inName);
    Farg<TTID> type() const;
    Error destroy_child(ID);
    Error create_child(Farg<ThingData> inData, Sarg inUniqueName = "");
    Error own_child(ID, Sarg inUniqueName = "");
    Error disown_child(ID);


    constexpr bool operator==(Farg<Thing> inThing)
    { return mUID == inThing.mUID; }

    constexpr bool operator<(Farg<Thing> inThing)
    { return mUID < inThing.mUID; }

protected:
    Thing(ID) noexcept;
    Thing(Farg<ThingData>) noexcept;

    ID mUID{};
    std::string mName{"Untitled Thing"};
    TTID mType{"Thing"};

    Shared<ThingChildren> mChildren{nullptr};
    std::recursive_mutex mChildrenMutex{};
};

template<typename T>
    concept ThingDerived = std::derived_from<T, Thing>;

constexpr bool operator==(Farg<Shared<Thing>> inPtrL, Farg<Shared<Thing>> inPtrR)
{ return inPtrL->uid() == inPtrR->uid(); }

constexpr bool operator<(Farg<Shared<Thing>> inPtrL, Farg<Shared<Thing>> inPtrR)
{ return inPtrL->uid() < inPtrR->uid(); }

constexpr bool operator==(Farg<Shared<Thing>> inThingPtr, ID inID)
{ return inThingPtr->uid() == inID; }

constexpr bool operator<(Farg<Shared<Thing>> inThingPtr, ID inID)
{ return inThingPtr->uid() == inID; }

struct ThingChildren
{ Thing::Children data{}; };

#endif // THING_H

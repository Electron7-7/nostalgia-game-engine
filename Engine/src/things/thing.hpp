#ifndef THING_H
#define THING_H

#include "fwd/theatre.hpp"
#include "fwd/core.hpp"
#include "things/types.hpp"
#include "core/smart_pointers.hpp"
#include "core/mutex.hpp"
#include "components/game_loop.hpp"
#include "components/event_handling.hpp"
#include <vector>

struct ThingChild
{
    ThingChild() = default;
    ThingChild(ID inID, bool isUnique = false):
        id{inID}, is_unique{isUnique} {}
    ID id{};
    bool is_unique{false};
    constexpr bool operator==(Farg<ThingChild> other) const noexcept
    { return id == other.id and is_unique == other.is_unique; }
};
using children_t = std::vector<ThingChild>;

class Thing : public OnInput, public OnTick, public OnUpdate
{
public:

    Thing() noexcept;
    virtual ~Thing() noexcept;

    virtual void Ready();
    virtual void Shutdown() {}
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

    void Free();

    ID uid() const;
    bool uid(ID inID);
    Sarg name() const;
    void name(Sarg inName);
    const char* const c_name() const;
    Farg<TTID> type() const;

    Farg<children_t> children() const;
    ID parent() const;

    Error add_child(ThingChild);
    Error remove_child(ThingChild);
    Error swap_child(ThingChild, ThingChild);

    Error set_parent(ID);
    Error unset_parent();

protected:
    ID mUID{};
    std::string mName{"Untitled Thing"};
    TTID mType{"Thing"};
    RMutex mChildrenMutex{};
    ID mParentID{};
    children_t mChildren{};
};

template<typename T>
    concept ThingDerived = std::derived_from<T, Thing>;

#endif // THING_H

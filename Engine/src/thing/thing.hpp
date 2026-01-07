#ifndef THING_H
#define THING_H

#include "fwd/theatre.hpp"
#include "fwd/core.hpp"
#include "thing/types.hpp"
#include "core/smart_pointers.hpp"
#include "core/mutex.hpp"
#include "components/game_loop.hpp"
#include "components/event_handling.hpp"
#include <vector>

struct ThingRelative
{
    ThingRelative() = default;
    ThingRelative(ID inID, Farg<TTID> inType):
        id{inID}, type{inType} {}
    ID id{};
    TTID type{};
    constexpr bool operator==(Farg<ThingRelative> other) const noexcept
    { return id == other.id and type == other.type; }
};
using relatives_t = std::vector<ThingRelative>;

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

    ThingData GetStartingVariables() const;

    void Free();

    ID uid() const;
    bool uid(ID inID);
    Sarg name() const;
    void name(Sarg inName);
    const char* const c_name() const;
    Farg<TTID> type() const;

    relatives_t children() const;
    relatives_t parents()  const;

    Error add_child(ThingRelative, bool doUpdateChild = false);
    Error remove_child(ThingRelative, bool doUpdateChild = false);
    Error swap_child(ThingRelative, ThingRelative, bool doUpdateChildren = false);

    Error add_parent(ThingRelative, bool doUpdateParent = false);
    Error remove_parent(ThingRelative, bool doUpdateParent = false);
    Error swap_parent(ThingRelative, ThingRelative, bool doUpdateParents = false);

protected:
    ID mUID{};
    std::string mName{"Untitled Thing"};
    TTID mType{"Thing"};
    RMutex mChildrenMutex{};
    Unique<ThingData> mStartingData{nullptr};

    relatives_t mParents{};
    relatives_t mChildren{};
};

template<typename T>
    concept ThingDerived = std::derived_from<T, Thing>;

#endif // THING_H

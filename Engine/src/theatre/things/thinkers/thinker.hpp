#ifndef THINKER_H
#define THINKER_H

#include "theatre/things/thing.hpp"
#include "core/mutex.hpp"

struct ThinkerRelative
{
    ThinkerRelative() = default;
    ThinkerRelative(ID inID, FPID inType):
        id{inID}, type{inType} {}
    ID id{};
    PID type{};
    constexpr bool operator==(Farg<ThinkerRelative> other) const noexcept
    { return id == other.id and type == other.type; }
};

using ThinkerChildren = std::vector<ThinkerRelative>;

// Similar to Godot's `Node`
class Thinker : public Thing
{
public:
    virtual void Free() override;
    virtual void Ready() override;
    virtual void SetVariables(Farg<ThingData>) override;
    virtual Shared<ThingData> GetVariables() const override;

    ThinkerChildren Children() const;
    ThinkerRelative Parent() const;

    Error add_child(ThinkerRelative, bool doUpdateChild = true);
    Error remove_child(ThinkerRelative, bool doUpdateChild = true);
    Error swap_child(ThinkerRelative, ThinkerRelative, bool doUpdateChildren = true);

    Error set_parent(ThinkerRelative, bool doUpdateParent = true);
    Error swap_parent(ThinkerRelative, ThinkerRelative, bool doUpdateParents = true);
    Error remove_parent(ThinkerRelative, bool doUpdateParent = true);

protected:
    RMutex mChildrenMutex{};
    ThinkerRelative mParent{};
    ThinkerChildren mChildren{};
};

#endif // THINKER_H

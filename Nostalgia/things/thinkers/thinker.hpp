#ifndef THINKER_H
#define THINKER_H

#include <Nostalgia/things/thing.hpp>

class Thinker : public Thing
{
public:
	SET_SUPER(Thing)
	SET_TYPEID(ThingType::Thinker)
    SET_VARIABLES_OVERRIDE
    GET_VARIABLES_OVERRIDE

    bool Visible() const;
    void SetVisible(bool);

    IdSet_t Children() const;
    ID Parent() const;

    Error SetParent(ID inParentID);
    // Equivalent to calling `SetParent` with the current parent's parent. If the current parent
    // has no parent (e.g: `ID::Invalid`), nothing happens (if you would consult the graphs).
    Error DropParent();

protected:
    friend class Theatre;

    struct Relative
    {
        Relative() noexcept {}
        Relative(Farg<Shared<Thinker>> inThinker) noexcept:
            uid{inThinker->uid()},
            type{inThinker->Type()},
            name{inThinker->name()} {}

        ID uid{};
        PID type{};
        std::string name{};

        constexpr bool invalid() const noexcept
        { return uid.invalid() or type.invalid(); }
    };

    bool mVisible{true};

    virtual void OnChildAdded(Relative) {}
    virtual void OnChildRemoved(Relative) {}
    virtual void OnParentChanged(Relative inNewParent, Relative inOldParent = {}) {}
    virtual void OnAncestorAdded(Relative) {}
    virtual void OnAncestorRemoved(Relative) {}
    virtual void OnDescendantAdded(Relative) {}
    virtual void OnDescendantRemoved(Relative) {}
};

template<typename T>
    concept Thinker_t = std::derived_from<T, Thinker>;

#endif // THINKER_H

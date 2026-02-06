#ifndef THINKER_H
#define THINKER_H

#ifdef NOSTALGIA_GCC_PCH
#   include <Nostalgia/Nostalgia.hpp>
#endif

// Similar to Godot's `Node`
class Thinker : public Thing
{
public:
    virtual void SetVariables(Farg<TheatreFile::ThingData>) override;
    virtual Shared<TheatreFile::ThingData> GetVariables() const override;

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
            type{inThinker->type()},
            name{inThinker->name()} {}

        ID uid{};
        PID type{};
        std::string name{};

        constexpr bool invalid() const noexcept
        { return uid.invalid() or type.invalid(); }
    };

    RMutex mChildrenMutex{};
    bool mVisible{true};

    virtual void OnChildAdded(Relative) {}
    virtual void OnChildRemoved(Relative) {}
    virtual void OnParentChanged(Relative inNewParent, Relative inOldParent = {}) {}
    virtual void OnAncestorAdded(Relative) {}
    virtual void OnAncestorRemoved(Relative) {}
    virtual void OnDescendantAdded(Relative) {}
    virtual void OnDescendantRemoved(Relative) {}
};

#endif // THINKER_H

#ifndef THING_H
#define THING_H

#include <Nostalgia/events/event.hpp>

/// Forward Declaration
namespace TheatreFile { struct ThingData; }

#define THING_CLASS(CLASS, INHERITS) \
private: \
    inline static const PID TypeID{#CLASS}; \
    inline static const PID BaseTypeID{#INHERITS}; \
public: \
    using SelfType = CLASS; \
    using Super = INHERITS; \
    using INHERITS::INHERITS; \
    inline virtual FPID Type() const override { return TypeID; } \
    inline virtual FPID BaseType() const override { return BaseTypeID; } \
    inline static constexpr PID sClassType() { return {#CLASS}; } \
    inline static constexpr PID sClassBaseType() { return {#INHERITS}; } \
    virtual void InitVariables() override;

/// Helper macro for declaring an override for `Ready`
#define READY_OVERRIDE virtual void Ready() override;
/// Helper macro for declaring an override for `Shutdown`
#define SHUTDOWN_OVERRIDE virtual void Shutdown() override;
/// Helper macro for declaring an override for `Update`
#define UPDATE_OVERRIDE virtual void Update() override;
/// Helper macro for declaring an override for `Tick`
#define TICK_OVERRIDE virtual void Tick() override;
/// Helper macro for declaring an override for `Input`
#define INPUT_OVERRIDE virtual void Input(InputEvent*) override;
/// Helper macro for declaring an override for `SetVariables`
#define SET_VARIABLES_OVERRIDE virtual void SetVariables(Farg<TheatreFile::ThingData>) override;
/// Helper macro for declaring an override for `GetVariables`
#define GET_VARIABLES_OVERRIDE virtual Shared<TheatreFile::ThingData> GetVariables() const override;

class Thing
{
private:
    inline static const PID TypeID{"Thing"};
    inline static const PID BaseTypeID{TypeID};

public:
    using SelfType = Thing;
    using Super = Thing;
    inline virtual FPID Type() const { return TypeID; }
    inline virtual FPID BaseType() const { return BaseTypeID; }
    inline static constexpr PID sClassType() { return "Thing"; }
    inline static constexpr PID sClassBaseType() { return sClassType(); }

    Thing(Sarg inName) noexcept;
    // Used by the Theatre & ResourceDatabase when a `::GetThing` function fails, in order to return
    // a `Thing` with an invalid UID. This lets `Thing::invalid` return `true`.
    Thing() noexcept;
    virtual ~Thing() noexcept;

    virtual void InitVariables();
    // Derived classes must call `Super::SetVariables` with the given `Farg<TheatreFile::ThingData>` argument
    // at the start of their own implementation.
    virtual void SetVariables(Farg<TheatreFile::ThingData>);
    // Derived classes must call `Super::GetVariables` at the start of their own implementation and use the
    // returned `Shared<TheatreFile::ThingData>` variable as their return value.
    virtual Shared<TheatreFile::ThingData> GetVariables() const;

    bool DerivedFrom(FPID inType) const;
    bool IsThinker() const;
    bool IsResource() const;
    TheatreFile::ThingData GetDefaultVariables() const;
    ID uid() const;
    Sarg name() const;
    const char* const c_name() const;
    Error rename(Sarg inNewName);
    bool invalid() const;
    std::string to_string() const;

    void _initialize_variables();

protected:
    friend class Theatre;
    friend class ResourceDatabase;

    inline static Shared<TheatreFile::ThingData> s_pDefaultVariables{nullptr};

    typedef Error (*pNameChangeCallback_f)(Sarg inOldName, Sarg inNewName);

    virtual void Ready() {}
    virtual void Shutdown() {}
    virtual void Tick() {}
    virtual void Update() {}
    virtual void Input(InputEvent*) {}

    void SetNameChangeCallback(pNameChangeCallback_f = nullptr);

private:
    inline static RMutex m_sUIDMutex{};
    inline static std::set<uint> m_sActiveUIDs{};
    inline static std::uniform_int_distribution<uint> m_sDistribution{UID::front, UID::back};
    inline static std::random_device m_sRandomSeed{};
    inline static std::mt19937 m_sIdEngine{m_sRandomSeed()};

    mutable RMutex mMutex{};
    ID mUID{};
    std::string mName{GlobalConstants::Init::cstr_empty};
    pNameChangeCallback_f m_pNameChangeCallback{nullptr};

    ID _generate();
    uint _get_random();
};

template<typename T>
    concept Thing_t = std::derived_from<T, Thing>;

#endif // THING_H

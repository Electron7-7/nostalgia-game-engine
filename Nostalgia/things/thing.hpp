#ifndef THING_H
#define THING_H

#include <Nostalgia/events/event.hpp>

/// Forward Declaration
namespace TheatreFile { struct ThingData; }

/// Macro for defining the `Super` keyword and base-class constructor inheritance.
/// See other Thing-derived classes like `Thinker` for example use cases.
#define SET_SUPER(SUPER) using Super = SUPER; using SUPER::SUPER;
/// Macro for defining the `Type` function override.
/// See other Thing-derived classes like `Thinker` for example use cases.
#define SET_TYPEID(TYPE_PID) virtual constexpr FPID Type() const override { return TYPE_PID; }
/// Macro for quickly creating a custom, hidden `PID` variable, defining the `Type` function override, and using
/// the previously created variable as the return value for `Type`.
/// See 'Editor/things/player.hpp' for an example use case.
#define DEFINE_TYPEID(CLASS, VAR_NAME) inline static const PID VAR_NAME{#CLASS}; SET_TYPEID(VAR_NAME)

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
public:
    virtual constexpr FPID Type() const { return ThingType::Thing; }

    Thing(Sarg inName) noexcept;
    // Used by the Theatre & ResourceDatabase when a '::GetThing' function fails to return a `Thing` with an invalid UID.
    Thing() noexcept;
    virtual ~Thing() noexcept;

    // Derived classes must call `Super::SetVariables` at the start of their own implementation.
    virtual void SetVariables(Farg<TheatreFile::ThingData>);
    // Derived classes must call `Super::GetVariables` at the start of their own implementation.
    virtual Shared<TheatreFile::ThingData> GetVariables() const;

    bool DerivedFrom(FPID inType) const;
    bool IsThinker() const;
    bool IsResource() const;
    TheatreFile::ThingData GetStartingVariables() const;
    ID uid() const;
    Sarg name() const;
    const char* const c_name() const;
    Error rename(Sarg inNewName);
    bool Invalid() const;

protected:
    friend class Theatre;
    friend class ResourceDatabase;

    typedef Error (*pNameChangeCallback_f)(Sarg inOldName, Sarg inNewName);

    Shared<TheatreFile::ThingData> m_pStartingData{nullptr};

    virtual void Ready() {}
    virtual void Shutdown() {}
    virtual void Tick() {}
    virtual void Update() {}
    virtual void Input(InputEvent*) {}

    void SetNameChangeCallback(pNameChangeCallback_f = nullptr);

private:
    friend class ThingFactory;

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

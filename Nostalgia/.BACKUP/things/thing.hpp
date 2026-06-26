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
    inline virtual FPID Type() const override { return TypeID; } \
    inline virtual FPID BaseType() const override { return BaseTypeID; } \
    inline static constexpr PID sClassType() { return {#CLASS}; } \
    inline static constexpr PID sClassBaseType() { return {#INHERITS}; } \
    virtual void InitVariables() override; \
    inline static Shared<SelfType> Invalid() \
    { \
        auto _output{MakeShared<SelfType>()}; \
        _output->mName = GlobalConstants::str_NA; \
        _output->mUID = ID::Invalid; \
        return _output; \
    }

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
    inline virtual FPID Type() const { return Thing::TypeID; }
    inline virtual FPID BaseType() const { return Thing::BaseTypeID; }
    inline static constexpr PID sClassType() { return Thing::TypeID; }
    inline static constexpr PID sClassBaseType() { return Thing::sClassType(); }

    static Shared<Thing> Invalid();

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
    friend struct ThingFactory;

    Shared<TheatreFile::ThingData> m_pDefaultVariables{nullptr};

    static Shared<Thing> InvalidThinker();
    static Shared<Thing> InvalidResource();

    virtual void Ready() {}
    virtual void Shutdown() {}
    virtual void Tick() {}
    virtual void Update() {}
    virtual void Input(InputEvent*) {}

    mutable RMutex mMutex{};
    ID mUID{};
    std::string mName{};
};

template<typename T>
    concept Thing_t = std::derived_from<T, Thing>;

#endif // THING_H

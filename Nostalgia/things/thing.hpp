#ifndef THING_H
#define THING_H

/// Forward Declaration
class Theatre;
/// Forward Declaration
class InputEvent;
/// Forward Declaration
class ResourceDatabase;
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

#define SET_VARIABLES_OVERRIDE \
    virtual void SetVariables(Farg<TheatreFile::ThingData>) override;
#define GET_VARIABLES_OVERRIDE \
    virtual Shared<TheatreFile::ThingData> GetVariables() const override;
#define READY_OVERRIDE \
    virtual void Ready() override;

// Similar to Godot's `Object`
class Thing
{
public:
    virtual constexpr FPID Type() const { return ThingType::Thing; }

    bool mIsHoveredInDebugger{false};

    Thing() noexcept;
    Thing(FPID inType, Sarg inName, ID inUID = {}) noexcept;
    virtual ~Thing() noexcept;

    virtual void Init() {}
    virtual void Ready() {}
    virtual void Shutdown() {}
    virtual void Tick() {}
    virtual void Update() {}
    virtual void Input(InputEvent*) {}

    // Derived classes must call their base class' `::SetVariables` method at the start of their own implementation of `::SetVariables`. If done properly, this will result in a chain of function calls all the way to `Thing::SetVariables`.
    //
    // See `Material::SetVariables` for an example
    virtual void SetVariables(Farg<TheatreFile::ThingData>);
    // Derived classes must call their base class' `::GetVariables` method at the start of their own implementation of `::GetVariables`.
    //
    // See `Material::GetVariables` for an example.
    virtual Shared<TheatreFile::ThingData> GetVariables() const;

    void Free();
    bool DerivedFrom(FPID inType) const;
    bool IsThinker() const;
    bool IsResource() const;
    TheatreFile::ThingData GetStartingVariables() const;
    ID uid() const;
    Sarg name() const;
    const char* const c_name() const;

protected:
    friend class Theatre;
    friend class ResourceDatabase;
    std::string mName{""};
    Shared<TheatreFile::ThingData> m_pStartingData{nullptr};

private:
    friend Error UID::Generate(Shared<Thing>);
    ID mUID{};
};

template<typename T>
    concept ThingDerived = std::derived_from<T, Thing>;

#endif // THING_H

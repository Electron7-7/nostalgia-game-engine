#ifndef THING_H
#define THING_H

/// Forward Declaration
class Theatre;
/// Forward Declaration
class Resource;
/// Forward Declaration
class InputEvent;
/// Forward Declaration
namespace ResourceDatabase { ID Register(Shared<Resource>, Sarg); }

#define SUPER(Class) \
    using Super = Class; \
    using Class::Class;
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
    TheatreFile::ThingData GetStartingVariables() const;
    ID uid() const;
    Sarg name() const;
    const char* const c_name() const;
    FPID type() const;

protected:
    friend class Theatre;
    friend ID ResourceDatabase::Register(Shared<Resource>, Sarg);
    std::string mName{""};
    Shared<TheatreFile::ThingData> m_pStartingData{nullptr};

private:
    friend Error UID::Generate(Shared<Thing>);
    ID mUID{};
    PID mType{};
};

template<typename T>
    concept ThingDerived = std::derived_from<T, Thing>;

#endif // THING_H

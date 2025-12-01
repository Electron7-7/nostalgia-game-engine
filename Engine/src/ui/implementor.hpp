#ifndef UI_IMPLEMENTOR_H
#define UI_IMPLEMENTOR_H

#include "fwd.hpp"
#include "events/fwd.hpp"

#include "core/id.hpp"
#include "core/type_helpers.hpp"
#include "core/printing.hpp"

#include <string>

// Shamelessly stolen from Hazel's 'Layer' system(s) (https://github.com/TheCherno/Hazel)
// Also, typing 'IUIImplementor' is funny
class IUIImplementor
{
public:
    // All implementations of `UI_Implementor` must update their `mState` appropriately
    enum InstanceState {
        STATE_ATTACHING, // During `Attach`
        STATE_IDLE, // After `Attach` or `End` but before `Begin`
        STATE_BEGINNING_FRAME, // During `Begin`
        STATE_PROCESSING_FRAME, // After `Begin` but before `End`
        STATE_ENDING_FRAME, // During `End`
        STATE_DETACHING, // During `Detach`
        STATE_INACTIVE, // After `Detach` and/or before `Attach`
    };

    virtual void Attach() = 0;
    virtual void Detach() = 0;
    virtual void Begin()  = 0;
    virtual void End()    = 0;

    IUIImplementor(FARG(std::string));
    virtual ~IUIImplementor();

    const ID& GetID() const;
    InstanceState GetState() const;
    std::vector<Unique<IUISolution>>& GetObjects();

    typedef void (IUIImplementor::*ImplementorFunctionPtr)();
    typedef void (IUISolution::*SolutionFunction_ptr)();
    typedef void (IUISolution::*SolutionInputEventFunction_ptr)(FARG(InputEvent));

    static void InvokeMethod(ImplementorFunctionPtr);
    static void InvokeMethod(SolutionFunction_ptr);
    static void InvokeMethod(SolutionInputEventFunction_ptr, FARG(InputEvent));

    static void AttachAll();
    static void DetachAll();
    static void BeginAll();
    static void EndAll();
    static const std::vector<Unique<IUIImplementor>>& GetInstances();

    inline static constexpr const char* StateString(InstanceState inState)
    {
        switch(inState)
        {
        case STATE_ATTACHING: return "STATE_ATTACHING";
        case STATE_IDLE: return "STATE_IDLE";
        case STATE_BEGINNING_FRAME: return "STATE_BEGINNING_FRAME";
        case STATE_PROCESSING_FRAME: return "STATE_PROCESSING_FRAME";
        case STATE_ENDING_FRAME: return "STATE_ENDING_FRAME";
        case STATE_DETACHING: return "STATE_DETACHING";
        case STATE_INACTIVE: return "STATE_INACTIVE";
        }
    }

    constexpr bool operator==(FARG(Unique<IUIImplementor>) other) const
    { return mID == other->mID; }

protected:
    ID mID{"IUIImplementor"};
    bool mGlobalCanHandleEvents{true};
    bool mAttached{false};
    std::vector<Unique<IUISolution>> mObjects{};
    InstanceState mState{STATE_INACTIVE};

    virtual bool DeactivateOpportunity() = 0;

    static std::vector<Unique<IUIImplementor>> m_sInstances;
};

class UI_Implementor : public IUIImplementor
{
public:
    virtual void Attach() override {}
    virtual void Detach() override {}
    virtual void Begin()  override {}
    virtual void End()    override {}

    UI_Implementor(FARG(std::string));

    void StopHandlingEvents(bool inStopHandlingEvents);
};


// All `UI_Implementor` implementations must use this macro in their definitions.
// See `Engine/src/backends/imgui/imgui_implementor.hpp:` for an example.
#define UI_IMPLEMENTOR_CONSTRUCTOR(CLASS) \
private: \
    CLASS(): UI_Implementor{#CLASS} { PRINT_PRETTY_FUNCTION; } \

// All `UI_Implementor` implementations must use this macro in their definitions.
// See `Engine/src/backends/imgui/imgui_implementor.hpp:` for an example.
#define UI_IMPLEMENTOR_INSTANCE(CLASS) \
public: \
    static UI_Implementor* Instance() \
    { \
        if(!s_pInstance) { Activate(); } \
        return s_pInstance; \
    } \
private: \
    inline static CLASS* s_pInstance{nullptr};

// All `UI_Implementor` implementations must use this macro in their definitions.
// See `Engine/src/backends/imgui/imgui_implementor.hpp:` for an example.
#define UI_IMPLEMENTOR_ACTIVATE(CLASS) \
public: \
    static void Activate() { \
        PRINT_PRETTY_FUNCTION; \
        if(s_pInstance) { return; } \
        s_pInstance = new CLASS{}; \
        IUIImplementor::m_sInstances.push_back(Unique<IUIImplementor>(s_pInstance)); \
        s_pInstance->Attach(); \
    } \
private:

// All `UI_Implementor` implementations must use this macro in their definitions.
// See `Engine/src/backends/imgui/imgui_implementor.hpp:` for an example.
#define UI_IMPLEMENTOR_DEACTIVATE(CLASS) \
protected: \
    bool DeactivateOpportunity() final { return CLASS::Deactivate(); } \
public: \
    static bool Deactivate() { typedef IUIImplementor IUII; \
        PRINT_PRETTY_FUNCTION; \
        if(!s_pInstance || s_pInstance->mObjects.size() > 0) { return false; } \
        for(auto it{IUII::m_sInstances.begin()}; it != IUII::m_sInstances.end(); ++it) { \
            if(*s_pInstance == *it) { \
                s_pInstance->Detach(); \
                IUII::m_sInstances.erase(it); \
                break; \
            } \
        } s_pInstance = nullptr; return true; \
    } \
private:

#endif // UI_IMPLEMENTOR_H

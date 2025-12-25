#ifdef FWD_DCL
class UI_Implementor;
#elif !defined UI_IMPLEMENTOR_H
#define UI_IMPLEMENTOR_H

#define FWD_DCL
#   include "events/event.hpp"
#undef  FWD_DCL

#include "solution.hpp"
#include "core/id.hpp"
#include "core/error.hpp"
#include "core/smart_pointers.hpp"

#include <map>
#include <typeindex>

#define STATE_STRING(STATE) case STATE: return #STATE;

// Shamelessly stolen from Hazel's 'Layer' system(s) (https://github.com/TheCherno/Hazel)
// Also, typing 'UI_Implementor' is funny
class UI_Implementor
{
public:
    using Instances = std::map<std::type_index, Unique<UI_Implementor>>;
    using Solutions = std::map<std::type_index, Unique<UI_Solution>>;

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

    virtual ~UI_Implementor();

    virtual void Attach() {}
    virtual void Detach() {}
    virtual void Begin()  {}
    virtual void End()    {}

    InstanceState GetState() const;

    typedef void (UI_Implementor::*ImplementorFunctionPtr)();
    typedef void (UI_Solution::*SolutionFunction_ptr)();
    typedef Error (UI_Solution::*SolutionInit_ptr)();
    typedef void (UI_Solution::*SolutionInputEventFunction_ptr)(Farg<InputEvent>);

    static void InvokeMethod(ImplementorFunctionPtr);
    static void InvokeMethod(SolutionFunction_ptr);
    static void InvokeMethod(SolutionInit_ptr);
    static void InvokeMethod(SolutionInputEventFunction_ptr, Farg<InputEvent>);

    static void AttachAll();
    static void DetachAll();
    static void BeginAll();
    static void EndAll();

    static constexpr const char* StateString(InstanceState inState)
    {
        switch(inState)
        {
        STATE_STRING(STATE_ATTACHING)
        STATE_STRING(STATE_IDLE)
        STATE_STRING(STATE_BEGINNING_FRAME)
        STATE_STRING(STATE_PROCESSING_FRAME)
        STATE_STRING(STATE_ENDING_FRAME)
        STATE_STRING(STATE_DETACHING)
        STATE_STRING(STATE_INACTIVE)
        }
    }

    void StopHandlingEvents(bool inStopHandlingEvents)
    { mGlobalCanHandleEvents = !inStopHandlingEvents; }

    template<IsUiSolution T>
        Unique<UI_Solution>& CreateSolution()
        {
            mObjects[typeid(T)] = MakeUnique<T>();
            mObjects[typeid(T)]->mImplementorIndex = mIndex;
            mObjects[typeid(T)]->mImplementorIndex = typeid(T);
            return mObjects[typeid(T)];
        }

    template<typename T> requires std::derived_from<T, UI_Implementor>
        static Unique<UI_Implementor>& Create() noexcept
        {
            m_sInstances[typeid(T)] = MakeUnique<T>();
            m_sInstances[typeid(T)]->mIndex = typeid(T);
            return m_sInstances[typeid(T)];
        }

protected:
    std::type_index mIndex{typeid(UI_Implementor)};
    bool mGlobalCanHandleEvents{true};
    bool mAttached{false};
    InstanceState mState{STATE_INACTIVE};

    Solutions mObjects{};
    inline static Instances m_sInstances{};
};

template<typename T>
    concept IsUiImplementor = std::derived_from<T, UI_Implementor>;

#undef STATE_STRING
#endif // UI_IMPLEMENTOR_H

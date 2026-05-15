#ifndef UI_IMPLEMENTOR_H
#define UI_IMPLEMENTOR_H

#include <Nostalgia/ui/solution.hpp>

#define STATE_STRING(STATE) case STATE: return #STATE;

// Shamelessly stolen from Hazel's 'Layer' system(s) (https://github.com/TheCherno/Hazel)
class UI_Implementor
{
public:
    using Instances = std::map<std::type_index, Unique<UI_Implementor>>;
    using Solutions = std::map<std::type_index, Unique<UI_Solution>>;

    // All implementations of `UI_Implementor` must update their `mState` appropriately
    enum InstanceState : int {
        STATE_ATTACHING, // During `Attach`
        STATE_IDLE, // After `Attach` or `End` but before `Begin`
        STATE_BEGINNING_FRAME, // During `Begin`
        STATE_PROCESSING_FRAME, // After `Begin` but before `End`
        STATE_ENDING_FRAME, // During `End`
        STATE_DETACHING, // During `Detach`
        STATE_INACTIVE, // After `Detach` and/or before `Attach`
    };

    UI_Implementor();
    virtual ~UI_Implementor();

    virtual void Attach() = 0;
    virtual void Detach() = 0;
    virtual void Begin()  = 0;
    virtual void End()    = 0;

    InstanceState GetState() const;

    typedef void (UI_Implementor::*ImplementorFunctionPtr)();
    typedef void (UI_Solution::*SolutionFunction_ptr)();

    static void InvokeMethod(ImplementorFunctionPtr);
    static void InvokeMethod(SolutionFunction_ptr);
    static void InvokeInput(InputEvent*);
    static void InvokeEvent(IEvent*);

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

    bool CanHandleEvents() const
    { return mCanHandleEvents; }

    void CanHandleEvents(bool inCanHandleEvents)
    { mCanHandleEvents = inCanHandleEvents; }

    static void SetGlobalCanHandleEvents(bool inCanHandleEvents);

    template<typename T> requires std::derived_from<T, UI_Solution>
        T* CreateSolution()
        {
            mObjects[typeid(T)] = MakeUnique<T>();
            mObjects[typeid(T)]->mImplementorIndex = mIndex;
            return static_cast<T*>(mObjects[typeid(T)].get());
        }

    template<typename T> requires std::derived_from<T, UI_Implementor>
        static UI_Implementor* Create() noexcept
        {
            m_sInstances[typeid(T)] = MakeUnique<T>();
            m_sInstances[typeid(T)]->mIndex = typeid(T);
            return m_sInstances[typeid(T)].get();
        }

protected:
    std::type_index mIndex{typeid(UI_Implementor)};
    bool mCanHandleEvents{true};
    bool mAttached{false};
    InstanceState mState{STATE_INACTIVE};

    Solutions mObjects{};
    static Instances m_sInstances;
    static std::recursive_mutex m_sInstancesMutex;
};

#undef STATE_STRING
#endif // UI_IMPLEMENTOR_H

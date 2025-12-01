#ifndef UI_INSTANCE_H
#define UI_INSTANCE_H

#include "fwd.hpp"

#include "core/id.hpp"
#include "core/error.hpp"
#include "core/printing.hpp"
#include "core/type_helpers.hpp"
#include "components/game_loop.hpp"
#include "components/input_handling.hpp"

class IUISolution : public OnUpdate, public OnInputUI, public OnTheatreChanged
{
public:
    IUISolution(const std::string& inDebugName = "IUISolution"): mID{inDebugName}
    { PRINT_PRETTY_FUNCTION; }

    virtual ~IUISolution()
    { PRINT_PRETTY_FUNCTION; }

    virtual Error Init()     = 0;
    virtual void  Shutdown() = 0;

    using OnUpdate::Update;
    using OnInputUI::Input;
    using OnTheatreChanged::OnTheatreEntered;
    using OnTheatreChanged::OnTheatreExited;

    constexpr FARG(ID) GetID() const { return mID; }
    void StopHandlingEvents(bool inStopHandlingEvents = true) { mCanHandleEvents = !inStopHandlingEvents; }

    constexpr bool operator==(FARG(Unique<IUISolution>) other) const
    { return mID == other->mID; }

protected:
    ID mID{};
    bool mCanHandleEvents{true};
};

template<is_UI_Implementor T>
class UI_Solution : public IUISolution
{
public:
    typedef T Implementor;

    UI_Solution(FARG(std::string) inName): IUISolution{inName}
    {
        PRINT_PRETTY_FUNCTION;
        Init();
    }

    virtual ~UI_Solution()
    {
        PRINT_PRETTY_FUNCTION;
        Shutdown();
    }

    virtual Error Init()             override { return UNIMPLEMENTED; }
    virtual void  Shutdown()         override {}
    virtual void  Update()           override {}
    virtual void  Input(Event*)      override {}
    virtual void  OnTheatreEntered() override {}
    virtual void  OnTheatreExited()  override {}
};

// All `UI_Solution` implementations must use this macro in their definition.
// See `Editor/src/gui/imgui_debugger.hpp:13` for an example.
#define UI_SOLUTION_CONSTRUCTOR(CLASS, IMPLEMENTOR) \
public: \
    CLASS(): UI_Solution<IMPLEMENTOR>{#CLASS} { PRINT_PRETTY_FUNCTION; } \
private:

// All `UI_Solution` implementations must use this macro in their definition.
// See `Editor/src/gui/imgui_debugger.hpp:14` for an example.
#define UI_SOLUTION_INSTANCE(CLASS) \
public: \
    static UI_Solution<Implementor>* Instance() \
    { \
        if(!s_pInstance) { Activate(); } \
        return s_pInstance; \
    } \
private: \
    inline static CLASS* s_pInstance{nullptr};

// All `UI_Solution` implementations must use this macro in their definition.
// See `Editor/src/gui/imgui_debugger.hpp:15` for an example.
#define UI_SOLUTION_ACTIVATE(CLASS) \
public: \
    static void Activate() { \
        if(s_pInstance) { return; } \
        s_pInstance = new CLASS{}; \
        Implementor::Instance()->GetObjects().push_back(Unique<IUISolution>(s_pInstance)); \
    } \
private:

// All `UI_Solution` implementations must use this macro in their definition.
// See `Editor/src/gui/imgui_debugger.hpp:16` for an example.
#define UI_SOLUTION_DEACTIVATE(CLASS) \
public: \
    static bool Deactivate() { auto inst{Implementor::Instance()}; \
        if(!s_pInstance) { return false; } \
        for(auto it{inst->GetObjects().begin()}; it != inst->GetObjects().end(); ++it) { \
            if(*s_pInstance == *it) { \
                s_pInstance->Shutdown(); \
                inst->GetObjects().erase(it); \
                break; \
            } \
        } s_pInstance = nullptr; return true; \
    } \
private:

#endif // UI_INSTANCE_H

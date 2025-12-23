#ifdef FWD_DCL
class UI_Solution;
class UI_Solution;
#elif !defined UI_SOLUTION_H
#define UI_SOLUTION_H

#define FWD_DCL
#   include "implementor.hpp"
#undef  FWD_DCL

#include "core/id.hpp"
#include "core/error.hpp"
#include "core/printing.hpp"
#include "components/game_loop.hpp"
#include "components/event_handling.hpp"
#include <concepts>
#include <typeindex>

class UI_Solution : public OnUpdate, public OnInput, public OnTheatreChanged
{
public:
    virtual Error Init()     { PRINT_PRETTY_FUNCTION; return UNIMPLEMENTED; }
    virtual void  Shutdown() { PRINT_PRETTY_FUNCTION; }

    virtual void  Update()           override {}
    virtual void  Input(InputEvent*) override {}
    virtual void  OnTheatreEntered() override {}
    virtual void  OnTheatreExited()  override {}

    void StopHandlingEvents(bool inStopHandlingEvents = true)
    { mCanHandleEvents = !inStopHandlingEvents; }

protected:
    friend class UI_Implementor;
    bool mCanHandleEvents{true};
    std::type_index mImplementorIndex{typeid(void)};
    std::type_index mSolutionIndex{typeid(void)};
};

template<typename T>
    concept IsUiSolution = std::derived_from<T, UI_Solution>;

#endif // UI_INSTANCE_H

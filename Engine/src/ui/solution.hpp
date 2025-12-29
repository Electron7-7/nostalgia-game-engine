#ifndef UI_SOLUTION_H
#define UI_SOLUTION_H

#include "fwd/ui.hpp"
#include "components/game_loop.hpp"
#include "components/event_handling.hpp"
#include <typeindex>

class UI_Solution : public OnUpdate, public OnInput, public OnTheatreChanged
{
public:
    virtual ~UI_Solution() = default;

    virtual void Init();
    virtual void Shutdown();

    virtual void  Update()           override {}
    virtual void  Input(InputEvent*) override {}
    virtual void  TheatreEntered()   override {}
    virtual void  TheatreExited()    override {}

    void StopHandlingEvents(bool inStopHandlingEvents = true)
    { mCanHandleEvents = !inStopHandlingEvents; }

protected:
    friend class UI_Implementor;
    bool mCanHandleEvents{true};
    std::type_index mImplementorIndex{typeid(void)};
    std::type_index mSolutionIndex{typeid(void)};
};

#endif // UI_INSTANCE_H

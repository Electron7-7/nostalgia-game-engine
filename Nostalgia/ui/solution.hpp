#ifndef UI_SOLUTION_H
#define UI_SOLUTION_H

#include <Nostalgia/components/event_handling.hpp>
#include <Nostalgia/components/game_loop.hpp>

class UI_Solution : public OnUpdate, public OnTick, public OnInput, public OnEvent, public OnTheatreChanged
{
public:
    virtual ~UI_Solution() = default;

    virtual void Init();
    virtual void Shutdown();

    virtual void  Update()            override {}
    virtual void  Tick()              override {}
    virtual void  Input(InputEvent*)  override {}
    virtual void  Event(IEvent*)      override {}
    virtual void  TheatreEntered()    override {}
    virtual void  TheatreExited()     override {}

    bool CanHandleEvents() const;
    void CanHandleEvents(bool inStopHandlingEvents = true);

protected:
    friend class UI_Implementor;
    bool mCanHandleEvents{true};
    std::type_index mImplementorIndex{typeid(void)};
    std::type_index mSolutionIndex{typeid(void)};
};

#endif // UI_INSTANCE_H

#ifndef UI_SOLUTION_H
#define UI_SOLUTION_H

class UI_Solution
{
public:
    virtual ~UI_Solution() = default;

    virtual void Init();
    virtual void Shutdown();

    virtual void  Update()           {}
    virtual void  Tick()             {}
    virtual void  Input(InputEvent*) {}
    virtual void  Event(IEvent*)     {}
    virtual void  TheatreEntered()   {}
    virtual void  TheatreExited()    {}

    bool CanHandleEvents() const;
    void CanHandleEvents(bool inStopHandlingEvents = true);

protected:
    friend class UI_Implementor;
    bool mCanHandleEvents{true};
    std::type_index mImplementorIndex{typeid(void)};
    std::type_index mSolutionIndex{typeid(void)};
};

#endif // UI_INSTANCE_H

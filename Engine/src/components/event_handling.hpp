#ifndef EVENT_HANDLING_COMPONENTS_H
#define EVENT_HANDLING_COMPONENTS_H

#define FWD_DCL
#   include "events/event.hpp"
#undef  FWD_DCL

#include "core/smart_pointers.hpp"

#include <vector>

class OnInput
{
public:
    OnInput() noexcept;
    virtual ~OnInput() noexcept;

    virtual void Input(InputEvent*) = 0;
};

class OnAppEvent
{
public:
    OnAppEvent() noexcept;
    virtual ~OnAppEvent() noexcept;

    virtual void Event(AppEvent*) = 0;
};

class OnEngineEvent
{
public:
    OnEngineEvent() noexcept;
    virtual ~OnEngineEvent() noexcept;

    virtual void Event(EngineEvent*) = 0;
};

class EventHandler
{
public:
    ~EventHandler() noexcept;

    static Unique<EventHandler>& Instance();

    void Add(OnInput* inInstancePtr);
    void Add(OnAppEvent* inInstancePtr);
    void Add(OnEngineEvent* inInstancePtr);
    void Erase(OnInput* inInstancePtr);
    void Erase(OnAppEvent* inInstancePtr);
    void Erase(OnEngineEvent* inInstancePtr);

    template<typename T>
        void HandleEvent(Shared<IEvent> inEvent);

    template<> void HandleEvent<AppEvent>(Shared<IEvent> inEvent);
    template<> void HandleEvent<EngineEvent>(Shared<IEvent> inEvent);
    template<> void HandleEvent<InputEvent>(Shared<IEvent> inEvent);

private:
    EventHandler() noexcept;

    static Unique<EventHandler> m_sInstance;

    std::vector<OnInput*>       mInputEventInstances{};
    std::vector<OnAppEvent*>    mAppEventInstances{};
    std::vector<OnEngineEvent*> mEngineEventInstances{};
};

#endif // EVENT_HANDLING_COMPONENTS_H

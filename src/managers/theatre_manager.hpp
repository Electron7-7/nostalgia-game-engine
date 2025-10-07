#ifndef THEATRE_MANAGER_H
#define THEATRE_MANAGER_H

#include "things/fwd.hpp"
#include "theatre/fwd.hpp"

#include "things/thing.hpp"
#include "manager.hpp"
#include "input/event.hpp"

#include <memory>
#include <map>
#include <glm/glm.hpp>

class TheatreManager : public Manager
{
public:
    void Update();
    void Tick();
    ManagerEnums::TheatreReturnValue_t TheatreInit(bool IsFirstCall);
    ManagerEnums::TheatreReturnValue_t TheatreShutdown(bool IsFirstCall);

    void RenderWorld();

    static TheatreData& GetData();
    static void DelegateInputEvent(const InputEvent& InputEvent);
    static ID CreateThing(const ThingData& ThingData);
    static std::shared_ptr<Thing> GetThing(ID ObjectID);
    static std::shared_ptr<NostalgiaPlayer> GetLocalPlayer();
    static bool DestroyThing(ID);
    static bool debug_GetThingAtIndex(unsigned int MapIndex, std::shared_ptr<Thing>& Output);

    template<IsThing T>
    static std::shared_ptr<T> GetThing(ID ObjectID);

private:
    static std::map<ID, std::shared_ptr<Thing>> m_sThings;

    static void CreateThings();
    static void DestroyThings();
    static bool s_DestroyThing(ID);
};

extern TheatreManager* g_pTheatreManager;

#endif // THEATRE_MANAGER_H

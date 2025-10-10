#ifndef THEATRE_MANAGER_H
#define THEATRE_MANAGER_H

#include "things/fwd.hpp"
#include "theatre_parser/fwd.hpp"

#include "things/thing_factory.hpp"
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

    static void LoadTheatreData(const TheatreData&);
    static bool LoadTheatreFromMemory(const std::string& Data);
    static bool LoadTheatreFromFile(const std::string& Path);

    static const TheatreData& GetInitialState();
    static TheatreData GetCurrentState();

    static void DelegateInputEvent(const InputEvent& InputEvent);
    static ID CreateThing(const ThingData& ThingData);
    static std::shared_ptr<NostalgiaPlayer> GetLocalPlayer();
    static bool DestroyThing(ID);

    static std::shared_ptr<Thing> GetThing(ID ObjectID)
    { return (m_sThings.contains(ObjectID)) ? m_sThings.at(ObjectID) : std::make_shared<Thing>(); }

    template<ThingDerived T>
    static std::shared_ptr<T> GetThing(ID ObjectID)
    {
        if(std::shared_ptr<T> thing = dynamic_pointer_cast<T>(TheatreManager::GetThing(ObjectID)))
            { return thing; }
        return std::make_shared<T>();
    }

    static bool debug_GetThingAtIndex(unsigned int MapIndex, std::shared_ptr<Thing>& Output);

private:
    static std::map<ID, std::shared_ptr<Thing>> m_sThings;

    static void CreateThings();
    static void DestroyThings();
};

extern TheatreManager* g_pTheatreManager;

#endif // THEATRE_MANAGER_H

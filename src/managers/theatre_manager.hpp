#ifndef THEATRE_MANAGER_H
#define THEATRE_MANAGER_H

#include "manager.hpp"
#include "time.hpp"
#include "things/thing.hpp"
#include "things/fwd.hpp"

#include <memory>

class TheatreManager : public Manager
{
public:
    void Update();
    void Tick();

    ManagerEnums::TheatreReturnValue_t TheatreInit(bool IsFirstCall);
    ManagerEnums::TheatreReturnValue_t TheatreShutdown(bool IsFirstCall);

    // FIXME: Refactor this to be more efficient
    template<IsThing T>
    static std::shared_ptr<T> GetThing(id_t id)
    {
        WAIT(s_AreThingsLocked, 10.0f);
        s_AreThingsLocked = true;

        if(!s_Things.contains(id))
        {
            PRINT_WARNING("TheatreManager::GetThing - Bad ID: '{}'", id);
            return std::make_shared<T>();
        }

        auto try_Downcast = std::dynamic_pointer_cast<T>(s_Things.at(id));
        if(!try_Downcast)
        {
            PRINT_WARNING("'Thing' downcast attempted with a non-'Thing' class")
            return std::make_shared<T>();
        }

        s_AreThingsLocked = false;
        return try_Downcast;
    }

    static NostalgiaPlayer* GetLocalPlayer();

    static bool try_DestroyThing(id_t ThingID);

#ifdef DEBUGGING
    static const std::map<id_t, std::shared_ptr<Thing>>& debug_GetThings();
#endif // DEBUGGING

private:
    static bool s_AreThingsLocked;
    static std::map<id_t, std::shared_ptr<Thing>> s_Things;

    static void CreateObjects();
    static void DestroyObjects();
};

extern TheatreManager* g_pTheatreManager;

#endif // THEATRE_MANAGER_H

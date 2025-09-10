#ifndef THEATRE_MANAGER_H
#define THEATRE_MANAGER_H

#include "manager.hpp"
#include "time.hpp"
#include "resources/resource.hpp"
#include "resources/fwd.hpp"
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
    template<IsResource T>
    static std::shared_ptr<T> GetResource(id_t id)
    {
        WAIT(s_AreResourcesLocked, 10.0f);
        s_AreResourcesLocked = true;

        if(!s_Resources.contains(id))
        {
            PRINT_WARNING("TheatreManager::GetResource - Bad ID: '{}'", id);
            return std::make_shared<T>();
        }

        auto try_Downcast = std::dynamic_pointer_cast<T>(s_Resources.at(id));
        if(!try_Downcast)
        {
            PRINT_WARNING("'Resource' downcast attempted with a non-'Resource' class")
            return std::make_shared<T>();
        }

        s_AreResourcesLocked = false;
        return try_Downcast;
    }

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

    static bool try_DestroyResource(id_t ResourceID);
    static bool try_DestroyThing(id_t ThingID);

#ifdef DEBUGGING
    static const std::map<id_t, std::shared_ptr<Thing>>& debug_GetThings();
    static const std::map<id_t, std::shared_ptr<Resource>>& debug_GetResources();
#endif // DEBUGGING

private:
    static bool s_AreResourcesLocked;
    static bool s_AreThingsLocked;
    static std::map<id_t, std::shared_ptr<Thing>>    s_Things;
    static std::map<id_t, std::shared_ptr<Resource>> s_Resources;

    static void CreateObjects();
    static void DestroyObjects();
};

extern TheatreManager* g_pTheatreManager;

#endif // THEATRE_MANAGER_H

#ifndef THEATRE_MANAGER_H
#define THEATRE_MANAGER_H

#include "manager.hpp"
#include "theatre/theatre_data.hpp"
#include "resources/resource.hpp"
#include "things/thing.hpp"
#include "resources/fwd.hpp"
#include "things/fwd.hpp"

#include <memory>

class TheatreManager : public Manager
{
public:
    void Update();
    void Tick();

    ManagerEnums::TheatreReturnValue_t TheatreInit(bool IsFirstCall);
    ManagerEnums::TheatreReturnValue_t TheatreShutdown(bool IsFirstCall);

    template<IsResource T> static std::shared_ptr<T> GetResource(rid_t id);
    template<IsThing T>    static std::shared_ptr<T> GetThing(tid_t id);

    static NostalgiaPlayer* GetLocalPlayer();

    static bool try_DestroyResource(rid_t ResourceID);
    static bool try_DestroyThing(tid_t ThingID);

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

    static id_t GetNewID();
    static void SetVariables(size_t, id_t, const data_t&);
};

extern TheatreManager* g_pTheatreManager;

#endif // THEATRE_MANAGER_H

#ifndef THEATRE_MANAGER_H
#define THEATRE_MANAGER_H

#include "manager.hpp"
#include "debug.hpp"
#include "things/fwd.hpp"
#include "theatre/fwd.hpp"

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

    static std::shared_ptr<NostalgiaPlayer> GetLocalPlayer();

    static std::shared_ptr<Thing>& GetThing(id_t ID);
    static const std::shared_ptr<Thing>& cGetThing(id_t ID);

    static id_t CreateThing(const data_t& ThingData);
    static bool DestroyThing(id_t ID);

private:
    DEBUG(friend class imgui_Debugger;)
    static std::map<id_t, std::shared_ptr<Thing>> s_Things;

    static void CreateThings();
    static void DestroyThings();
    static bool s_DestroyThing(id_t);
};

extern TheatreManager* g_pTheatreManager;

#endif // THEATRE_MANAGER_H

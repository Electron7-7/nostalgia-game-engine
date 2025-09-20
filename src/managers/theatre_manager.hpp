#ifndef THEATRE_MANAGER_H
#define THEATRE_MANAGER_H

#include "manager.hpp"
#include "debug.hpp"
#include "things/thing.hpp"
#include "things/fwd.hpp"
#include "rendering/render_command.hpp"

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

    static bool try_DestroyThing(id_t ThingID);
    static std::shared_ptr<NostalgiaPlayer> GetLocalPlayer();

    static std::shared_ptr<Thing>& GetThing(id_t ID);
    static const std::shared_ptr<Thing>& cGetThing(id_t ID);

    static int PointLightsCount();
    static int SpotLightsCount();
    static int DirectionalLightsCount();

private:
    DEBUG(friend class imgui_Debugger;)
    static std::map<id_t, std::shared_ptr<Thing>> s_Things;
    static int s_PointLightsCount;
    static int s_SpotLightsCount;
    static int s_DirectionalLightsCount;

    static void CreateObjects();
    static void DestroyObjects();
};

extern TheatreManager* g_pTheatreManager;

#endif // THEATRE_MANAGER_H

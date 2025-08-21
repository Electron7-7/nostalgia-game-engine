#ifndef THEATRE_MANAGER_H
#define THEATRE_MANAGER_H

#include "manager.hpp"
#include "theatre/theatre.hpp"
#include "rendering/mesh.hpp"
#include "rendering/render_command.hpp"
#include "things/actors/nostalgia_player.hpp"

#include <list>

class TheatreManager : public Manager
{
public:
    TheatreManager();
    ~TheatreManager();
    ManagerEnums::TheatreReturnValue_t TheatreInit(bool IsFirstCall);
    ManagerEnums::TheatreReturnValue_t TheatreShutdown(bool IsFirstCall);

private:
    void CreateThings();
    void DestroyThings();

    NostalgiaPlayer m_LocalPlayer;
};

extern TheatreManager* g_pTheatreManager;

#endif // THEATRE_MANAGER_H

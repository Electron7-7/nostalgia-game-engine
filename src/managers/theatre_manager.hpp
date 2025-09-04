#ifndef THEATRE_MANAGER_H
#define THEATRE_MANAGER_H

#include "manager.hpp"
#include "things/actors/nostalgia_player.hpp"

class TheatreManager : public Manager
{
public:
    void Update();
    void Tick();

    ManagerEnums::TheatreReturnValue_t TheatreInit(bool IsFirstCall);
    ManagerEnums::TheatreReturnValue_t TheatreShutdown(bool IsFirstCall);

private:
    void CreateThings();
    void DestroyThings();

    NostalgiaPlayer m_LocalPlayer;
};

extern TheatreManager* g_pTheatreManager;

#endif // THEATRE_MANAGER_H

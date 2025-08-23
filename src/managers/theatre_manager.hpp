#ifndef THEATRE_MANAGER_H
#define THEATRE_MANAGER_H

#include "manager.hpp"
#include "theatre/theatre.hpp"
#include "things/actors/nostalgia_player.hpp"

#include <string>

class TheatreManager : public Manager
{
public:
    TheatreManager();
    ~TheatreManager();
    ManagerEnums::TheatreReturnValue_t TheatreInit(bool IsFirstCall);
    ManagerEnums::TheatreReturnValue_t TheatreShutdown(bool IsFirstCall);

    bool try_LoadTheatre(const std::string& TheatreFile);
    void LoadTheatreFromMemory(const Theatre& Theatre);

private:
    void CreateThings();
    void DestroyThings();

    NostalgiaPlayer m_LocalPlayer;
    Theatre m_CurrentTheatre;
};

extern TheatreManager* g_pTheatreManager;

#endif // THEATRE_MANAGER_H

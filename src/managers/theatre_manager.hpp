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

    void LoadTheatre(const Theatre& Theatre);
    bool try_LoadTheatre(const std::string& TheatreFile);
    Theatre* GetCurrentTheatre();

private:
    void CreateThings();
    void DestroyThings();

    NostalgiaPlayer m_LocalPlayer;
};

extern TheatreManager* g_pTheatreManager;

#endif // THEATRE_MANAGER_H

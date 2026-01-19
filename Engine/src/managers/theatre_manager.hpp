#ifndef THEATRE_MANAGER_H
#define THEATRE_MANAGER_H

#include "fwd/filesystem.hpp"
#include "managers/manager.hpp"
#include "theatre/theatre.hpp"
#include "core/smart_pointers.hpp"
#include "core/mutex.hpp"
#include "core/farg.hpp"

class TheatreManager : public Manager
{
public:
    consteval const char* DebugName() override { return "TheatreManager"; }
    bool Init() override;
    ManagerEnums::TheatreReturnValue_t TheatreInit(bool IsFirstCall) override;
    ManagerEnums::TheatreReturnValue_t TheatreShutdown(bool IsFirstCall) override;
    void Update() override;
    void Tick() override;
    void Input(InputEvent*) override;

    Theatre*       CurrentTheatre();
    const Theatre* CurrentTheatre() const;

    void DrawCurrentTheatre();
    void LoadNewTheatre(Unique<Theatre> inTheatre);
    bool LoadNewTheatre(Farg<FileData> inTheatreFileData);
    bool LoadNewTheatre(Sarg inTheatreFilePath);

private:
    RMutex mTheatreMutex{};
    Unique<Theatre> mCurrentTheatre{nullptr};
};

extern TheatreManager* g_pTheatreManager;

#endif // THEATRE_MANAGER_H

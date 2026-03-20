#ifndef THEATRE_MANAGER_H
#define THEATRE_MANAGER_H

#include <Nostalgia/managers/manager.hpp>
#include <Nostalgia/theatre/theatre.hpp>

class TheatreManager : public Manager
{
public:
    Theatre* Current();
    void     LoadFromData(Shared<TheatreFile::TheatreData>);
    void     LoadFromFileData(Shared<FileData> inTheatreFileData);
    void     LoadFromFile(Sarg inTheatreFilePath);

    consteval const char* DebugName() override { return "TheatreManager"; }
    bool Init() override;
    ManagerEnums::TheatreReturnValue_t TheatreInit(bool IsFirstCall) override;
    ManagerEnums::TheatreReturnValue_t TheatreShutdown(bool IsFirstCall) override;
    void Update() override;
    void Tick() override;
    void Input(InputEvent*) override;

    void DrawCurrentTheatre();

private:
    RMutex mTheatreMutex{};
    Unique<Theatre> m_pCurrentTheatre{MakeUnique<Theatre>()};
};

extern TheatreManager* g_pTheatreManager;

#endif // THEATRE_MANAGER_H

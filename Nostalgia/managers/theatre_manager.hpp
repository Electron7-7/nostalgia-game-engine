#ifndef THEATRE_MANAGER_H
#define THEATRE_MANAGER_H

#include <Nostalgia/managers/manager.hpp>
#include <Nostalgia/theatre/theatre.hpp>

class TheatreManager : public Manager
{
public:
    template<typename T> requires std::derived_from<T, Theatre>
        void SetNextTheatreType()
        {
            LockGuard<RMutex> _theatre_lock{mTheatreMutex};
            m_spNewTheatreType = MakeUnique<T>();
        }

    Theatre* Current();
    Farg<Unique<Theatre>> Current() const;
    void LoadFromData(Farg<TheatreFile::TheatreData>);
    void LoadFromFileData(Farg<FileData> inTheatreFileData);
    void LoadFromFile(Sarg inTheatreFilePath);

    constexpr const char* DebugName() override { return "TheatreManager"; }
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
    inline static Unique<Theatre> m_spNewTheatreType{nullptr};
};

extern TheatreManager* g_pTheatreManager;

#endif // THEATRE_MANAGER_H

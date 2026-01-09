#ifndef THEATRE_MANAGER_H
#define THEATRE_MANAGER_H

#include "fwd/theatre.hpp"
#include "managers/manager.hpp"
#include "core/id.hpp"
#include "core/error.hpp"
#include "core/mutex.hpp"
#include "core/smart_pointers.hpp"
#include <unordered_map>

using things_t = std::unordered_map<ID,Shared<Thing>>;

class TheatreManager : public Manager
{
public:
    consteval const char* DebugName() { return "TheatreManager"; }
    bool Init();
    ManagerEnums::TheatreReturnValue_t TheatreInit(bool IsFirstCall);
    ManagerEnums::TheatreReturnValue_t TheatreShutdown(bool IsFirstCall);
    void Update();
    void Tick();
    void Input(InputEvent*);

    void LoadTheatreData(Farg<TheatreData>);
    bool LoadTheatreFromMemory(Sarg Data);
    bool LoadTheatreFromFile(Sarg Path);

    void ClearViewports();
    void DrawTheatre();

    const TheatreData& GetInitialState();
    TheatreData GetCurrentState();
    IdVec_t GetThingIDs();
    IdSet_arg GetViewportIDs();
    IdVec_t GetViewportIDList();

    bool ThingExists(ID UID);
    Farg<PID> GetType(ID ObjectID);
    Error ChangeThingID(ID inOldID, ID inNewID);
    uint CreateThing(Farg<ThingData> ThingData);
    bool DestroyThing(ID);

    Shared<Thing> GetThing(ID ObjectID);
    Shared<Resource> GetResource(ID ObjectID);
    Shared<Thinker> GetThinker(ID ObjectID);

    template<typename T> requires std::derived_from<T,Thinker>
        Shared<T> GetThinker(ID ObjectID)
        {
            if(auto thinker{DCast<T>(GetThinker(ObjectID))})
                { return thinker; }
            return MakeShared<T>();
        }

    template<typename T> requires std::derived_from<T,Resource>
        Shared<T> GetResource(ID ObjectID)
        {
            if(auto resource{DCast<T>(GetResource(ObjectID))})
                { return resource; }
            return MakeShared<T>();
        }

private:
    things_t mThings{};
    RMutex mThingsMutex{};
    IdSet_t mLightIDs{},
        mVisual3DIDs{},
        mVisual2DIDs{},
        mViewportIDs{};

    void CreateThings();
    void DestroyThings();
    things_t::iterator DestroyThing(things_t::iterator);
    uint CreateThingNoReady(Farg<ThingData>);
    void CreateEmbeddedResources();
    void DrawActor(Shared<Actor3D>, Shared<Camera3D>);
    // void DrawActor(Shared<Actor2D>, Shared<Camera2D>);
};

extern bool gPrintLoadedTheatreData;

extern TheatreManager* g_pTheatreManager;

#endif // THEATRE_MANAGER_H

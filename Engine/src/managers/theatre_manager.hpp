#ifndef THEATRE_MANAGER_H
#define THEATRE_MANAGER_H

#include "managers/manager.hpp"
#include "fwd/things.hpp"
#include "fwd/theatre.hpp"
#include "core/id.hpp"
#include "core/error.hpp"
#include "core/smart_pointers.hpp"
#include <unordered_map>
#include <unordered_set>
#include <string>
#include <mutex>

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
    bool LoadTheatreFromMemory(Farg<std::string> Data);
    bool LoadTheatreFromFile(Farg<std::string> Path);

    bool IsCurrentCamera(ID UID) const;
    Error SetCurrentCamera(ID UID);
    void UnsetCurrentCamera(ID UID);
    ID GetCurrentCamera() const;

    void DrawTheatre();
    bool ThingExists(ID UID);
    Farg<TTID> GetType(ID ObjectID);

    const TheatreData& GetInitialState();
    TheatreData GetCurrentState();
    std::vector<ID> GetThingIDs();
    Farg<std::unordered_set<ID>> GetCameraIDs();
    Shared<NostalgiaPlayer> GetPlayer();

    Error ChangeThingID(ID inOldID, ID inNewID);
    uint CreateThing(Farg<ThingData> ThingData);
    bool DestroyThing(ID);

    Shared<Thing> GetThing(ID ObjectID);

    template<typename T> requires std::derived_from<T,Thing>
        Shared<T> GetThing(ID ObjectID)
        {
            if(auto thing{DCast<T>(GetThing(ObjectID))})
                { return thing; }
            return MakeShared<T>();
        }

    // More efficient when breaking on an invalid UID is preferred. See `PhysicsManager::CreateBody` for an example.
    template<typename T> requires std::derived_from<T,Thing>
        Error GetThing(ID ObjectID, Shared<T>& Output)
        {
            if(auto thing{DCast<T>(GetThing(ObjectID))})
            {
                Output = thing;
                return OK;
            }
            return (ThingExists(ObjectID))
                ? ERR_INVALID_ID
                : ERR_INVALID_TYPE;
        }

private:
    things_t mThings{};
    std::recursive_mutex mThingsMutex{};
    std::unordered_set<ID> mActiveCameras{};
    ID mCurrentCamera{};

    void CreateThings();
    void DestroyThings();
    things_t::iterator DestroyThing(things_t::iterator);
    uint CreateThingNoReady(Farg<ThingData>);
    void CreateEmbeddedResources();
    void DrawActor(Shared<Actor>, Shared<Camera3D>);
};

extern bool gPrintLoadedTheatreData;

extern TheatreManager* g_pTheatreManager;

#endif // THEATRE_MANAGER_H

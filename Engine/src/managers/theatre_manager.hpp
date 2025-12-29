#ifndef THEATRE_MANAGER_H
#define THEATRE_MANAGER_H

#include "managers/manager.hpp"
#include "fwd/things.hpp"
#include "fwd/theatre.hpp"
#include "fwd/rendering.hpp"
#include "fwd/filesystem.hpp"
#include "core/id.hpp"
#include "core/error.hpp"
#include "rendering/mesh_buffers.hpp"
#include <unordered_map>
#include <string>
#include <mutex>

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

    void DrawTheatre();
    void ReadyThings();
    bool ThingExists(ID UID);
    Farg<TTID> GetType(ID ObjectID);

    void LoadTheatreData(Farg<TheatreData>);
    bool LoadTheatreFromMemory(Farg<std::string> Data);
    bool LoadTheatreFromFile(Farg<std::string> Path);

    const TheatreData& GetInitialState();
    TheatreData GetCurrentState();
    std::vector<ID> GetThingIDs();

    Error ChangeThingID(ID inOldID, ID inNewID);
    uint CreateThing(Farg<ThingData> ThingData);
    Shared<NostalgiaPlayer> GetLocalPlayer();
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
    std::unordered_map<ID, Shared<Thing>> mThings{};
    std::unordered_map<ID, Shared<VertexArray>> mTheatreVAOs{};
    std::unordered_map<ID, Shared<TextureBuffer>> mTheatreTextures{};

    std::recursive_mutex mThingsMutex{};

    void CreateThings();
    void DestroyThings();

    Error BufferMesh(Farg<FileData> inData, ID inID, Farg<IBuffer::Layout> inLayout);
    Error BufferTexture(Farg<FileData> inData, ID inID, Farg<TextureFormat> inFormat);
    void  BufferEmbeddedResources();

    Shared<TextureBuffer>& GetTextureBuffer(ID inID);
    Shared<VertexArray>& GetVertexArray(ID inID);
};

inline bool gPrintLoadedTheatreData{false};

extern TheatreManager* g_pTheatreManager;

#endif // THEATRE_MANAGER_H

#ifndef THEATRE_H
#define THEATRE_H

#include "fwd/theatre.hpp"
#include "fwd/filesystem.hpp"
#include "components/event_handling.hpp"
#include "components/game_loop.hpp"
#include "core/id.hpp"
#include "core/error.hpp"
#include "core/mutex.hpp"
#include "core/smart_pointers.hpp"
#include <unordered_map>

class Theatre : public OnUpdate, public OnTick, public OnInput
{
public:
    using Things_t = std::unordered_map<ID, Shared<Thing>>;

    bool mDoPrintDebugLogs{false};

    Theatre(Farg<TheatreData>) noexcept;
    Theatre(Farg<FileData> inTheatreFileData) noexcept;
    Theatre(Sarg inTheatreFilePath) noexcept;

    virtual ~Theatre() noexcept;

    virtual void Update() override;
    virtual void Tick() override;
    virtual void Input(InputEvent*) override;

    virtual bool Startup();
    virtual bool Shutdown();
    virtual void Draw();

    Error             InitStatus()   const;
    bool              IsStarted()    const;
    Farg<TheatreData> InitialState() const;
    TheatreData       CurrentState();

    IdVec_t   ThingIDs();
    IdSet_arg ViewportIDs();
    bool      ThingExists(ID);
    Farg<PID> TypeOf(ID);
    Error     ChangeThingID(ID inOldID, ID inNewID);
    ID        CreateThing(Farg<ThingData>);
    Error     DestroyThing(ID);

    Shared<Thing>    GetThing(ID ObjectID);
    Shared<Resource> GetResource(ID ObjectID);
    Shared<Thinker>  GetThinker(ID ObjectID);

    template<typename T> requires std::derived_from<T,Resource>
        Shared<T> GetResource(ID ObjectID)
        {
            if(auto resource{DCast<T>(GetResource(ObjectID))})
                { return resource; }
            return MakeShared<T>();
        }

    template<typename T> requires std::derived_from<T,Thinker>
        Shared<T> GetThinker(ID ObjectID)
        {
            if(auto thinker{DCast<T>(GetThinker(ObjectID))})
                { return thinker; }
            return MakeShared<T>();
        }

private:
    RMutex   mThingsMutex{};
    Things_t mThings{};
    IdSet_t  mLightIDs{},
             mVisual3DIDs{},
             mVisual2DIDs{},
             mViewportIDs{};

    Unique<TheatreData> mInitialState{nullptr};
    Error mInitStatus{ERR_UNINITIALIZED};

    bool mIsStarted{false};

    void CreateEmbeddedResources();
    ID   CreateThingNoReady(Farg<ThingData>);
    Things_t::iterator DestroyThing(Things_t::iterator);

    void Draw3DThinkers(Shared<Camera3D>);
    // void Draw2DThinkers(Shared<Camera2D>);
};

#endif // THEATRE_H

#ifndef THEATRE_H
#define THEATRE_H

#include "fwd/filesystem.hpp"
#include "fwd/theatre.hpp"
#include "theatre/variable_registry.hpp"
#include "theatre/call_sheet.hpp"
#include "components/event_handling.hpp"
#include "components/game_loop.hpp"
#include "core/uid.hpp"
#include "core/error.hpp"
#include "core/mutex.hpp"
#include "core/smart_pointers.hpp"
#include <unordered_map>

class Theatre : public OnUpdate, public OnTick, public OnInput
{
public:
    using Things_t = std::unordered_map<ID, Shared<Thing>>;

    bool mDoPrintDebugLogs{false};

    Theatre(Shared<TheatreFile::TheatreData>) noexcept;
    Theatre(Farg<FileData> inTheatreFileData) noexcept;

    virtual ~Theatre() noexcept;

    virtual void Update() override;
    virtual void Tick() override;
    virtual void Input(InputEvent*) override;

    virtual bool Startup();
    virtual bool Shutdown();
    virtual void Draw();

    Sarg Name() const;
    uint Index() const;

    Error InitStatus() const;
    bool  IsStarted()  const;
    Farg<VariableRegistry>         Registry() const;
    Farg<TheatreFile::TheatreData> InitialState() const;
    TheatreFile::TheatreData       CurrentState();

    IdVec_t   ThingIDs();
    bool      ThingExists(ID);
    Farg<PID> TypeOf(ID);
    bool      DerivedFrom(ID, FPID);
    Error     ChangeThingID(ID inOldID, ID inNewID);
    ID        CreateThing(Farg<TheatreFile::ThingData>);
    Error     DestroyThing(ID);

    IdSet_arg GetCameras();
    IdSet_arg GetViewports();

    IdSet_t GetChildren(ID inParentID);
    ID GetParent(ID inChildID);
    IdSet_t GetAllChildren(ID inParentID);
    IdSet_t GetAllParents(ID inChildID);

    Error SetParent(ID inChildID, ID inParentID);
    Error DropParent(ID inChildID);

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

protected:
    std::string mName{"Untitled Theatre"};
    uint mIndex{ID::Invalid};

    RMutex   mThingsMutex{},
             mCallSheetMutex{};
    Things_t mThings{};
    IdSet_t  mLightIDs{},
             mCameraIDs{},
             mVisual3DIDs{},
             mVisual2DIDs{},
             mViewportIDs{};
    CallSheet mCallSheet{};
    UID mUIDs{};

    Shared<VariableRegistry> m_pRegistry{nullptr};
    Shared<TheatreFile::TheatreData> m_pInitialState{nullptr};

    Error mInitStatus{ERR_UNINITIALIZED};
    bool mIsStarted{false};

    void SetupOwnership(Farg<TheatreFile::ThingData>);
    void SetupUID(TheatreFile::ThingData&);

    void  CreateEmbeddedResources();
    ID    CreateThingNoReady(Farg<TheatreFile::ThingData>);
    ID    CreateThingNoReady(TheatreFile::ThingData&);
    Error DestroyThingOnly(ID);

    void Draw3DThinkers(ID, Shared<Camera3D>);
    // void Draw2DThinkers(ID, Shared<Camera2D>);
};

#endif // THEATRE_H

#ifndef THEATRE_H
#define THEATRE_H

#include <Nostalgia/fwd/things.hpp>
#include <Nostalgia/things/thinkers/thinker.hpp>
#include <Nostalgia/things/resources/resource.hpp>
#include <Nostalgia/theatre/theatre_file.hpp>
#include <Nostalgia/theatre/call_sheet.hpp>

class Theatre
{
public:
    using Things_t = std::unordered_map<ID, Shared<Thing>>;
    using Names_t = std::map<std::string, ID>;

    enum FileOverwriteAction { OVERWRITE, RENAME, CANCEL };

    static Theatre* Current();

    explicit Theatre() noexcept;
    virtual ~Theatre() noexcept;

    virtual void Update();
    virtual void Tick();
    virtual void Input(InputEvent*);

    virtual void  LoadTheatreData(Shared<TheatreFile::TheatreData> inTheatreData);
    virtual Error LoadData(Farg<FileData> inData);
    virtual Error LoadFile(std::string inFilePath);
    virtual Error Save(Sarg inOutputFilePath, FileOverwriteAction = RENAME);
    virtual bool  Startup();
    virtual bool  Shutdown();
    virtual void  Draw();

    Sarg Name() const;
    uint Index() const;
    bool WasLoadedFromFile() const;
    Sarg TheatreFileDirectory() const;

    Error InitStatus() const;
    bool  IsStarted()  const;
    Farg<TheatreFile::TheatreData> InitialState() const;
    TheatreFile::TheatreData       CurrentState();

    ID    GetCurrentCamera2D(ID inViewportID = {});
    ID    GetCurrentCamera3D(ID inViewportID = {});
    Error SetCurrentCamera(ID inCameraID, ID inViewportID = {});

    IdVec_t   ThingUIDs();
    IdVec_t   ThinkersWithNoParents();
    IdSet_arg ThinkerUIDs();
    IdSet_arg ResourceUIDs();

    bool    ThingExists(ID);
    bool    ThingExists(Sarg inName);
    FPID    TypeOf(ID);
    bool    DerivedFrom(ID, FPID);
    ID      CreateThing(Farg<TheatreFile::ThingData>, bool inDoReadyThing = true);
    Error   DestroyThing(ID);
    ID      GetUID(Sarg inName);
    Sarg    GetName(ID);
    Error   SetName(ID inUID, Sarg inNewName);
    Error   SetName(Sarg inOldName, Sarg inNewName);

    Shared<Thinker> GetPlayer();
    IdSet_arg GetViewports();

    IdSet_t GetChildren(ID inParentID);
    ID GetParent(ID inChildID);
    IdSet_t GetAllChildren(ID inParentID);
    IdSet_t GetAllParents(ID inChildID);

    Error SetParent(ID inChildID, ID inParentID);
    Error DropParent(ID inChildID);

    Shared<Thing>    GetThing(Sarg ThingName);
    Shared<Thing>    GetThing(ID ObjectID);
    Shared<Resource> GetResource(ID ObjectID);
    Shared<Thinker>  GetThinker(ID ObjectID);

    const LockGuard<RMutex> GetThingsLock();
    const LockGuard<RMutex> GetCallSheetLock();

    template<typename T> requires std::derived_from<T, NostalgiaPlayer>
        Shared<T> GetPlayer()
        {
            if(auto player{DCast<T>(m_pPlayer)})
                { return player; }
            return MakeShared<T>();
        }

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
    std::string mTheatreFileDirectory{""};
    bool mIsStarted{false},
        mWasLoadedFromFile{false};
    Error mInitStatus{ERR_UNINITIALIZED};

    RMutex mThingsMutex{},
        mCallSheetMutex{};
    Things_t mThings{};
    Names_t mNames{};
    ID mRootViewportCurrentCamera3D{},
        mRootViewportCurrentCamera2D{};
    IdSet_t mThinkerUIDs{}, mResourceUIDs{},
        mLightIDs{},
        mVisual3DIDs{},
        mVisual2DIDs{},
        mViewportIDs{};
    CallSheet mCallSheet{};

    Shared<Thinker> m_pPlayer{nullptr};
    Shared<TheatreFile::TheatreData> m_pInitialState{MakeShared<TheatreFile::TheatreData>()};

    void UpdateCallsheet(ID, Farg<TheatreFile::ThingData>);
    void UpdateIdSetsAndSpecialThings(FPID, ID);
    Error DestroyThingOnly(ID);
    void Draw3DThinkers(Shared<Camera3D>);
    void Draw2DThinkers(Shared<Camera2D>);

    static Error SetThingName(Sarg,Sarg);
};

#endif // THEATRE_H

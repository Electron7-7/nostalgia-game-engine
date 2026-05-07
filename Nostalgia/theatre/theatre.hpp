#ifndef THEATRE_H
#define THEATRE_H

#include <Nostalgia/fwd/things.hpp>
#include <Nostalgia/things/thinkers/thinker.hpp>
#include <Nostalgia/things/resources/resource.hpp>
#include <Nostalgia/theatre/theatre_file.hpp>

class Theatre
{
public:
    using Things_t = std::unordered_map<ID, Shared<Thing>>;
    using Names_t = std::map<std::string, ID>;

    static Theatre* Current();

    Theatre() noexcept;
    virtual ~Theatre() noexcept;

    template<typename T> requires std::derived_from<T, Theatre>
        void Copy(Farg<T> inTheatre) noexcept
        {
            LockGuard<RMutex> _things_lock{mThingsMutex};
            LockGuard<RMutex> _callsheet_lock{mCallSheetMutex};
            mName = inTheatre.mName;
            mIndex = inTheatre.mIndex;
            mIsStarted = inTheatre.mIsStarted;
            mWasLoadedFromFile = inTheatre.mWasLoadedFromFile;
            mInitStatus = inTheatre.mInitStatus;
            mThings = inTheatre.mThings;
            mNames = inTheatre.mNames;
            mRootViewportCurrentCamera3D = inTheatre.mRootViewportCurrentCamera3D;
            mRootViewportCurrentCamera2D = inTheatre.mRootViewportCurrentCamera2D;
            mThinkerUIDs = inTheatre.mThinkerUIDs;
            mResourceUIDs = inTheatre.mResourceUIDs;
            mLightIDs = inTheatre.mLightIDs;
            mVisual3DIDs = inTheatre.mVisual3DIDs;
            mVisual2DIDs = inTheatre.mVisual2DIDs;
            mViewportIDs = inTheatre.mViewportIDs;
            mCallSheet = inTheatre.mCallSheet;
            m_pPlayer = inTheatre.m_pPlayer;
            m_pInitialState = inTheatre.m_pInitialState;
        }

    virtual void Update();
    virtual void Tick();
    virtual void Input(InputEvent*);

    virtual Shared<Image> TakeScreenshot(ID inViewportUID = ID::Invalid);

    virtual void  LoadTheatreData(Farg<TheatreFile::TheatreData> inTheatreData);
    virtual Error LoadData(Farg<FileData> inData);
    virtual Error LoadFile(std::string inFilePath);
    virtual std::string GetSaveData();
    virtual Error SaveToFile(Sarg inOutputFilePath, FileSystem::OverwriteAction = FileSystem::RENAME);
    virtual bool  Startup();
    virtual bool  Shutdown();
    virtual void  Draw();

    Sarg Name() const;
    uint Index() const;
    bool WasLoadedFromFile() const;
    Sarg TheatreFilePath() const;
    std::string TheatreFileDirectory() const;
    std::string TheatreFileName() const;
    std::string InheritanceLog() const;

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
    std::unordered_set<ID> GetAllChildren(ID inParentID);
    std::unordered_set<ID> GetAllParents(ID inChildID);

    Error SetParent(ID inChildID, ID inParentID);
    Error DropParent(ID inChildID);

    Shared<Thing>    GetThing(Sarg ThingName);
    Shared<Thing>    GetThing(ID ObjectID);
    Shared<Resource> GetResource(ID ObjectID);
    Shared<Thinker>  GetThinker(ID ObjectID);

    const LockGuard<RMutex> GetThingsLock();
    const LockGuard<RMutex> GetCallSheetLock();

    template<Thing_t T>
        Shared<T> GetThing(Sarg inName)
        {
            if(auto _thing{DCast<T>(GetThing(inName))})
                { return _thing; }
            return MakeShared<T>();
        }

    template<Thing_t T>
        Shared<T> GetThing(ID inUID)
        {
            if(auto _thing{DCast<T>(GetThing(inUID))})
                { return _thing; }
            return MakeShared<T>();
        }

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
    Tree<ID> mCallSheet{};

    Shared<Thinker> m_pPlayer{nullptr};
    Shared<TheatreFile::TheatreData> m_pInitialState{MakeShared<TheatreFile::TheatreData>()};

    static Error SetThingName(Sarg,Sarg);

    virtual void  UpdateCallsheet(ID, Farg<TheatreFile::ThingData>);
    virtual void  UpdateIdSetsAndSpecialThings(FPID, ID);
    virtual Error DestroyThingOnly(ID);
};

#endif // THEATRE_H

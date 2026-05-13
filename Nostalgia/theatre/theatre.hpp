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
    using Names_t  = std::unordered_map<std::string, ID>;

    static Theatre* Current();

    Theatre() noexcept;
    virtual ~Theatre() noexcept;

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
    IdVec_t   SortedThingUIDs(int& outBeginResourcesIndex);
    IdVec_t   ThinkersWithNoParents();
    IdSet_arg ThinkerUIDs();
    IdSet_arg ResourceUIDs();

    bool    Contains(ID);
    bool    Contains(Sarg inName);
    ID      CreateThing(Farg<TheatreFile::ThingData>, bool inDoReadyThing = true);
    Error   DestroyThing(ID);

    Shared<Thinker> GetPlayer();

    IdSet_t GetChildren(ID inParentID);
    ID GetParent(ID inChildID);
    std::unordered_set<ID> GetAllChildren(ID inParentID);
    std::unordered_set<ID> GetAllParents(ID inChildID);

    Error SetParent(ID inChildID, ID inParentID);
    Error DropParent(ID inChildID);

    const LockGuard<RMutex> GetThingsLock();
    const LockGuard<RMutex> GetCallSheetLock();

    template<typename T> requires std::derived_from<T, NostalgiaPlayer>
        Shared<T> GetPlayer()
        {
            if(auto player{DCast<T>(m_pPlayer)})
                { return player; }
            return MakeShared<T>();
        }

protected:
    friend struct ThingFactory;

    std::string mName{"Untitled_Theatre"};
    uint mIndex{ID::Invalid};
    bool mIsStarted{false},
        mWasLoadedFromFile{false};
    Error mInitStatus{ERR_UNINITIALIZED};

    RMutex mThingsMutex{},
        mCallSheetMutex{};
    ID mRootViewportCurrentCamera3D{},
        mRootViewportCurrentCamera2D{};
    IdSet_t mThingUIDs{}, mThinkerUIDs{}, mResourceUIDs{},
        mLightIDs{},
        mVisual3DIDs{},
        mVisual2DIDs{};
    std::unordered_set<Shared<Viewport>> mViewports{};
    Tree<ID> mCallSheet{};

    Shared<Thinker> m_pPlayer{nullptr};
    Shared<TheatreFile::TheatreData> m_pInitialState{MakeShared<TheatreFile::TheatreData>()};

    virtual void  UpdateCallsheet(ID, Farg<TheatreFile::ThingData>);
    virtual void  UpdateIdSetsAndSpecialThings(FPID, ID);
    virtual Error DestroyThingOnly(ID);
};

#endif // THEATRE_H

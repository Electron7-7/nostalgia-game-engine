#ifndef THEATRE_H
#define THEATRE_H

#include <Nostalgia/fwd/things.hpp>
#include <Nostalgia/things/thinkers/thinker.hpp>
#include <Nostalgia/things/resources/resource.hpp>
#include <Nostalgia/theatre/theatre_file.hpp>
#include <Nostalgia/theatre/call_sheet.hpp>
#include <Nostalgia/theatre/variable_registry.hpp>

class Theatre
{
public:
    enum FileOverwriteAction { OVERWRITE, RENAME, CANCEL };

    using Things_t = std::unordered_map<ID, Shared<Thing>>;

    static Theatre* Current();

    explicit Theatre() noexcept;
    virtual ~Theatre() noexcept;

    virtual void Update();
    virtual void Tick();
    virtual void Input(InputEvent*);

    virtual void  LoadTheatreData(Shared<TheatreFile::TheatreData> inTheatreData);
    virtual Error LoadData(Farg<FileData> inData);
    virtual Error LoadFile(Sarg inFilePath);
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

    IdVec_t ThingIDs();
    bool    ThingExists(ID);
    bool    ThingExists(Sarg inName);
    FPID    TypeOf(ID);
    bool    DerivedFrom(ID, FPID);
    ID      CreateThing(Farg<TheatreFile::ThingData>);
    Error   DestroyThing(ID);
    ID      GetUID(Sarg inName);
    Sarg    GetName(ID);
    Error   SetName(ID inUID, Sarg inNewName);
    Error   SetName(Sarg inOldName, Sarg inNewName);

    Shared<Viewport> GetRootViewport();
    IdSet_arg GetViewports(); // Does not include the root viewport
    IdSet_arg Get3DCameras();
    IdSet_arg Get2DCameras();

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

    template<typename T> requires std::derived_from<T,Resource>
        Shared<T> GetResource(ID ObjectID)
        {
            if(auto resource{DCast<T>(GetResource(ObjectID))})
                { return resource; }
            auto output{MakeShared<T>()};
            return output;
        }

    template<typename T> requires std::derived_from<T,Thinker>
        Shared<T> GetThinker(ID ObjectID)
        {
            if(auto thinker{DCast<T>(GetThinker(ObjectID))})
                { return thinker; }
            auto output{MakeShared<T>()};
            return output;
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
    std::map<std::string, ID> mNames{};
    IdSet_t mLightIDs{},
        mCamera3DIDs{},
        mCamera2DIDs{},
        mVisual3DIDs{},
        mVisual2DIDs{},
        mViewportIDs{};
    CallSheet mCallSheet{};

    Shared<Viewport> m_pRootViewport{nullptr};
    Shared<TheatreFile::TheatreData> m_pInitialState{nullptr};

    bool  LoadCurrentTheatreData();
    void SetupOwnership(TheatreFile::ThingData&, bool isStartup = false);
    void SetupUID(TheatreFile::ThingData&);

    void  CreateEmbeddedResources();
    ID    CreateThingNoReady(Farg<TheatreFile::ThingData>, bool doSetup = true);
    ID    CreateThingNoReady(TheatreFile::ThingData&, bool doSetup = true);
    Error DestroyThingOnly(ID);

    void Draw3DThinkers(Shared<Viewport>);
    void Draw2DThinkers(Shared<Viewport>);
};

#endif // THEATRE_H

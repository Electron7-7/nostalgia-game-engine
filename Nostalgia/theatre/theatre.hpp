#ifndef THEATRE_H
#define THEATRE_H

#include <Nostalgia/fwd/theatre.hpp>
#include <Nostalgia/fwd/things.hpp>
#include <Nostalgia/theatre/call_sheet.hpp>
#include <Nostalgia/theatre/variable_registry.hpp>

class Theatre
{
public:
    enum FileOverwriteAction { OVERWRITE, RENAME, CANCEL };

    using Things_t = std::unordered_map<ID, Shared<Thing>>;

    bool mDoPrintDebugLogs{false};

    Theatre() noexcept;
    Theatre(Shared<TheatreFile::TheatreData>) noexcept;
    Theatre(Farg<FileData> inTheatreFileData) noexcept;
    Theatre(Sarg inTheatreFilePath) noexcept;

    virtual ~Theatre() noexcept;

    virtual void Update();
    virtual void Tick();
    virtual void Input(InputEvent*);

    virtual Error Load(Farg<FileData> inTheatreFileData);
    virtual Error Load(Sarg inTheatreFilePath);
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
    Farg<VariableRegistry>         Registry() const;
    Farg<TheatreFile::TheatreData> InitialState() const;
    TheatreFile::TheatreData       CurrentState();

    IdVec_t   ThingIDs();
    bool      ThingExists(ID);
    bool      ThingExists(Sarg inName);
    FPID      TypeOf(ID);
    bool      DerivedFrom(ID, FPID);
    ID        CreateThing(Farg<TheatreFile::ThingData>);
    Error     DestroyThing(ID);

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

    /// Note: This will return the *first* `Thing` that has the name `ThingName`.
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
            output->m_pRootTheatre = this;
            return output;
        }

    template<typename T> requires std::derived_from<T,Thinker>
        Shared<T> GetThinker(ID ObjectID)
        {
            if(auto thinker{DCast<T>(GetThinker(ObjectID))})
                { return thinker; }
            auto output{MakeShared<T>()};
            output->m_pRootTheatre = this;
            return output;
        }

protected:
    std::string mName{"Untitled Theatre"};
    uint mIndex{ID::Invalid};
    std::string mTheatreFileDirectory{""};
    bool mWasLoadedFromFile{false};

    Mutex mCallSheetMutex{};
    RMutex mThingsMutex{};
    Things_t mThings{};
    IdSet_t mLightIDs{},
        mCamera3DIDs{},
        mCamera2DIDs{},
        mVisual3DIDs{},
        mVisual2DIDs{},
        mViewportIDs{};
    CallSheet mCallSheet{};
    UID mUIDs{};

    Shared<Viewport> m_pRootViewport{nullptr};
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

    void Draw3DThinkers(Shared<Viewport>);
    void Draw2DThinkers(Shared<Viewport>);
};

#endif // THEATRE_H

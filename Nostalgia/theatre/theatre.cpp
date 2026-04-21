#include "./theatre.hpp"
#include "application/application.hpp"
#include "managers/theatre_manager.hpp"
#include "things/thing_data.hpp"
#include "things/resource_database.hpp"
#include "things/thing_factory.hpp"
#include "things/thinkers/2d/camera_2d.hpp"
#include "things/thinkers/3d/camera_3d.hpp"
#include "things/thinkers/3d/light_3d.hpp"
#include "things/thinkers/viewport.hpp"
#include "managers/render_manager.hpp"
#include "settings/graphics.hpp"
#include "console/console.hpp"

#define LOCK_THINGS LockGuard<RMutex> things_lock{mThingsMutex}
#define LOCK_CALLSHEET LockGuard<RMutex> callsheet_lock{mCallSheetMutex}
#define FOUND_IT(MAP, KEY, ...) auto found_it##__VA_ARGS__{MAP.find(KEY)}; found_it##__VA_ARGS__ != MAP.end()

using namespace TheatreFile;

Theatre::Theatre() noexcept = default;
Theatre::~Theatre() noexcept = default;

Theatre* Theatre::Current()
{ return g_pTheatreManager->Current(); }

void Theatre::Update()
{
    LockGuard<RMutex> lock{mThingsMutex};
    for(auto& [id, thing] : mThings)
        { thing->Update(); }
}

void Theatre::Tick()
{
    LockGuard<RMutex> lock{mThingsMutex};
    for(auto& [id, thing] : mThings)
        { thing->Tick(); }
}

void Theatre::Input(InputEvent* inInput)
{
    LockGuard<RMutex> lock{mThingsMutex};
    for(auto& [id, thing] : mThings)
        { thing->Input(inInput); }
}

void Theatre::LoadTheatreData(Farg<TheatreFile::TheatreData> inData)
{
    Shutdown();
    *m_pInitialState = inData;
    mWasLoadedFromFile = false;
    mName  = m_pInitialState->name;
    mIndex = m_pInitialState->index;
    mInitStatus = OK;
}

Error Theatre::LoadFile(std::string inFilePath)
{
    Shutdown();
    if(not print_error_enum(TheatreFile::Load(inFilePath, m_pInitialState)))
        { return mInitStatus = ERR_INIT_FAILED; }
    mWasLoadedFromFile = true;
    mName  = m_pInitialState->name;
    mIndex = m_pInitialState->index;
    return mInitStatus = OK;
}

Error Theatre::LoadData(Farg<FileData> inData)
{
    if(not print_error_enum(TheatreFile::Load(inData, m_pInitialState)))
        { return mInitStatus = ERR_INIT_FAILED; }
    mWasLoadedFromFile = inData.has_filepath();
    mName  = m_pInitialState->name;
    mIndex = m_pInitialState->index;
    return mInitStatus = OK;
}

std::string Theatre::GetSaveData()
{
    LOCK_THINGS;
    std::string _output{std::format("@{}#{}\n", mName, mIndex)};
    for(FAUTO [id, thing] : mThings)
    {
        if(Console::GetVariable("Theatre.debug_save_msgs")->int_value)
            { print_debug("Saving [{}, {}]", thing->name(), id()); }
        _output += thing->GetVariables()->get_parsable_string();
    }
    return _output;
}

Error Theatre::SaveToFile(Sarg inPath, FileSystem::OverwriteAction inAction)
{ return print_error_enum(FileSystem::Lazy::Write(inPath, GetSaveData(), inAction)); }

bool Theatre::Startup()
{
    assert(mInitStatus == OK and m_pInitialState);
    if(mIsStarted)
        { return true; }

    LOCK_THINGS;
    LOCK_CALLSHEET;

    mName  = m_pInitialState->name;
    mIndex = m_pInitialState->index;

    m_pInitialState->sort_by_priority();

    for(auto iter{m_pInitialState->begin()}; iter != m_pInitialState->end();)
    {
        if(iter->invalid())
        {
            if(iter->name.empty())
                { print_warning("Removing `ThingData` with empty name"); }
            else if(not ThingFactory::IsThing(iter->type))
                { print_warning("Removing `ThingData` with invalid type id: '{}'", iter->type.name()); }
            iter = m_pInitialState->erase(iter);
            continue;
        }
        else if(ThingFactory::IsDerivedFrom(iter->type, ThingType::NostalgiaPlayer) and m_pPlayer)
        {
            print_warning("Only one player at a time, please");
            iter = m_pInitialState->erase(iter);
            continue;
        }
        auto thing{ThingFactory::MakeThing(iter->type, iter->name)};
        mThings[thing->uid()] = thing;
        mNames[thing->name()] = thing->uid();
        thing->SetNameChangeCallback(&Theatre::SetThingName);
        thing->Init();
        UpdateIdSetsAndSpecialThings(thing->Type(), thing->uid());
        mCallSheet.Add(thing->uid());
        if(Console::GetVariable("Theatre.debug_create_thing_msgs")->int_value)
            { print_debug("Created {} [{}, {}]", thing->Type().name(), thing->name(), thing->uid()()); }
        ++iter;
    }

    for(auto iter{m_pInitialState->begin()}; iter != m_pInitialState->end();)
    {
        if(auto found_it{mNames.find(iter->name)}; found_it != mNames.end())
        {
            if(mThings.contains(found_it->second))
            {
                if(ID parent{GetUID(iter->parent_variable.value)}; not parent.invalid())
                {
                    if(not mCallSheet.Has(parent))
                        { mCallSheet.Add(parent); }
                    if(not mCallSheet.Reparent(found_it->second, parent))
                        { mCallSheet.Add(found_it->second, parent); }
                }
                for(FAUTO child_var : iter->children_variables)
                {
                    ID child{GetUID(child_var.value)};
                    if(not child.invalid() and not mCallSheet.Reparent(child, found_it->second))
                        { mCallSheet.Add(child, found_it->second); }
                }
                ++iter;
                continue;
            }
        }
        mNames.erase(iter->name);
        print_warning("Removing broken ThingData with name '{}'", iter->name);
        iter = m_pInitialState->erase(iter);
    }

    for(FAUTO thing_data : *m_pInitialState)
        { mThings.at(mNames.at(thing_data.name))->SetVariables(thing_data); }

    for(FAUTO thing_data : *m_pInitialState)
        { mThings.at(mNames.at(thing_data.name))->Ready(); }

    if(Console::GetVariable("Theatre.debug_callsheet_msgs")->int_value)
        { print_debug("{}", mCallSheet.debug_log(mThings)); }

    mIsStarted = true;
    return true;
}

bool Theatre::Shutdown()
{
    if(not mIsStarted)
        { return true; }
    mIsStarted = false;
    LOCK_THINGS;
    IdVec_t _uids{ThingUIDs()};
    for(FAUTO uid : _uids)
        { DestroyThing(uid); }
    mNames.clear();
    mCallSheet.Clear();
    mThings.clear();
    return true;
}

void Theatre::Draw()
{
    bool _enable_3d_rendering = Console::GetVariable("Theatre.draw_3d")->int_value,
        _enable_2d_rendering = Console::GetVariable("Theatre.draw_2d")->int_value;

    LOCK_THINGS;

    Light3D::ClearCounts();
    for(ID id : mLightIDs)
        { g_pRenderManager->GetAPI()->SetLight_TempBlinnPhongSolution(GetThinker<Light3D>(id)); }

    // Render the root viewport
    g_pRenderManager->GetAPI()->BindFramebuffer();
    g_pRenderManager->GetAPI()->SetViewport({0, 0}, MainWindow()->GetScale());
    g_pRenderManager->GetAPI()->SetClearColor(Settings::Graphics::ClearColor);
    g_pRenderManager->GetAPI()->Clear();

    if(_enable_3d_rendering and not mRootViewportCurrentCamera3D.invalid())
    {
        auto _camera{GetThinker<Camera3D>(mRootViewportCurrentCamera3D)};
        _camera->DrawBackground();
        for(ID _uid : mVisual3DIDs)
            { _camera->Draw(GetThinker<Visual3D>(_uid)); }
    }
    if(_enable_2d_rendering and not mRootViewportCurrentCamera3D.invalid())
    {
        auto _camera{GetThinker<Camera2D>(mRootViewportCurrentCamera2D)};
        for(ID _uid : mVisual2DIDs)
            { _camera->Draw(GetThinker<Visual2D>(_uid)); }
    }

    // Render other viewports
    for(ID viewport_id : mViewportIDs)
    {
        auto viewport{GetThinker<Viewport>(viewport_id)};
        viewport->Attach();
        if(_enable_3d_rendering and not viewport->CurrentCamera3D().invalid())
        {
            auto _camera{GetThinker<Camera3D>(viewport->CurrentCamera3D())};
            _camera->DrawBackground();
            for(ID _uid : mVisual3DIDs)
                { _camera->Draw(GetThinker<Visual3D>(_uid)); }
        }
        if(_enable_2d_rendering and not viewport->CurrentCamera2D().invalid())
        {
            auto _camera{GetThinker<Camera2D>(viewport->CurrentCamera2D())};
            for(ID _uid : mVisual2DIDs)
                { _camera->Draw(GetThinker<Visual2D>(_uid)); }
        }
        viewport->Detach();
    }
}

Sarg Theatre::Name() const
{ return mName; }

uint Theatre::Index() const
{ return mIndex; }

bool Theatre::WasLoadedFromFile() const
{ return mWasLoadedFromFile; }

Sarg Theatre::TheatreFilePath() const
{ return m_pInitialState->file_path; }

std::string Theatre::TheatreFileDirectory() const
{ return FileSystem::GetDir(m_pInitialState->file_path); }

std::string Theatre::TheatreFileName() const
{ return FileSystem::GetStem(m_pInitialState->file_path, true); }

Error Theatre::InitStatus() const
{ return mInitStatus; }

bool Theatre::IsStarted() const
{ return mIsStarted; }

Farg<TheatreData> Theatre::InitialState() const
{ return *m_pInitialState; }

TheatreData Theatre::CurrentState()
{
    LockGuard<RMutex> lock{mThingsMutex};
    TheatreData data{*m_pInitialState};
    data.clear();
    for(FAUTO [id, thing] : mThings)
        { data.push_back(*thing->GetVariables()); }
    return data;
}

ID Theatre::GetCurrentCamera2D(ID inViewportID)
{
    if(inViewportID.invalid())
        { return mRootViewportCurrentCamera2D; }
    return GetThinker<Viewport>(inViewportID)->CurrentCamera2D();
}

ID Theatre::GetCurrentCamera3D(ID inViewportID)
{
    if(inViewportID.invalid())
        { return mRootViewportCurrentCamera3D; }
    return GetThinker<Viewport>(inViewportID)->CurrentCamera3D();
}

Error Theatre::SetCurrentCamera(ID inCameraID, ID inViewportID)
{
    LOCK_THINGS;
    LOCK_CALLSHEET;

    bool is_3d{DerivedFrom(inCameraID, ThingType::Camera3D)};
    IdSet_t ancestors{GetAllParents(inCameraID)};

    if(not is_3d and not DerivedFrom(inCameraID, ThingType::Camera2D))
        { return ERR_INVALID_ID; }
    else if(inViewportID.invalid())
    {
        for(FAUTO ancestor : ancestors)
        {
            if(DerivedFrom(ancestor, ThingType::Viewport))
                { return ERR_INVALID; }
        }
        if(is_3d)
            { mRootViewportCurrentCamera3D = inCameraID; }
        else
            { mRootViewportCurrentCamera2D = inCameraID; }
        return OK;
    }
    else if(auto viewport{GetThinker<Viewport>(inViewportID)}; not viewport->Invalid() and ancestors.contains(inViewportID))
    {
        if(is_3d)
            { viewport->SetCurrentCamera3D(inCameraID); }
        else
            { viewport->SetCurrentCamera2D(inCameraID); }
        return OK;
    }
    return ERR_NOT_FOUND;
}

IdVec_t Theatre::ThingUIDs()
{
    LOCK_THINGS;
    auto keys{std::views::keys(mThings)};
    return {keys.begin(), keys.end()};
}

IdVec_t Theatre::ThinkersWithNoParents()
{
    LOCK_THINGS;
    LOCK_CALLSHEET;
    IdVec_t _thinkers{};
    for(ID uid : mThinkerUIDs)
    {
        if(FAUTO node{mCallSheet.Get(uid)}; not node.invalid() and node.parent.invalid())
            { _thinkers.push_back(uid); }
    }
    return _thinkers;
}

IdSet_arg Theatre::ThinkerUIDs()
{
    LOCK_THINGS;
    return mThinkerUIDs;
}

IdSet_arg Theatre::ResourceUIDs()
{
    LOCK_THINGS;
    return mResourceUIDs;
}

bool Theatre::ThingExists(ID inUID)
{
    LockGuard<RMutex> lock{mThingsMutex};
    return mThings.contains(inUID) or ResourceDatabase::Contains(inUID);
}

bool Theatre::ThingExists(Sarg inName)
{
    LockGuard<RMutex> lock{mThingsMutex};
    if(mNames.contains(inName) or ResourceDatabase::Contains(inName))
        { return true; }
    for(FAUTO [id, thing] : mThings)
    {
        if(not thing->name().compare(inName))
            { return true; }
    }
    return false;
}

FPID Theatre::TypeOf(ID inID)
{
    LockGuard<RMutex> lock{mThingsMutex};
    if(auto found_it{mThings.find(inID)}; found_it != mThings.end())
        { return found_it->second->Type(); }
    return ResourceDatabase::TypeOf(inID);
}

bool Theatre::DerivedFrom(ID inID, FPID inType)
{
    LockGuard<RMutex> lock{mThingsMutex};
    if(auto found_it{mThings.find(inID)}; found_it != mThings.end())
        { return found_it->second->DerivedFrom(inType); }
    return ResourceDatabase::DerivedFrom(inID, inType);
}

ID Theatre::CreateThing(Farg<TheatreFile::ThingData> inData, bool inDoReadyThing)
{
    LockGuard<RMutex> lock{mThingsMutex};
    if(inData.name.empty())
        { print_warning("ThingData::name cannot be empty"); return ID::Invalid; }
    else if(ThingFactory::IsDerivedFrom(inData.type, ThingType::NostalgiaPlayer) and m_pPlayer)
        { print_warning("Only one player at a time, please!"); return m_pPlayer->uid(); }
    else if(ThingExists(inData.name))
        { return GetUID(inData.name); }

    auto thing{ThingFactory::MakeThing(inData.type, inData.name)};
    mThings[thing->uid()] = thing;
    mNames[thing->name()] = thing->uid();
    thing->SetNameChangeCallback(&Theatre::SetThingName);
    thing->Init();
    UpdateCallsheet(thing->uid(), inData);
    UpdateIdSetsAndSpecialThings(thing->Type(), thing->uid());
    thing->SetVariables(inData);
    if(inDoReadyThing)
        { thing->Ready(); }
    if(Console::GetVariable("Theatre.debug_create_thing_msgs")->int_value)
        { print_debug("Created {} [{}, {}]", thing->Type().name(), thing->name(), thing->uid()()); }
    return thing->uid();
}

Error Theatre::DestroyThing(ID inID)
{
    LOCK_THINGS;
    if(not mThings.contains(inID))
        { return ERR_NOT_FOUND; }
    auto children{GetChildren(inID)};
    for(ID child : children)
        { DestroyThing(child); }
    auto status{DestroyThingOnly(inID)};
    return status;
}

ID Theatre::GetUID(Sarg inName)
{
    LOCK_THINGS;
    if(ID uid{ResourceDatabase::GetUID(inName)}; not uid.invalid())
        { return uid; }
    else if(auto found_it{mNames.find(inName)}; found_it != mNames.end())
        { return found_it->second; }
    return ID::Invalid;
}

Sarg Theatre::GetName(ID inUID)
{
    static std::string empty{};
    LOCK_THINGS;
    if(Sarg name{ResourceDatabase::GetName(inUID)}; not name.empty())
        { return name; }
    else if(auto found_it{mThings.find(inUID)}; found_it != mThings.end())
        { return found_it->second->name(); }
    for(FAUTO [name, uid] : mNames)
    {
        if(inUID == uid)
            { return name; }
    }
    return empty;
}

Error Theatre::SetName(ID inUID, Sarg inName)
{
    if(ThingExists(inName))
        { return ERR_ALREADY_EXISTS; }
    else if(not ThingExists(inUID))
        { return ERR_NOT_FOUND; }
    LOCK_THINGS;
    auto thing{mThings.at(inUID)};
    mNames.erase(thing->mName);
    mNames[inName] = inUID;
    thing->mName = inName;
    return OK;
}

Error Theatre::SetName(Sarg inOldName, Sarg inNewName)
{
    if(not ThingExists(inOldName))
        { return ERR_NOT_FOUND; }
    return SetName(GetUID(inOldName), inNewName);
}

Shared<Thinker> Theatre::GetPlayer()
{
    return (m_pPlayer)
        ? m_pPlayer
        : MakeShared<Thinker>();
}

IdSet_arg Theatre::GetViewports()
{ return mViewportIDs; }

IdSet_t Theatre::GetChildren(ID inParentID)
{
    LOCK_CALLSHEET;
    return mCallSheet.Get(inParentID).children;
}

ID Theatre::GetParent(ID inChildID)
{
    LOCK_CALLSHEET;
    return mCallSheet.Get(inChildID).parent;
}

IdSet_t Theatre::GetAllChildren(ID inParentID)
{
    LOCK_CALLSHEET;
    return mCallSheet.Descendants(inParentID);
}

IdSet_t Theatre::GetAllParents(ID inChildID)
{
    LOCK_CALLSHEET;
    return mCallSheet.Ancestors(inChildID);
}

Error Theatre::SetParent(ID inChildID, ID inParentID)
{
    LOCK_CALLSHEET;
    LOCK_THINGS;

    // NOTE: Probably redundant. Too bad!
    if(not mCallSheet.Has(inChildID))
        { print_error("CallSheet missing child UID {}", inChildID()); return print_error_enum(ERR_NOT_FOUND); }
    else if(not inParentID.invalid() and not mCallSheet.Has(inParentID))
        { print_error("CallSheet missing parent UID {}", inParentID()); return print_error_enum(ERR_NOT_FOUND); }

    auto old_parent_id{mCallSheet.Get(inChildID).parent};
    auto old_ancestors{mCallSheet.Ancestors(inChildID)};

    if(auto status{mCallSheet.Reparent(inChildID, inParentID)}; not status)
        { return print_error_enum(status); }

    auto child{GetThinker(inChildID)};
    auto parent{GetThinker(inParentID)};
    auto old_parent{GetThinker(old_parent_id)};

    parent->OnChildAdded(child);
    old_parent->OnChildRemoved(child);
    child->OnParentChanged(parent, old_parent);

    auto new_ancestors{mCallSheet.Ancestors(inChildID)};
    auto descendants{mCallSheet.Descendants(inParentID)};

    for(ID descendant : descendants)
    {
        auto descendant_thinker{GetThinker(descendant)};
        for(ID old_ancestor : old_ancestors)
        {
            auto old_ancestor_thinker{GetThinker(old_ancestor)};
            old_ancestor_thinker->OnDescendantRemoved(descendant_thinker);
            descendant_thinker->OnAncestorRemoved(old_ancestor_thinker);
        }
        for(ID new_ancestor : new_ancestors)
        {
            auto new_ancestor_thinker{GetThinker(new_ancestor)};
            new_ancestor_thinker->OnDescendantAdded(descendant_thinker);
            descendant_thinker->OnAncestorAdded(new_ancestor_thinker);
        }
    }

    return OK;
}

Error Theatre::DropParent(ID inChildID)
{
    LOCK_CALLSHEET;
    LOCK_THINGS;

    auto parent{mCallSheet.Get(mCallSheet.Get(inChildID).parent)};

    if(!mCallSheet.Has(inChildID) or !parent.children.contains(inChildID))
        { return ERR_NOT_FOUND; }

    return SetParent(inChildID, parent.parent);
}

Shared<Thing> Theatre::GetThing(Sarg inName)
{
    if(ResourceDatabase::Contains(inName))
        { return ResourceDatabase::GetResource(inName); }
    LockGuard<RMutex> lock{mThingsMutex};
    for(FAUTO [id, thing] : mThings)
        { if(!thing->name().compare(inName)) { return thing; } }
    return MakeShared<Thing>();
}

Shared<Thing> Theatre::GetThing(ID inID)
{
    if(ResourceDatabase::Contains(inID))
        { return ResourceDatabase::GetResource(inID); }
    LockGuard<RMutex> lock{mThingsMutex};
    if(auto found_it{mThings.find(inID)};
        found_it != mThings.end())
    { return found_it->second; }
    return MakeShared<Thing>();
}

Shared<Resource> Theatre::GetResource(ID inID)
{
    if(ResourceDatabase::Contains(inID))
        { return ResourceDatabase::GetResource(inID); }
    LockGuard<RMutex> lock{mThingsMutex};
    if(auto found_it{mThings.find(inID)};
        found_it != mThings.end())
    {
        if(found_it->second->IsResource())
            { return DCast<Resource>(found_it->second); }
    }
    return MakeShared<Resource>();
}

Shared<Thinker> Theatre::GetThinker(ID inID)
{
    LockGuard<RMutex> lock{mThingsMutex};
    if(auto found_it{mThings.find(inID)};
        found_it != mThings.end())
    {
        if(found_it->second->IsThinker())
            { return DCast<Thinker>(found_it->second); }
    }
    return MakeShared<Thinker>();
}

void Theatre::UpdateCallsheet(ID inUID, Farg<ThingData> inData)
{
    LOCK_CALLSHEET;

    if(not mCallSheet.Has(inUID))
        { mCallSheet.Add(inUID); }
    if(ID parent{GetUID(inData.parent_variable.value)}; not parent.invalid())
    {
        if(not mCallSheet.Has(parent))
            { mCallSheet.Add(parent); }
        if(not mCallSheet.Reparent(inUID, parent))
            { mCallSheet.Add(inUID, parent); }
    }
    for(FAUTO child_var : inData.children_variables)
    {
        ID child{GetUID(child_var.value)};
        if(not child.invalid() and not mCallSheet.Reparent(child, inUID))
            { mCallSheet.Add(child, inUID); }
    }
}

void Theatre::UpdateIdSetsAndSpecialThings(FPID inType, ID inUID)
{
    if(ThingFactory::IsThinker(inType))
    {
        mThinkerUIDs.insert(inUID);
        if(ThingFactory::IsDerivedFrom(inType, ThingType::NostalgiaPlayer) and not m_pPlayer)
            { m_pPlayer = GetThinker(inUID); }
        else if(ThingFactory::IsDerivedFrom(inType, ThingType::Viewport))
            { mViewportIDs.insert(inUID); }
        else if(ThingFactory::IsDerivedFrom(inType, ThingType::Visual3D))
        {
            mVisual3DIDs.insert(inUID);
            if(ThingFactory::IsDerivedFrom(inType, ThingType::Light3D))
                { mLightIDs.insert(inUID); }
        }
        else if(ThingFactory::IsDerivedFrom(inType, ThingType::Visual2D))
            { mVisual2DIDs.insert(inUID); }
    }
    else if(ThingFactory::IsResource(inType))
        { mResourceUIDs.insert(inUID); }
}

Error Theatre::DestroyThingOnly(ID inID)
{
    LockGuard<RMutex> lock{mThingsMutex};
    LOCK_CALLSHEET;

    if(!mThings.contains(inID))
    {
        return (mThings.empty())
            ? ERR_EMPTY
            : ERR_NOT_FOUND;
    }

    for(auto iter{mNames.begin()}; iter != mNames.end(); ++iter)
    {
        if(iter->second == inID)
        {
            mNames.erase(iter);
            break;
        }
    }

    mThinkerUIDs.erase(inID);
    mResourceUIDs.erase(inID);
    mViewportIDs.erase(inID);
    mVisual2DIDs.erase(inID);
    mVisual3DIDs.erase(inID);
    mLightIDs.erase(inID);
    mCallSheet.Remove(inID);
    mThings.erase(inID);
    return OK;
}

Error Theatre::SetThingName(Sarg inOldName, Sarg inNewName)
{
    if(not Theatre::Current())
        { return ERR_NULLPTR; }
    return Theatre::Current()->SetName(inOldName, inNewName);
}

const LockGuard<RMutex> Theatre::GetThingsLock()
{ return LockGuard<RMutex>{mThingsMutex}; }

const LockGuard<RMutex> Theatre::GetCallSheetLock()
{ return LockGuard<RMutex>{mCallSheetMutex}; }

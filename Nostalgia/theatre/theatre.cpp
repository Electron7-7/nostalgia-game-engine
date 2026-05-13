#include "./theatre.hpp"
#include "application/application.hpp"
#include "managers/theatre_manager.hpp"
#include "things/thing_factory.hpp"
#include "things/thinkers/2d/camera_2d.hpp"
#include "things/thinkers/3d/camera_3d.hpp"
#include "things/thinkers/3d/light_3d.hpp"
#include "things/thinkers/viewport.hpp"
#include "rendering/renderer_api.hpp"
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
    LOCK_THINGS;
    for(ID uid : mThingUIDs)
        { ThingFactory::GetThing(uid)->Update(); }
}

void Theatre::Tick()
{
    LOCK_THINGS;
    for(ID uid : mThingUIDs)
        { ThingFactory::GetThing(uid)->Tick(); }
}

void Theatre::Input(InputEvent* inInput)
{
    LOCK_THINGS;
    for(ID uid : mThingUIDs)
        { ThingFactory::GetThing(uid)->Input(inInput); }
}

Shared<Image> Theatre::TakeScreenshot(ID inViewportUID)
{
    LOCK_THINGS;
    if(auto _viewport{ThingFactory::GetThing<Viewport>(inViewportUID)}; not _viewport->invalid())
        { return _viewport->GetImage(); }
    return RendererAPI::Get()->GetFullScreenshot();
}

void Theatre::LoadTheatreData(Farg<TheatreFile::TheatreData> inData)
{
    if(Console::GetVariable("Theatre.debug_load_data_log").int_value)
    {
        print_debug("Old Theatre Data:\n{}", m_pInitialState->get_log());
        print_debug("New Theatre Data:\n{}", inData.get_log());
    }
    Shutdown();
    *m_pInitialState = inData;
    mWasLoadedFromFile = false;
    mName  = m_pInitialState->name;
    mIndex = m_pInitialState->index;
    mInitStatus = OK;
}

Error Theatre::LoadFile(std::string inFilePath)
{
    Shared<TheatreFile::TheatreData> _new_state{MakeShared<TheatreFile::TheatreData>()};
    if(not print_error_enum(TheatreFile::Load(inFilePath, _new_state)))
        { return ERR_FILE_LOAD; }
    Shutdown();
    m_pInitialState = _new_state;
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
    for(ID uid : mThingUIDs)
    {
        auto _thing{ThingFactory::GetThing(uid)};
        if(Console::GetVariable("Theatre.debug_save_msgs").int_value)
            { print_debug("Saving [{}, {}]", _thing->name(), uid()); }
        _output += _thing->GetVariables()->get_parsable_string();
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
        iter->name = thing->name();
        mThingUIDs.insert(thing->uid());
        UpdateIdSetsAndSpecialThings(thing->Type(), thing->uid());
        mCallSheet.add_node(thing->uid());
        if(Console::GetVariable("Theatre.debug_create_thing_msgs").int_value)
            { print_debug("Created {} [{}, {}]", thing->Type().name(), thing->name(), thing->uid()()); }
        ++iter;
    }

    for(auto iter{m_pInitialState->begin()}; iter != m_pInitialState->end();)
    {
        if(ID _uid{ThingFactory::GetUID(iter->name)};
            not _uid.invalid()
            and mThingUIDs.contains(_uid))
        {
            if(ID parent{ThingFactory::GetUID(iter->parent_variable.value)}; not parent.invalid())
            {
                if(not mCallSheet.has_node(parent))
                    { mCallSheet.add_node(parent); }
                if(not mCallSheet.set_parent(_uid, parent))
                    { mCallSheet.add_node(_uid, parent); }
            }
            for(FAUTO child_var : iter->children_variables)
            {
                ID child{ThingFactory::GetUID(child_var.value)};
                if(not child.invalid() and not mCallSheet.set_parent(child, _uid))
                    { mCallSheet.add_node(child, _uid); }
            }
            ++iter;
            continue;
        }
        print_warning("Removing broken ThingData with name '{}'", iter->name);
        iter = m_pInitialState->erase(iter);
    }

    for(FAUTO thing_data : *m_pInitialState)
        { ThingFactory::GetThing(thing_data.name)->SetVariables(thing_data); }

    for(FAUTO thing_data : *m_pInitialState)
        { ThingFactory::GetThing(thing_data.name)->Ready(); }

    if(Console::GetVariable("Theatre.debug_callsheet_msgs").int_value)
        { print_debug("{}", InheritanceLog()); }

    if(Console::GetVariable("Theatre.debug_startup_things_list").int_value)
    {
        print_debug("Theatre::mThings");
        for(FAUTO [uid, thing] : mThings)
            { debug_print("[{}, {}]", uid(), thing->name()); }
    }

    mIsStarted = true;
    return true;
}

bool Theatre::Shutdown()
{
    if(not mIsStarted)
        { return true; }
    mIsStarted = false;
    LOCK_THINGS;
    IdVec_t _uids{mThingUIDs.begin(), mThingUIDs.end()};
    for(ID uid : _uids)
        { DestroyThing(uid); }
    mCallSheet.clear();
    mThingUIDs.clear();
    return true;
}

void Theatre::Draw()
{
    bool _enable_3d_rendering = Console::GetVariable("Theatre.draw_3d").int_value,
        _enable_2d_rendering = Console::GetVariable("Theatre.draw_2d").int_value;

    LOCK_THINGS;

    Light3D::ClearCounts();
    for(ID id : mLightIDs)
        { RendererAPI::Get()->SetLight_TempBlinnPhongSolution(ThingFactory::GetThing<Light3D>(id)); }

    // Render the root viewport
    RendererAPI::Get()->BindFramebuffer();
    RendererAPI::Get()->SetViewport({0, 0}, MainWindow()->GetScale());
    RendererAPI::Get()->SetClearColor(Settings::Graphics::ClearColor);
    RendererAPI::Get()->Clear();

    if(_enable_3d_rendering and not mRootViewportCurrentCamera3D.invalid())
    {
        auto _camera{ThingFactory::GetThing<Camera3D>(mRootViewportCurrentCamera3D)};
        _camera->DrawBackground();
        for(ID _uid : mVisual3DIDs)
            { _camera->Draw(ThingFactory::GetThing<Visual3D>(_uid)); }
    }
    if(_enable_2d_rendering and not mRootViewportCurrentCamera3D.invalid())
    {
        auto _camera{ThingFactory::GetThing<Camera2D>(mRootViewportCurrentCamera2D)};
        for(ID _uid : mVisual2DIDs)
            { _camera->Draw(ThingFactory::GetThing<Visual2D>(_uid)); }
    }

    // Render other viewports
    for(auto& viewport : mViewports)
    {
        viewport->Attach();
        if(_enable_3d_rendering and not viewport->CurrentCamera3D()->invalid())
        {
            viewport->CurrentCamera3D()->DrawBackground();
            for(ID _uid : mVisual3DIDs)
                { viewport->CurrentCamera3D()->Draw(ThingFactory::GetThing<Visual3D>(_uid)); }
        }
        if(_enable_2d_rendering and not viewport->CurrentCamera2D()->invalid())
        {
            for(ID _uid : mVisual2DIDs)
                { viewport->CurrentCamera2D()->Draw(ThingFactory::GetThing<Visual2D>(_uid)); }
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

std::string Theatre::InheritanceLog() const
{
    std::string log{"Inheritance Log:\n"};
    for(FAUTO [uid, node] : mCallSheet.nodes)
    {
        auto _name{ThingFactory::GetName(uid)};
        log += std::format("\tNode [{}, {}]\n",
            uid(),
            (_name.empty()) ? GlobalConstants::str_NA : _name);
        for(ID child : node.children)
        {
            _name = ThingFactory::GetName(child);
            log += std::format("\t\t[{}, {}]\n",
                child(),
                (_name.empty()) ? GlobalConstants::str_NA : _name);
        }
    }
    return log;
}

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
    for(ID uid : mThingUIDs)
        { data.push_back(*ThingFactory::GetThing(uid)->GetVariables()); }
    return data;
}

ID Theatre::GetCurrentCamera2D(ID inViewportID)
{
    if(inViewportID.invalid())
        { return mRootViewportCurrentCamera2D; }
    return ThingFactory::GetThing<Viewport>(inViewportID)->CurrentCamera2D()->uid();
}

ID Theatre::GetCurrentCamera3D(ID inViewportID)
{
    if(inViewportID.invalid())
        { return mRootViewportCurrentCamera3D; }
    return ThingFactory::GetThing<Viewport>(inViewportID)->CurrentCamera3D()->uid();
}

Error Theatre::SetCurrentCamera(ID inCameraID, ID inViewportID)
{
    LOCK_THINGS;
    LOCK_CALLSHEET;

    bool is_3d{ThingFactory::DerivedFrom(inCameraID, ThingType::Camera3D)};
    auto ancestors{GetAllParents(inCameraID)};

    if(not is_3d and not ThingFactory::DerivedFrom(inCameraID, ThingType::Camera2D))
        { return ERR_INVALID_ID; }
    else if(inViewportID.invalid())
    {
        for(FAUTO ancestor : ancestors)
        {
            if(ThingFactory::DerivedFrom(ancestor, ThingType::Viewport))
                { return ERR_INVALID; }
        }
        if(is_3d)
            { mRootViewportCurrentCamera3D = inCameraID; }
        else
            { mRootViewportCurrentCamera2D = inCameraID; }
        return OK;
    }
    else if(auto viewport{ThingFactory::GetThing<Viewport>(inViewportID)};
        not viewport->invalid()
        and ancestors.contains(inViewportID))
    {
        if(is_3d)
            { viewport->SetCurrentCamera3D(ThingFactory::GetThing<Camera3D>(inCameraID)); }
        else
            { viewport->SetCurrentCamera2D(ThingFactory::GetThing<Camera2D>(inCameraID)); }
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
        if(FAUTO node{mCallSheet.get_node(uid)}; not node.invalid() and node.parent.invalid())
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

bool Theatre::Contains(ID inUID)
{
    LOCK_THINGS;
    return mThingUIDs.contains(inUID);
}

bool Theatre::Contains(Sarg inName)
{
    LOCK_THINGS;
    return mThingUIDs.contains(ThingFactory::GetUID(inName));
}

ID Theatre::CreateThing(Farg<TheatreFile::ThingData> inData, bool inDoReadyThing)
{
    LockGuard<RMutex> lock{mThingsMutex};
    if(inData.name.empty())
        { print_warning("ThingData::name cannot be empty"); return ID::Invalid; }
    else if(ThingFactory::IsDerivedFrom(inData.type, ThingType::NostalgiaPlayer) and m_pPlayer)
        { print_warning("Only one player at a time, please!"); return m_pPlayer->uid(); }
    else if(Contains(inData.name))
        { return ThingFactory::GetUID(inData.name); }

    auto thing{ThingFactory::MakeThing(inData.type, inData.name)};
    mThingUIDs.insert(thing->uid());
    UpdateCallsheet(thing->uid(), inData);
    UpdateIdSetsAndSpecialThings(thing->Type(), thing->uid());
    thing->SetVariables(inData);
    if(inDoReadyThing)
        { thing->Ready(); }
    if(Console::GetVariable("Theatre.debug_create_thing_msgs").int_value)
        { print_debug("Created {} [{}, {}]", thing->Type().name(), thing->name(), thing->uid()()); }
    return thing->uid();
}

Error Theatre::DestroyThing(ID inID)
{
    LOCK_THINGS;
    if(inID.invalid())
        { return ERR_INVALID_ID; }
    else if(not mThingUIDs.contains(inID))
        { return ERR_NOT_FOUND; }
    auto children{GetChildren(inID)};
    for(ID child : children)
        { DestroyThing(child); }
    auto status{DestroyThingOnly(inID)};
    return status;
}

Shared<Thinker> Theatre::GetPlayer()
{
    return (m_pPlayer)
        ? m_pPlayer
        : MakeShared<Thinker>();
}

IdSet_t Theatre::GetChildren(ID inParentID)
{
    LOCK_CALLSHEET;
    return mCallSheet.get_node(inParentID).children;
}

ID Theatre::GetParent(ID inChildID)
{
    LOCK_CALLSHEET;
    return mCallSheet.get_node(inChildID).parent;
}

std::unordered_set<ID> Theatre::GetAllChildren(ID inParentID)
{
    LOCK_CALLSHEET;
    return mCallSheet.get_descendants(inParentID);
}

std::unordered_set<ID> Theatre::GetAllParents(ID inChildID)
{
    LOCK_CALLSHEET;
    return mCallSheet.get_ancestors(inChildID);
}

Error Theatre::SetParent(ID inChildID, ID inParentID)
{
    LOCK_CALLSHEET;
    LOCK_THINGS;

    // NOTE: Probably redundant. Too bad!
    if(not mCallSheet.has_node(inChildID))
        { print_error("CallSheet missing child UID {}", inChildID()); return print_error_enum(ERR_NOT_FOUND); }
    else if(not inParentID.invalid() and not mCallSheet.has_node(inParentID))
        { print_error("CallSheet missing parent UID {}", inParentID()); return print_error_enum(ERR_NOT_FOUND); }

    auto old_parent_id{mCallSheet.get_node(inChildID).parent};
    auto old_ancestors{mCallSheet.get_ancestors(inChildID)};

    if(auto status{mCallSheet.set_parent(inChildID, inParentID)}; not status)
        { return print_error_enum(status); }

    auto child{ThingFactory::GetThinker(inChildID)};
    auto parent{ThingFactory::GetThinker(inParentID)};
    auto old_parent{ThingFactory::GetThinker(old_parent_id)};

    parent->OnChildAdded(child);
    old_parent->OnChildRemoved(child);
    child->OnParentChanged(parent, old_parent);

    auto new_ancestors{mCallSheet.get_ancestors(inChildID)};
    auto descendants{mCallSheet.get_descendants(inParentID)};

    for(ID descendant : descendants)
    {
        auto descendant_thinker{ThingFactory::GetThinker(descendant)};
        for(ID old_ancestor : old_ancestors)
        {
            auto old_ancestor_thinker{ThingFactory::GetThinker(old_ancestor)};
            old_ancestor_thinker->OnDescendantRemoved(descendant_thinker);
            descendant_thinker->OnAncestorRemoved(old_ancestor_thinker);
        }
        for(ID new_ancestor : new_ancestors)
        {
            auto new_ancestor_thinker{ThingFactory::GetThinker(new_ancestor)};
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

    auto parent{mCallSheet.get_node(mCallSheet.get_node(inChildID).parent)};

    if(!mCallSheet.has_node(inChildID) or !parent.children.contains(inChildID))
        { return ERR_NOT_FOUND; }

    return SetParent(inChildID, parent.parent);
}

void Theatre::UpdateCallsheet(ID inUID, Farg<ThingData> inData)
{
    LOCK_CALLSHEET;

    if(not mCallSheet.has_node(inUID))
        { mCallSheet.add_node(inUID); }
    if(ID parent{ThingFactory::GetUID(inData.parent_variable.value)}; not parent.invalid())
    {
        if(not mCallSheet.has_node(parent))
            { mCallSheet.add_node(parent); }
        if(not mCallSheet.set_parent(inUID, parent))
            { mCallSheet.add_node(inUID, parent); }
    }
    for(FAUTO child_var : inData.children_variables)
    {
        ID child{ThingFactory::GetUID(child_var.value)};
        if(not child.invalid() and not mCallSheet.set_parent(child, inUID))
            { mCallSheet.add_node(child, inUID); }
    }
}

void Theatre::UpdateIdSetsAndSpecialThings(FPID inType, ID inUID)
{
    if(ThingFactory::IsThinker(inType))
    {
        mThinkerUIDs.insert(inUID);
        if(ThingFactory::IsDerivedFrom(inType, ThingType::NostalgiaPlayer) and not m_pPlayer)
            { m_pPlayer = ThingFactory::GetThinker(inUID); }
        else if(ThingFactory::IsDerivedFrom(inType, ThingType::Viewport))
            { mViewports.insert(ThingFactory::GetThing<Viewport>(inUID)); }
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

    if(not mThingUIDs.contains(inID))
        { return (mThingUIDs.empty()) ? ERR_EMPTY : ERR_NOT_FOUND; }
    else if(Console::GetVariable("Theatre.debug_shutdown_things_list").int_value)
        { print_debug("Destroyed Thing: [{}, {}]", ThingFactory::GetName(inID), inID()); }

    for(auto iter{mViewports.begin()}; iter != mViewports.end(); ++iter)
    {
        if(iter->get()->uid() == inID)
        {
            mViewports.erase(iter);
            break;
        }
    }

    mThingUIDs.erase(inID);
    mThinkerUIDs.erase(inID);
    mResourceUIDs.erase(inID);
    mVisual2DIDs.erase(inID);
    mVisual3DIDs.erase(inID);
    mLightIDs.erase(inID);
    mCallSheet.remove_node(inID);
    return ThingFactory::Destroy(inID);
}

const LockGuard<RMutex> Theatre::GetThingsLock()
{ return LockGuard<RMutex>{mThingsMutex}; }

const LockGuard<RMutex> Theatre::GetCallSheetLock()
{ return LockGuard<RMutex>{mCallSheetMutex}; }

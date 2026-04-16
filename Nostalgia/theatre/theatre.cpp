#include "./theatre.hpp"
#include "application/application.hpp"
#include "things/thing_data.hpp"
#include "things/resource_database.hpp"
#include "things/thing_factory.hpp"
#include "things/thinkers/2d/camera_2d.hpp"
#include "things/thinkers/2d/sprite_2d.hpp"
#include "things/thinkers/2d/text_2d.hpp"
#include "things/thinkers/3d/light_3d.hpp"
#include "things/thinkers/3d/sprite_3d.hpp"
#include "things/thinkers/3d/mesh_instance_3d.hpp"
#include "things/thinkers/3d/camera_3d.hpp"
#include "things/thinkers/3d/visual_3d.hpp"
#include "things/thinkers/viewport.hpp"
#include "things/resources/material.hpp"
#include "things/resources/texture.hpp"
#include "things/resources/array_mesh.hpp"
#include "things/resources/font.hpp"
#include "managers/theatre_manager.hpp"
#include "managers/render_manager.hpp"
#include "rendering/shader.hpp"
#include "settings/graphics.hpp"
#include "console/console.hpp"

#define LOCK_THINGS LockGuard<RMutex> things_lock{mThingsMutex}
#define LOCK_CALLSHEET LockGuard<RMutex> callsheet_lock{mCallSheetMutex}
#define FOUND_IT(MAP, KEY, ...) auto found_it##__VA_ARGS__{MAP.find(KEY)}; found_it##__VA_ARGS__ != MAP.end()

using namespace TheatreFile;

bool gDebugToggleTextRenderingMethod{false},
    gDebugEnable3DRendering{true},
    gDebugEnable2DRendering{true};

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

void Theatre::LoadTheatreData(Shared<TheatreFile::TheatreData> inData)
{
    Shutdown();
    m_pInitialState = inData;
    mWasLoadedFromFile = false;
    mTheatreFileDirectory.clear();
    mName  = m_pInitialState->name;
    mIndex = m_pInitialState->index;
    mInitStatus = OK;
}

Error Theatre::LoadFile(std::string inFilePath)
{
    Shutdown();
    if(not print_error_enum(TheatreFile::Load(inFilePath, m_pInitialState)))
        { return mInitStatus = ERR_INIT_FAILED; }
    mTheatreFileDirectory = FileSystem::GetDir(inFilePath, true);
    mWasLoadedFromFile = true;
    mName  = m_pInitialState->name;
    mIndex = m_pInitialState->index;
    return mInitStatus = OK;
}

Error Theatre::LoadData(Farg<FileData> inData)
{
    if(not print_error_enum(TheatreFile::Load(inData, m_pInitialState)))
        { return mInitStatus = ERR_INIT_FAILED; }
    else if((mWasLoadedFromFile = inData.has_filepath()))
        { mTheatreFileDirectory = FileSystem::GetDir(inData.filepath(), true); }
    mName  = m_pInitialState->name;
    mIndex = m_pInitialState->index;
    return mInitStatus = OK;
}

Error Theatre::Save(Sarg inOutputFilePath, FileOverwriteAction inAction)
{
    std::string file_path{inOutputFilePath};
    if(FileSystem::IsFile(inOutputFilePath))
    {
        switch(inAction)
        {
        case CANCEL:
            print_warning("A file already exists at '{}'", inOutputFilePath);
            return ERR_FILE_EXISTS;
        case RENAME:
            {
                uint i{0};
                std::string file_directory{FileSystem::GetDir(file_path)};
                std::string file_stem{FileSystem::GetStem(file_path, true)};
                std::string file_extension{FileSystem::GetExtension(file_path)};
                while(FileSystem::Exists(file_path))
                {
                    file_path = std::format("{}/{}_{:#0}{}",
                        file_directory,
                        file_stem,
                        ++i,
                        file_extension);
                }
                break;
            }
        case OVERWRITE:
            print_warning("The file at '{}' will be overwritten", inOutputFilePath);
            break;
        }
    }
    std::string output{std::format("@{}#{}\n", mName, mIndex)};

    LOCK_THINGS;
    for(FAUTO [id, thing] : mThings)
    {
        if(Console::GetVariable("Theatre.debug_save_msgs")->int_value)
            { print_debug("Saving [{}, {}]", thing->name(), id()); }
        output += thing->GetVariables()->get_parsable_string();
    }
    return print_error_enum(FileSystem::try_WriteFileFromString(file_path, output));
}

bool Theatre::Startup()
{
    assert(mInitStatus == OK and m_pInitialState);
    if(mIsStarted)
        { return true; }

    LOCK_THINGS;
    LOCK_CALLSHEET;

    mName  = m_pInitialState->name;
    mIndex = m_pInitialState->index;

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

void Theatre::Draw(bool inRenderMainViewport)
{
    Shared<Console::Variable> var{nullptr};
    if(Console::GetVariable("Theatre.draw_3d", var) == OK)
        { gDebugEnable3DRendering = var->int_value; }
    if(Console::GetVariable("Theatre.draw_2d", var) == OK)
        { gDebugEnable2DRendering = var->int_value; }
    if(Console::GetVariable("Theatre.draw_text_new", var) == OK)
        { gDebugToggleTextRenderingMethod = var->int_value; }

    LOCK_THINGS;

    Light3D::ClearCounts();
    for(ID id : mLightIDs)
        { g_pRenderManager->GetAPI()->SetLight_TempBlinnPhongSolution(GetThinker<Light3D>(id)); }

    for(ID viewport_id : mViewportIDs)
    {
        auto viewport{GetThinker<Viewport>(viewport_id)};

        viewport->Framebuffer()->Bind();

        g_pRenderManager->GetAPI()->SetViewport({0, 0}, viewport->Size());
        g_pRenderManager->GetAPI()->SetClearColor(Settings::Graphics::ClearColor.glm());
        g_pRenderManager->GetAPI()->Clear();

        if(gDebugEnable3DRendering and not viewport->CurrentCamera3D().invalid())
            { Draw3DThinkers(GetThinker<Camera3D>(viewport->CurrentCamera3D())); }
        if(gDebugEnable2DRendering and not viewport->CurrentCamera2D().invalid())
            { Draw2DThinkers(GetThinker<Camera2D>(viewport->CurrentCamera2D())); }

        viewport->Framebuffer()->Unbind();
    }

    if(not inRenderMainViewport)
        { return; }

    g_pRenderManager->GetAPI()->SetViewport({0, 0}, MainWindow()->GetScale());
    g_pRenderManager->GetAPI()->SetClearColor(Settings::Graphics::ClearColor.glm());
    g_pRenderManager->GetAPI()->Clear();

    if(gDebugEnable3DRendering and not mRootViewportCurrentCamera3D.invalid())
        { Draw3DThinkers(GetThinker<Camera3D>(mRootViewportCurrentCamera3D)); }
    if(gDebugEnable2DRendering and not mRootViewportCurrentCamera3D.invalid())
        { Draw2DThinkers(GetThinker<Camera2D>(mRootViewportCurrentCamera2D)); }
}

Sarg Theatre::Name() const
{ return mName; }

uint Theatre::Index() const
{ return mIndex; }

bool Theatre::WasLoadedFromFile() const
{ return mWasLoadedFromFile; }

Sarg Theatre::TheatreFileDirectory() const
{ return mTheatreFileDirectory; }

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
        { data.emplace_back(*thing->GetVariables()); }
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
    else if(auto viewport{GetThinker<Viewport>(inViewportID)}; not viewport->uid().invalid() and ancestors.contains(inViewportID))
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

void Theatre::Draw3DThinkers(Shared<Camera3D> inCamera)
{
    if(not inCamera)
        { return; }

    LOCK_THINGS;

    FAUTO renderer_api{g_pRenderManager->GetAPI()};
    auto shader{renderer_api->GetShader(Shaders::Fullbright)};
    auto view_matrix{inCamera->ViewMatrix()};
    auto projection_matrix{inCamera->ProjectionMatrix()};

    auto missing_texture{GetResource<Texture>(UID::i_Missing)};
    auto mesh{GetResource<ArrayMesh>(UID::m_Error)};

    switch(inCamera->mEnvironment.mType)
    {
    case Environment::BG_SKYBOX:
        renderer_api->SetClearColor(Settings::Graphics::ClearColor.glm());
        renderer_api->Clear();
        renderer_api->SetWireframe(false);
        renderer_api->BindTexture(GetResource<Texture>(inCamera->mEnvironment.mSkyboxTextureID), 0);
        renderer_api->GetShader(Shaders::SkyBox)->Bind();
        renderer_api->GetShader(Shaders::SkyBox)->SetUniform("view_matrix", glm::mat4{glm::mat3{view_matrix}});
        renderer_api->GetShader(Shaders::SkyBox)->SetUniform("projection_matrix", projection_matrix);
        renderer_api->GetShader(Shaders::SkyBox)->SetUniform("skybox", 0);
        renderer_api->SetDepthMask(false);
        for(int i{0}; i < GetResource<ArrayMesh>(UID::m_Cube)->SurfaceCount(); ++i)
        	{ renderer_api->DrawIndexed(GetResource<ArrayMesh>(UID::m_Cube)->SurfaceGetVertexArray(i)); }
        renderer_api->SetDepthMask(true);
        break;
    case Environment::BG_CUSTOM_COLOR:
        renderer_api->SetClearColor(inCamera->mEnvironment.get_custom_color().glm());
        renderer_api->Clear();
        break;
    case Environment::BG_CLEAR_COLOR:
        renderer_api->SetClearColor(Settings::Graphics::ClearColor.glm());
        renderer_api->Clear();
        break;
    }

    for(ID v3d_id : mVisual3DIDs)
    {
        auto visual3d{GetThinker<Visual3D>(v3d_id)};
        if(DerivedFrom(v3d_id, ThingType::Light3D) or
            not visual3d->Visible()
            or not inCamera->LayersMask().contains(visual3d->Layers())
            or inCamera->EditorMeshInstanceID() == v3d_id)
                { continue; }

        glm::vec3 scale_vector{visual3d->GlobalScale()};

        if(visual3d->DerivedFrom(ThingType::MeshInstance3D))
        {
            auto mesh_instance{DCast<MeshInstance3D>(visual3d)};
            if(DCast<ArrayMesh>(mesh_instance->GetMesh()))
                { mesh = DCast<ArrayMesh>(mesh_instance->GetMesh()); }
            auto material{mesh->mMaterial};

            if(mesh_instance->GetMaterialOverride())
                { material = mesh_instance->GetMaterialOverride(); }

            auto diffuse_texture{material->DiffuseTexture()};
            auto specular_texture{material->SpecularTexture()};

            if(not material->DiffuseTexture()->uid().invalid() and not diffuse_texture->GetBuffer())
                { diffuse_texture = missing_texture; }

            if(not material->SpecularTexture()->uid().invalid() and not specular_texture->GetBuffer())
                { specular_texture = missing_texture; }

            shader = renderer_api->GetShader((material->mFullBright)
                ? Shaders::Fullbright
                : Shaders::BlinnPhong);

            bool use_diffuse  {renderer_api->BindTexture(diffuse_texture,  0)};
            bool use_specular {renderer_api->BindTexture(specular_texture, 1)};

            renderer_api->SetWireframe(Settings::Graphics::GlobalWireframe or mesh_instance->Wireframe());
            renderer_api->SetFramebufferSRGB(use_diffuse or use_specular);

            shader->SetUniform("current_material.texture_diffuse",  0);
            shader->SetUniform("current_material.texture_specular", 1);
            shader->SetUniform("current_material.use_diffuse",  use_diffuse);
            shader->SetUniform("current_material.use_specular", use_specular);
            shader->SetUniform("current_material.diffuse_color", material->mColor);
            shader->SetUniform("current_material.alpha", material->mAlpha);
            shader->SetUniform("current_material.specular_sharpness", material->mSpecularSharpness);
            shader->SetUniform("current_material.specular_strength", material->SpecularStrength());
        }
        else if(visual3d->DerivedFrom(ThingType::Sprite3D))
        {
            auto sprite{DCast<Sprite3D>(visual3d)};
            mesh = GetResource<ArrayMesh>(UID::m_Quad);

            auto texture{(not sprite->GetTexture())
                ? missing_texture
                : sprite->GetTexture()};

            shader = renderer_api->GetShader(Shaders::Fullbright);

            renderer_api->BindTexture(texture, 0);
            renderer_api->SetWireframe(Settings::Graphics::GlobalWireframe);
            renderer_api->SetFramebufferSRGB(true);

            shader->SetUniform("current_material.texture_diffuse", 0);
            shader->SetUniform("current_material.use_diffuse", true);
            shader->SetUniform("current_material.diffuse_color", {1.0f,1.0f,1.0f});

            auto height{texture->Format().height};
            auto width{texture->Format().width};
            if(width > height)
                { scale_vector *= glm::vec3{1.0f / ((float)height / width), 1.0f, 1.0f}; }
            else
                { scale_vector *= glm::vec3{1.0f, 1.0f / ((float)height / width), 1.0f}; }
        }

        if(not mesh->SurfaceCount())
            { mesh = GetResource<ArrayMesh>(UID::m_Error); }

        glm::mat4 model_matrix{visual3d->GlobalTransform().model_matrix()};

        shader->SetUniform("model_matrix", model_matrix);
        shader->SetUniform("normal_matrix", glm::mat3{glm::transpose(glm::inverse(model_matrix))});
        shader->SetUniform("projection_matrix", projection_matrix);
        shader->SetUniform("debug_output", static_cast<int>(gShaderDebugOutput));
        shader->SetUniform("point_lights_count", PointLight3D::GetCount());
        shader->SetUniform("spot_lights_count", SpotLight3D::GetCount());
        shader->SetUniform("directional_lights_count", DirectionalLight3D::GetCount());
        shader->SetUniform("view_matrix", view_matrix);
        shader->SetUniform("view_position", inCamera->GlobalPosition());
        shader->SetUniform("debug_highlight", visual3d->DebugHighlight());

        shader->Bind();
        for(int i{0}; i < mesh->SurfaceCount(); ++i)
        	{ renderer_api->DrawIndexed(mesh->SurfaceGetVertexArray(i)); }
        renderer_api->SetFramebufferSRGB(false);
    }
}

void Theatre::Draw2DThinkers(Shared<Camera2D> inCamera)
{
    if(not inCamera)
        { return; }

    LOCK_THINGS;

    FAUTO renderer_api{g_pRenderManager->GetAPI()};

    auto missing_texture{GetResource<Texture>(UID::i_Missing)};
    auto quad_mesh{GetResource<ArrayMesh>(UID::m_Quad)};

    for(ID v2d_id : mVisual2DIDs)
    {
        auto shader{renderer_api->GetShader(Shaders::Fast2D)};
        auto visual2d{GetThinker<Visual2D>(v2d_id)};
        if(not visual2d->Visible()
            or not inCamera->LayersMask().contains(visual2d->Layers()))
                { continue; }

        if(visual2d->DerivedFrom(ThingType::Sprite2D))
        {
            auto sprite{DCast<Sprite2D>(visual2d)};
            auto texture{sprite->GetTexture()};

            glm::vec2 texture_size{texture->Format().width, texture->Format().height};

            if(texture and not texture->GetBuffer())
                { texture = missing_texture; }

            auto size{sprite->GlobalScale() * texture_size};

            auto model_matrix{glm::translate(glm::mat4{1.0f}, glm::vec3{sprite->GlobalPosition(), 0.0f})};
            model_matrix = glm::rotate(model_matrix, sprite->GlobalRotation(), glm::vec3{0.0f, 0.0f, -1.0f});
            model_matrix = glm::scale(model_matrix, glm::vec3{size, 1.0f});

            bool use_texture {renderer_api->BindTexture(texture, 0)};

            renderer_api->SetWireframe(Settings::Graphics::GlobalWireframe or sprite->Wireframe());
            renderer_api->SetFramebufferSRGB(use_texture);

            shader->SetUniform("model_matrix", model_matrix);
            shader->SetUniform("projection_matrix", inCamera->ProjectionMatrix());
            shader->SetUniform("view_matrix", inCamera->ViewMatrix());
            shader->SetUniform("view_position", inCamera->GlobalPosition());
            shader->SetUniform("sprite_texture",  0);
            shader->SetUniform("use_texture",  use_texture);
            shader->SetUniform("sprite_color", glm::vec3{1.0f});
            shader->SetUniform("debug_highlight", sprite->DebugHighlight());

            shader->Bind();
            for(int i{0}; i < quad_mesh->SurfaceCount(); ++i)
            	{ renderer_api->DrawIndexed(quad_mesh->SurfaceGetVertexArray(i)); }
            renderer_api->SetFramebufferSRGB(false);
        }
        else if(visual2d->DerivedFrom(ThingType::Text2D))
        {
            auto text2d{DCast<Text2D>(visual2d)};
            auto font{GetResource<Font>(text2d->Font())};
            auto shader{renderer_api->GetShader(Shaders::Fonts)};

            glm::mat4 default_model{1.0f};
            default_model = glm::translate(default_model, {text2d->GlobalPosition(), 0.0f});
            default_model = glm::rotate(default_model, text2d->GlobalRotation(), {0.0f, 0.0f, -1.0f});
            default_model = glm::scale(default_model, {text2d->GlobalScale(), 0.0f});

            shader->SetUniform("debug_highlight", text2d->DebugHighlight());
            shader->SetUniform("projection_matrix", inCamera->ProjectionMatrix());
            shader->SetUniform("model_matrix", default_model);
            shader->SetUniform("view_matrix", inCamera->ViewMatrix());
            shader->SetUniform("view_position", inCamera->GlobalPosition());
            shader->SetUniform("glyph", 0);
            shader->Bind();

            if(!gDebugToggleTextRenderingMethod)
            {
                if(text2d->mDebugOutline)
                {
                    shader->SetUniform("debug_solid", 1);
                    shader->SetUniform("text_color", glm::vec3{1.0f, 0.4f, 1.0f});
                    renderer_api->SetWireframe(true);
                    renderer_api->DrawText(text2d->Text(), font, glm::vec2{0.0f}, glm::vec2{1.0f});
                }

                shader->SetUniform("debug_solid", text2d->mDebugSolid);
                shader->SetUniform("text_color", text2d->Color().glm());

                renderer_api->SetWireframe(Settings::Graphics::GlobalWireframe or text2d->Wireframe());
                renderer_api->DrawText(text2d->Text(), font, glm::vec2{0.0f}, glm::vec2{1.0f});
            }
            else
            {
                FAUTO text{text2d->Text()};
                glm::vec2 position{0.0f, 0.0f};
                auto glyph_top{font->GetGlyph('H').bitmap_top};
                for(auto c{text.cbegin()}; c != text.cend(); ++c)
                {
                    FAUTO glyph{font->GetGlyph(*c)};
                    if(!glyph.texture) { continue; }

                    glm::vec2 pos{
                        (c != text.cbegin()) * position.x + glyph.bitmap_left,
                        (c != text.cbegin()) * position.y - (glyph_top - glyph.bitmap_top),
                    },
                    scale{
                        glyph.bitmap_width,
                        glyph.bitmap_height,
                    };

                    glm::mat4 model_matrix{1.0f};
                    model_matrix = glm::translate(model_matrix, {text2d->GlobalPosition(), 0.0f});
                    model_matrix = glm::rotate(model_matrix, text2d->GlobalRotation(), {0.0f, 0.0f, -1.0f});
                    model_matrix = glm::scale(model_matrix, {text2d->GlobalScale(), 0.0f});

                    model_matrix = glm::translate(model_matrix, {pos, 0.0f});
                    model_matrix = glm::scale(model_matrix, {scale, 0.0f});

                    shader->SetUniform("debug_solid", 0);
                    shader->SetUniform("text_color", text2d->Color().glm());
                    shader->SetUniform("model_matrix", model_matrix);

                    renderer_api->BindTexture(glyph.texture, 0);
                    renderer_api->SetWireframe(false);

                    for(int i{0}; i < quad_mesh->SurfaceCount(); ++i)
                    	{ renderer_api->DrawIndexed(quad_mesh->SurfaceGetVertexArray(i)); }

                    if(text2d->mDebugSolid)
                    {
                        shader->SetUniform("debug_solid", 1);
                        if(text2d->Wireframe() and text2d->mDebugSolid)
                            { shader->SetUniform("text_color", glm::vec3{1.0f, 0.4f, 1.0f}); }
                        renderer_api->SetWireframe(Settings::Graphics::GlobalWireframe or text2d->Wireframe());
                        for(int i{0}; i < quad_mesh->SurfaceCount(); ++i)
                        	{ renderer_api->DrawIndexed(quad_mesh->SurfaceGetVertexArray(i)); }
                    }

                    position.x += (glyph.advance_x >> 6);
                    position.y -= (glyph.advance_y >> 6);
                }
            }
        }
    }
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

#include "./theatre.hpp"
#include "things/thing_data.hpp"
#include "models.hpp"
#include "images.hpp"
#include "fonts.hpp"
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
#include "things/resources/cubemap.hpp"
#include "things/resources/image.hpp"
#include "things/resources/material.hpp"
#include "things/resources/mesh.hpp"
#include "things/resources/font.hpp"
#include "managers/render_manager.hpp"
#include "managers/theatre_manager.hpp"
#include "rendering/renderer_api.hpp"
#include "rendering/shader.hpp"
#include "settings/graphics.hpp"
#include "console/console.hpp"

#define LOCK_THINGS LockGuard<RMutex> things_lock{mThingsMutex}

using namespace TheatreFile;

bool gDebugToggleTextRenderingMethod{false},
    gDebugEnable3DRendering{true},
    gDebugEnable2DRendering{true};

Theatre::Theatre() noexcept:
    m_pRootViewport{MakeShared<Viewport>()},
    m_pInitialState{MakeShared<TheatreFile::TheatreData>()} {}

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

    LoadCurrentTheatreData();
}

Error Theatre::LoadFile(Sarg inFilePath)
{
    Shutdown();
    if(not print_error_enum(TheatreFile::Load(inFilePath, m_pInitialState)))
        { return mInitStatus = ERR_INIT_FAILED; }
    mWasLoadedFromFile = true;
    mTheatreFileDirectory = FileSystem::GetDir(inFilePath, true);
    LoadCurrentTheatreData();
    return OK;
}

Error Theatre::LoadData(Farg<FileData> inData)
{
    if(not print_error_enum(TheatreFile::Load(inData, m_pInitialState)))
        { return mInitStatus = ERR_INIT_FAILED; }
    if((mWasLoadedFromFile = inData.has_filepath()))
        { mTheatreFileDirectory = FileSystem::GetDir(inData.filepath(), true); }
    LoadCurrentTheatreData();
    return OK;
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
        if(Console::try_GetVariable("Theatre.debug_save_msgs")->int_value)
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

    mName  = m_pInitialState->name;
    mIndex = m_pInitialState->index;

    CreateEmbeddedResources();

    m_pRootViewport = GetThinker<Viewport>(UID::o_RootViewport = CreateThingNoReady({ThingType::Viewport, "Root_Viewport"}));

    for(auto& thing_dat : *m_pInitialState)
        { CreateThingNoReady(thing_dat, false); }

    for(auto& thing_dat : *m_pInitialState)
    {
        if(auto found_it{mThings.find(thing_dat._uid)}; found_it != mThings.end())
            { found_it->second->SetVariables(thing_dat); }
    }

    auto uids{ThingIDs()};
    for(ID uid : uids)
        { mThings.at(uid)->Ready(); }

    mIsStarted = true;

    for(ID uid : GetChildren({}))
    {
        if(DerivedFrom(uid, ThingType::Actor3D))
        {
            auto actor{GetThinker<Actor3D>(uid)};
            actor->SetPosition(actor->Position());
            actor->SetQuaternion(actor->Quaternion());
            actor->SetScale(actor->Scale());
        }
        else if(DerivedFrom(uid, ThingType::Actor2D))
        {
            auto actor{GetThinker<Actor2D>(uid)};
            actor->SetPosition(actor->Position());
            actor->SetRotation(actor->Rotation());
            actor->SetScale(actor->Scale());
        }
    }

    return true;
}

bool Theatre::Shutdown()
{
    if(!mIsStarted)
        { return true; }
    LOCK_THINGS;
    for(ID uid : ThingIDs())
    {
        if(!mThings.contains(uid))
            { continue; }
        DestroyThing(uid);
    }
    mNames.clear();
    UID::Clear();
    mCallSheet.Clear();
    mThings.clear();
    return !(mIsStarted = false);
}

void Theatre::Draw()
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

        if(!viewport->Framebuffer())
            { continue; }

        viewport->Framebuffer()->Bind();

        g_pRenderManager->GetAPI()->SetViewport({0, 0}, viewport->Size());
        g_pRenderManager->GetAPI()->SetClearColor(Settings::Graphics::ClearColor.glm());
        g_pRenderManager->GetAPI()->Clear();

        if(gDebugEnable3DRendering)
            { Draw3DThinkers(viewport); }
        if(gDebugEnable2DRendering)
            { Draw2DThinkers(viewport); }

        viewport->Framebuffer()->Unbind();
    }
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

IdVec_t Theatre::ThingIDs()
{
    LockGuard<RMutex> lock{mThingsMutex};
    auto keys{std::views::keys(mThings)};
    return {keys.begin(), keys.end()};
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

ID Theatre::CreateThing(Farg<TheatreFile::ThingData> inData)
{
    LockGuard<RMutex> lock{mThingsMutex};
    ID output{CreateThingNoReady(inData)};
    GetThing(output)->Ready();
    return output;
}

Error Theatre::DestroyThing(ID inID)
{
    LOCK_THINGS;

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

Shared<Viewport> Theatre::GetRootViewport()
{ return m_pRootViewport; }

IdSet_arg Theatre::GetViewports()
{ return mViewportIDs; }

IdSet_arg Theatre::Get3DCameras()
{ return mCamera3DIDs; }

IdSet_arg Theatre::Get2DCameras()
{ return mCamera2DIDs; }

IdSet_t Theatre::GetChildren(ID inParentID)
{
    LockGuard<RMutex> callsheet_lock{mCallSheetMutex};
    return mCallSheet.Get(inParentID).children;
}

ID Theatre::GetParent(ID inChildID)
{
    LockGuard<RMutex> callsheet_lock{mCallSheetMutex};
    return mCallSheet.Get(inChildID).parent;
}

IdSet_t Theatre::GetAllChildren(ID inParentID)
{
    LockGuard<RMutex> callsheet_lock{mCallSheetMutex};
    return mCallSheet.Descendants(inParentID);
}

IdSet_t Theatre::GetAllParents(ID inChildID)
{
    LockGuard<RMutex> callsheet_lock{mCallSheetMutex};
    return mCallSheet.Ancestors(inChildID);
}

Error Theatre::SetParent(ID inChildID, ID inParentID)
{
    LockGuard<RMutex> callsheet_lock{mCallSheetMutex};
    LOCK_THINGS;

    // NOTE: Probably redundant. Too bad!
    if(not mCallSheet.Has(inChildID)
        or (not inParentID.invalid() and not mCallSheet.Has(inParentID)))
            { return print_error_enum(ERR_NOT_FOUND); }

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
    LockGuard<RMutex> callsheet_lock{mCallSheetMutex};
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

bool Theatre::LoadCurrentTheatreData()
{
    if(mIsStarted)
        { return false; }

    mName  = m_pInitialState->name;
    mIndex = m_pInitialState->index;

    for(auto& thing_data : *m_pInitialState)
        { SetupUID(thing_data); }

    for(auto& thing_data : *m_pInitialState)
        { SetupOwnership(thing_data, true); }

    mInitStatus = OK;
    return true;
}

void Theatre::SetupUID(ThingData& ioData)
{
    ioData._uid = UID::Generate();
    if(ioData.name.empty())
        { ioData.name = "Untitled_Thing"; }
    if(not ioData.name.empty())
        { mNames[ioData.name] = ioData._uid; }
    if(ThingFactory::IsThinker(ioData.type))
        { mCallSheet.Add(ioData._uid); }
}

void Theatre::SetupOwnership(ThingData& ioData, bool isStartup)
{
    LockGuard<RMutex> callsheet_lock{mCallSheetMutex};

    if(ID parent{GetUID(ioData.parent_variable.value)}; not parent.invalid())
    {
        if(not mCallSheet.Has(parent) and isStartup)
            { mCallSheet.Add(parent); }
        if(not mCallSheet.Reparent(ioData._uid, parent))
            { mCallSheet.Add(ioData._uid, parent); }
    }
    for(FAUTO child_var : ioData.children_variables)
    {
        ID child{GetUID(child_var.value)};
        if(not child.invalid() and not mCallSheet.Reparent(child, ioData._uid) and isStartup)
            { mCallSheet.Add(child, ioData._uid); }
    }

    if(not isStartup)
        { return; }

    ioData.parent_variable.clear();
    ioData.children_variables.clear();

    if(FAUTO node{mCallSheet.Get(ioData._uid)}; not node.invalid())
    {
        ioData.parent_variable = {"Parent", GetName(node.parent), ThingVarType::Parent};

        for(ID child : node.children)
            { ioData.children_variables.emplace_back("Child", GetName(child), ThingVarType::Child); }
    }
}

void Theatre::CreateEmbeddedResources()
{
    UID::f_Audiowide =
        ResourceDatabase::Register(Font::CreateFromMemory(Fonts::Audiowide,
            std::size(Fonts::Audiowide)), "Audiowide");
    UID::f_DejaVuSans =
        ResourceDatabase::Register(Font::CreateFromMemory(Fonts::DejaVuSans,
            std::size(Fonts::DejaVuSans)), "DejaVuSans");
    UID::f_Verdana =
        ResourceDatabase::Register(Font::CreateFromMemory(Fonts::Verdana,
            std::size(Fonts::Verdana)), "Verdana");
    UID::m_Error =
        ResourceDatabase::Register(Mesh::CreateFromMemory(Models::Error,
            std::size(Models::Error), Mesh::MODEL_OBJ), "ErrorModel");
    UID::m_Cube =
        ResourceDatabase::Register(Mesh::CreateFromMemory(Models::Cube,
            std::size(Models::Cube), Mesh::MODEL_OBJ), "DefaultCube");
    UID::m_Quad =
        ResourceDatabase::Register(Mesh::CreateFromMemory(Models::Quad,
            std::size(Models::Quad), Mesh::MODEL_OBJ), "DefaultQuad");
    UID::m_Ramiel =
        ResourceDatabase::Register(Mesh::CreateFromMemory(Models::Ramiel,
            std::size(Models::Ramiel), Mesh::MODEL_OBJ), "RamielModel");
    UID::m_Camera3D =
        ResourceDatabase::Register(Mesh::CreateFromMemory(Models::Camera,
            std::size(Models::Camera), Mesh::MODEL_OBJ), "CameraModel");
    UID::m_DebugAxis =
        ResourceDatabase::Register(Mesh::CreateFromMemory(Models::DebugAxis,
            std::size(Models::DebugAxis), Mesh::MODEL_OBJ), "3DAxisModel");
    UID::i_Missing =
        ResourceDatabase::Register(Image::CreateFromData(Images::Missing,
            std::size(Images::Missing)), "MissingImage");
    UID::i_LightDebug =
        ResourceDatabase::Register(Image::CreateFromData(Images::LightDebug,
            std::size(Images::LightDebug)), "LightImage");
    UID::i_COMP04_5 =
        ResourceDatabase::Register(Image::CreateFromData(Images::COMP04_5,
            std::size(Images::COMP04_5)), "DoomImage");
    UID::i_LolBit =
        ResourceDatabase::Register(Image::CreateFromData(Images::LolBit,
            std::size(Images::LolBit)), "LolBitImage");
    UID::t_ShittySkybox =
        ResourceDatabase::Register(Cubemap::CreateFromImages({
            Image::CreateFromData(Images::SkyboxXp, std::size(Images::SkyboxXp)),
            Image::CreateFromData(Images::SkyboxXn, std::size(Images::SkyboxXn)),
            Image::CreateFromData(Images::SkyboxYp, std::size(Images::SkyboxYp)),
            Image::CreateFromData(Images::SkyboxYn, std::size(Images::SkyboxYn)),
            Image::CreateFromData(Images::SkyboxZp, std::size(Images::SkyboxZp)),
            Image::CreateFromData(Images::SkyboxZn, std::size(Images::SkyboxZn))}),
        "ShittySkybox");
}

ID Theatre::CreateThingNoReady(Farg<TheatreFile::ThingData> inData, bool doSetup)
{ auto data{inData}; return CreateThingNoReady(data, doSetup); }

ID Theatre::CreateThingNoReady(TheatreFile::ThingData& ioData, bool doSetup)
{
    LockGuard<RMutex> lock{mThingsMutex};

    bool is_player{ThingFactory::IsDerivedFrom(ioData.type, ThingType::NostalgiaPlayer3D)};

    if(is_player and ThingExists(UID::o_Player))
        { print_warning("Only one player at a time, please!"); return UID::o_Player; }

    if(doSetup)
    {
        if(ThingExists(ioData.name))
            { return GetUID(ioData.name); }
        SetupUID(ioData);
        SetupOwnership(ioData);
    }

    auto& thing{mThings[ioData._uid] = ThingFactory::MakeThing(ioData.type, ioData.name, ioData._uid)};
    thing->Init();
    if(doSetup)
        { thing->SetVariables(ioData); }

    if(is_player)
        { UID::o_Player = ioData._uid; }
    else if(thing->DerivedFrom(ThingType::Viewport))
        { mViewportIDs.insert(thing->uid()); }
    else if(thing->DerivedFrom(ThingType::Camera3D))
        { mCamera3DIDs.insert(thing->uid()); }
    else if(thing->DerivedFrom(ThingType::Camera2D))
        { mCamera2DIDs.insert(thing->uid()); }
    else if(thing->DerivedFrom(ThingType::Visual3D))
    {
        mVisual3DIDs.insert(thing->uid());
        if(thing->DerivedFrom(ThingType::Light3D))
            { mLightIDs.insert(thing->uid()); }
    }
    else if(thing->DerivedFrom(ThingType::Visual2D))
        { mVisual2DIDs.insert(thing->uid()); }

    return thing->uid();
}

Error Theatre::DestroyThingOnly(ID inID)
{
    LockGuard<RMutex> lock{mThingsMutex};
    LockGuard<RMutex> callsheet_lock{mCallSheetMutex};

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
    UID::Erase(inID());
    mViewportIDs.erase(inID);
    mVisual2DIDs.erase(inID);
    mVisual3DIDs.erase(inID);
    mCamera3DIDs.erase(inID);
    mCamera2DIDs.erase(inID);
    mLightIDs.erase(inID);
    mCallSheet.Remove(inID);
    mThings.erase(inID);
    return OK;
}

void Theatre::Draw3DThinkers(Shared<Viewport> inViewport)
{
    LOCK_THINGS;

    if(inViewport->CurrentCamera3D().invalid()
        and not inViewport->SetCurrentCamera3D())
            { return; }

    FAUTO renderer_api{g_pRenderManager->GetAPI()};
    auto shader{renderer_api->GetShader(Shaders::Fullbright)};
    auto camera{GetThinker<Camera3D>(inViewport->CurrentCamera3D())};
    auto view_matrix{camera->ViewMatrix()};
    auto projection_matrix{camera->ProjectionMatrix()};

    auto missing_texture{GetResource<Texture>(UID::i_Missing)};
    auto mesh{GetResource<Mesh>(ID::Invalid)};

    switch(camera->mEnvironment.mType)
    {
    case Environment::BG_SKYBOX:
        renderer_api->SetClearColor(Settings::Graphics::ClearColor.glm());
        renderer_api->Clear();
        renderer_api->SetWireframe(false);
        renderer_api->BindTexture(GetResource<Texture>(camera->mEnvironment.mSkyboxTextureID), 0);
        renderer_api->GetShader(Shaders::SkyBox)->Bind();
        renderer_api->GetShader(Shaders::SkyBox)->SetUniform("view_matrix", glm::mat4{glm::mat3{view_matrix}});
        renderer_api->GetShader(Shaders::SkyBox)->SetUniform("projection_matrix", projection_matrix);
        renderer_api->GetShader(Shaders::SkyBox)->SetUniform("skybox", 0);
        renderer_api->DrawSkybox(GetResource<Mesh>(UID::m_Cube)->MeshData());
        break;
    case Environment::BG_CUSTOM_COLOR:
        renderer_api->SetClearColor(camera->mEnvironment.get_custom_color().glm());
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
            or not camera->LayersMask().contains(visual3d->Layers())
            or camera->EditorMeshInstanceID() == v3d_id)
                { continue; }

        glm::vec3 scale_vector{visual3d->GlobalScale()};

        if(visual3d->DerivedFrom(ThingType::MeshInstance3D))
        {
            auto mesh_instance{DCast<MeshInstance3D>(visual3d)};
            if(mesh_instance->MeshID().invalid())
                { continue; }
            mesh = GetResource<Mesh>(mesh_instance->MeshID());
            auto material{GetResource<Material>(mesh->MaterialID())};

            if(!mesh_instance->MaterialOverrideID().invalid())
                { material = GetResource<Material>(mesh_instance->MaterialOverrideID()); }

            auto diffuse_texture{GetResource<Texture>(material->DiffuseTextureID())};
            auto specular_texture{GetResource<Texture>(material->SpecularTextureID())};

            if(!material->DiffuseTextureID().invalid() and !diffuse_texture->GetBuffer())
                { diffuse_texture = missing_texture; }

            if(!material->SpecularTextureID().invalid() and !specular_texture->GetBuffer())
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
            mesh = GetResource<Mesh>(UID::m_Quad);

            auto texture{(sprite->TextureID().invalid())
                ? missing_texture
                : GetResource<Texture>(sprite->TextureID())};

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

        if(not mesh->MeshData())
            { mesh = GetResource<Mesh>(UID::m_Error); }

        glm::mat4 model_matrix{visual3d->GlobalTransform().model_matrix()};

        shader->SetUniform("model_matrix", model_matrix);
        shader->SetUniform("normal_matrix", glm::mat3{glm::transpose(glm::inverse(model_matrix))});
        shader->SetUniform("projection_matrix", projection_matrix);
        shader->SetUniform("debug_output", static_cast<int>(gShaderDebugOutput));
        shader->SetUniform("point_lights_count", PointLight3D::GetCount());
        shader->SetUniform("spot_lights_count", SpotLight3D::GetCount());
        shader->SetUniform("directional_lights_count", DirectionalLight3D::GetCount());
        shader->SetUniform("view_matrix", view_matrix);
        shader->SetUniform("view_position", camera->GlobalPosition());

        glm::vec4 debug_highlight{visual3d->mDebugHighlight};
        debug_highlight *= (visual3d->mIsHoveredInDebugger or visual3d->mOverrideEnableDebugHighlight);
        shader->SetUniform("debug_highlight", debug_highlight);

        shader->Bind();
        renderer_api->DrawIndexed(mesh->MeshData());
        renderer_api->SetFramebufferSRGB(false);
    }
}

void Theatre::Draw2DThinkers(Shared<Viewport> inViewport)
{
    LOCK_THINGS;

    if(inViewport->uid() != UID::o_RootViewport
        and inViewport->CurrentCamera2D().invalid()
        and not inViewport->SetCurrentCamera2D())
            { return; }

    FAUTO renderer_api{g_pRenderManager->GetAPI()};
    auto camera{GetThinker<Camera2D>(inViewport->CurrentCamera2D())};

    auto missing_texture{GetResource<Texture>(UID::i_Missing)};
    auto quad_mesh{GetResource<Mesh>(UID::m_Quad)};

    for(ID v2d_id : mVisual2DIDs)
    {
        auto shader{renderer_api->GetShader(Shaders::Fast2D)};
        auto visual2d{GetThinker<Visual2D>(v2d_id)};
        if(not visual2d->Visible()
            or not camera->LayersMask().contains(visual2d->Layers()))
                { continue; }

        if(visual2d->DerivedFrom(ThingType::Sprite2D))
        {
            auto sprite{DCast<Sprite2D>(visual2d)};
            auto texture{GetResource<Texture>(sprite->TextureID())};

            glm::vec2 texture_size{texture->Format().width, texture->Format().height};

            if(!sprite->TextureID().invalid() and !texture->GetBuffer())
                { texture = missing_texture; }

            auto size{sprite->GlobalScale() * texture_size};

            auto model_matrix{glm::translate(glm::mat4{1.0f}, glm::vec3{sprite->GlobalPosition(), 0.0f})};
            model_matrix = glm::rotate(model_matrix, sprite->GlobalRotation(), glm::vec3{0.0f, 0.0f, -1.0f});
            model_matrix = glm::scale(model_matrix, glm::vec3{size, 1.0f});

            bool use_texture {renderer_api->BindTexture(texture, 0)};

            renderer_api->SetWireframe(Settings::Graphics::GlobalWireframe or sprite->Wireframe());
            renderer_api->SetFramebufferSRGB(use_texture);

            shader->SetUniform("model_matrix", model_matrix);
            shader->SetUniform("projection_matrix", camera->ProjectionMatrix());
            shader->SetUniform("view_matrix", camera->ViewMatrix());
            shader->SetUniform("view_position", camera->GlobalPosition());
            shader->SetUniform("sprite_texture",  0);
            shader->SetUniform("use_texture",  use_texture);
            shader->SetUniform("sprite_color", glm::vec3{1.0f});

            glm::vec4 debug_highlight{sprite->mDebugHighlight};
            debug_highlight *= (sprite->mIsHoveredInDebugger);
            shader->SetUniform("debug_highlight", debug_highlight);

            shader->Bind();
            renderer_api->DrawIndexed(quad_mesh->MeshData());
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

            shader->SetUniform("debug_highlight", text2d->mDebugHighlight
                * static_cast<float>(text2d->mIsHoveredInDebugger));
            shader->SetUniform("projection_matrix", camera->ProjectionMatrix());
            shader->SetUniform("model_matrix", default_model);
            shader->SetUniform("view_matrix", camera->ViewMatrix());
            shader->SetUniform("view_position", camera->GlobalPosition());
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
                    renderer_api->DrawIndexed(quad_mesh->MeshData());

                    if(text2d->mDebugSolid)
                    {
                        shader->SetUniform("debug_solid", 1);
                        if(text2d->Wireframe() and text2d->mDebugSolid)
                            { shader->SetUniform("text_color", glm::vec3{1.0f, 0.4f, 1.0f}); }
                        renderer_api->SetWireframe(Settings::Graphics::GlobalWireframe or text2d->Wireframe());
                        renderer_api->DrawIndexed(quad_mesh->MeshData());
                    }

                    position.x += (glyph.advance_x >> 6);
                    position.y -= (glyph.advance_y >> 6);
                }
            }
        }
    }
}

#include "theatre.hpp"
#include "parser.hpp"
#include "thing_factory.hpp"
#include "core/uid.hpp"
#include "filesystem/file_data.hpp"
#include "managers/render_manager.hpp"
#include "rendering/renderer_api.hpp"
#include "rendering/shader.hpp"
#include "settings/graphics.hpp"
#include "theatre/things/thing.hpp"
#include "theatre/things/thinkers/thinker.hpp"
#include "theatre/things/thinkers/3d/light_3d.hpp"
#include "theatre/things/thinkers/3d/mesh_instance_3d.hpp"
#include "theatre/things/thinkers/3d/camera_3d.hpp"
#include "theatre/things/thinkers/3d/visual_3d.hpp"
#include "theatre/things/thinkers/viewport.hpp"
#include "theatre/things/resources/resource.hpp"
#include "theatre/things/resources/texture.hpp"
#include "theatre/things/resources/material.hpp"
#include "theatre/things/resources/mesh.hpp"
#include <ranges>

using namespace TheatreFile;

Theatre::Theatre(Shared<TheatreData> inData) noexcept:
    m_pRegistry{MakeShared<VariableRegistry>()},
    m_pInitialState{inData},
    mInitStatus{OK} {}

Theatre::Theatre(Farg<FileData> inData) noexcept:
    m_pRegistry{MakeShared<VariableRegistry>()},
    m_pInitialState{MakeShared<TheatreData>()}
{
    TokenArray tokens{};
    if(!print_error_enum(inData.Status()))
        { mInitStatus = ERR_DATA_LOAD; }
    else if(!print_error_enum(Lexer(inData, tokens)))
        { mInitStatus = ERR_THEATRE_LEXER; }
    else if(!print_error_enum(Parser(tokens, m_pInitialState)))
        { mInitStatus = ERR_THEATRE_PARSER; }
    else { mInitStatus = OK; }
    print_error_enum(mInitStatus);
}

Theatre::~Theatre() noexcept
{ Shutdown(); }

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

bool Theatre::Startup()
{
    assert(mInitStatus == OK and m_pInitialState and m_pRegistry);
    if(mIsStarted)
        { return false; }

    LockGuard<RMutex> lock{mThingsMutex};

    mUIDs.Clear();
    mCallSheet.Clear();
    m_pRegistry->ClearIDs();
    m_pRegistry->Init();

    CreateEmbeddedResources();

    for(auto& thing_dat : *m_pInitialState)
        { SetRegistryAndUID(thing_dat); }

    for(auto& thing_dat : *m_pInitialState)
        { CreateThingNoReady(thing_dat); }

    if(!mThings.contains(UID::a_Player))
    {
        CreateThingNoReady({ThingType::NostalgiaPlayer3D,
            "Default NostalgiaPlayer3D",
            {},
            UID::a_Player});
    }

    if(!mThings.contains(UID::a_Global3DViewport))
    {
        CreateThingNoReady({ThingType::Viewport,
            "Global 3D Viewport",
            {},
            UID::a_Global3DViewport});
    }

    if(!mThings.contains(UID::a_Global2DViewport))
    {
        CreateThingNoReady({ThingType::Viewport,
            "Global 2D Viewport",
            {},
            UID::a_Global2DViewport});
    }

    mViewportIDs.insert(UID::a_Global3DViewport);
    mViewportIDs.insert(UID::a_Global2DViewport);

    for(auto& [id, thing] : mThings)
        { thing->Ready(); }

    return mIsStarted = true;
}

bool Theatre::Shutdown()
{
    if(!mIsStarted)
        { return false; }
    LockGuard<RMutex> lock{mThingsMutex};
    for(auto it{mThings.begin()}; it != mThings.end();)
        { it = DestroyThing(it); }
    m_pRegistry->ClearIDs();
    mUIDs.Clear();
    mCallSheet.Clear();
    return !(mIsStarted = false);
}

void Theatre::Draw()
{
    LockGuard<RMutex> lock{mThingsMutex};
    Light3D::ClearCounts();
    for(ID id : mLightIDs)
        { g_pRenderManager->GetAPI()->SetLight_TempBlinnPhongSolution(GetThinker<Light3D>(id)); }
    for(ID viewport_id : mViewportIDs)
    {
        auto viewport{GetThinker<Viewport>(viewport_id)};
        auto camera_type{TypeOf(viewport->CurrentCamera())};

        if(!viewport->Framebuffer())
            { continue; }

        viewport->Framebuffer()->Bind();
        g_pRenderManager->GetAPI()->SetViewport({0, 0}, viewport->Size());

        if(ThingFactory::IsDerivedFrom(camera_type, ThingType::Camera3D))
            { Draw3DThinkers(GetThinker<Camera3D>(viewport->CurrentCamera())); }
        else if(ThingFactory::IsDerivedFrom(camera_type, ThingType::Camera2D))
            { /*Draw2DThinkers(GetThinker<Camera2D>(viewport->CurrentCamera()));*/ }

        viewport->Framebuffer()->Unbind();
    }
}

Error Theatre::InitStatus() const
{ return mInitStatus; }

bool Theatre::IsStarted() const
{ return mIsStarted; }

Farg<VariableRegistry> Theatre::Registry() const
{ return *m_pRegistry; }

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

IdSet_arg Theatre::ViewportIDs()
{
    LockGuard<RMutex> lock{mThingsMutex};
    return mViewportIDs;
}

bool Theatre::ThingExists(ID inID)
{
    LockGuard<RMutex> lock{mThingsMutex};
    return mThings.contains(inID);
}

FPID Theatre::TypeOf(ID inID)
{
    LockGuard<RMutex> lock{mThingsMutex};
    if(auto found_it{mThings.find(inID)}; found_it != mThings.end())
        { return found_it->second->type(); }
    return ThingType::Invalid;
}

bool Theatre::DerivedFrom(ID inID, FPID inType)
{
    LockGuard<RMutex> lock{mThingsMutex};
    if(auto found_it{mThings.find(inID)}; found_it != mThings.end())
        { return ThingFactory::IsDerivedFrom(found_it->second->mType, inType); }
    return false;
}

Error Theatre::ChangeThingID(ID inOldID, ID inNewID)
{
    LockGuard<RMutex> lock{mThingsMutex};
    if(mThings.contains(inNewID))
        { return ERR_ALREADY_EXISTS; }
    else if(!mThings.contains(inOldID))
        { return ERR_NOT_FOUND; }
    auto found_it{mThings.extract(inOldID)};
    found_it.mapped()->mUID = inNewID;
    found_it.key() = inNewID;
    mThings.insert(std::move(found_it));
    return OK;
}

ID Theatre::CreateThing(Farg<TheatreFile::ThingData> inData)
{
    LockGuard<RMutex> lock{mThingsMutex};
    ID output{CreateThingNoReady(inData)};
    mThings.at(output)->Ready();
    return output;
}

Error Theatre::DestroyThing(ID inID)
{
    LockGuard<RMutex> lock{mThingsMutex};
    return (DestroyThing(mThings.find(inID)) != mThings.end())
        ? OK
        : (mThings.empty())
            ? ERR_EMPTY
            : ERR_NOT_FOUND;
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
    LockGuard<RMutex> things_lock{mThingsMutex};

    // NOTE: Probably redundant. Too bad!
    if(!mCallSheet.Has(inChildID)
        or (!inParentID.invalid() and !mCallSheet.Has(inParentID)))
            { return print_error_enum(ERR_NOT_FOUND); }

    auto old_parent_id{mCallSheet.Get(inChildID).parent};
    auto old_ancestors{GetAllParents(inChildID)};

    if(auto status{mCallSheet.Reparent(inChildID, inParentID)}; !status)
        { return print_error_enum(status); }

    auto new_ancestors{GetAllParents(inChildID)};
    auto descendants{GetAllChildren(inParentID)};

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
    LockGuard<RMutex> things_lock{mThingsMutex};

    auto parent{mCallSheet.Get(mCallSheet.Get(inChildID).parent)};

    if(!mCallSheet.Has(inChildID) or !parent.children.contains(inChildID))
        { return ERR_NOT_FOUND; }

    return SetParent(inChildID, parent.parent);
}

Shared<Thing> Theatre::GetThing(ID inID)
{
    LockGuard<RMutex> lock{mThingsMutex};
    if(auto found_it{mThings.find(inID)};
        found_it != mThings.end())
    { return found_it->second; }
    return MakeShared<Thing>();
}

Shared<Resource> Theatre::GetResource(ID inID)
{
    LockGuard<RMutex> lock{mThingsMutex};
    if(auto found_it{mThings.find(inID)};
        found_it != mThings.end())
    {
        if(ThingFactory::IsResource(found_it->second->type()))
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
        if(ThingFactory::IsThinker(found_it->second->type()))
            { return DCast<Thinker>(found_it->second); }
    }
    return MakeShared<Thinker>();
}

void Theatre::SetRegistryAndUID(ThingData& ioData)
{
    if(ThingFactory::IsDerivedFrom(ioData.type, ThingType::NostalgiaPlayer3D))
        { ioData.uid = UID::a_Player; }
    if(ioData.uid.invalid())
        { ioData.uid = mUIDs.Generate(); }
    if(!mUIDs.Contains(ioData.uid[]))
        { mUIDs.Push(ioData.uid[]); }
    if(!m_pRegistry->HasID(ioData.name))
        { m_pRegistry->RegisterID(ioData.name, ioData.uid[]); }
    if(!mCallSheet.Has(ioData.uid))
        { mCallSheet.Add(ioData.uid); }
    ioData.theatre_registry = m_pRegistry;
}

void Theatre::CreateEmbeddedResources()
{
    // "Audiowide" UID::f_Audiowide
    // "DejaVuSans" UID::f_DejaVuSans
    // "Verdana" UID::f_Verdana
    CreateThingNoReady({ThingType::Mesh,    "ErrorModel",{},          UID::m_Error});
    CreateThingNoReady({ThingType::Mesh,    "DefaultCube",{},         UID::m_Cube});
    CreateThingNoReady({ThingType::Mesh,    "RamielModel",{},         UID::m_Ramiel});
    CreateThingNoReady({ThingType::Mesh,    "CameraModel",{},         UID::m_Camera3D});
    CreateThingNoReady({ThingType::Texture, "MissingTexture",{},      UID::t_Missing});
    CreateThingNoReady({ThingType::Texture, "LightTexture",{},        UID::t_LightDebug});
    CreateThingNoReady({ThingType::Texture, "DoomTexture",{},         UID::t_COMP04_5});
    CreateThingNoReady({ThingType::Texture, "LolBitTexture",{},       UID::t_LolBit});
    CreateThingNoReady({ThingType::Texture,
        "ShittySkybox",
        {{"Type",m_pRegistry->GetEnumName(TextureType::TEXTURE_TYPE_CUBE),ThingVarType::Enum}},
        UID::t_ShittySkybox});
}

ID Theatre::CreateThingNoReady(Farg<TheatreFile::ThingData> inData)
{ auto data{inData}; return CreateThingNoReady(data); }

ID Theatre::CreateThingNoReady(TheatreFile::ThingData& ioData)
{
    LockGuard<RMutex> lock{mThingsMutex};

    SetRegistryAndUID(ioData);

    auto& thing{mThings[ioData.uid] = ThingFactory::MakeThing(ioData.type)};
    thing->SetVariables(ioData);

    if(ThingFactory::IsDerivedFrom(ioData.type, ThingType::Thinker))
    {
        auto thinker{DCast<Thinker>(thing)};
        if(!mCallSheet.Reparent(thinker->uid(), thinker->Parent().id))
            { mCallSheet.Add(thinker->uid(), thinker->Parent().id); }
        for(FAUTO child : thinker->Children())
        {
            if(!mCallSheet.Reparent(child.id, thinker->uid()))
                { print_warning("Invalid Child UID: {}", child.id[]); }
        }
    }

    if(ThingFactory::IsDerivedFrom(thing->type(), ThingType::Viewport))
        { mViewportIDs.insert(thing->uid()); }
    else if(ThingFactory::IsDerivedFrom(thing->type(), ThingType::Visual3D))
    {
        mVisual3DIDs.insert(thing->uid());
        if(ThingFactory::IsDerivedFrom(thing->type(), ThingType::Light3D))
            { mLightIDs.insert(thing->uid()); }
    }
    else if(ThingFactory::IsDerivedFrom(thing->type(), ThingType::Visual2D))
        { mVisual2DIDs.insert(thing->uid()); }
    else if(ThingFactory::IsDerivedFrom(thing->type(), ThingType::NostalgiaPlayer3D))
        { ChangeThingID(thing->uid(), UID::a_Player); }

    return thing->uid();
}

Theatre::Things_t::iterator Theatre::DestroyThing(Things_t::iterator inIterator)
{
    LockGuard<RMutex> lock{mThingsMutex};
    if(inIterator == mThings.end())
        { return inIterator; }
    if(!m_pRegistry->RemoveID(inIterator->first[])
        and !m_pRegistry->RemoveID(inIterator->second->name()))
        { print_warning("Unable to remove Thing#{} from the variable registry!", inIterator->first[]); }
    mCallSheet.Remove(inIterator->first);
    mUIDs.Erase(inIterator->first[]);
    mViewportIDs.erase(inIterator->first);
    mVisual2DIDs.erase(inIterator->first);
    mVisual3DIDs.erase(inIterator->first);
    mLightIDs.erase(inIterator->first);
    inIterator->second->Shutdown();
    return mThings.erase(inIterator);
}

void Theatre::Draw3DThinkers(Shared<Camera3D> inCamera)
{
    LockGuard<RMutex> lock{mThingsMutex};
    FAUTO renderer_api{g_pRenderManager->GetAPI()};

    switch(inCamera->mEnvironment.mType)
    {
    case Environment::BG_SKYBOX:
        renderer_api->SetClearColor(Settings::Graphics::ClearColor.glm());
        renderer_api->Clear();
        renderer_api->SetWireframe(false);
        renderer_api->BindTexture(
            GetResource<Texture>(inCamera->mEnvironment.mSkyboxTextureID)->GetBuffer(),
            0);
        renderer_api->GetShader(Shaders::SkyBox)->Bind();
        renderer_api->GetShader(Shaders::SkyBox)->SetUniform("view_matrix", glm::mat4{glm::mat3{inCamera->ViewMatrix()}});
        renderer_api->GetShader(Shaders::SkyBox)->SetUniform("projection_matrix", inCamera->ProjectionMatrix());
        renderer_api->GetShader(Shaders::SkyBox)->SetUniform("skybox", 0);
        renderer_api->DrawSkybox(GetResource<Mesh>(UID::m_Cube)->MeshData());
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
        // HACKY SOLUTION UNTIL I IMPLEMENT SCENE TREES
        auto parent{GetThinker<Actor3D>(visual3d->Parent().id)};
        if(!inCamera->LayersMask().contains(visual3d->Layers())
            or parent->uid() == inCamera->uid())
            { continue; }
        else if(ThingFactory::IsDerivedFrom(visual3d->type(), ThingType::MeshInstance3D))
        {
            auto mesh_instance{DCast<MeshInstance3D>(visual3d)};
            auto mesh{GetResource<Mesh>(mesh_instance->MeshID()[])};
            auto material{GetResource<Material>(mesh->MaterialID())};
            if(!mesh_instance->MaterialOverrideID().invalid())
                { material = GetResource<Material>(mesh_instance->MaterialOverrideID()); }
            FAUTO renderer_api{g_pRenderManager->GetAPI()};
            auto shader{renderer_api->GetShader((material->mFullBright) ? Shaders::Fullbright : Shaders::BlinnPhong)};

            if(mesh->uid().invalid())
                { mesh = GetResource<Mesh>(UID::m_Error); }

            // https://www.reddit.com/r/opengl/comments/t01fwn/comment/hy7mezc
            glm::mat4 scaleMat     {glm::scale(glm::mat4{1.0f}, parent->Scale() * mesh_instance->Scale())};
            glm::mat4 rotMat       {glm::mat4_cast(parent->Quaternion())};
            glm::mat4 rotMat2      {glm::mat4_cast(mesh_instance->Quaternion())};
            glm::mat4 transMat     {glm::translate(glm::mat4{1.0f}, parent->Position() + mesh_instance->Position())};
            glm::mat4 model_matrix {transMat * rotMat * rotMat2 * scaleMat};
            glm::mat4 projection_matrix{inCamera->ProjectionMatrix()};
            glm::mat4 view_matrix{inCamera->ViewMatrix()};

            renderer_api->SetFramebufferSRGB(!material->mDontUseTexture);
            renderer_api->SetWireframe(Settings::Graphics::GlobalWireframe or mesh_instance->Wireframe());
            renderer_api->BindTexture(GetResource<Texture>(material->DiffuseTextureID()[])->GetBuffer(), 0);
            renderer_api->BindTexture(GetResource<Texture>(material->SpecularTextureID()[])->GetBuffer(), 1);

            shader->SetUniform("model_matrix", model_matrix);
            shader->SetUniform("normal_matrix", glm::mat3{glm::transpose(glm::inverse(model_matrix))});
            shader->SetUniform("projection_matrix", projection_matrix);
            shader->SetUniform("debug_output", static_cast<int>(gShaderDebugOutput));
            shader->SetUniform("debug_highlight", parent->mDebugHighlight * parent->mDebugHighlight.a);
            shader->SetUniform("point_lights_count", PointLight3D::GetCount());
            shader->SetUniform("spot_lights_count", SpotLight3D::GetCount());
            shader->SetUniform("directional_lights_count", DirectionalLight3D::GetCount());
            shader->SetUniform("view_matrix", view_matrix);
            shader->SetUniform("view_position", inCamera->Position());
            shader->SetUniform("current_material.texture_diffuse",  0);
            shader->SetUniform("current_material.texture_specular", 1);
            shader->SetUniform("current_material.use_textures", !material->mDontUseTexture);
            shader->SetUniform("current_material.diffuse_color", material->mColor);
            shader->SetUniform("current_material.alpha", material->mAlpha);
            shader->SetUniform("current_material.specular_sharpness", material->mSpecularSharpness);
            shader->SetUniform("current_material.specular_strength", material->mSpecularStrength);

            shader->Bind();
            renderer_api->DrawIndexed(mesh->MeshData());
            renderer_api->SetFramebufferSRGB(false);
        }
    }
}


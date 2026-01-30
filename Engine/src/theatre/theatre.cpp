#include "./theatre.hpp"
#include "./parser.hpp"
#include "./theatre_data.hpp"
#include "./thing_factory.hpp"
#include "./things/thinkers/2d/camera_2d.hpp"
#include "./things/thinkers/2d/sprite_2d.hpp"
#include "./things/thinkers/2d/text_2d.hpp"
#include "./things/thinkers/3d/light_3d.hpp"
#include "./things/thinkers/3d/mesh_instance_3d.hpp"
#include "./things/thinkers/3d/camera_3d.hpp"
#include "./things/thinkers/3d/visual_3d.hpp"
#include "./things/thinkers/viewport.hpp"
#include "./things/resources/texture.hpp"
#include "./things/resources/material.hpp"
#include "./things/resources/mesh.hpp"
#include "./things/resources/font.hpp"
#include "managers/render_manager.hpp"
#include "rendering/renderer_api.hpp"
#include "rendering/shader.hpp"
#include "settings/graphics.hpp"
#include <ranges>

using namespace TheatreFile;

Theatre::Theatre() noexcept:
    m_pRegistry{MakeShared<VariableRegistry>()} {}

Theatre::Theatre(Shared<TheatreData> inData) noexcept:
    m_pRegistry{MakeShared<VariableRegistry>()},
    m_pInitialState{inData},
    mInitStatus{OK} {}

Theatre::Theatre(Farg<FileData> inData) noexcept:
    m_pRegistry{MakeShared<VariableRegistry>()},
    m_pInitialState{MakeShared<TheatreData>()}
{ Load(inData); }

Theatre::~Theatre() noexcept = default;

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

Error Theatre::Load(Farg<FileData> inData)
{
    TokenArray tokens{};
    if(!print_error_enum(inData.Status()))
        { mInitStatus = ERR_DATA_LOAD; }
    else if(!print_error_enum(Lexer(inData, tokens)))
        { mInitStatus = ERR_THEATRE_LEXER; }
    else if(!print_error_enum(Parser(tokens, m_pInitialState)))
        { mInitStatus = ERR_THEATRE_PARSER; }
    else { mInitStatus = OK; }
    return print_error_enum(mInitStatus);
}

bool Theatre::Startup()
{
    assert(mInitStatus == OK and m_pInitialState and m_pRegistry);
    if(mIsStarted)
        { return true; }

    LockGuard<RMutex> things_lock{mThingsMutex};

    mName  = m_pInitialState->name;
    mIndex = m_pInitialState->index;

    mUIDs.Clear();
    mCallSheet.Clear();
    m_pRegistry->ClearIDs();
    m_pRegistry->Init();

    m_pRootViewport = GetThinker<Viewport>(CreateThingNoReady({ThingType::Viewport,
        "Root_Viewport", {}, UID::a_RootViewport}));
    CreateEmbeddedResources();

    for(auto& thing_dat : *m_pInitialState)
        { SetupUID(thing_dat); }

    for(auto& thing_dat : *m_pInitialState)
        { CreateThingNoReady(thing_dat); }

    for(auto& thing_dat : *m_pInitialState)
        { SetupOwnership(thing_dat); }

    auto uids{ThingIDs()};
    for(ID uid : uids)
        { mThings.at(uid)->Ready(); }

    return mIsStarted = true;
}

bool Theatre::Shutdown()
{
    if(!mIsStarted)
        { return true; }
    LockGuard<RMutex> things_lock{mThingsMutex};
    for(ID uid : ThingIDs())
    {
        if(!mThings.contains(uid))
            { continue; }
        DestroyThing(uid);
    }
    m_pRegistry->ClearIDs();
    mUIDs.Clear();
    mCallSheet.Clear();
    return !(mIsStarted = false);
}

void Theatre::Draw()
{
    LockGuard<RMutex> things_lock{mThingsMutex};

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

ID Theatre::CreateThing(Farg<TheatreFile::ThingData> inData)
{
    LockGuard<RMutex> lock{mThingsMutex};
    ID output{CreateThingNoReady(inData)};
    mThings.at(output)->Ready();
    return output;
}

Error Theatre::DestroyThing(ID inID)
{
    LockGuard<RMutex> things_lock{mThingsMutex};

    auto children{GetChildren(inID)};
    for(ID child : children)
        { DestroyThing(child); }

    GetThing(inID)->Shutdown();
    auto status{DestroyThingOnly(inID)};
    return status;
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
    LockGuard<Mutex> callsheet_lock{mCallSheetMutex};
    return mCallSheet.Get(inParentID).children;
}

ID Theatre::GetParent(ID inChildID)
{
    LockGuard<Mutex> callsheet_lock{mCallSheetMutex};
    return mCallSheet.Get(inChildID).parent;
}

IdSet_t Theatre::GetAllChildren(ID inParentID)
{
    LockGuard<Mutex> callsheet_lock{mCallSheetMutex};
    return mCallSheet.Descendants(inParentID);
}

IdSet_t Theatre::GetAllParents(ID inChildID)
{
    LockGuard<Mutex> callsheet_lock{mCallSheetMutex};
    return mCallSheet.Ancestors(inChildID);
}

Error Theatre::SetParent(ID inChildID, ID inParentID)
{
    LockGuard<Mutex> callsheet_lock{mCallSheetMutex};
    LockGuard<RMutex> things_lock{mThingsMutex};

    // NOTE: Probably redundant. Too bad!
    if(!mCallSheet.Has(inChildID)
        or (!inParentID.invalid() and !mCallSheet.Has(inParentID)))
            { return print_error_enum(ERR_NOT_FOUND); }

    auto old_parent_id{mCallSheet.Get(inChildID).parent};
    auto old_ancestors{mCallSheet.Ancestors(inChildID)};

    if(auto status{mCallSheet.Reparent(inChildID, inParentID)}; !status)
        { return print_error_enum(status); }

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
    LockGuard<Mutex> callsheet_lock{mCallSheetMutex};
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

void Theatre::SetupUID(ThingData& ioData)
{
    ioData.theatre_registry = m_pRegistry;

    if(ThingFactory::IsDerivedFrom(ioData.type, ThingType::NostalgiaPlayer3D))
        { ioData.uid = UID::a_Player; }
    if(ioData.uid.invalid())
        { ioData.uid = mUIDs.Generate(); }
    if(!mUIDs.Contains(ioData.uid[]))
        { mUIDs.Push(ioData.uid[]); }
    if(!m_pRegistry->HasID(ioData.uid[]))
        { m_pRegistry->RegisterID(ioData.name, ioData.uid[]); }

    mCallSheet.Add(ioData.uid);
}

void Theatre::SetupOwnership(Farg<ThingData> ioData)
{
    LockGuard<Mutex> callsheet_lock{mCallSheetMutex};

    ID parent{ioData.get_parent()};
    IdSet_t children{ioData.get_children()};

    if(!parent.invalid())
    {
        if(!mCallSheet.Has(parent))
            { mCallSheet.Add(parent); }
        if(!mCallSheet.Reparent(ioData.uid, parent))
            { mCallSheet.Add(ioData.uid, parent); }
    }
    for(ID child : children)
    {
        if(child.invalid()) { continue; }
        else if(!mCallSheet.Reparent(child, ioData.uid))
            { mCallSheet.Add(child, ioData.uid); }
    }
}

void Theatre::CreateEmbeddedResources()
{
    CreateThingNoReady({ThingType::Font,    "Audiowide",{},           UID::f_Audiowide});
    CreateThingNoReady({ThingType::Font,    "DejaVuSans",{},          UID::f_DejaVuSans});
    CreateThingNoReady({ThingType::Font,    "Verdana",{},             UID::f_Verdana});
    CreateThingNoReady({ThingType::Mesh,    "ErrorModel",{},          UID::m_Error});
    CreateThingNoReady({ThingType::Mesh,    "DefaultCube",{},         UID::m_Cube});
    CreateThingNoReady({ThingType::Mesh,    "DefaultQuad",{},         UID::m_Quad});
    CreateThingNoReady({ThingType::Mesh,    "RamielModel",{},         UID::m_Ramiel});
    CreateThingNoReady({ThingType::Mesh,    "CameraModel",{},         UID::m_Camera3D});
    CreateThingNoReady({ThingType::Mesh,    "3DAxisModel",{},         UID::m_DebugAxis});
    CreateThingNoReady({ThingType::Texture, "MissingTexture",{},      UID::t_Missing});
    CreateThingNoReady({ThingType::Texture, "LightTexture",{},        UID::t_LightDebug});
    CreateThingNoReady({ThingType::Texture, "DoomTexture",{},         UID::t_COMP04_5});
    CreateThingNoReady({ThingType::Texture, "LolBitTexture",{},       UID::t_LolBit});
    CreateThingNoReady({ThingType::Texture,
        "ShittySkybox",
        {{"Type", "CubeMapTexture", ThingVarType::Enum}},
        UID::t_ShittySkybox});
}

ID Theatre::CreateThingNoReady(Farg<TheatreFile::ThingData> inData)
{ auto data{inData}; return CreateThingNoReady(data); }

ID Theatre::CreateThingNoReady(TheatreFile::ThingData& ioData)
{
    LockGuard<RMutex> lock{mThingsMutex};

    if(ThingFactory::IsDerivedFrom(ioData.type, ThingType::NostalgiaPlayer3D)
        and mThings.contains(UID::a_Player))
            { print_warning("Only one player at a time, please!"); return UID::a_Player; }
    else if(UID::IsReserved(ioData.uid[]) and mThings.contains(ioData.uid[]))
        { return ioData.uid; }

    SetupUID(ioData);
    SetupOwnership(ioData);

    auto& thing{mThings[ioData.uid] = ThingFactory::MakeThing(ioData.type)};
    thing->m_pRootTheatre = this;
    thing->SetVariables(ioData);

    if(ThingFactory::IsDerivedFrom(thing->type(), ThingType::Viewport))
        { mViewportIDs.insert(thing->uid()); }
    else if(ThingFactory::IsDerivedFrom(thing->type(), ThingType::Camera3D))
        { mCamera3DIDs.insert(thing->uid()); }
    else if(ThingFactory::IsDerivedFrom(thing->type(), ThingType::Camera2D))
        { mCamera2DIDs.insert(thing->uid()); }
    else if(ThingFactory::IsDerivedFrom(thing->type(), ThingType::Visual3D))
    {
        mVisual3DIDs.insert(thing->uid());
        if(ThingFactory::IsDerivedFrom(thing->type(), ThingType::Light3D))
            { mLightIDs.insert(thing->uid()); }
    }
    else if(ThingFactory::IsDerivedFrom(thing->type(), ThingType::Visual2D))
        { mVisual2DIDs.insert(thing->uid()); }

    return thing->uid();
}

Error Theatre::DestroyThingOnly(ID inID)
{
    LockGuard<RMutex> lock{mThingsMutex};
    LockGuard<Mutex> callsheet_lock{mCallSheetMutex};

    if(!mThings.contains(inID))
    {
        return (mThings.empty())
            ? ERR_EMPTY
            : ERR_NOT_FOUND;
    }

    mUIDs.Erase(inID[]);
    mViewportIDs.erase(inID);
    mVisual2DIDs.erase(inID);
    mVisual3DIDs.erase(inID);
    mCamera3DIDs.erase(inID);
    mCamera2DIDs.erase(inID);
    mLightIDs.erase(inID);
    print_error_enum(mCallSheet.Remove(inID));
    if(!m_pRegistry->RemoveID(inID)
        and !m_pRegistry->RemoveID(mThings.at(inID)->name()))
    {
        print_warning("Unable to remove [{}, {}] from the variable registry!",
            inID[],
            mThings.at(inID)->name());
    }
    mThings.erase(inID);
    return OK;
}

void Theatre::Draw3DThinkers(Shared<Viewport> inViewport)
{
    LockGuard<RMutex> things_lock{mThingsMutex};

    if(inViewport->CurrentCamera3D().invalid()
        and !inViewport->SetCurrentCamera3D())
            { return; }

    FAUTO renderer_api{g_pRenderManager->GetAPI()};
    auto camera{GetThinker<Camera3D>(inViewport->CurrentCamera3D())};
    auto view_matrix{camera->ViewMatrix()};
    auto projection_matrix{camera->ProjectionMatrix()};

    if(!mThings.contains(UID::m_Error) or !mThings.contains(UID::t_Missing))
        { CreateEmbeddedResources(); }

    auto missing_texture{GetResource<Texture>(UID::t_Missing)};
    auto error_mesh{GetResource<Mesh>(UID::m_Error)};

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
        if(!visual3d->Visible()
            or !camera->LayersMask().contains(visual3d->Layers())
            or (visual3d->Viewport() != UID::a_RootViewport
                    and visual3d->Viewport() != inViewport->uid()))
                        { continue; }

        if(ThingFactory::IsDerivedFrom(visual3d->type(), ThingType::MeshInstance3D))
        {
            auto mesh_instance{DCast<MeshInstance3D>(visual3d)};
            auto mesh{GetResource<Mesh>(mesh_instance->MeshID())};
            auto material{GetResource<Material>(mesh->MaterialID())};

            if(!mesh_instance->MaterialOverrideID().invalid())
                { material = GetResource<Material>(mesh_instance->MaterialOverrideID()); }

            if(!mesh->MeshData())
                { mesh = error_mesh; }

            auto diffuse_texture{GetResource<Texture>(material->DiffuseTextureID())};
            auto specular_texture{GetResource<Texture>(material->SpecularTextureID())};

            if(!material->DiffuseTextureID().invalid() and !diffuse_texture->GetBuffer())
                { diffuse_texture = missing_texture; }

            if(!material->SpecularTextureID().invalid() and !specular_texture->GetBuffer())
                { specular_texture = missing_texture; }

            auto shader{renderer_api->GetShader((material->mFullBright) ? Shaders::Fullbright : Shaders::BlinnPhong)};

            // https://www.reddit.com/r/opengl/comments/t01fwn/comment/hy7mezc
            glm::mat4 scaleMat    {glm::scale(glm::mat4{1.0f}, visual3d->GlobalScale())},
                transMat          {glm::translate(glm::mat4{1.0f}, visual3d->GlobalPosition())},
                rotMat            {glm::mat4_cast(glm::normalize(glm::quat{visual3d->GlobalRotation()}))},
                model_matrix      {transMat * rotMat * scaleMat};

            bool use_diffuse  {renderer_api->BindTexture(diffuse_texture,  0)};
            bool use_specular {renderer_api->BindTexture(specular_texture, 1)};

            renderer_api->SetWireframe(Settings::Graphics::GlobalWireframe or mesh_instance->Wireframe());
            renderer_api->SetFramebufferSRGB(use_diffuse or use_specular);

            shader->SetUniform("model_matrix", model_matrix);
            shader->SetUniform("normal_matrix", glm::mat3{glm::transpose(glm::inverse(model_matrix))});
            shader->SetUniform("projection_matrix", projection_matrix);
            shader->SetUniform("debug_output", static_cast<int>(gShaderDebugOutput));
            shader->SetUniform("point_lights_count", PointLight3D::GetCount());
            shader->SetUniform("spot_lights_count", SpotLight3D::GetCount());
            shader->SetUniform("directional_lights_count", DirectionalLight3D::GetCount());
            shader->SetUniform("view_matrix", view_matrix);
            shader->SetUniform("view_position", camera->GlobalPosition());
            shader->SetUniform("current_material.texture_diffuse",  0);
            shader->SetUniform("current_material.texture_specular", 1);
            shader->SetUniform("current_material.use_diffuse",  use_diffuse);
            shader->SetUniform("current_material.use_specular", use_specular);
            shader->SetUniform("current_material.diffuse_color", material->mColor);
            shader->SetUniform("current_material.alpha", material->mAlpha);
            shader->SetUniform("current_material.specular_sharpness", material->mSpecularSharpness);
            shader->SetUniform("current_material.specular_strength", material->SpecularStrength());

            glm::vec4 debug_highlight{visual3d->mDebugHighlight};
            debug_highlight *= (visual3d->mIsHoveredInDebugger or visual3d->mOverrideEnableDebugHighlight);
            shader->SetUniform("debug_highlight", debug_highlight);

            shader->Bind();
            renderer_api->DrawIndexed(mesh->MeshData());
            renderer_api->SetFramebufferSRGB(false);
        }
    }
}

void Theatre::Draw2DThinkers(Shared<Viewport> inViewport)
{
    LockGuard<RMutex> things_lock{mThingsMutex};

    if(inViewport->uid() != UID::a_RootViewport
        and inViewport->CurrentCamera2D().invalid()
        and !inViewport->SetCurrentCamera2D())
            { return; }

    FAUTO renderer_api{g_pRenderManager->GetAPI()};
    auto camera{GetThinker<Camera2D>(inViewport->CurrentCamera2D())};

    if(!mThings.contains(UID::t_Missing))
        { CreateEmbeddedResources(); }

    auto missing_texture{GetResource<Texture>(UID::t_Missing)};
    auto quad_mesh{GetResource<Mesh>(UID::m_Quad)};

    for(ID v2d_id : mVisual2DIDs)
    {
        auto shader{renderer_api->GetShader(Shaders::Fast2D)};
        auto visual2d{GetThinker<Visual2D>(v2d_id)};
        if(!visual2d->Visible()
            or !camera->LayersMask().contains(visual2d->Layers())
            or (visual2d->Viewport() != UID::a_RootViewport
                    and visual2d->Viewport() != inViewport->uid()))
                        { continue; }

        if(ThingFactory::IsDerivedFrom(visual2d->type(), ThingType::Sprite2D))
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
        else if(ThingFactory::IsDerivedFrom(visual2d->type(), ThingType::Text2D))
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

            if(text2d->mDebugOutline)
            {
                shader->SetUniform("debug_solid", 1);
                shader->SetUniform("text_color", glm::vec3{1.0f, 0.4f, 1.0f});
                renderer_api->SetWireframe(true);
                renderer_api->DrawText(text2d->Text(), font, glm::vec2{0.0f}, glm::vec2{1.0f});
            }
        }
    }
}

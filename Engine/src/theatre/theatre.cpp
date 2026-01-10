#include "theatre.hpp"
#include "core/uid.hpp"
#include "managers/render_manager.hpp"
#include "settings/graphics.hpp"
#include "theatre/things/thinkers/3d/light_3d.hpp"
#include "thing_factory.hpp"
#include "filesystem/file_data.hpp"
#include "theatre/parser/theatre_parser.hpp"
#include "theatre/parser/theatre_data.hpp"
#include "theatre/things/thing.hpp"
#include "theatre/things/thinkers/thinker.hpp"
#include "theatre/things/thinkers/3d/mesh_instance_3d.hpp"
#include "theatre/things/thinkers/3d/camera_3d.hpp"
#include "theatre/things/thinkers/3d/visual_3d.hpp"
#include "theatre/things/thinkers/viewport.hpp"
#include "theatre/things/resources/resource.hpp"
#include "theatre/things/resources/texture.hpp"
#include "theatre/things/resources/material.hpp"
#include "theatre/things/resources/mesh.hpp"
#include "rendering/renderer_api.hpp"
#include "rendering/shader.hpp"
#include <ranges>

Theatre::Theatre() noexcept = default;

Theatre::Theatre(Farg<TheatreData> inData) noexcept:
    mInitialState{MakeUnique<TheatreData>(inData)},
    mInitStatus{OK} {}

Theatre::Theatre(Farg<FileData> inData) noexcept:
    mInitialState{MakeUnique<TheatreData>()}
{
    mInitStatus = (TheatreParser::ParseTheatreFileFromMemory(inData, *mInitialState))
        ? OK
        : ERR_DATA_LOAD;
}

Theatre::Theatre(Sarg inPath) noexcept:
    mInitialState{MakeUnique<TheatreData>()}
{
    mInitStatus = (TheatreParser::ParseTheatreFile(inPath, *mInitialState))
        ? OK
        : ERR_FILE_LOAD;
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
    if(mIsStarted or !mInitStatus)
        { return false; }

    LockGuard<RMutex> lock{mThingsMutex};

    mInitialState->SetupUIDsAndPriorities();
    if(mDoPrintDebugLogs)
        { mInitialState->debug_PrintData(); }

    CreateEmbeddedResources();

    for(ThingData& data : mInitialState->things_data)
        { CreateThingNoReady(data); }

    if(!mThings.contains(UID::a_Player))
        { CreateThingNoReady({"Default NostalgiaPlayer3D", ThingType::NostalgiaPlayer3D, UID::a_Player}); }

    CreateThingNoReady({"Global 3D Viewport", ThingType::Viewport, UID::a_Global3DViewport});
    CreateThingNoReady({"Global 2D Viewport", ThingType::Viewport, UID::a_Global2DViewport});

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

Farg<TheatreData> Theatre::InitialState() const
{ return *mInitialState; }

TheatreData Theatre::CurrentState()
{
    LockGuard<RMutex> lock{mThingsMutex};
    TheatreData data{*mInitialState};
    data.clear();
    for(const auto& [id, thing] : mThings)
        { data.AddData(*thing->GetVariables()); }
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

Error Theatre::ChangeThingID(ID inOldID, ID inNewID)
{
    LockGuard<RMutex> lock{mThingsMutex};
    if(mThings.contains(inNewID))
        { return ERR_ALREADY_EXISTS; }
    else if(!mThings.contains(inOldID))
        { return ERR_NOT_FOUND; }
    auto found_it{mThings.extract(inOldID)};
    found_it.mapped()->uid(inNewID);
    found_it.key() = inNewID;
    mThings.insert(std::move(found_it));
    return OK;
}

ID Theatre::CreateThing(Farg<ThingData> inData)
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

void Theatre::CreateEmbeddedResources()
{
    CreateThingNoReady({"Error Mesh",          ThingType::Mesh,    UID::m_Error});
    CreateThingNoReady({"Cube Mesh",           ThingType::Mesh,    UID::m_Cube});
    CreateThingNoReady({"Ramiel Mesh",         ThingType::Mesh,    UID::m_Ramiel});
    CreateThingNoReady({"Camera3D Mesh",       ThingType::Mesh,    UID::m_Camera3D});
    CreateThingNoReady({"Missing Texture",     ThingType::Texture, UID::t_Missing});
    CreateThingNoReady({"LolBit Texture",      ThingType::Texture, UID::t_LolBit});
    CreateThingNoReady({"Debug Light Texture", ThingType::Texture, UID::t_LightDebug});
    CreateThingNoReady({"Doom Texture",        ThingType::Texture, UID::t_COMP04_5});
    CreateThingNoReady({"Shitty SkyBox",       ThingType::Texture, UID::t_ShittySkybox,
        {{TextureType::TEXTURE_TYPE_CUBE, "Type"}}});
}

ID Theatre::CreateThingNoReady(Farg<ThingData> inData)
{
    LockGuard<RMutex> lock{mThingsMutex};
    ThingData data{inData};

    if(mThings.contains(data.uid))
    {
        if(!mThings.at(data.uid)->name().compare(data.name))
        {
            print_warning("Duplicate Thing '{}' [{}] detected", data.name, data.uid[]);
            return data.uid[];
        }
        else if(!UID::Contains(data.uid[]))
            { UID::Push(data.uid[]); }
        else
        {
            print_warning("UID#{} is already in use; a new ID will be generated for Thing '{}'",
                data.uid[],
                data.name);
            data.uid = UID::Generate();
        }
    }

    auto& thing{mThings[data.uid] = ThingFactory::MakeThing(data.type())};
    thing->SetVariables(data);
    g_pVariableRegistry->RegisterID(thing->name(), thing->uid()[]);

    if(ThingFactory::IsDerivedFrom(data.type(), ThingType::Viewport))
        { mViewportIDs.insert(data.uid); }
    else if(ThingFactory::IsDerivedFrom(data.type(), ThingType::Visual3D))
    {
        mVisual3DIDs.insert(data.uid);
        if(ThingFactory::IsDerivedFrom(data.type(), ThingType::Light3D))
            { mLightIDs.insert(data.uid); }
    }
    else if(ThingFactory::IsDerivedFrom(data.type(), ThingType::Visual2D))
        { mVisual2DIDs.insert(data.uid); }
    else if(ThingFactory::IsDerivedFrom(data.type(), ThingType::NostalgiaPlayer3D))
        { ChangeThingID(thing->uid(), UID::a_Player); }

    return thing->uid();
}

Theatre::Things_t::iterator Theatre::DestroyThing(Things_t::iterator inIterator)
{
    LockGuard<RMutex> lock{mThingsMutex};
    if(inIterator == mThings.end())
        { return inIterator; }
    if(!g_pVariableRegistry->RemoveID(inIterator->first[])
        and !g_pVariableRegistry->RemoveID(inIterator->second->name()))
        { print_warning("Unable to remove Thing#{} from the variable registry!", inIterator->first[]); }
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


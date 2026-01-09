#include "theatre_manager.hpp"
#include "render_manager.hpp"
#include "core/uid.hpp"
#include "common/colors.hpp"
#include "filesystem/filesystem.hpp"
#include "filesystem/file_data.hpp"
#include "math/containers.hpp"
#include "settings/engine.hpp"
#include "settings/graphics.hpp"
#include "rendering/renderer_api.hpp"
#include "rendering/shader.hpp"
#include "theatre/parser/theatre_data.hpp"
#include "theatre/parser/theatre_parser.hpp"
#include "theatre/things/thinkers/3d/camera_3d.hpp"
#include "theatre/thing_factory.hpp"
#include "theatre/things/resources/mesh.hpp"
#include "theatre/things/resources/texture.hpp"
#include "theatre/things/thinkers/viewport.hpp"
#include "theatre/things/resources/material.hpp"
#include "theatre/things/thinkers/3d/mesh_instance_3d.hpp"
#include "theatre/things/thinkers/3d/nostalgia_player_3d.hpp"
#include "theatre/things/thinkers/3d/light_3d.hpp"
#include <ranges>

using namespace ManagerEnums;

bool gPrintLoadedTheatreData{false};

static TheatreData sCurrentTheatreData{};
static TheatreManager sTheatreManager;
TheatreManager* g_pTheatreManager{&sTheatreManager};

void TheatreManager::CreateEmbeddedResources()
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

bool TheatreManager::Init()
{
    PRINT_PRETTY_FUNCTION;
    return ThingFactory::Init();
}

void TheatreManager::Update()
{
    if(GetTheatreState() != IN_LEVEL)
        { return; }
    const std::lock_guard<std::recursive_mutex> lock{mThingsMutex};
    for(auto& [id, thing] : mThings)
        { thing->Update(); }
}

void TheatreManager::Tick()
{
    if(GetTheatreState() != IN_LEVEL)
        { return; }
    const std::lock_guard<std::recursive_mutex> lock{mThingsMutex};
    for(auto& [id, thing] : mThings)
        { thing->Tick(); }
}

void TheatreManager::Input(InputEvent* inInput)
{
    if(GetTheatreState() != IN_LEVEL)
        { return; }
    const std::lock_guard<std::recursive_mutex> lock{mThingsMutex};
    for(auto& [id, thing] : mThings)
        { thing->Input(inInput); }
}

Shared<Thing> TheatreManager::GetThing(ID inID)
{
    const std::lock_guard<std::recursive_mutex> lock{mThingsMutex};
    if(auto found_it{mThings.find(inID)};
        found_it != mThings.end())
    { return found_it->second; }
    return MakeShared<Thing>();
}

Shared<Thinker> TheatreManager::GetThinker(ID inID)
{
    const std::lock_guard<std::recursive_mutex> lock{mThingsMutex};
    if(auto found_it{mThings.find(inID)};
        found_it != mThings.end())
    {
        if(ThingFactory::IsThinker(found_it->second->type()))
            { return DCast<Thinker>(found_it->second); }
    }
    return MakeShared<Thinker>();
}

Shared<Resource> TheatreManager::GetResource(ID inID)
{
    const std::lock_guard<std::recursive_mutex> lock{mThingsMutex};
    if(auto found_it{mThings.find(inID)};
        found_it != mThings.end())
    {
        if(ThingFactory::IsResource(found_it->second->type()))
            { return DCast<Resource>(found_it->second); }
    }
    return MakeShared<Resource>();
}

void TheatreManager::DrawTheatre()
{
    if(GetTheatreState() != IN_LEVEL)
        { return; }
    const std::lock_guard<std::recursive_mutex> lock{mThingsMutex};
    Light3D::ClearCounts();
#pragma message("TODO: I have to loop through everything twice since I've removed the render command, so fix that...")
    for(auto& [id, thing] : mThings)
    {
        if(auto light{DCast<Light3D>(thing)}; light and light->IncrementIndex())
            { g_pRenderManager->GetAPI()->SetLight_TempBlinnPhongSolution(light.get()); }
    }
    for(ID viewport_id : mViewportIDs)
    {
        auto viewport{GetThinker<Viewport>(viewport_id)};
        auto framebuffer{viewport->Framebuffer()};
        auto camera{GetThinker<Camera3D>(viewport->CurrentCamera())};
        FAUTO renderer_api{g_pRenderManager->GetAPI()};
        framebuffer->Bind();
        renderer_api->SetViewport({0, 0}, viewport->Size());
        switch(camera->mEnvironment.mType)
        {
        case Environment::BG_SKYBOX:
            renderer_api->SetWireframe(false);
            renderer_api->BindTexture(
                g_pTheatreManager->GetResource<Texture>(camera->mEnvironment.mSkyboxTextureID)->GetBuffer(),
                0);
            renderer_api->GetShader(Shaders::SkyBox)->Bind();
            renderer_api->GetShader(Shaders::SkyBox)->SetUniform("view_matrix", glm::mat4{glm::mat3{camera->ViewMatrix()}});
            renderer_api->GetShader(Shaders::SkyBox)->SetUniform("projection_matrix", camera->ProjectionMatrix());
            renderer_api->GetShader(Shaders::SkyBox)->SetUniform("skybox", 0);
            renderer_api->DrawSkybox(g_pTheatreManager->GetResource<Mesh>(UID::m_Cube)->MeshData());
            break;
        default:
            break;
        }
        for(auto& [id, thing] : mThings)
        {
            if(auto actor{DCast<Actor3D>(thing)}; actor and actor->Visible())
                { DrawActor(actor, camera); }
        }
        framebuffer->Unbind();
    }
}

void TheatreManager::ClearViewports()
{
    for(ID viewport_id : mViewportIDs)
    {
        auto framebuffer{GetThinker<Viewport>(viewport_id)->Framebuffer()};
        framebuffer->Bind();
        g_pRenderManager->GetAPI()->Clear();
        framebuffer->Unbind();
    }
}

void TheatreManager::DrawActor(Shared<Actor3D> actor, Shared<Camera3D> camera)
{
    const std::lock_guard<std::recursive_mutex> lock{mThingsMutex};

    if(!camera->Current() or actor->uid() == camera->uid())
        { return; }
    auto children{actor->Children()};
    if(Settings::Engine::IsEditorHint and !actor->mDebugMeshInstanceID.invalid())
        { children.emplace_back(actor->mDebugMeshInstanceID, ThingType::MeshInstance3D); }
    for(auto child : children)
    {
        auto mesh_instance{GetThinker<MeshInstance3D>(child.id)};
        if(mesh_instance->uid().invalid() or
            !camera->LayersMask().contains(mesh_instance->Layers()))
            { continue; }
        auto mesh{GetResource<Mesh>(mesh_instance->MeshID()[])};
        auto material{GetResource<Material>(mesh->MaterialID())};
        if(!mesh_instance->MaterialOverrideID().invalid())
            { material = GetResource<Material>(mesh_instance->MaterialOverrideID()); }
        FAUTO renderer_api{g_pRenderManager->GetAPI()};
        auto shader{renderer_api->GetShader((material->mFullBright) ? Shaders::Fullbright : Shaders::BlinnPhong)};

        if(mesh->uid().invalid())
            { mesh = g_pTheatreManager->GetResource<Mesh>(UID::m_Error); }

        // https://www.reddit.com/r/opengl/comments/t01fwn/comment/hy7mezc
        glm::mat4 scaleMat     {glm::scale(glm::mat4{1.0f}, actor->Scale() * mesh_instance->Scale())};
        glm::mat4 rotMat       {glm::mat4_cast(actor->Quaternion())};
        glm::mat4 rotMat2      {glm::mat4_cast(mesh_instance->Quaternion())};
        glm::mat4 transMat     {glm::translate(glm::mat4{1.0f}, actor->Position() + mesh_instance->Position())};
        glm::mat4 model_matrix {transMat * rotMat * rotMat2 * scaleMat};
        glm::mat4 projection_matrix{camera->ProjectionMatrix()};
        glm::mat4 view_matrix{camera->ViewMatrix()};

        renderer_api->SetFramebufferSRGB(!material->mDontUseTexture);
        renderer_api->SetWireframe(Settings::Graphics::GlobalWireframe or mesh_instance->Wireframe());
        renderer_api->BindTexture(GetResource<Texture>(material->DiffuseTextureID()[])->GetBuffer(), 0);
        renderer_api->BindTexture(GetResource<Texture>(material->SpecularTextureID()[])->GetBuffer(), 1);

        shader->SetUniform("model_matrix", model_matrix);
        shader->SetUniform("normal_matrix", glm::mat3{glm::transpose(glm::inverse(model_matrix))});
        shader->SetUniform("projection_matrix", projection_matrix);
        shader->SetUniform("debug_output", static_cast<int>(gShaderDebugOutput));
        shader->SetUniform("debug_highlight", actor->mDebugHighlight * actor->mDebugHighlight.a);
        shader->SetUniform("point_lights_count", PointLight3D::GetCount());
        shader->SetUniform("spot_lights_count", SpotLight3D::GetCount());
        shader->SetUniform("directional_lights_count", DirectionalLight3D::GetCount());
        shader->SetUniform("view_matrix", view_matrix);
        shader->SetUniform("view_position", camera->Position());
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

ManagerEnums::TheatreReturnValue_t TheatreManager::TheatreInit(bool is_first_call)
{
    if(!is_first_call)
        { return FINISHED; }
    const std::lock_guard<std::recursive_mutex> lock{mThingsMutex};
    CreateThings();
    return FINISHED;
}

ManagerEnums::TheatreReturnValue_t TheatreManager::TheatreShutdown(bool is_first_call)
{
    if(!is_first_call)
        { return FINISHED; }
    const std::lock_guard<std::recursive_mutex> lock{mThingsMutex};
    DestroyThings();
    return FINISHED;
}

bool TheatreManager::ThingExists(ID uid)
{
    const std::lock_guard<std::recursive_mutex> lock{mThingsMutex};
    return mThings.contains(uid);
}

Farg<PID> TheatreManager::GetType(ID uid)
{
    static PID invalid{ID::Invalid, "Invalid"};
    const std::lock_guard<std::recursive_mutex> lock{mThingsMutex};
    if(auto found_it{mThings.find(uid)}; found_it != mThings.end())
        { return found_it->second->type(); }
    return invalid;
}

void TheatreManager::LoadTheatreData(const TheatreData& data)
{
    sCurrentTheatreData = data;
    StartNewTheatre();
}

bool TheatreManager::LoadTheatreFromMemory(const std::string& data)
{
    if(!TheatreParser::ParseTheatreFileFromMemory(data, sCurrentTheatreData))
    {
        debug_print("{}Theatre File Data:{}\n{}", Sty::Bold + Fg::Cyan, Sty::Reset, data);
        return print_error("unable to load Theatre");
    }
    StartNewTheatre();
    return true;
}

bool TheatreManager::LoadTheatreFromFile(const std::string& path)
{
    print_debugv(VERBOSE3, "current working directory: {}", FileSystem::GetCurrentDirectory());
    if(!TheatreParser::ParseTheatreFile(path, sCurrentTheatreData))
        { return print_error("unable to load Theatre file '{}'", path); }
    StartNewTheatre();
    return true;
}

const TheatreData& TheatreManager::GetInitialState()
{ return sCurrentTheatreData; }

TheatreData TheatreManager::GetCurrentState()
{
    const std::lock_guard<std::recursive_mutex> lock{mThingsMutex};
    TheatreData data{sCurrentTheatreData};
    data.clear();
    for(const auto& [id, thing] : mThings)
        { data.AddData(*thing->GetVariables()); }
    return data;
}

std::vector<ID> TheatreManager::GetThingIDs()
{
    const std::lock_guard<std::recursive_mutex> lock{mThingsMutex};
    auto keys{std::views::keys(mThings)};
    return {keys.begin(), keys.end()};
}

Farg<std::unordered_set<ID>> TheatreManager::GetViewportIDs()
{ return mViewportIDs; }

std::vector<ID> TheatreManager::GetViewportIDList()
{ return std::vector<ID>{mViewportIDs.cbegin(), mViewportIDs.cend()}; }

Error TheatreManager::ChangeThingID(ID inOldID, ID inNewID)
{
    const std::lock_guard<std::recursive_mutex> lock{mThingsMutex};
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

uint TheatreManager::CreateThing(Farg<ThingData> inData)
{
    const std::lock_guard<std::recursive_mutex> lock{mThingsMutex};
    uint output{CreateThingNoReady(inData)};
    mThings.at(output)->Ready();
    return output;
}

bool TheatreManager::DestroyThing(ID id)
{ return DestroyThing(mThings.find(id)) != mThings.end() or mThings.empty(); }

things_t::iterator TheatreManager::DestroyThing(things_t::iterator inIterator)
{
    const std::lock_guard<std::recursive_mutex> lock{mThingsMutex};
    if(inIterator == mThings.end())
        { return inIterator; }
    if(!g_pVariableRegistry->RemoveID(inIterator->first[])
        and !g_pVariableRegistry->RemoveID(inIterator->second->name()))
        { print_warning("Unable to remove Thing#{} from the variable registry!", inIterator->first[]); }
    inIterator->second->Shutdown();
    mViewportIDs.erase(inIterator->first);
    return mThings.erase(inIterator);
}

uint TheatreManager::CreateThingNoReady(Farg<ThingData> inData)
{
    const std::lock_guard<std::recursive_mutex> lock{mThingsMutex};
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

    if(DCast<Viewport>(thing))
        { mViewportIDs.insert(thing->uid()); }
    else if(DCast<NostalgiaPlayer3D>(thing))
        { ChangeThingID(thing->uid(), UID::a_Player); }

    return thing->uid()[];
}

void TheatreManager::CreateThings()
{
    const std::lock_guard<std::recursive_mutex> lock{mThingsMutex};
    sCurrentTheatreData.SetupUIDsAndPriorities();
    if(gPrintLoadedTheatreData)
        { sCurrentTheatreData.debug_PrintData(); }

    CreateEmbeddedResources();

    for(ThingData& data : sCurrentTheatreData.things_data)
        { CreateThingNoReady(data); }

    if(!mThings.contains(UID::a_Player))
        { CreateThingNoReady({"Default NostalgiaPlayer3D", ThingType::NostalgiaPlayer3D, UID::a_Player}); }

    CreateThingNoReady({"Global 3D Viewport", ThingType::Viewport, UID::a_Global3DViewport});
    CreateThingNoReady({"Global 2D Viewport", ThingType::Viewport, UID::a_Global2DViewport});

    for(auto& [id, thing] : mThings)
        { thing->Ready(); }
}

void TheatreManager::DestroyThings()
{
    const std::lock_guard<std::recursive_mutex> lock{mThingsMutex};
    for(auto it{mThings.begin()}; it != mThings.end();)
        { it = DestroyThing(it); }
}

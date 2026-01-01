#include "theatre_manager.hpp"
#include "render_manager.hpp"
#include "core/uid.hpp"
#include "common/colors.hpp"
#include "filesystem/filesystem.hpp"
#include "filesystem/file_data.hpp"
#include "application/application.hpp"
#include "math/containers.hpp"
#include "settings/engine.hpp"
#include "settings/graphics.hpp"
#include "settings/player.hpp"
#include "rendering/renderer_api.hpp"
#include "rendering/shader.hpp"
#include "theatre/parser/theatre_data.hpp"
#include "theatre/parser/theatre_parser.hpp"
#include "things/actors/camera_3d.hpp"
#include "things/thing_factory.hpp"
#include "things/resources/mesh.hpp"
#include "things/resources/texture.hpp"
#include "things/devices/material.hpp"
#include "things/devices/mesh_instance.hpp"
#include "things/actors/nostalgia_player.hpp"
#include "things/actors/light.hpp"
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
}

bool TheatreManager::Init()
{
    PRINT_PRETTY_FUNCTION;
    return g_pThingFactory->Init();
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
    return (mThings.contains(inID))
        ? mThings.at(inID)
        : MakeShared<Thing>();
}

void TheatreManager::DrawTheatre()
{
    if(GetTheatreState() != IN_LEVEL)
        { return; }
    const std::lock_guard<std::recursive_mutex> lock{mThingsMutex};
    light_t::ClearCounts();
#pragma message("TODO: I have to loop through everything twice since I've removed the render command, so fix that...")
    for(auto& [id, thing] : mThings)
    {
        if(auto light{DCast<light_t>(thing)}; light and light->IncrementIndex())
        {
            g_pRenderManager->GetAPI()->SetLight_TempBlinnPhongSolution(light.get());
            auto material{GetThing<Material>(GetThing<MeshInstance>(light->MeshInstanceID())->MaterialID())};
            material->mColor = light->mColor;
            material->mFullBright = true;
        }
    }
    for(ID cam_id : mActiveCameras)
    {
        for(auto& [id, thing] : mThings)
        {
            if(id == cam_id) { continue; }
            else if(auto actor{DCast<Actor>(thing)}; actor and actor->Visible())
                { DrawActor(actor, GetThing<Camera3D>(cam_id)); }
        }
    }
}

void TheatreManager::DrawActor(Shared<Actor> actor, Shared<Camera3D> camera)
{
    if(!camera->GetRenderLayers().contains(actor->mRenderLayers))
        { return; }
    const std::lock_guard<std::recursive_mutex> lock{mThingsMutex};
    auto mesh_instance{GetThing<MeshInstance>(actor->MeshInstanceID())};
    auto material{GetThing<Material>(mesh_instance->MaterialID())};
    FAUTO renderer_api{g_pRenderManager->GetAPI()};
    auto shader{renderer_api->GetShader((material->mFullBright) ? Shaders::Fullbright : Shaders::BlinnPhong)};

    renderer_api->SetFramebufferSRGB(!material->mDontUseTexture);
    renderer_api->SetWireframe(Settings::Graphics::GlobalWireframe or actor->Wireframe());
    renderer_api->BindTexture(GetThing<Texture>(material->DiffuseTextureID()[]), 0);
    renderer_api->BindTexture(GetThing<Texture>(material->SpecularTextureID()[]), 1);

    glm::mat4 projection_matrix{glm::perspective(
        glm::radians(Settings::Player::FOV),
        static_cast<float>(MainWindow()->GetScale().AspectRatio()),
        Settings::Player::ViewCutoffNear,
        Settings::Player::ViewCutoffFar
    )};

    // https://www.reddit.com/r/opengl/comments/t01fwn/comment/hy7mezc
    glm::mat4 scaleMat     {glm::scale(glm::mat4(1.0f), actor->Scale())};
    glm::mat4 rotMat       {glm::mat4_cast(actor->Quaternion())};
    glm::mat4 transMat     {glm::translate(glm::mat4(1.0f), actor->Origin())};
    glm::mat4 model_matrix {transMat * rotMat * scaleMat};

    shader->Bind();
    shader->SetUniform("model_matrix", model_matrix);
    shader->SetUniform("normal_matrix", glm::mat3{glm::transpose(glm::inverse(model_matrix))});
    shader->SetUniform("projection_matrix", projection_matrix);
    shader->SetUniform("debug_output", static_cast<int>(gShaderDebugOutput));
    shader->SetUniform("debug_highlight", actor->mDebugHighlight * actor->mDebugHighlight.a);
    shader->SetUniform("point_lights_count", PointLight::GetCount());
    shader->SetUniform("spot_lights_count", SpotLight::GetCount());
    shader->SetUniform("directional_lights_count", DirectionalLight::GetCount());
    shader->SetUniform("view_matrix", camera->ViewMatrix());
    shader->SetUniform("view_position", camera->Origin());
    shader->SetUniform("current_material.texture_diffuse",  0);
    shader->SetUniform("current_material.texture_specular", 1);
    shader->SetUniform("current_material.use_textures", !material->mDontUseTexture);
    shader->SetUniform("current_material.diffuse_color", material->mColor);
    shader->SetUniform("current_material.alpha", material->mAlpha);
    shader->SetUniform("current_material.specular_sharpness", material->mSpecularSharpness);
    shader->SetUniform("current_material.specular_strength", material->mSpecularStrength);
    renderer_api->DrawIndexed(camera, GetThing<Mesh>(mesh_instance->MeshID()[]));
    renderer_api->SetFramebufferSRGB(false);
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

Farg<TTID> TheatreManager::GetType(ID uid)
{
    static TTID invalid{ID::Invalid, "Invalid"};
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

bool TheatreManager::IsCurrentCamera(ID inUID) const
{ return mCurrentCamera == inUID; }

Error TheatreManager::SetCurrentCamera(ID inID)
{
    if(inID.invalid())
        { return ERR_INVALID_ID; }
    else if(mActiveCameras.contains(inID))
    {
        mCurrentCamera = inID;
        return OK;
    }
    return ERR_NOT_FOUND;
}

void TheatreManager::UnsetCurrentCamera(ID inID)
{
    if(mCurrentCamera != inID)
        { return; }
    // for(Farg<ID> uid : mActiveCameras)
    // {
    //     if(mCurrentCamera != uid and DCast<Camera3D>(mThings.at(uid))->Visible())
    //         { mCurrentCamera = uid; return; }
    // }
    mCurrentCamera = ID::Invalid;
}

ID TheatreManager::GetCurrentCamera() const
{ return mCurrentCamera; }

Shared<NostalgiaPlayer> TheatreManager::GetPlayer()
{
    if(mThings.empty() or !mThings.contains(UID::a_Player))
        { return MakeShared<NostalgiaPlayer>(); }
    return DCast<NostalgiaPlayer>(mThings.at(UID::a_Player));
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

Farg<std::unordered_set<ID>> TheatreManager::GetCameraIDs()
{ return mActiveCameras; }

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
    mActiveCameras.erase(inIterator->first);
    return mThings.erase(inIterator);
}

uint TheatreManager::CreateThingNoReady(Farg<ThingData> inData)
{
    const std::lock_guard<std::recursive_mutex> lock{mThingsMutex};
    ThingData data{inData};

    if(mThings.contains(data.uid))
    {
        print_warning("A Thing with uid #{} already exists; a new ID will be generated and a duplicate Thing will be made", data.uid[]);
        if(!UID::Contains(data.uid[]))
            { std::println("{}[NOTE] uid #{} wasn't generated by `UIDs`, which is what caused the previous warning (it has just been added to avoid future issues){}", Sty::Bold + Fg::Cyan, data.uid[], Sty::Reset); }
        data.uid = UID::Generate();
    }

    auto& thing{mThings[data.uid] = g_pThingFactory->MakeThing(data.type())()};
    thing->SetVariables(data);
    g_pVariableRegistry->RegisterID(thing->name(), thing->uid()[]);

    if(DCast<Camera3D>(thing))
        { mActiveCameras.insert(thing->uid()); }
    else if(DCast<NostalgiaPlayer>(thing))
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
        { CreateThingNoReady({"Default NostalgiaPlayer", ThingType::NostalgiaPlayer, UID::a_Player}); }

    if(Settings::Engine::IsEditorHint)
        { CreateThingNoReady({"Editor Camera", ThingType::Camera3D, UID::a_EditorCamera}); }

    for(auto& [id, thing] : mThings)
        { thing->Ready(); }

    if(Settings::Engine::IsEditorHint)
        { mCurrentCamera = ID::Invalid; }
}

void TheatreManager::DestroyThings()
{
    const std::lock_guard<std::recursive_mutex> lock{mThingsMutex};
    for(auto it{mThings.begin()}; it != mThings.end();)
        { it = DestroyThing(it); }
}

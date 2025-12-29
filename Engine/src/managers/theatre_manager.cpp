#include "theatre_manager.hpp"
#include "render_manager.hpp"
#include "physics_manager.hpp"
#include "core/uid.hpp"
#include "common/colors.hpp"
#include "math/containers.hpp"
#include "embedded/models.hpp"
#include "embedded/images.hpp"
#include "filesystem/filesystem.hpp"
#include "filesystem/file_data.hpp"
#include "application/application.hpp"
#include "settings/graphics.hpp"
#include "settings/player.hpp"
#include "rendering/mesh_parser.hpp"
#include "rendering/vertex_array.hpp"
#include "rendering/mesh_buffers.hpp"
#include "rendering/texture_buffer.hpp"
#include "rendering/renderer_api.hpp"
#include "rendering/shader.hpp"
#include "theatre/parser/theatre_data.hpp"
#include "theatre/parser/theatre_parser.hpp"
#include "things/thing_factory.hpp"
#include "things/resources/mesh.hpp"
#include "things/resources/texture.hpp"
#include "things/devices/material.hpp"
#include "things/devices/mesh_instance.hpp"
#include "things/devices/collider.hpp"
#include "things/actors/nostalgia_player.hpp"
#include "things/actors/light.hpp"

#include <vector>
#include <ranges>
#include <glm/glm.hpp>

using namespace ManagerEnums;

static TheatreManager sTheatreManager;
TheatreManager* g_pTheatreManager{&sTheatreManager};

static bool sReadyToRender{false};
static TheatreData sCurrentTheatreData{};
static Shared<NostalgiaPlayer> sLocalPlayer{MakeShared<NostalgiaPlayer>()};
static IBuffer::Layout sDefaultLayout{
    { IBuffer::Element::Type::Float3, "position" },
    { IBuffer::Element::Type::Float3, "color"    },
    { IBuffer::Element::Type::Float3, "normal"   },
    { IBuffer::Element::Type::Float2, "uv"       },
};

Error TheatreManager::BufferMesh(Farg<FileData> inData, ID inID, Farg<IBuffer::Layout> inLayout = sDefaultLayout)
{
    if(mTheatreVAOs.contains(inID))
        { return ERR_ALREADY_EXISTS; }
    std::vector<float> vertices{};
    std::vector<uint>  indices{};
    if(!MeshParser::CreateMeshData(vertices, indices, inData))
        { print_error("Failed to parse mesh"); return FAILED; }
    auto& array{mTheatreVAOs[inID] = VertexArray::Create()};
    Shared<VertexBuffer> vbo{VertexBuffer::Create(vertices.data(), vertices.size())};
    vbo->SetLayout(inLayout);
    array->AddVertexBuffer(vbo);
    array->Bind();
    Shared<IndexBuffer> ibo{IndexBuffer::Create(indices.data(), indices.size())};
    array->SetIndexBuffer(ibo);
    return OK;
}

Error TheatreManager::BufferTexture(Farg<FileData> inData, ID inID, Farg<TextureFormat> inFormat = {})
{
    if(mTheatreTextures.contains(inID))
        { return ERR_ALREADY_EXISTS; }
    mTheatreTextures[inID] = TextureBuffer::Create(inFormat, SamplerState::JuliansPreferredDefaults, &inData);
    Error status{mTheatreTextures.at(inID)->Status()};
    if(!status)
    {
        print_error("Failed to buffer texture (ID#{})", inID[]);
        mTheatreTextures.erase(inID);
    }
    return status;
}

Shared<TextureBuffer>& TheatreManager::GetTextureBuffer(ID ID)
{
    if(auto found_it{mTheatreTextures.find(ID)}; found_it != mTheatreTextures.end())
        { return found_it->second; }
    return mTheatreTextures.at(UID::i_Missing);
}

Shared<VertexArray>& TheatreManager::GetVertexArray(ID ID)
{
    if(auto found_it{mTheatreVAOs.find(ID)}; found_it != mTheatreVAOs.end())
        { return found_it->second; }
    return mTheatreVAOs.at(UID::m_Error);
}

void TheatreManager::BufferEmbeddedResources()
{
    BufferMesh({Models::Error,         std::size(Models::Error),      FileType::model_OBJ}, UID::m_Error);
    BufferMesh({Models::Cube,          std::size(Models::Cube),       FileType::model_OBJ}, UID::m_Cube);
    BufferMesh({Models::Ramiel,        std::size(Models::Ramiel),     FileType::model_OBJ}, UID::m_Ramiel);
    BufferTexture({Images::Missing,    std::size(Images::Missing),    FileType::image_PNG}, UID::i_Missing);
    BufferTexture({Images::LolBit,     std::size(Images::LolBit),     FileType::image_PNG}, UID::i_LolBit);
    BufferTexture({Images::LightDebug, std::size(Images::LightDebug), FileType::image_JPG}, UID::i_LightDebug);
    BufferTexture({Images::COMP04_5,   std::size(Images::COMP04_5),   FileType::image_PNG}, UID::i_COMP04_5);
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
    sLocalPlayer->Update();
    for(auto& [id, thing] : mThings)
        { thing->Update(); }
}

void TheatreManager::Tick()
{
    if(GetTheatreState() != IN_LEVEL)
        { return; }
    const std::lock_guard<std::recursive_mutex> lock{mThingsMutex};
    sLocalPlayer->Tick();
    for(auto& [id, thing] : mThings)
        { thing->Tick(); }
}

void TheatreManager::Input(InputEvent* inInput)
{
    if(GetTheatreState() != IN_LEVEL)
        { return; }
    const std::lock_guard<std::recursive_mutex> lock{mThingsMutex};
    sLocalPlayer->Input(inInput);
    for(auto& [id, thing] : mThings)
        { thing->Input(inInput); }
}

Shared<Thing> TheatreManager::GetThing(ID inID)
{
    const std::lock_guard<std::recursive_mutex> lock{mThingsMutex};
    return (mThings.contains(inID) && GetTheatreState() == IN_LEVEL)
        ? mThings.at(inID)
        : MakeShared<Thing>();
}

void TheatreManager::DrawTheatre()
{
    if(!sReadyToRender || GetTheatreState() != IN_LEVEL)
        { return; }
    const std::lock_guard<std::recursive_mutex> lock{mThingsMutex};
    light_t::ClearCounts();
#pragma message("TODO: I have to loop through everything twice since I've removed the render command, so fix that...")
    for(auto& [id, thing] : mThings)
    {
        if(auto light{DCast<light_t>(thing)}; light and light->IncrementIndex())
        {
            g_pRenderManager->GetAPI()->SetLight_TempBlinnPhongSolution(light.get());
            auto material{g_pTheatreManager
                ->GetThing<Material>(g_pTheatreManager
                    ->GetThing<MeshInstance>(light->MeshInstanceID())
                        ->GetMaterialID())};
            material->mColor = light->mColor;
            material->mFullBright = true;
        }
    }
    for(auto& [id, thing] : mThings)
    {
        if(auto actor{DCast<Actor>(thing)}; actor and actor->mVisible)
        {
            auto mesh_instance{g_pTheatreManager->GetThing<MeshInstance>(actor->MeshInstanceID())};
            auto material{g_pTheatreManager->GetThing<Material>(mesh_instance->GetMaterialID())};
            auto& renderer_api{g_pRenderManager->GetAPI()};
            auto shader{renderer_api->GetShader((material->mFullBright) ? Shaders::Fullbright : Shaders::BlinnPhong)};

            renderer_api->SetFramebufferSRGB(!material->mDontUseTexture);
            renderer_api->SetWireframe(Settings::Graphics::GlobalWireframe || actor->mWireframe);

            GetTextureBuffer(material->GetDiffuseTexture()[])->Bind(0);
            GetTextureBuffer(material->GetSpecularTexture()[])->Bind(1);

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
            shader->SetUniform("view_matrix", sLocalPlayer->ViewMatrix());
            shader->SetUniform("view_position", sLocalPlayer->ViewPosition());
            shader->SetUniform("current_material.texture_diffuse",  0);
            shader->SetUniform("current_material.texture_specular", 1);
            shader->SetUniform("current_material.use_textures", !material->mDontUseTexture);
            shader->SetUniform("current_material.diffuse_color", material->mColor);
            shader->SetUniform("current_material.alpha", material->mAlpha);
            shader->SetUniform("current_material.specular_sharpness", material->mSpecularSharpness);
            shader->SetUniform("current_material.specular_strength", material->mSpecularStrength);
            renderer_api->DrawIndexed(GetVertexArray(mesh_instance->GetMeshID()[]));
            renderer_api->SetFramebufferSRGB(false);
        }
    }
}

ManagerEnums::TheatreReturnValue_t TheatreManager::TheatreInit(bool is_first_call)
{
    if(!is_first_call)
        { return FINISHED; }
    sReadyToRender = false;
    mTheatreVAOs.clear();
    mTheatreTextures.clear();
    BufferEmbeddedResources();
    CreateThings();
    ReadyThings();
    sReadyToRender = true;
    return FINISHED;
}

ManagerEnums::TheatreReturnValue_t TheatreManager::TheatreShutdown(bool is_first_call)
{
    if(!is_first_call)
        { return FINISHED; }
    sReadyToRender = false;
    DestroyThings();
    mTheatreVAOs.clear();
    mTheatreTextures.clear();
    return FINISHED;
}

void TheatreManager::ReadyThings()
{
    for(const auto& [id, thing] : mThings)
    {
        thing->Ready();
        if(auto mesh{DCast<Mesh>(thing)})
            { BufferMesh(*mesh->Data(), mesh->uid()); }
        else if(auto texture{DCast<Texture>(thing)})
            { BufferTexture(*texture->Data(), texture->uid()); }
    }
}

bool TheatreManager::ThingExists(ID uid)
{ return mThings.contains(uid); }

Farg<TTID> TheatreManager::GetType(ID uid)
{
    static TTID invalid{ID::Invalid};
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
    TheatreData data{sCurrentTheatreData};
    data.clear();
    for(const auto& [id, thing] : mThings)
        { data.AddData(*thing->GetVariables()); }
    return data;
}

std::vector<ID> TheatreManager::GetThingIDs()
{
    auto keys{std::views::keys(mThings)};
    return {keys.begin(), keys.end()};
}

uint TheatreManager::CreateThing(Farg<ThingData> inData)
{
    Shared<Thing> thing;
    ThingData data{inData};

    if(mThings.contains(data.uid))
    {
        print_warning("A Thing with uid #{} already exists; a new ID will be generated and a duplicate Thing will be made", data.uid[]);
        if(!UID::Contains(data.uid[]))
            { std::println("{}[NOTE] uid #{} wasn't generated by `UIDs`, which is what caused the previous warning (it has just been added to avoid future issues){}", Sty::Bold + Fg::Cyan, data.uid[], Sty::Reset); }
        data.uid = UID::Generate();
    }

    if(data.type() == ThingType::NostalgiaPlayer)
    {
        thing = static_pointer_cast<Thing>(sLocalPlayer);
        thing->SetVariables(ThingData::PlayerDefaultData);
    }
    else
    { thing = mThings[data.uid] = g_pThingFactory->MakeThing(data.type())(); }

    thing->SetVariables(data);
    if(auto collider{DCast<Collider>(thing)})
        { g_pPhysicsManager->CreateBody(thing->uid(), collider); }

    g_pVariableRegistry->RegisterID(thing->name(), thing->uid()[]);
    return thing->uid()[];
}

Shared<NostalgiaPlayer> TheatreManager::GetLocalPlayer()
{ return sLocalPlayer; }

bool TheatreManager::DestroyThing(ID id)
{
    if(!mThings.contains(id))
    {
        print_warning("No Thing with uid#{} exists", id[]);
        return false;
    }
    else if(id == sLocalPlayer->uid())
    {
        print_warning("Cannot destroy the NostalgiaPlayer Thing! (uid#{})", sLocalPlayer->uid()[]);
        print_debug("Player ID: {}, Thing ID: {}", sLocalPlayer->uid()[], id[]);
        return false;
    }
    if(!UID::Contains(id[]))
        { print_warning("{} is not in 's_ExistingIDs', but is somehow in 'mThings'! The Thing will still be destroyed, but this is highly unusual behaviour!", id[]); }

    std::string thing_name{mThings.at(id)->name()};
    if(mThings.erase(id))
    {
        if(!g_pVariableRegistry->RemoveID(id[]) and !g_pVariableRegistry->RemoveID(thing_name))
            { print_warning("Unable to remove the Thing UID {} from the variable registry!", id[]); }
        return true;
    }
    return false;
}

void TheatreManager::CreateThings()
{
    Time::Wait(mDestroyingThings);
    mCreatingThings = true;
    sReadyToRender = false;

    sCurrentTheatreData.SetupUIDsAndPriorities();
    if(gPrintLoadedTheatreData)
        { sCurrentTheatreData.debug_PrintData(); }

    for(ThingData& data : sCurrentTheatreData.things_data)
        { CreateThing(data); }

    sReadyToRender = true;
    mCreatingThings = false;
}

void TheatreManager::DestroyThings()
{
    Time::Wait(mCreatingThings);
    mDestroyingThings = true;
#pragma message("When I implement multiple Theatres, they should be able to clear their own sets of IDs")
    for(const auto& [id, thing] : mThings)
    {
        if(auto collider{DCast<Collider>(thing)})
            { g_pPhysicsManager->DestroyBody(id, collider); }
    }
    UID::Clear();
    mThings.clear();
    mDestroyingThings = false;
}

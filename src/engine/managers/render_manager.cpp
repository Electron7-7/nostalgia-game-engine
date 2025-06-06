#include "render_manager.hpp"
#include "backend_manager.hpp"
#include "world_manager.hpp"
#include "engine/backends/backend.hpp"
#include "math/math_definitions.hpp"
#include <glm/glm.hpp>
#define GLM_ENABLE_EXPERIMENTAL
#include <glm/gtc/quaternion.hpp>

// Singleton Accessor
RenderManager singleton_RenderManager; // When making documentation about naming conventions, remember that the prefix "s_" stands for "singleton_"
RenderManager* global_RenderManager = &singleton_RenderManager; // When making documentation about naming conventions, remember that the prefix "g_" stands for "global_"

// Prototype Functions
bool RenderManager::prototype_SetFullscreen(bool enable_fullscreen)
{
    if(global_BackendManager->GetWindowingBackend()->prototype_SetFullscreen(enable_fullscreen))
        return true;

    return false;
}
// End Prototype Functions


bool RenderManager::Init()
{
    return true;
}

void RenderManager::Shutdown()
{}

TheatreReturnValue_t RenderManager::TheatreInit(bool is_first_call)
{ return FINISHED; }

TheatreReturnValue_t RenderManager::TheatreShutdown(bool is_first_call)
{ return FINISHED; }

/*void RenderManager::UpdateLocalPlayerCamera()
{
    float delta_time = _Manager::DeltaTime();
    R_Camera* camera = world_manager->GetLocalPlayer()->camera_property;

    camera->origin.x = camera->origin.x + delta_time * camera->velocity.x;
    camera->origin.y = camera->origin.y + delta_time * camera->velocity.y;
    camera->origin.z = camera->origin.z + delta_time * camera->velocity.z;

    camera->angles.x = camera->angles.x + delta_time * camera->angular_velocity.x;
    camera->angles.y = camera->angles.y + delta_time * camera->angular_velocity.y;
    camera->angles.z = camera->angles.z + delta_time * camera->angular_velocity.z;
}*/

void RenderManager::Update()
{
    if(GetTheatreState() == NOT_IN_LEVEL)
    {
        global_BackendManager->GetGraphicsBackend()->prototype_ClearBuffer(glm::vec4(0.29f, 0.34f, 0.26f, 1.0f));
        // ui_manager->DrawUI();
        global_BackendManager->GetWindowingBackend()->prototype_SwapBuffers();
        return;
    }

    global_BackendManager->GetGraphicsBackend()->prototype_ClearBuffer(glm::vec4(0.0f, 0.0f, 0.0f, 1.0f));
    global_WorldManager->RenderWorld();
    // ui_manager->DrawUI();
    global_BackendManager->GetWindowingBackend()->prototype_SwapBuffers();
}

// FIXME: Replace with functions for better control over view distance & near-clip distance
#define ZNEAR 0.01f
#define ZFAR 100000.0f

void RenderManager::SetProjectionMatrix(int width, int height, float fov)
{
    glm::mat4 projection_matrix;
    float z_near = ZNEAR;
    float z_far = ZFAR;
    float aspect_ratio = (height != 0.0f) ? (float)width / (float)height : 100.0f;

    float half_width = tan(fov * M_PI / 360.0);
    float half_height = half_width / aspect_ratio;

    projection_matrix[0][0] = 1.0f / half_width;
    projection_matrix[1][1] = 1.0f / half_height;
    projection_matrix[2][2] = z_far / (z_near - z_far);
    projection_matrix[3][3] = -1.0f;
    projection_matrix[2][3] = z_near * z_far / (z_near - z_far);

    // MaterialRenderContextPointer render_context(material_system);
    // render_context->MatrixMode(MATERIAL_PROJECTION);
    // render_context->LoadMatrix(projection_matrix);
}

void RenderManager::SetOrthoMatrix(int width, int height)
{
    // MaterialRenderContextPointer render_context(material_system);
    // render_context->MatrixMode(MATERIAL_PROJECTION);
    // render_context->LoadIdentity();
    // render_context->Ortho(0, 0, width, height, -1.0f, 1.0f);
}

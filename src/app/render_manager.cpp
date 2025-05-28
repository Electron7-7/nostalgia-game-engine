#include "render_manager.hpp"
#include <math/math_definitions.hpp>


CameraProperty::CameraProperty()
{}

// FIXME: use global orientation functions, similar to "graphx::orientation::up/front/right"
// (use functions so that I can account for different graphics APIs)
glm::vec3 shitty_global_front = glm::vec3(0.0f, 1.0f, -1.0f);

void CameraProperty::GetForward(glm::vec3* forward_vector)
{
    *forward_vector = quaternion * shitty_global_front;
}

// Singleton Accessor
static RenderManager singleton_RenderManager; // When making documentation about naming conventions, remember that the prefix "s_" stands for "singleton_"
RenderManager* global_RenderManager = &singleton_RenderManager; // When making documentation about naming conventions, remember that the prefix "g_" stands for "global_"

bool RenderManager::Init()
{
    render_world_fullscreen = true;
    return true;
}

void RenderManager::Shutdown()
{}

TheatreReturnValue_t RenderManager::TheatreInit(bool is_first_call)
{ return FINISHED; }

TheatreReturnValue_t RenderManager::TheatreShutdown(bool is_first_call)
{ return FINISHED; }

CameraProperty* RenderManager::CreateCameraProperty()
{ return new CameraProperty; }

void RenderManager::DestroyCameraProperty(CameraProperty* property)
{ delete property; }

void RenderManager::RenderWorldFullscreen()
{ render_world_fullscreen = true; }

void RenderManager::RenderWorldInrect(int x, int y, int width, int height)
{
    render_world_fullscreen = false;
    render_x = x;
    render_y = y;
    render_width = width;
    render_height = height;
}

void RenderManager::UpdateLocalPlayerCamera()
{
    float delta_time = _Manager::DeltaTime();
    // CameraProperty* camera = world_manager->GetLocalPlayer()->camera_property;

    // camera->origin.x = camera->origin.x + delta_time * camera->velocity.x;
    // camera->origin.y = camera->origin.y + delta_time * camera->velocity.y;
    // camera->origin.z = camera->origin.z + delta_time * camera->velocity.z;

    // camera->angles.x = camera->angles.x + delta_time * camera->angular_velocity.x;
    // camera->angles.y = camera->angles.y + delta_time * camera->angular_velocity.y;
    // camera->angles.z = camera->angles.z + delta_time * camera->angular_velocity.z;
}

void RenderManager::Update()
{
    // MaterialRenderContextPointer render_context(material_system);
    if(GetTheatreState() == NOT_IN_LEVEL)
    {
        // material_system->BeginFrame(0);
        // render_context->ClearColor4ub(76, 88, 68, 255);
        // render_context->ClearBuffers(true, true);
        // ui_manager->DrawUI();
        // material_system->EndFrame();
        // material_system->SwapBuffers();
        return;
    }

    UpdateLocalPlayerCamera();

    // material_system->BeginFrame(0);
    // render_context->ClearColor4ub(0, 0, 0, 255);
    // render_context->ClearBuffers(true, true);
    RenderWorld();
    // ui_manager->DrawUI();
    // material_system->EndFrame();
    // material_system->SwapBuffers();
}

void RenderManager::SetupCameraRenderState()
{

}

// FIXME: Better control over Z range
#define ZNEAR 0.01f
#define ZFAR 100000.0f

void RenderManager::SetupProjectionMatrix(int width, int height, float fov)
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

void RenderManager::SetupOrthoMatrix(int width, int height)
{
    // MaterialRenderContextPointer render_context(material_system);
    // render_context->MatrixMode(MATERIAL_PROJECTION);
    // render_context->LoadIdentity();
    // render_context->Ortho(0, 0, width, height, -1.0f, 1.0f);
}

void RenderManager::RenderWorld()
{
    // MaterialRenderContextPointer render_context(material_system);
    // render_context->MatrixMode(MATERIAL_PROJECTION);
    // render_context->PushMatrix();

    // render_context->MatrixMode(MATERIAL_VIEW);
    // render_context->PushMatrix();

    // render_context->MatrixMode(MATERIAL_MODEL);
    // render_context->PushMatrix();
    // render_context->LoadIdentity();

    if(render_world_fullscreen)
    {
        render_x = render_y = 0;
        // render_context->GetRenderTargetDimensions(render_width, render_height);
    }

    // render_context->DepthRange(0, 1);
    // render_context->Viewport(render_x, render_y, render_width, render_height);

    SetupProjectionMatrix(render_width, render_height, 90);

    SetupCameraRenderState();

    // world_manager->DrawWorld();

    // render_context->MatrixMode(MATERIAL_PROJECTION);
    // render_context->PopMatrix();

    // render_context->MatrixMode(MATERIAL_VIEW);
    // render_context->PopMatrix();

    // render_context->MatrixMode(MATERIAL_MODEL);
    // render_context->PopMatrix();
}
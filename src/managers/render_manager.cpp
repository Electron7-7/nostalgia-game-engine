#include "render_manager.hpp"
#include "theatre_manager.hpp"
#include "ui_manager.hpp"
#include "backend_manager.hpp"

#include <glm/glm.hpp>
#define GLM_ENABLE_EXPERIMENTAL
#include <glm/gtc/quaternion.hpp>

using namespace ManagerEnums;

static RenderManager s_RenderManager;
RenderManager* g_pRenderManager = &s_RenderManager;


bool RenderManager::Init()
{
    return true;
}

void RenderManager::Shutdown()
{}

ManagerEnums::TheatreReturnValue_t RenderManager::TheatreInit(bool is_first_call)
{ return FINISHED; }

ManagerEnums::TheatreReturnValue_t RenderManager::TheatreShutdown(bool is_first_call)
{ return FINISHED; }

void RenderManager::Update()
{
    if(GetTheatreState() == NOT_IN_LEVEL)
    {
        g_pBackendManager->Graphics()->ClearBuffer(glm::vec4(0.29f, 0.34f, 0.26f, 1.0f));
        g_pUIManager->DrawUI();
        g_pBackendManager->Windowing()->SwapBuffers();
        return;
    }

    g_pBackendManager->Graphics()->ClearBuffer(glm::vec4(0.29f, 0.34f, 0.26f, 1.0f));
    g_pTheatreManager->RenderWorld();
    g_pUIManager->DrawUI();
    g_pBackendManager->Windowing()->SwapBuffers();
}

// #pragma message("(FIXME) Replace with functions for better control over view distance & near-clip distance")
// #define ZNEAR 0.01f
// #define ZFAR 100000.0f

// void RenderManager::SetProjectionMatrix(int width, int height, float fov)
// {
//     glm::mat4 projection_matrix;
//     float z_near = ZNEAR;
//     float z_far = ZFAR;
//     float aspect_ratio = (height != 0.0f) ? (float)width / (float)height : 100.0f;

//     float half_width = tan(fov * std::numbers::pi / 360.0);
//     float half_height = half_width / aspect_ratio;

//     projection_matrix[0][0] = 1.0f / half_width;
//     projection_matrix[1][1] = 1.0f / half_height;
//     projection_matrix[2][2] = z_far / (z_near - z_far);
//     projection_matrix[3][3] = -1.0f;
//     projection_matrix[2][3] = z_near * z_far / (z_near - z_far);
// }

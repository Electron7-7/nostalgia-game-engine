#include "opengl.hpp"
#include "opengl_includes.hpp"
#include "common/debugging.hpp"
#include "world/3d_common.hpp"
#include "engine/rendering/render_command.hpp"

//--------------------
// PROTOTYPE FUNCTIONS
//--------------------

void OpenGL_Backend::prototype_ClearBuffer(glm::vec4 clear_color)
{
    glClearColor(clear_color[0], clear_color[1], clear_color[2], clear_color[3]);
    glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
}

void OpenGL_Backend::prototype_RenderFrame()
{
    for(int i = 0 ; i < global_RenderCommandQueue.size() ; i++)
    {
        // RenderContext render_context = global_RenderCommandQueue.at(i).render_context;
        // Do rendering stuff...
        global_RenderCommandQueue.erase(global_RenderCommandQueue.begin() + i); // Remove the render command from the queue
        PRINTDEBUG("Processed RenderCommand #" + std::to_string(i));
    }
}

//------------------------
// END PROTOTYPE FUNCTIONS
//------------------------


bool OpenGL_Backend::Init()
{
    if(is_initialized)
        return true;

    // TODO: See note in 'src/world/3d_common.hpp'
    World::Orientation::SetWorldUp(glm::vec3(0.0f, 1.0f, 0.0f));
    World::Orientation::SetWorldRight(glm::vec3(1.0f, 0.0f, 0.0f));
    World::Orientation::SetWorldFront(glm::vec3(0.0f, 0.0f, -1.0f));

    is_initialized = true;
    return true;
}

void OpenGL_Backend::Shutdown()
{ is_initialized = false; }

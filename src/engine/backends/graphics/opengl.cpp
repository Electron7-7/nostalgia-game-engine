#include "opengl.hpp"
#include "opengl_includes.hpp"
#include "debugging.hpp"
#include "world/3d_common.hpp"

//--------------------
// PROTOTYPE FUNCTIONS
//--------------------

void OpenGL_Backend::prototype_ClearBuffer(glm::vec4 clear_color)
{
    glClearColor(clear_color[0], clear_color[1], clear_color[2], clear_color[3]);
    glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
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

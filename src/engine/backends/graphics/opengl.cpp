#include "opengl.hpp"
#include "opengl_includes.hpp"
#include "debugging.hpp"
#include "world/3d_common.hpp"
#include "engine/rendering/shader_interfaces/shader_interface.hpp"
#include "engine/rendering/shader_interfaces/gl_shader.hpp"
#include "engine/embedded/opengl_shaders.hpp"

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

    // The safety shader must ALWAYS compile; the rest can fail without causing crashes
    if(!BuildShader(Shaders::SAFETY, glsl_safety_shader_vert, glsl_safety_shader_frag))
        return false;

    BuildShader(Shaders::BLINN_PHONG, glsl_blinn_phong_vert, glsl_blinn_phong_frag);

    is_initialized = true;
    return true;
}

void OpenGL_Backend::Shutdown()
{ is_initialized = false; }

bool OpenGL_Backend::BuildShader(unsigned int shader_label, const std::string& vertex_shader_code, const std::string& fragment_shader_code)
{
    if(shaders.contains(shader_label))
        PRINTWARN("OpenGL_Backend::BuildShader - a shader with the supplied label already exists and will be overwritten");

    shaders[shader_label] = GLShader(vertex_shader_code, fragment_shader_code);
    return shaders.at(shader_label).IsValid();
}


// FIXME: Is it a good idea to let outside code access shader interfaces directly?
const ShaderInterface* OpenGL_Backend::GetShader(unsigned int shader_selection) const
{
    if(shaders.size() <= shader_selection)
    {
        PRINTERR("OpenGL_Backend::GetShader - index overflow! Returning nullptr");
        return nullptr;
    }

    return &shaders.at(shader_selection);
}

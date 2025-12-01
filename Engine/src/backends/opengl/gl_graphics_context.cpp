#include "gl_graphics_context.hpp"
#include "core/printing.hpp"

#include <GLFW/glfw3.h>
#include <glad/glad.h>

OpenGLContext::OpenGLContext(GLFWwindow* inNativeWindow):
    mWindow{inNativeWindow}
{ if(!mWindow) { print_error("mWindow is nullptr!"); } }

Error OpenGLContext::Init()
{
    glfwMakeContextCurrent(mWindow);
    if(!gladLoadGLLoader((GLADloadproc)glfwGetProcAddress))
        { print_error("Failed to initialize GLAD!"); return ERR_INIT_FAILED; }
    print_debug("Using OpenGL Graphics API\n\tVendor: {}\n\tRenderer: {}\n\tVersion: {}",
        (const char*)glGetString(GL_VENDOR),
        (const char*)glGetString(GL_RENDERER),
        (const char*)glGetString(GL_VERSION));
    return OK;
}

void OpenGLContext::SwapBuffers()
{ glfwSwapBuffers(mWindow); }

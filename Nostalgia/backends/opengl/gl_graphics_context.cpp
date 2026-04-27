#include "gl_graphics_context.hpp"
#include "thirdparty/glad/glad.h"
#include "thirdparty/glfw/include/GLFW/glfw3.h"

OpenGLContext::OpenGLContext(GLFWwindow* inNativeWindow):
    mWindow{inNativeWindow}
{
    if(not inNativeWindow)
        { print_error("Native window pointer is nullptr or of invalid type (required: GLFWwindow*)"); }
}

#pragma message("TODO: if GLFW is the only windowing solution, this is fine; otherwise, make this agnostic!")
Error OpenGLContext::Init()
{
    glfwMakeContextCurrent(mWindow);
    if(not gladLoadGLLoader(reinterpret_cast<GLADloadproc>(glfwGetProcAddress)))
    {
        print_error("Failed to initialize GLAD");
        return ERR_INIT_FAILED;
    }
    print_debug("Using OpenGL Graphics API\n\tVendor: {}\n\tRenderer: {}\n\tVersion: {}",
        (const char*)glGetString(GL_VENDOR),
        (const char*)glGetString(GL_RENDERER),
        (const char*)glGetString(GL_VERSION));
    return OK;
}

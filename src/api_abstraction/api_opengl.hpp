#ifndef API_OPENGL_H
#define API_OPENGL_H

#include "api_dedicated.hpp"
#include <macros/forward_declare.hpp>

FORWARD_DECLARE(struct GLFWwindow;)

class API_GL : public API
{
    virtual bool Init();
    virtual void Shutdown();
    virtual bool CreateMainWindow();

protected:
    GLFWwindow* glfw_MainWindow = nullptr;
};

#endif // API_OPENGL_H
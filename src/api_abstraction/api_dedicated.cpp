#include "api_dedicated.hpp"
#include "api_opengl.hpp"

API_GL singleton_OpenGLAPI;

API* GraphicsAPI::OpenGL = &singleton_OpenGLAPI;
API* GraphicsAPI::Default = &singleton_OpenGLAPI;
/*
    This file exists to make sure that glad.h is ALWAYS included BEFORE glfw3.h and is NEVER included MORE than ONCE
*/
#ifndef OPENGL_INCLUDES_H
#define OPENGL_INCLUDES_H
#include "glad/glad.h"
#include "GLFW/glfw3.h"
#endif // OPENGL_INCLUDES_H

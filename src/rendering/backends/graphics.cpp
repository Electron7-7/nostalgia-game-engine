#include "graphics.hpp"
#include "../shader_interfaces/shader_interface.hpp"

unsigned int GraphicsBackend::s_CurrentlyBoundShader = Shaders::BlinnPhong;

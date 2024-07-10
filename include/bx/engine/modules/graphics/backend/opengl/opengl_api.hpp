#pragma once

#include "bx/engine/core/type.hpp"

#ifdef BX_WINDOW_GLFW_BACKEND
#include <GLFW/glfw3.h>
#endif // BX_WINDOW_GLFW_BACKEND

#include <glad/glad.h>

namespace Gl
{
	b8 Init(b8 debug);
}
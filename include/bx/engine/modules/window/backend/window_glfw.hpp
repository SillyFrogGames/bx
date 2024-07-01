#pragma once

#include "bx/engine/modules/window.hpp"

#include <GLFW/glfw3.h>

class WindowGLFW
{
public:
	static GLFWwindow* GetWindowPtr();
};
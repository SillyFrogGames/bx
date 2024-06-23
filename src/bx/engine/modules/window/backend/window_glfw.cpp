#ifdef WINDOW_GLFW_BACKEND
#include "Engine/Modules/Window.hpp"

#include "Engine/Core/Macros.hpp"
#include "Engine/Core/Data.hpp"
#include "Engine/Core/Profiler.hpp"

#include <GLFW/glfw3.h>
#include <stdlib.h>

static GLFWwindow* pWindow = nullptr;

bool Window::IsOpen()
{
	return !glfwWindowShouldClose(pWindow);
}

void Window::GetSize(int* width, int* height)
{
	glfwGetFramebufferSize(pWindow, width, height);
}

void Window::SetCursorMode(CursorMode mode)
{
	switch (mode)
	{
	case CursorMode::NORMAL:
		glfwSetInputMode(pWindow, GLFW_CURSOR, GLFW_CURSOR_NORMAL);
		break;
	case CursorMode::HIDDEN:
		glfwSetInputMode(pWindow, GLFW_CURSOR, GLFW_CURSOR_HIDDEN);
		break;
	case CursorMode::DISABLED:
		glfwSetInputMode(pWindow, GLFW_CURSOR, GLFW_CURSOR_DISABLED);
		break;
	case CursorMode::CAPTURED:
		glfwSetInputMode(pWindow, GLFW_CURSOR, GLFW_CURSOR_CAPTURED);
		break;
	default:
		glfwSetInputMode(pWindow, GLFW_CURSOR, GLFW_CURSOR_NORMAL);
	}
}

static void glfw_error_callback(int i, const char* c)
{
	ENGINE_LOGE("GLFW ({}) {}", i, c);
}

static void glfw_window_size_callback(GLFWwindow* window, int width, int height)
{
	Screen::SetWidth(width);
	Screen::SetHeight(height);
}

bool Window::Create()
{
#ifdef __arm__
	if (putenv((char*)"DISPLAY=:0"))
	{
		ENGINE_LOGE("Failed to set DISPLAY enviroment variable!");
		return false;
	}
#endif

	glfwSetErrorCallback(glfw_error_callback);
	if (!glfwInit())
	{
		ENGINE_LOGE("Failed to initialize GLFW!");
		return false;
	}

	const String& title = Data::GetString("Title", "Title", DataTarget::SYSTEM);
	int width = Data::GetInt("Width", 800, DataTarget::SYSTEM);
	int height = Data::GetInt("Height", 600, DataTarget::SYSTEM);

	GLFWmonitor* pMonitor = nullptr;

#ifdef GRAPHICS_OPENGL_BACKEND

#ifdef DEBUG_BUILD
	glfwWindowHint(GLFW_OPENGL_DEBUG_CONTEXT, GLFW_TRUE);
#else
	glfwWindowHint(GLFW_OPENGL_DEBUG_CONTEXT, GLFW_FALSE);
#endif

#ifdef WIN32
	glfwWindowHint(GLFW_CONTEXT_VERSION_MAJOR, 4);
	glfwWindowHint(GLFW_CONTEXT_VERSION_MINOR, 6);
	glfwWindowHint(GLFW_CLIENT_API, GLFW_OPENGL_API);
	glfwWindowHint(GLFW_OPENGL_PROFILE, GLFW_OPENGL_CORE_PROFILE);
	glfwWindowHint(GLFW_SAMPLES, 8);

	glfwWindowHint(GLFW_MAXIMIZED, GLFW_TRUE);

#elif defined __arm__
	glfwWindowHint(GLFW_CONTEXT_VERSION_MAJOR, 3);
	glfwWindowHint(GLFW_CONTEXT_VERSION_MINOR, 1);
	glfwWindowHint(GLFW_CLIENT_API, GLFW_OPENGL_ES_API);
	//glfwWindowHint(GLFW_OPENGL_PROFILE, GLFW_OPENGL_CORE_PROFILE);
	//glfwWindowHint(GLFW_SAMPLES, 2);

	pMonitor = glfwGetPrimaryMonitor();
	const GLFWvidmode* pMode = glfwGetVideoMode(pMonitor);
	width = pMode->width;
	height = pMode->height;

	glfwWindowHint(GLFW_RED_BITS, pMode->redBits);
	glfwWindowHint(GLFW_GREEN_BITS, pMode->greenBits);
	glfwWindowHint(GLFW_BLUE_BITS, pMode->blueBits);
	glfwWindowHint(GLFW_REFRESH_RATE, pMode->refreshRate);
#endif

#else
	glfwWindowHint(GLFW_CLIENT_API, GLFW_NO_API);
	glfwWindowHint(GLFW_MAXIMIZED, GLFW_TRUE);
#endif

	pWindow = glfwCreateWindow(width, height, title.c_str(), pMonitor, NULL);

	if (pWindow == NULL)
	{
		ENGINE_LOGE("Failed to create GLFW window!");
		glfwTerminate();
		return false;
	}

	glfwGetWindowSize(pWindow, &width, &height);
	Screen::SetWidth(width);
	Screen::SetHeight(height);

	glfwSetWindowSizeCallback(pWindow, glfw_window_size_callback);

#ifdef GRAPHICS_OPENGL_BACKEND
	glfwMakeContextCurrent(pWindow);
	glfwSwapInterval(1);

#elif defined GRAPHICS_VULKAN_BACKEND
	if (!glfwVulkanSupported())
	{
		ENGINE_LOGE("GLFW: Vulkan Not Supported!");
		return false;
	}
#endif

	return true;
}

void Window::Destroy()
{
	glfwDestroyWindow(pWindow);
	glfwTerminate();
}

void* Window::GetDevicePtr()
{
	return (void*)pWindow;
}

void Window::PollEvents()
{
	PROFILE_FUNCTION();
	glfwPollEvents();

	if (glfwGetKey(pWindow, GLFW_KEY_ESCAPE) == GLFW_PRESS)
		glfwSetWindowShouldClose(pWindow, true);
}

void Window::Display()
{
	PROFILE_FUNCTION();

#ifdef GRAPHICS_OPENGL_BACKEND
	glfwSwapBuffers(pWindow);
#endif
}

#endif
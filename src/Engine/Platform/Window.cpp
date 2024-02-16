#include <Engine/Platform/Window.hpp>

#include <Core/Module.hpp>

#include <iostream>

void Window::Open()
{
	Module::Register();

#ifdef EDITOR_BUILD
	std::cout << "EDITOR Window::Open" << std::endl;
#else
	std::cout << "Window::Open" << std::endl;
#endif
}
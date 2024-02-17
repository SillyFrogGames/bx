#include <Dummy.hpp>

#include <Core/Module.hpp>

#include <iostream>

void Dummy::Initialize()
{
#ifdef EDITOR_BUILD
	std::cout << "EDITOR Dummy::Test" << std::endl;
	Module::EditorPrint();
#else
	std::cout << "Dummy::Test" << std::endl;
#endif

	Module::Register();
}

void Dummy::Shutdown()
{
}

Plugin* Load()
{
	return new Dummy();
}
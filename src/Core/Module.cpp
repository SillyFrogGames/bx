#include <Core/Module.hpp>

#include <iostream>

void Module::Register()
{
#ifdef EDITOR_BUILD
	std::cout << "EDITOR Module::Register" << std::endl;
#else
	std::cout << "Module::Register" << std::endl;
#endif
}
#include <Dummy.hpp>

#include <iostream>

void Dummy::Test()
{
#ifdef EDITOR_BUILD
	std::cout << "EDITOR Dummy::Test" << std::endl;
#else
	std::cout << "Dummy::Test" << std::endl;
#endif
}
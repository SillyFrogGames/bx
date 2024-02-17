#include <Core/Plugin.hpp>

#include <Windows.h>
#include <iostream>
#include <cassert>

#include <vector>

struct PluginInfo
{
	HINSTANCE hInst = NULL;
	Plugin* pPlugin = nullptr;
};

static std::vector<PluginInfo> g_plugins;

int PluginRegistry::Register(const char* filepath)
{
	PluginInfo info;
	info.hInst = LoadLibrary(filepath);

	if (info.hInst == NULL)
	{
		std::cout << "Cannot locate the .dll file" << std::endl;
		assert(false);
	}

	LoadProc loadFunc = (LoadProc)GetProcAddress(info.hInst, "Load");
	if (!loadFunc)
	{
		std::cout << "Could not locate the function" << std::endl;
		assert(false);
	}

	info.pPlugin = loadFunc();

	if (info.pPlugin->Version() != BX_ENGINE_VERSION)
	{
		std::cout << "Plugin was built using a different engine version" << std::endl;
		assert(false);
	}

	g_plugins.emplace_back(info);

	return g_plugins.size() - 1;
	
	//delete ptr;
	//FreeLibrary(info.hInst);
}

void PluginRegistry::Load(int plugin)
{
	auto& info = g_plugins[plugin];
	info.pPlugin->Initialize();
}

void PluginRegistry::Unload(int plugin)
{
	auto& info = g_plugins[plugin];
	info.pPlugin->Shutdown();
}
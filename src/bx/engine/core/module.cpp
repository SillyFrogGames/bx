#include "bx/engine/core/module.hpp"
#include "bx/engine/containers/list.hpp"

struct ModuleHandle
{
	ModuleInitializeFn Initialize = nullptr;
	ModuleBindApiFn BindApi = nullptr;
	ModuleReloadFn Reload = nullptr;
	ModuleShutdownFn Shutdown = nullptr;
};

static List<ModuleHandle> s_wrenModulesSource;

void Module::Register(ModuleInitializeFn initialize, ModuleReloadFn bindApi, ModuleReloadFn reload, ModuleShutdownFn shutdown)
{
	ModuleHandle module;
	module.Initialize = initialize;
	module.BindApi = bindApi;
	module.Reload = reload;
	module.Shutdown = shutdown;

	s_wrenModulesSource.emplace_back(module);
}

void Module::Initialize()
{
	for (auto& module : s_wrenModulesSource)
		if (module.Initialize)
			module.Initialize();
}

void Module::BindApi()
{
	for (auto& module : s_wrenModulesSource)
		if (module.BindApi)
			module.BindApi();
}

void Module::Reload()
{
	for (auto& module : s_wrenModulesSource)
		if (module.Reload)
			module.Reload();
}

void Module::Shutdown()
{
	for (auto& module : s_wrenModulesSource)
		if (module.Shutdown)
			module.Shutdown();
}

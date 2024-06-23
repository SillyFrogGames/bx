#pragma once

// TODO: This is meant to be the common entry point for dynamically loaded libraries
// In the engine source code a few built-in modules are provided, however, they do not use this API and should!
// There should be considerations for inter-module dependencies, so for now this remains unused.

using ModuleInitializeFn = void(*)();
using ModuleBindApiFn = void(*)();
using ModuleReloadFn = void(*)();
using ModuleShutdownFn = void(*)();

class Module
{
public:
	static void Register(ModuleInitializeFn initialize, ModuleReloadFn bindApi, ModuleReloadFn reload, ModuleShutdownFn shutdown);

private:
	friend class Application;
	static void Initialize();
	static void BindApi();
	static void Reload();
	static void Shutdown();
};
#include "bx/runtime/game_application.hpp"

#include <bx/engine/core/memory.hpp>
#include <bx/engine/core/log.hpp>
#include <bx/engine/core/time.hpp>
#include <bx/engine/core/data.hpp>
#include <bx/engine/core/profiler.hpp>
#include <bx/engine/core/file.hpp>
#include <bx/engine/core/input.hpp>
#include <bx/engine/core/module.hpp>
#include <bx/engine/core/ecs.hpp>
#include <bx/engine/core/resource.hpp>
#include <bx/engine/modules/window.hpp>
#include <bx/engine/modules/graphics.hpp>
#include <bx/engine/modules/physics.hpp>
#include <bx/engine/modules/script.hpp>
#include <bx/engine/modules/imgui.hpp>

#include <bx/framework/systems/renderer.hpp>
#include <bx/framework/systems/dynamics.hpp>
#include <bx/framework/systems/acoustics.hpp>
#include <bx/framework/gameobject.hpp>

static bool s_running = true;

int main(int argc, char** argv)
{
	return Application::Run(argc, argv);
}

bool Application::IsRunning()
{
	return s_running && Window::IsOpen();
}

void Application::Close()
{
	s_running = false;
}

void Application::Reload()
{
	Script::DestroyVm();
	GameObject::Shutdown();
	EntityManager::Shutdown();
	SystemManager::Shutdown();

	Script::CreateVm();
	Script::BindApi();
	Module::BindApi();

	Script::Configure();
	Script::Initialize();

	SystemManager::AddSystem<Dynamics>();
	SystemManager::AddSystem<Acoustics>();
	SystemManager::AddSystem<Renderer>();
	SystemManager::Initialize();

	GameObject::Initialize();
}

int Application::Run(int argc, char** argv)
{
	if (!Initialize())
		return EXIT_FAILURE;

	Window::SetCursorMode(CursorMode::DISABLED);

	const String& mainScene = Data::GetString("Main Scene", "[assets]/main.scene", DataTarget::GAME);
	Scene::Load(mainScene);

	while (IsRunning())
	{
		PROFILE_SECTION("Runtime");

		Profiler::Update();
		Script::CollectGarbage();
		Time::Update();
		Window::PollEvents();
		Input::Poll();

		SystemManager::Update();
		Scene::GetCurrent().Update();
		Script::Update();
		
		Graphics::NewFrame();
		ImGuiImpl::NewFrame();

		SystemManager::Render();
		Script::Render();

		ImGuiImpl::EndFrame();
		Graphics::EndFrame();

		Window::Display();
	}

	Shutdown();
	return EXIT_SUCCESS;
}

bool Application::Initialize()
{
#ifdef MEMORY_CUSTOM_CONTAINERS
	Memory::Initialize();
#endif

	Time::Initialize();
	File::Initialize();
	Data::Initialize();
	ResourceManager::Initialize();

	Script::CreateVm();
	Script::BindApi();
	Module::BindApi();

	Script::Configure();

	if (!Window::Create())
	{
		BX_LOGE("Failed to create window!");
		return false;
	}

	Input::Initialize(Window::GetDevicePtr());

	if (!Graphics::Initialize(Window::GetDevicePtr()))
	{
		BX_LOGE("Failed to initialize graphics!");
		return false;
	}

	if (!ImGuiImpl::Initialize(Window::GetDevicePtr()))
	{
		BX_LOGE("Failed to initialize ImGui!");
		return false;
	}

	Physics::Initialize();

	Module::Initialize();
	Script::Initialize();

	SystemManager::AddSystem<Dynamics>();
	SystemManager::AddSystem<Acoustics>();
	SystemManager::AddSystem<Renderer>();
	SystemManager::Initialize();

	GameObject::Initialize();

	return true;
}

void Application::Shutdown()
{
	SystemManager::Shutdown();

	Module::Shutdown();

	Script::Shutdown();
	ImGuiImpl::Shutdown();
	Graphics::Shutdown();
	Physics::Shutdown();
	Input::Shutdown(Window::GetDevicePtr());
	Window::Destroy();
	Data::Shutdown();
	ResourceManager::Shutdown();

#ifdef MEMORY_CUSTOM_CONTAINERS
	Memory::Shutdown();
#endif
}
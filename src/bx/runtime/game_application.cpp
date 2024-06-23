#include "Runtime/GameApplication.hpp"

#include <Engine/Core/Memory.hpp>
#include <Engine/Core/Log.hpp>
#include <Engine/Core/Time.hpp>
#include <Engine/Core/Data.hpp>
#include <Engine/Core/Profiler.hpp>
#include <Engine/Core/File.hpp>
#include <Engine/Core/Input.hpp>
#include <Engine/Core/Module.hpp>
#include <Engine/Core/Ecs.hpp>
#include <Engine/Core/Resource.hpp>
#include <Engine/Modules/Window.hpp>
#include <Engine/Modules/Graphics.hpp>
#include <Engine/Modules/Physics.hpp>
#include <Engine/Modules/Script.hpp>
#include <Engine/Modules/GameObject.hpp>
#include <Engine/Modules/ImGui.hpp>

#include <Framework/Systems/Renderer.hpp>
#include <Framework/Systems/Dynamics.hpp>
#include <Framework/Systems/Acoustics.hpp>

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
		ENGINE_LOGE("Failed to create window!");
		return false;
	}

	Input::Initialize(Window::GetDevicePtr());

	if (!Graphics::Initialize(Window::GetDevicePtr()))
	{
		ENGINE_LOGE("Failed to initialize graphics!");
		return false;
	}

	if (!ImGuiImpl::Initialize(Window::GetDevicePtr()))
	{
		ENGINE_LOGE("Failed to initialize ImGui!");
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
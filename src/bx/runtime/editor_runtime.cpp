#include "bx/runtime/runtime.hpp"

#include <bx/engine/core/memory.hpp>
#include <bx/engine/core/log.hpp>
#include <bx/engine/core/time.hpp>
#include <bx/engine/core/data.hpp>
#include <bx/engine/core/profiler.hpp>
#include <bx/engine/core/file.hpp>
#include <bx/engine/core/input.hpp>
#include <bx/engine/core/module.hpp>
#include <bx/engine/core/resource.hpp>
#include <bx/engine/core/application.hpp>
#include <bx/engine/modules/window.hpp>
#include <bx/engine/modules/graphics.hpp>
#include <bx/engine/modules/physics.hpp>
#include <bx/engine/modules/audio.hpp>
#include <bx/engine/modules/script.hpp>
#include <bx/engine/modules/imgui.hpp>

#include <bx/framework/gameobject.hpp>

#include "bx/editor/core/assets.hpp"
#include "bx/editor/core/toolbar.hpp"
#include "bx/editor/views/profiler_view.hpp"
#include "bx/editor/views/data_view.hpp"
#include "bx/editor/views/scene_view.hpp"
#include "bx/editor/views/inspector_view.hpp"
#include "bx/editor/views/assets_view.hpp"
#include "bx/editor/views/settings_view.hpp"

static bool s_running = true;

int main(int argc, char** argv)
{
	return Runtime::Launch(argc, argv);
}

bool Runtime::IsRunning()
{
	return s_running && Window::IsOpen();
}

void Runtime::Close()
{
	s_running = false;
}

void Runtime::Reload()
{
	Module::Reload();
	Toolbar::Reset();
}

int Runtime::Launch(int argc, char** argv)
{
	if (!Initialize(argc, argv))
		return EXIT_FAILURE;

	AssetManager::Initialize();
	Toolbar::Initialize();

	AssetsView::Initialize();
	SceneView::Initialize();

	String scenePath = Data::GetString("Current Scene", "", DataTarget::EDITOR);
	if (!scenePath.empty())
	{
		scenePath = Data::GetString("Main Scene", "[assets]/main.scene", DataTarget::GAME);
		Data::SetString("Current Scene", scenePath, DataTarget::EDITOR);
	}

	Reload();
	Scene::Load(scenePath);

	do
	{
		PROFILE_SECTION("Application");

		Profiler::Update();
		Script::CollectGarbage();
		Time::Update();
		Window::PollEvents();
		Input::Poll();

		if (Toolbar::IsPlaying() && (Toolbar::ConsumeNextFrame() || !Toolbar::IsPaused()))
		{
			SystemManager::Update();
			Scene::GetCurrent().Update();
			Script::Update();
		}

		Graphics::NewFrame();
		ImGuiImpl::NewFrame();

		if (Toolbar::IsPlaying() && !Toolbar::IsPaused())
		{
			SystemManager::Render();
			Script::Render();
		}

		Toolbar::Present();

		ImGuiImpl::EndFrame();
		Graphics::EndFrame();

		Window::Display();

		AssetManager::Refresh();
	}
	while (IsRunning());

	SceneView::Shutdown();
	//AssetsView::Shutdown();

	Toolbar::Shutdown();
	AssetManager::Shutdown();

	Shutdown();
	return EXIT_SUCCESS;
}

bool Runtime::Initialize(int argc, char** argv)
{
	// Initialze core
#ifdef MEMORY_CUSTOM_CONTAINERS
	Memory::Initialize();
#endif

	Time::Initialize();
	File::Initialize();
	Data::Initialize();
	ResourceManager::Initialize();

	// Register modules
	Module::Register<Script>(0);
	Module::Register<Window>(1);
	Module::Register<Input>(2);
	Module::Register<Graphics>(3);
	Module::Register<ImGuiImpl>(4);
	Module::Register<Physics>(5);
	Module::Register<Audio>(6);
	Module::Register<GameObject>(7);

	// Configure application
	Application::Configure(argc, argv);

	// Initialize modules
	Module::Initialize();

	// Initialize application
	if (!Application::Initialize())
	{
		BX_LOGE("Failed to initialize application!");
		return false;
	}

	return true;
}

void Runtime::Shutdown()
{
	Application::Shutdown();

	Module::Shutdown();

	ResourceManager::Shutdown();
	Data::Shutdown();
	//File::Shutdown();
	//Time::Shutdown();

#ifdef MEMORY_CUSTOM_CONTAINERS
	Memory::Shutdown();
#endif
}
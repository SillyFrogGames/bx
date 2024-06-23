#pragma once

class ImGuiImpl
{
public:
	static bool Initialize(void* device);
	static void Shutdown();

	static void NewFrame();
	static void EndFrame();
};
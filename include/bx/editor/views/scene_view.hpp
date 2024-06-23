#pragma once

class SceneView
{
public:
	static void Initialize();
	static void Shutdown();

	static void Present(bool& show);
};
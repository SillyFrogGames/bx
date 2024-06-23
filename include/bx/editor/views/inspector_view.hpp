#pragma once

class InspectorView
{
public:
	static void Initialize();
	static void Shutdown();

	static void Present(bool& show);
};
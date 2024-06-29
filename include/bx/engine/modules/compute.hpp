#pragma once

class Compute
{
public:
	// TODO

private:
	static void NewFrame();
	static void EndFrame();

	static bool Initialize() { return true; }
	static void Reload() {}
	static void Shutdown() {}
};
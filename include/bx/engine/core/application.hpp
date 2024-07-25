#pragma once

class Application
{
private:
	friend class Runtime;

	static bool Initialize(int argc, char** argv);
	static void Shutdown();
};
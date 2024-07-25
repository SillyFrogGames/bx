#pragma once

class Application
{
private:
	friend class Runtime;

	static void Configure(int argc, char** argv);
	static bool Initialize();
	static void Shutdown();
};
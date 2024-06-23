#pragma once

class Application
{
public:
	static bool IsRunning();
	static void Close();

	static void Reload();

	static int Run(int argc, char** argv);

private:
	static bool Initialize();
	static void Shutdown();
};
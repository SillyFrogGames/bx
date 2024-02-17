#include <Core/Plugin.hpp>

int main(int argc, char** argv)
{
	int dummyPlugin = PluginRegistry::Register("D:/Github/BitCacheLabs/bx/demo/lib/DummyEditor.dll");
	PluginRegistry::Load(dummyPlugin);
	PluginRegistry::Unload(dummyPlugin);

	return 0;
	//return Runtime::Launch<GauntletEditor>(argc, argv);
}
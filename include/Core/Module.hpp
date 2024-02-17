#pragma once

class Module
{
public:
	static void Register();

#ifdef EDITOR_BUILD
	static void EditorPrint();
#endif
};
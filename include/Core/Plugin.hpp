#pragma once

#ifdef BUILD_DLL
#define DLLAPI __declspec(dllexport)
#else
#define DLLAPI
#endif // BUILD_DLL

#define BX_ENGINE_VERSION 1

class Plugin
{
public:
    virtual ~Plugin() = default;

    virtual const char* Name() = 0;
    virtual int Version() = 0;

    virtual void Initialize() = 0;
    virtual void Shutdown() = 0;
};

class PluginRegistry
{
public:
    static int Register(const char* filepath);

    static void Load(int plugin);
    static void Unload(int plugin);
};

typedef Plugin*(__cdecl* LoadProc)();

extern "C" { DLLAPI Plugin* Load(); }
#pragma once

#include "bx/engine/core/guard.hpp"
#include "bx/engine/containers/string.hpp"

#include "opengl_api.hpp"

namespace Gl
{
    class UniformBuffer : NoCopy
    {
    public:
        UniformBuffer(const String& name, u32 size);
        UniformBuffer(UniformBuffer&& other);
        UniformBuffer& operator=(UniformBuffer&& other);
        ~UniformBuffer();

        void Write(const void* data, u32 size);
        void Bind(u32 slot);

    private:
        u32 handle;
        u32 size;
    };

    class StorageBuffer : NoCopy
    {
    public:
        StorageBuffer(const String& name, u32 size);
        StorageBuffer(StorageBuffer&& other);
        StorageBuffer& operator=(StorageBuffer&& other);
        ~StorageBuffer();

        void Write(const void* data, u32 size);
        void Read(void* data, u32 size);
        void Bind(u32 slot);

    private:
        u32 handle;
        u32 size;
    };
}
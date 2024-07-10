#include "bx/engine/modules/graphics/backend/opengl/buffer.hpp"

#include "bx/engine/core/macros.hpp"

#include "bx/engine/modules/graphics/backend/opengl/validation.hpp"

namespace Gl
{
    UniformBuffer::UniformBuffer(const String& name, u32 size) : size(size) {
        glGenBuffers(1, &this->handle);
        DebugNames::Set(GL_BUFFER, this->handle, name);

        glBindBuffer(GL_UNIFORM_BUFFER, this->handle);
        glBufferData(GL_UNIFORM_BUFFER, size, NULL, GL_DYNAMIC_DRAW);
        glBindBuffer(GL_UNIFORM_BUFFER, 0);
    }

    UniformBuffer::UniformBuffer(UniformBuffer&& other) : handle(other.handle), size(other.size) {
        other.size = 0;
    }

    UniformBuffer& UniformBuffer::operator=(UniformBuffer&& other) {
        glDeleteBuffers(1, &this->handle);
        this->handle = other.handle;
        this->size = other.size;
        other.size = 0;
        return *this;
    }

    UniformBuffer::~UniformBuffer() {
        if (this->size > 0) {
            glDeleteBuffers(1, &this->handle);
            this->size = 0;
        }
    }

    void UniformBuffer::Write(const void* data, u32 size) {
        BX_ASSERT(size <= this->size, "UniformBuffer write out of bounds.");
        BX_ENSURE(this->size > 0);

        glBindBuffer(GL_UNIFORM_BUFFER, this->handle);
        glBufferData(GL_UNIFORM_BUFFER, size, data, GL_DYNAMIC_DRAW);
        glBindBuffer(GL_UNIFORM_BUFFER, 0);
    }

    void UniformBuffer::Bind(u32 slot) {
        BX_ENSURE(this->size > 0);

        glBindBufferRange(GL_UNIFORM_BUFFER, slot, this->handle, 0, this->size);
    }

    StorageBuffer::StorageBuffer(const String& name, u32 size) : size(size) {
        glGenBuffers(1, &this->handle);
        DebugNames::Set(GL_BUFFER, this->handle, name);

        glBindBuffer(GL_SHADER_STORAGE_BUFFER, this->handle);
        glBufferData(GL_SHADER_STORAGE_BUFFER, size, NULL, GL_DYNAMIC_COPY);
        glBindBuffer(GL_SHADER_STORAGE_BUFFER, 0);
    }

    StorageBuffer::StorageBuffer(StorageBuffer&& other) : handle(other.handle), size(other.size) {
        other.size = 0;
    }

    StorageBuffer& StorageBuffer::operator=(StorageBuffer&& other) {
        glDeleteBuffers(1, &this->handle);
        this->handle = other.handle;
        this->size = other.size;
        other.size = 0;
        return *this;
    }

    StorageBuffer::~StorageBuffer() {
        if (this->size > 0) {
            glDeleteBuffers(1, &this->handle);
            this->size = 0;
        }
    }

    void StorageBuffer::Write(const void* data, u32 size) {
        BX_ASSERT(size <= this->size, "StorageBuffer write out of bounds.");
        BX_ENSURE(this->size > 0);

        glBindBuffer(GL_SHADER_STORAGE_BUFFER, this->handle);
        glBufferData(GL_SHADER_STORAGE_BUFFER, size, data, GL_DYNAMIC_COPY);
        glBindBuffer(GL_SHADER_STORAGE_BUFFER, 0);
    }

    void StorageBuffer::Read(void* data, u32 size) {
        BX_ASSERT(size <= this->size, "StorageBuffer read out of bounds.");
        BX_ENSURE(this->size > 0);

        glGetNamedBufferSubData(this->handle, 0, size, data);
    }

    void StorageBuffer::Bind(u32 slot) {
        BX_ENSURE(this->size > 0);

        glBindBufferBase(GL_SHADER_STORAGE_BUFFER, slot, this->handle);
    }
}
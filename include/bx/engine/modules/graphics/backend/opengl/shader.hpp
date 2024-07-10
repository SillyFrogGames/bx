#pragma once

#include "bx/engine/core/guard.hpp"
#include "bx/engine/containers/string.hpp"

#include "opengl_api.hpp"

namespace Gl
{
    class ShaderProgram;

    class Shader : NoCopy
    {
    public:
        Shader(const String& name, GLenum type, const String& src);
        ~Shader();

    private:
        friend class ShaderProgram;
        u32 handle;
    };

    class ShaderProgram : NoCopy
    {
    public:
        ShaderProgram(const String& name, const List<Shader>& shaders);
        ~ShaderProgram();

    private:
        u32 handle;
    };
}
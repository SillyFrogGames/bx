#pragma once

#include "bx/engine/core/guard.hpp"
#include "bx/engine/containers/string.hpp"
#include "bx/engine/modules/graphics/type.hpp"

#include "opengl_api.hpp"

namespace Gl
{
    class ShaderProgram;

    class GraphicsPipeline : NoCopy
    {
    public:
        GraphicsPipeline(ShaderProgram&& shaderProgram, const List<VertexBufferLayout>& vertexBuffers);
        ~GraphicsPipeline();

        u32 GetVaoHandle() const;
        u32 GetShaderProgramHandle() const;

    private:
        u32 vao;
        ShaderProgram shaderProgram;
    };
}
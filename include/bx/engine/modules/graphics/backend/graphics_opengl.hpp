#pragma once

#include "bx/engine/modules/graphics.hpp"

#include "bx/engine/containers/string.hpp"
#include "bx/engine/containers/hash_map.hpp"

#include <glad/glad.h>

#ifdef BX_GRAPHICS_OPENGL_BACKEND
#define GLSL_VERSION "#version 460 core\n"
#endif

#ifdef BX_GRAPHICS_OPENGLES_BACKEND
#define GLSL_VERSION "#version 310 es\n"
#endif

#define GLSL_VERT_SHADER GLSL_VERSION "#define VERTEX\n"
#define GLSL_FRAG_SHADER GLSL_VERSION "#define PIXEL\n"

#define GPU_MEMORY_INFO_DEDICATED_VIDMEM_NVX            0x9047
#define GPU_MEMORY_INFO_TOTAL_AVAILABLE_MEMORY_NVX      0x9048
#define GPU_MEMORY_INFO_CURRENT_AVAILABLE_VIDMEM_NVX    0x9049
#define GPU_MEMORY_INFO_EVICTION_COUNT_NVX              0x904A
#define GPU_MEMORY_INFO_EVICTED_MEMORY_NVX              0x904B

#define VBO_FREE_MEMORY_ATI                             0x87FB
#define TEXTURE_FREE_MEMORY_ATI                         0x87FC
#define RENDERBUFFER_FREE_MEMORY_ATI                    0x87FD

#define MAX_BOUND_VERTEX_BUFFERS                        16

//struct ShaderImpl
//{
//    GLuint handle = 0;
//};
//
//struct BufferImpl
//{
//    GLuint handle = 0;
//    GLenum target = 0;
//    GLenum usage = 0;
//    GLsizei stride = 0;
//};
//
//struct TextureImpl
//{
//    GLuint texture = 0;
//    GLuint sampler = 0;
//    GLuint fbo = 0;
//    GLuint rbo = 0;
//
//    GLuint64 handle = 0;
//};
//
//struct ResourceBindingImpl
//{
//    struct Data
//    {
//        ShaderType shaderType = ShaderType::UNKNOWN;
//        u32 count = 0;
//        ResourceBindingType type = ResourceBindingType::UNKNOWN;
//        ResourceBindingAccess access = ResourceBindingAccess::STATIC;
//
//        GraphicsHandle handle = INVALID_GRAPHICS_HANDLE;
//    };
//    HashMap<String, Data> resources;
//};
//
//struct PipelineImpl
//{
//    GLuint program = 0;
//    GLuint vao = 0;
//
//    GLenum faceCull = GL_CCW;
//
//    bool depthEnable = true;
//    bool blendEnable = true;
//
//    GLuint bufferCount = 0;
//};

class GraphicsOpenGL
{
public:
    static GLuint GetTextureHandle(HTexture texture);
};
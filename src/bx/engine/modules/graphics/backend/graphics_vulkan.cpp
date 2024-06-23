#ifdef GRAPHICS_VULKAN_BACKEND
#include "Engine/Modules/Graphics.hpp"

#include "Engine/Core/File.hpp"
#include "Engine/Core/Profiler.hpp"
#include "Engine/Core/Memory.hpp"
#include "Engine/Core/Macros.hpp"

#include "Engine/Modules/Window.hpp"

#ifdef WINDOW_GLFW_BACKEND
#define GLFW_INCLUDE_NONE
#define GLFW_INCLUDE_VULKAN
#include <GLFW/glfw3.h>
#include <vulkan/vulkan.h>
#endif

#ifdef _DEBUG
#define GRAPHICS_VK_DEBUG_REPORT
#endif

#define VK_GLSL_VERSION "#version 450\n"
#define VK_GLSL_VERTEX "#define VERTEX\n"
#define VK_GLSL_PIXEL "#define PIXEL\n"

#include <glslang/Include/glslang_c_interface.h>
// Required for use of glslang_default_resource
#include <glslang/Public/resource_limits_c.h>
#include <glslang/Public/ResourceLimits.h>

typedef struct SpirVBinary
{
    uint32_t* words; // SPIR-V words
    int size; // number of words in SPIR-V binary
} SpirVBinary;

static const TBuiltInResource DefaultTBuiltInResource =
{
    /* .MaxLights = */ 32,
    /* .MaxClipPlanes = */ 6,
    /* .MaxTextureUnits = */ 32,
    /* .MaxTextureCoords = */ 32,
    /* .MaxVertexAttribs = */ 64,
    /* .MaxVertexUniformComponents = */ 4096,
    /* .MaxVaryingFloats = */ 64,
    /* .MaxVertexTextureImageUnits = */ 32,
    /* .MaxCombinedTextureImageUnits = */ 80,
    /* .MaxTextureImageUnits = */ 32,
    /* .MaxFragmentUniformComponents = */ 4096,
    /* .MaxDrawBuffers = */ 32,
    /* .MaxVertexUniformVectors = */ 128,
    /* .MaxVaryingVectors = */ 8,
    /* .MaxFragmentUniformVectors = */ 16,
    /* .MaxVertexOutputVectors = */ 16,
    /* .MaxFragmentInputVectors = */ 15,
    /* .MinProgramTexelOffset = */ -8,
    /* .MaxProgramTexelOffset = */ 7,
    /* .MaxClipDistances = */ 8,
    /* .MaxComputeWorkGroupCountX = */ 65535,
    /* .MaxComputeWorkGroupCountY = */ 65535,
    /* .MaxComputeWorkGroupCountZ = */ 65535,
    /* .MaxComputeWorkGroupSizeX = */ 1024,
    /* .MaxComputeWorkGroupSizeY = */ 1024,
    /* .MaxComputeWorkGroupSizeZ = */ 64,
    /* .MaxComputeUniformComponents = */ 1024,
    /* .MaxComputeTextureImageUnits = */ 16,
    /* .MaxComputeImageUniforms = */ 8,
    /* .MaxComputeAtomicCounters = */ 8,
    /* .MaxComputeAtomicCounterBuffers = */ 1,
    /* .MaxVaryingComponents = */ 60,
    /* .MaxVertexOutputComponents = */ 64,
    /* .MaxGeometryInputComponents = */ 64,
    /* .MaxGeometryOutputComponents = */ 128,
    /* .MaxFragmentInputComponents = */ 128,
    /* .MaxImageUnits = */ 8,
    /* .MaxCombinedImageUnitsAndFragmentOutputs = */ 8,
    /* .MaxCombinedShaderOutputResources = */ 8,
    /* .MaxImageSamples = */ 0,
    /* .MaxVertexImageUniforms = */ 0,
    /* .MaxTessControlImageUniforms = */ 0,
    /* .MaxTessEvaluationImageUniforms = */ 0,
    /* .MaxGeometryImageUniforms = */ 0,
    /* .MaxFragmentImageUniforms = */ 8,
    /* .MaxCombinedImageUniforms = */ 8,
    /* .MaxGeometryTextureImageUnits = */ 16,
    /* .MaxGeometryOutputVertices = */ 256,
    /* .MaxGeometryTotalOutputComponents = */ 1024,
    /* .MaxGeometryUniformComponents = */ 1024,
    /* .MaxGeometryVaryingComponents = */ 64,
    /* .MaxTessControlInputComponents = */ 128,
    /* .MaxTessControlOutputComponents = */ 128,
    /* .MaxTessControlTextureImageUnits = */ 16,
    /* .MaxTessControlUniformComponents = */ 1024,
    /* .MaxTessControlTotalOutputComponents = */ 4096,
    /* .MaxTessEvaluationInputComponents = */ 128,
    /* .MaxTessEvaluationOutputComponents = */ 128,
    /* .MaxTessEvaluationTextureImageUnits = */ 16,
    /* .MaxTessEvaluationUniformComponents = */ 1024,
    /* .MaxTessPatchComponents = */ 120,
    /* .MaxPatchVertices = */ 32,
    /* .MaxTessGenLevel = */ 64,
    /* .MaxViewports = */ 16,
    /* .MaxVertexAtomicCounters = */ 0,
    /* .MaxTessControlAtomicCounters = */ 0,
    /* .MaxTessEvaluationAtomicCounters = */ 0,
    /* .MaxGeometryAtomicCounters = */ 0,
    /* .MaxFragmentAtomicCounters = */ 8,
    /* .MaxCombinedAtomicCounters = */ 8,
    /* .MaxAtomicCounterBindings = */ 1,
    /* .MaxVertexAtomicCounterBuffers = */ 0,
    /* .MaxTessControlAtomicCounterBuffers = */ 0,
    /* .MaxTessEvaluationAtomicCounterBuffers = */ 0,
    /* .MaxGeometryAtomicCounterBuffers = */ 0,
    /* .MaxFragmentAtomicCounterBuffers = */ 1,
    /* .MaxCombinedAtomicCounterBuffers = */ 1,
    /* .MaxAtomicCounterBufferSize = */ 16384,
    /* .MaxTransformFeedbackBuffers = */ 4,
    /* .MaxTransformFeedbackInterleavedComponents = */ 64,
    /* .MaxCullDistances = */ 8,
    /* .MaxCombinedClipAndCullDistances = */ 8,
    /* .MaxSamples = */ 4,
    /* .maxMeshOutputVerticesNV = */ 256,
    /* .maxMeshOutputPrimitivesNV = */ 512,
    /* .maxMeshWorkGroupSizeX_NV = */ 32,
    /* .maxMeshWorkGroupSizeY_NV = */ 1,
    /* .maxMeshWorkGroupSizeZ_NV = */ 1,
    /* .maxTaskWorkGroupSizeX_NV = */ 32,
    /* .maxTaskWorkGroupSizeY_NV = */ 1,
    /* .maxTaskWorkGroupSizeZ_NV = */ 1,
    /* .maxMeshViewCountNV = */ 4,
    /* .maxMeshOutputVerticesEXT = */ 256,
    /* .maxMeshOutputPrimitivesEXT = */ 256,
    /* .maxMeshWorkGroupSizeX_EXT = */ 128,
    /* .maxMeshWorkGroupSizeY_EXT = */ 128,
    /* .maxMeshWorkGroupSizeZ_EXT = */ 128,
    /* .maxTaskWorkGroupSizeX_EXT = */ 128,
    /* .maxTaskWorkGroupSizeY_EXT = */ 128,
    /* .maxTaskWorkGroupSizeZ_EXT = */ 128,
    /* .maxMeshViewCountEXT = */ 4,
    /* .maxDualSourceDrawBuffersEXT = */ 1,

    /* .limits = */
    {
        /* .nonInductiveForLoops = */ 1,
        /* .whileLoops = */ 1,
        /* .doWhileLoops = */ 1,
        /* .generalUniformIndexing = */ 1,
        /* .generalAttributeMatrixVectorIndexing = */ 1,
        /* .generalVaryingIndexing = */ 1,
        /* .generalSamplerIndexing = */ 1,
        /* .generalVariableIndexing = */ 1,
        /* .generalConstantMatrixVectorIndexing = */ 1,
    }
};

static SpirVBinary compileShaderToSPIRV_Vulkan(glslang_stage_t stage, const char* shaderSource, const char* fileName)
{
    glslang_input_t input{};
    input.language = GLSLANG_SOURCE_GLSL;
    input.stage = stage;
    input.client = GLSLANG_CLIENT_VULKAN;
    input.client_version = GLSLANG_TARGET_VULKAN_1_0; //GLSLANG_TARGET_VULKAN_1_2;
    input.target_language = GLSLANG_TARGET_SPV;
    input.target_language_version = GLSLANG_TARGET_SPV_1_0; //GLSLANG_TARGET_SPV_1_5;
    input.code = shaderSource;
    input.default_version = 100;
    input.default_profile = GLSLANG_CORE_PROFILE;
    input.force_default_version_and_profile = false;
    input.forward_compatible = false;
    input.messages = GLSLANG_MSG_DEFAULT_BIT;
    input.resource = reinterpret_cast<const glslang_resource_t*>(&DefaultTBuiltInResource);// glslang_default_resource();

    glslang_shader_t* shader = glslang_shader_create(&input);

    SpirVBinary bin{};
    bin.words = NULL;
    bin.size = 0;
    
    if (!glslang_shader_preprocess(shader, &input))
    {
        printf("GLSL preprocessing failed %s\n", fileName);
        printf("%s\n", glslang_shader_get_info_log(shader));
        printf("%s\n", glslang_shader_get_info_debug_log(shader));
        printf("%s\n", input.code);
        glslang_shader_delete(shader);
        return bin;
    }
    
    if (!glslang_shader_parse(shader, &input))
    {
        printf("GLSL parsing failed %s\n", fileName);
        printf("%s\n", glslang_shader_get_info_log(shader));
        printf("%s\n", glslang_shader_get_info_debug_log(shader));
        printf("%s\n", glslang_shader_get_preprocessed_code(shader));
        glslang_shader_delete(shader);
        return bin;
    }
    
    glslang_program_t* program = glslang_program_create();
    glslang_program_add_shader(program, shader);
    
    if (!glslang_program_link(program, GLSLANG_MSG_SPV_RULES_BIT | GLSLANG_MSG_VULKAN_RULES_BIT))
    {
        printf("GLSL linking failed %s\n", fileName);
        printf("%s\n", glslang_program_get_info_log(program));
        printf("%s\n", glslang_program_get_info_debug_log(program));
        glslang_program_delete(program);
        glslang_shader_delete(shader);
        return bin;
    }
    
    glslang_program_SPIRV_generate(program, stage);
    
    bin.size = glslang_program_SPIRV_get_size(program);
    bin.words = (u32*)malloc(bin.size * sizeof(uint32_t));
    glslang_program_SPIRV_get(program, bin.words);
    
    const char* spirv_messages = glslang_program_SPIRV_get_messages(program);
    if (spirv_messages)
        printf("(%s) %s\b", fileName, spirv_messages);
    
    glslang_program_delete(program);
    glslang_shader_delete(shader);

    return bin;
}

static void check_vk_result(VkResult err)
{
    if (err == 0)
        return;
    fprintf(stderr, "[vulkan] Error: VkResult = %d\n", err);
    if (err < 0)
        abort();
}

#define VK_CHECK(x) { VkResult err = x; check_vk_result(err); }

struct Frame
{
    VkImage                     image{ VK_NULL_HANDLE };
    VkImageView                 imageView{ VK_NULL_HANDLE };
    VkFramebuffer               framebuffer{ VK_NULL_HANDLE };
    VkCommandBuffer             commandBuffer{ VK_NULL_HANDLE };

    VkFence                     inFlightFence{ VK_NULL_HANDLE };
    VkSemaphore                 acquireSemaphore{ VK_NULL_HANDLE };
    VkSemaphore                 presentSemaphore{ VK_NULL_HANDLE };
};

struct Context
{
    // Core data
    VkInstance                  instance{ VK_NULL_HANDLE };
    VkPhysicalDevice            physicalDevice{ VK_NULL_HANDLE };
    VkDevice                    device{ VK_NULL_HANDLE };
    VkCommandPool               commandPool{ VK_NULL_HANDLE };
    VkDescriptorPool            descriptorPool{ VK_NULL_HANDLE };
    VkAllocationCallbacks*      allocator{ nullptr };

    i32                         graphicsQueueIndex{ -1 };
    i32                         presentQueueIndex{ -1 };
    VkQueue                     graphicsQueue{ VK_NULL_HANDLE };
    VkQueue                     presentQueue{ VK_NULL_HANDLE };

    // Swapchain data
    VkSurfaceKHR                surface{ VK_NULL_HANDLE };
    VkSwapchainKHR              swapchain{ VK_NULL_HANDLE };
    VkExtent2D                  extent{ 0, 0 };
    VkFormat                    format{ VK_FORMAT_UNDEFINED };
    std::vector<Frame>          frames{};
    std::vector<VkSemaphore>    semaphores{};

    // Global renderpass for simple rendering
    VkRenderPass                renderPass{ VK_NULL_HANDLE };
    bool                        bHasActiveRenderPass{ false };

    // Per frame data
    u32                         currentFrame{ 0 };

    // ???
    VkDescriptorSetLayout       descriptorSetLayout{ VK_NULL_HANDLE };
    VkDeviceSize                bufferMemoryAlignment{ 256 };
    VkDeviceMemory              uploadBufferMemory{ VK_NULL_HANDLE };
    VkBuffer                    uploadBuffer{ VK_NULL_HANDLE };

    // Debug stuff
    VkDebugReportCallbackEXT    debugCallback{ VK_NULL_HANDLE };
};

struct ShaderImpl
{
    VkShaderModule              shaderModule{ VK_NULL_HANDLE };
};

struct BufferImpl
{
    VkDeviceMemory              memory{ VK_NULL_HANDLE };
    VkDeviceSize                size{ 0 };
    VkBuffer                    buffer{ VK_NULL_HANDLE };
};

struct TextureImpl
{
    VkImage                     image{ VK_NULL_HANDLE };
    VkDeviceMemory              memory{ VK_NULL_HANDLE };
    VkImageView                 view{ VK_NULL_HANDLE };
    VkDescriptorSet             descriptorSet{ VK_NULL_HANDLE };

    //VkSampler                   FontSampler;
    //VkDeviceMemory              FontMemory;
    //VkImage                     FontImage;
    //VkImageView                 FontView;
    //VkDescriptorSet             FontDescriptorSet;
    //VkDeviceMemory              UploadBufferMemory;
    //VkBuffer                    UploadBuffer;
};

struct PipelineImpl
{
    VkDescriptorSetLayout       descriptorSetLayout{ VK_NULL_HANDLE };
    VkPipeline                  pipeline{ VK_NULL_HANDLE };
    VkPipelineLayout            pipelineLayout{ VK_NULL_HANDLE };

    // TODO: A pipeline should have a render pass, when null we use the context renderpass compatible with out swapchain
    //VkRenderPass                renderPass{ VK_NULL_HANDLE };

    // Tmp?
    std::vector<VkSampler>      samplers{};
};

static Context g_ctx;
static HashMap<GraphicsHandle, ShaderImpl> g_shaders;
static HashMap<GraphicsHandle, BufferImpl> g_buffers;
static HashMap<GraphicsHandle, TextureImpl> g_textures;
static HashMap<GraphicsHandle, PipelineImpl> g_pipelines;

#if defined(GRAPHICS_VK_DEBUG_REPORT)
static VKAPI_ATTR VkBool32 VKAPI_CALL DebugCallback(
    VkDebugReportFlagsEXT flags, VkDebugReportObjectTypeEXT type,
    uint64_t object, size_t location, int32_t messageCode,
    const char* layerPrefix, const char* message, void* userData)
{
    if (flags & VK_DEBUG_REPORT_ERROR_BIT_EXT)
    {
        Log::Error("Validation Layer: Error: {}: {}", layerPrefix, message);
    }
    else if (flags & VK_DEBUG_REPORT_WARNING_BIT_EXT)
    {
        Log::Error("Validation Layer: Warning: {}: {}", layerPrefix, message);
    }
    else if (flags & VK_DEBUG_REPORT_PERFORMANCE_WARNING_BIT_EXT)
    {
        Log::Warning("Validation Layer: Performance warning: {}: {}", layerPrefix, message);
    }
    else
    {
        Log::Info("Validation Layer: Information: {}: {}", layerPrefix, message);
    }
    return VK_FALSE;
}
#endif

static void InitializeInstance(uint32_t extensionsCount, const char** extensions)
{
    // Create Vulkan Instance
    {
        VkInstanceCreateInfo createInfo{ VK_STRUCTURE_TYPE_INSTANCE_CREATE_INFO };
        //createInfo.pNext = nullptr;
        //createInfo.flags = VkInstanceCreateFlags{};

        VkApplicationInfo appInfo{ VK_STRUCTURE_TYPE_APPLICATION_INFO };
        //appInfo.pNext = nullptr;
        appInfo.pApplicationName = "Engine";
        appInfo.applicationVersion = 0;
        appInfo.pEngineName = "Engine";
        appInfo.engineVersion = 0;
        appInfo.apiVersion = VK_MAKE_API_VERSION(0, 1, 0, 0);
        createInfo.pApplicationInfo = &appInfo;

#if defined(GRAPHICS_VK_DEBUG_REPORT)
        // Enabling validation layers
        const char* layers[] = { "VK_LAYER_KHRONOS_validation" };
        createInfo.enabledLayerCount = 1;
        createInfo.ppEnabledLayerNames = layers;

        // Enable debug report extension (we need additional storage, so we duplicate the user array to add our new extension to it)
        List<const char*> extensionsExt(extensionsCount + 1);
        Memory::Copy(extensions, extensionsExt.Data(), extensionsCount * sizeof(const char*));
        extensionsExt[extensionsCount] = VK_EXT_DEBUG_REPORT_EXTENSION_NAME;
        createInfo.enabledExtensionCount = extensionsCount + 1;
        createInfo.ppEnabledExtensionNames = extensionsExt.Data();

        // Create Vulkan Instance
        VK_CHECK(vkCreateInstance(&createInfo, g_ctx.allocator, &g_ctx.instance));

        // Get the function pointer (required for any extensions)
        auto vkCreateDebugReportCallbackEXT = (PFN_vkCreateDebugReportCallbackEXT)vkGetInstanceProcAddr(g_ctx.instance, "vkCreateDebugReportCallbackEXT");
        ENGINE_ASSERT(vkCreateDebugReportCallbackEXT != nullptr);

        // Setup the debug report callback
        VkDebugReportCallbackCreateInfoEXT debugInfo{ VK_STRUCTURE_TYPE_DEBUG_REPORT_CALLBACK_CREATE_INFO_EXT };
        //debugInfo.pNext = nullptr;
        debugInfo.flags = VK_DEBUG_REPORT_ERROR_BIT_EXT | VK_DEBUG_REPORT_WARNING_BIT_EXT | VK_DEBUG_REPORT_PERFORMANCE_WARNING_BIT_EXT;
        debugInfo.pfnCallback = DebugCallback;
        debugInfo.pUserData = nullptr;
        VK_CHECK(vkCreateDebugReportCallbackEXT(g_ctx.instance, &debugInfo, g_ctx.allocator, &g_ctx.debugCallback));
#else
        // Create Vulkan Instance without any debug feature
        createInfo.enabledExtensionCount = extensionsCount;
        createInfo.ppEnabledExtensionNames = extensions;
        VK_CHECK(vkCreateInstance(&createInfo, g_ctx.allocator, &g_ctx.instance));
#endif
    }
}

static void InitializeDevice()
{
    // Select GPU
    {
        u32 count;
        VK_CHECK(vkEnumeratePhysicalDevices(g_ctx.instance, &count, NULL));
        ENGINE_ASSERT(count > 0);

        List<VkPhysicalDevice> gpus(count);
        VK_CHECK(vkEnumeratePhysicalDevices(g_ctx.instance, &count, gpus.Data()));
        
        // If a number >1 of GPUs got reported, find discrete GPU if present, or use first one available.
        // This covers most common cases (multi-gpu/integrated+dedicated graphics).
        // TODO: Handling more complicated setups (multiple dedicated GPUs) is not implemented.
        uint32_t gpuIndex = 0;
        for (int i = 0; i < count; i++)
        {
            VkPhysicalDeviceProperties properties;
            vkGetPhysicalDeviceProperties(gpus[i], &properties);
            if (properties.deviceType == VK_PHYSICAL_DEVICE_TYPE_DISCRETE_GPU)
            {
                gpuIndex = i;
                break;
            }
        }

        g_ctx.physicalDevice = gpus[gpuIndex];
    }

    // Select queue family indices
    {
        uint32_t count;
        vkGetPhysicalDeviceQueueFamilyProperties(g_ctx.physicalDevice, &count, NULL);
        List<VkQueueFamilyProperties> queues(count);
        vkGetPhysicalDeviceQueueFamilyProperties(g_ctx.physicalDevice, &count, queues.Data());
        for (uint32_t i = 0; i < count; i++)
        {
            auto queue = queues[i];
            if (queue.queueFlags & VK_QUEUE_GRAPHICS_BIT)
            {
                g_ctx.graphicsQueueIndex = i;
                break;
            }
        }

        for (uint32_t i = 0; i < count; i++)
        {
            VkBool32 supported = false;
            VK_CHECK(vkGetPhysicalDeviceSurfaceSupportKHR(g_ctx.physicalDevice, i, g_ctx.surface, &supported));
            if (supported)
            {
                g_ctx.presentQueueIndex = i;
                break;
            }
        }

        ENGINE_ASSERT(g_ctx.graphicsQueueIndex != -1);
        ENGINE_ASSERT(g_ctx.presentQueueIndex != -1);
    }

    // Create logical device
    {
        uint32_t deviceExtensionCount = 1;
        const char* deviceExtensions[] = { VK_KHR_SWAPCHAIN_EXTENSION_NAME };

        // TODO: Should probably validate these
        //VK_CHECK(vkEnumerateDeviceExtensionProperties(context.gpu, nullptr, &device_extension_count, nullptr));
        //std::vector<VkExtensionProperties> device_extensions(device_extension_count);
        //VK_CHECK(vkEnumerateDeviceExtensionProperties(context.gpu, nullptr, &device_extension_count, device_extensions.data()));
        //
        //if (!validate_extensions(required_device_extensions, device_extensions))
        //{
        //    throw std::runtime_error("Required device extensions are missing, will try without.");
        //}

        const float queuePriority[] = { 1.0f };

        u32 queueCount = g_ctx.graphicsQueueIndex == g_ctx.presentQueueIndex ? 1 : 2;

        VkDeviceQueueCreateInfo queueInfo{ VK_STRUCTURE_TYPE_DEVICE_QUEUE_CREATE_INFO };
        //queueInfo.pNext = nullptr;
        //queueInfo.flags = VkDeviceQueueCreateFlags{};
        queueInfo.queueCount = 1;
        queueInfo.pQueuePriorities = queuePriority;

        List<VkDeviceQueueCreateInfo> queueInfos(queueCount);
        queueInfos[0] = queueInfo;
        queueInfos[0].queueFamilyIndex = g_ctx.graphicsQueueIndex;
        if (queueCount > 1)
        {
            queueInfos[1] = queueInfo;
            queueInfos[1].queueFamilyIndex = g_ctx.presentQueueIndex;
        }

        VkDeviceCreateInfo deviceInfo{ VK_STRUCTURE_TYPE_DEVICE_CREATE_INFO };
        //deviceInfo.pNext = nullptr;
        //deviceInfo.flags = VkDeviceCreateFlags{};
        deviceInfo.queueCreateInfoCount = queueCount;
        deviceInfo.pQueueCreateInfos = queueInfos.Data();
        // TODO: Should we pass enabled layers here?
        //deviceInfo.enabledLayerCount = 1;
        //deviceInfo.ppEnabledLayerNames = "";
        deviceInfo.enabledExtensionCount = deviceExtensionCount;
        deviceInfo.ppEnabledExtensionNames = deviceExtensions;
        //deviceInfo.pEnabledFeatures = nullptr;
        VK_CHECK(vkCreateDevice(g_ctx.physicalDevice, &deviceInfo, g_ctx.allocator, &g_ctx.device));

        vkGetDeviceQueue(g_ctx.device, g_ctx.graphicsQueueIndex, 0, &g_ctx.graphicsQueue);
        vkGetDeviceQueue(g_ctx.device, g_ctx.presentQueueIndex, 0, &g_ctx.presentQueue);
    }

    // Create command pool
    {
        VkCommandPoolCreateInfo cmdPoolInfo{ VK_STRUCTURE_TYPE_COMMAND_POOL_CREATE_INFO };
        cmdPoolInfo.flags = VK_COMMAND_POOL_CREATE_RESET_COMMAND_BUFFER_BIT;// VK_COMMAND_POOL_CREATE_TRANSIENT_BIT;
        cmdPoolInfo.queueFamilyIndex = g_ctx.graphicsQueueIndex;
        VK_CHECK(vkCreateCommandPool(g_ctx.device, &cmdPoolInfo, g_ctx.allocator, &g_ctx.commandPool));
    }

    // Create descriptor pool
    {
        VkDescriptorPoolSize poolSizes[] =
        {
            { VK_DESCRIPTOR_TYPE_SAMPLER, 1000 },
            { VK_DESCRIPTOR_TYPE_COMBINED_IMAGE_SAMPLER, 1000 },
            { VK_DESCRIPTOR_TYPE_SAMPLED_IMAGE, 1000 },
            { VK_DESCRIPTOR_TYPE_STORAGE_IMAGE, 1000 },
            { VK_DESCRIPTOR_TYPE_UNIFORM_TEXEL_BUFFER, 1000 },
            { VK_DESCRIPTOR_TYPE_STORAGE_TEXEL_BUFFER, 1000 },
            { VK_DESCRIPTOR_TYPE_UNIFORM_BUFFER, 1000 },
            { VK_DESCRIPTOR_TYPE_STORAGE_BUFFER, 1000 },
            { VK_DESCRIPTOR_TYPE_UNIFORM_BUFFER_DYNAMIC, 1000 },
            { VK_DESCRIPTOR_TYPE_STORAGE_BUFFER_DYNAMIC, 1000 },
            { VK_DESCRIPTOR_TYPE_INPUT_ATTACHMENT, 1000 }
        };
        VkDescriptorPoolCreateInfo poolInfo{ VK_STRUCTURE_TYPE_DESCRIPTOR_POOL_CREATE_INFO };
        poolInfo.flags = VK_DESCRIPTOR_POOL_CREATE_FREE_DESCRIPTOR_SET_BIT;
        poolInfo.maxSets = 1000 * ARRAYSIZE(poolSizes);
        poolInfo.poolSizeCount = (uint32_t)ARRAYSIZE(poolSizes);
        poolInfo.pPoolSizes = poolSizes;
        VK_CHECK(vkCreateDescriptorPool(g_ctx.device, &poolInfo, g_ctx.allocator, &g_ctx.descriptorPool));
    }
}

static VkSurfaceFormatKHR SelectSurfaceFormat()
{
    const VkFormat requestFormats[] = { VK_FORMAT_B8G8R8A8_UNORM, VK_FORMAT_R8G8B8A8_UNORM, VK_FORMAT_B8G8R8_UNORM, VK_FORMAT_R8G8B8_UNORM };
    const uint32_t requestFormatsCount = ARRAYSIZE(requestFormats);
    
    const VkColorSpaceKHR requestColorSpace = VK_COLORSPACE_SRGB_NONLINEAR_KHR;

    // Per Spec Format and View Format are expected to be the same unless VK_IMAGE_CREATE_MUTABLE_BIT was set at image creation
    // Assuming that the default behavior is without setting this bit, there is no need for separate Swapchain image and image view format
    // Additionally several new color spaces were introduced with Vulkan Spec v1.0.40,
    // hence we must make sure that a format with the mostly available color space, VK_COLOR_SPACE_SRGB_NONLINEAR_KHR, is found and used.
    uint32_t availCount;
    vkGetPhysicalDeviceSurfaceFormatsKHR(g_ctx.physicalDevice, g_ctx.surface, &availCount, nullptr);
    std::vector<VkSurfaceFormatKHR> availFormat;
    availFormat.resize(availCount);
    vkGetPhysicalDeviceSurfaceFormatsKHR(g_ctx.physicalDevice, g_ctx.surface, &availCount, availFormat.data());

    // First check if only one format, VK_FORMAT_UNDEFINED, is available, which would imply that any format is available
    if (availCount == 1)
    {
        if (availFormat[0].format == VK_FORMAT_UNDEFINED)
        {
            VkSurfaceFormatKHR ret;
            ret.format = requestFormats[0];
            ret.colorSpace = requestColorSpace;
            return ret;
        }
        else
        {
            // No point in searching another format
            return availFormat[0];
        }
    }
    else
    {
        // Request several formats, the first found will be used
        for (uint32_t request_i = 0; request_i < requestFormatsCount; request_i++)
            for (uint32_t avail_i = 0; avail_i < availCount; avail_i++)
                if (availFormat[avail_i].format == requestFormats[request_i] && availFormat[avail_i].colorSpace == requestColorSpace)
                    return availFormat[avail_i];

        // If none of the requested image formats could be found, use the first available
        return availFormat[0];
    }
}

static VkPresentModeKHR SelectPresentMode()
{
    VkPresentModeKHR presentModes[] = { VK_PRESENT_MODE_MAILBOX_KHR, VK_PRESENT_MODE_IMMEDIATE_KHR, VK_PRESENT_MODE_FIFO_KHR };
    uint32_t requestModesCount = ARRAYSIZE(presentModes);

    // Request a certain mode and confirm that it is available. If not use VK_PRESENT_MODE_FIFO_KHR which is mandatory
    uint32_t availCount = 0;
    vkGetPhysicalDeviceSurfacePresentModesKHR(g_ctx.physicalDevice, g_ctx.surface, &availCount, nullptr);
    std::vector<VkPresentModeKHR> availModes;
    availModes.resize(availCount);
    vkGetPhysicalDeviceSurfacePresentModesKHR(g_ctx.physicalDevice, g_ctx.surface, &availCount, availModes.data());

    //for (uint32_t avail_i = 0; avail_i < avail_count; avail_i++)
    //    printf("[vulkan] avail_modes[%d] = %d\n", avail_i, avail_modes[avail_i]);

    for (uint32_t request_i = 0; request_i < requestModesCount; request_i++)
        for (uint32_t avail_i = 0; avail_i < availCount; avail_i++)
            if (presentModes[request_i] == availModes[avail_i])
                return presentModes[request_i];

    return VK_PRESENT_MODE_FIFO_KHR; // Always available
}

static uint32_t GetMinImageCount(VkSurfaceCapabilitiesKHR surfaceProperties, VkPresentModeKHR presentMode)
{
    uint32_t minImageCount = 0;

    // (ImGui note) Even thought mailbox seems to get us maximum framerate with a single window, it halves framerate with a second window etc. (w/ Nvidia and SDK 1.82.1)
    if (presentMode == VK_PRESENT_MODE_MAILBOX_KHR)
        minImageCount = 3;

    // FIFO must be supported by all implementations.
    else if (presentMode == VK_PRESENT_MODE_FIFO_KHR || presentMode == VK_PRESENT_MODE_FIFO_RELAXED_KHR)
        minImageCount = 2;

    else if (presentMode == VK_PRESENT_MODE_IMMEDIATE_KHR)
        minImageCount = 1;

    // Make sure we have at least the minimun number of VkImages to use in the swapchain.
    if (minImageCount < surfaceProperties.minImageCount)
    {
        minImageCount = surfaceProperties.minImageCount;
    }
    else if ((surfaceProperties.maxImageCount > 0) && (minImageCount > surfaceProperties.maxImageCount))
    {
        // Application must settle for fewer images than desired.
        minImageCount = surfaceProperties.maxImageCount;
    }

    return minImageCount;
}

static VkSurfaceTransformFlagBitsKHR FindSuitablePreTransfrom(VkSurfaceCapabilitiesKHR surfaceProperties)
{
    if (surfaceProperties.supportedTransforms & VK_SURFACE_TRANSFORM_IDENTITY_BIT_KHR)
        return VK_SURFACE_TRANSFORM_IDENTITY_BIT_KHR;

    return surfaceProperties.currentTransform;
}

static VkCompositeAlphaFlagBitsKHR GetSupportedComposite(VkSurfaceCapabilitiesKHR surfaceProperties)
{
    if (surfaceProperties.supportedCompositeAlpha & VK_COMPOSITE_ALPHA_OPAQUE_BIT_KHR)
        return VK_COMPOSITE_ALPHA_OPAQUE_BIT_KHR;

    if (surfaceProperties.supportedCompositeAlpha & VK_COMPOSITE_ALPHA_INHERIT_BIT_KHR)
        return VK_COMPOSITE_ALPHA_INHERIT_BIT_KHR;

    if (surfaceProperties.supportedCompositeAlpha & VK_COMPOSITE_ALPHA_PRE_MULTIPLIED_BIT_KHR)
        return VK_COMPOSITE_ALPHA_PRE_MULTIPLIED_BIT_KHR;

    if (surfaceProperties.supportedCompositeAlpha & VK_COMPOSITE_ALPHA_POST_MULTIPLIED_BIT_KHR)
        return VK_COMPOSITE_ALPHA_POST_MULTIPLIED_BIT_KHR;
    
    return VK_COMPOSITE_ALPHA_OPAQUE_BIT_KHR;
}

static VkSharingMode GetSwapchainSharingMode()
{
    if (g_ctx.graphicsQueueIndex != g_ctx.presentQueueIndex)
        return VK_SHARING_MODE_CONCURRENT;

    return VK_SHARING_MODE_EXCLUSIVE; // Assume that graphics queue == present queue
}

static void InitializeSwapchain(uint32_t width, uint32_t height)
{
    VkSwapchainKHR oldSwapchain = g_ctx.swapchain;

    VkSurfaceCapabilitiesKHR surfaceProperties;
    VK_CHECK(vkGetPhysicalDeviceSurfaceCapabilitiesKHR(g_ctx.physicalDevice, g_ctx.surface, &surfaceProperties));

    VkSurfaceFormatKHR surfaceFormat = SelectSurfaceFormat();

    VkExtent2D swapchainExtent = surfaceProperties.currentExtent;
    if (swapchainExtent.width == 0xFFFFFFFF)
    {
        swapchainExtent.width = width;
        swapchainExtent.height = height;
    }

    VkPresentModeKHR presentMode = SelectPresentMode();
    uint32_t minImageCount = GetMinImageCount(surfaceProperties, presentMode);
    VkSurfaceTransformFlagBitsKHR preTransform = FindSuitablePreTransfrom(surfaceProperties);
    VkCompositeAlphaFlagBitsKHR composite = GetSupportedComposite(surfaceProperties);

    // Create swapchain
    {
        VkSwapchainCreateInfoKHR info{ VK_STRUCTURE_TYPE_SWAPCHAIN_CREATE_INFO_KHR };
        info.surface = g_ctx.surface;
        info.minImageCount = minImageCount;
        info.imageFormat = surfaceFormat.format;
        info.imageColorSpace = surfaceFormat.colorSpace;
        info.imageExtent.width = swapchainExtent.width;
        info.imageExtent.height = swapchainExtent.height;
        info.imageArrayLayers = 1;
        info.imageUsage = VK_IMAGE_USAGE_COLOR_ATTACHMENT_BIT | VK_IMAGE_USAGE_TRANSFER_DST_BIT;
        info.imageSharingMode = GetSwapchainSharingMode();
        info.preTransform = preTransform;
        info.compositeAlpha = composite;
        info.presentMode = presentMode;
        info.clipped = VK_TRUE;
        info.oldSwapchain = oldSwapchain;
        VK_CHECK(vkCreateSwapchainKHR(g_ctx.device, &info, g_ctx.allocator, &g_ctx.swapchain));
    }

    g_ctx.extent = swapchainExtent;
    g_ctx.format = surfaceFormat.format;

    // Clean up old swapchain
    if (oldSwapchain != VK_NULL_HANDLE)
    {
        // TODO: Handle this
        //for (VkImageView imageView : g_ctx.imageViews)
        //{
        //    vkDestroyImageView(g_ctx.device, imageView, nullptr);
        //}
        //
        //uint32_t imageCount;
        //err = vkGetSwapchainImagesKHR(g_ctx.device, oldSwapchain, &imageCount, nullptr);
        //
        //for (size_t i = 0; i < imageCount; i++)
        //{
        //    ShutdownPerFrame(g_ctx.perframe[i]);
        //}
        //
        //g_ctx.imageViews.clear();
        //vkDestroySwapchainKHR(g_ctx.device, oldSwapchain, nullptr);
    }

    // Create render pass
    {
        VkAttachmentDescription attachment = { 0 };
        // Backbuffer format.
        attachment.format = g_ctx.format;
        // Not multisampled.
        attachment.samples = VK_SAMPLE_COUNT_1_BIT;
        // When starting the frame, we want tiles to be cleared.
        attachment.loadOp = VK_ATTACHMENT_LOAD_OP_DONT_CARE;// VK_ATTACHMENT_LOAD_OP_CLEAR;
        // When ending the frame, we want tiles to be written out.
        attachment.storeOp = VK_ATTACHMENT_STORE_OP_STORE;
        // Don't care about stencil since we're not using it.
        attachment.stencilLoadOp = VK_ATTACHMENT_LOAD_OP_DONT_CARE;
        attachment.stencilStoreOp = VK_ATTACHMENT_STORE_OP_DONT_CARE;

        // The image layout will be undefined when the render pass begins.
        attachment.initialLayout = VK_IMAGE_LAYOUT_UNDEFINED;
        // After the render pass is complete, we will transition to PRESENT_SRC_KHR layout.
        attachment.finalLayout = VK_IMAGE_LAYOUT_PRESENT_SRC_KHR;

        // We have one subpass. This subpass has one color attachment.
        // While executing this subpass, the attachment will be in attachment optimal layout.
        VkAttachmentReference colorRef = { 0, VK_IMAGE_LAYOUT_COLOR_ATTACHMENT_OPTIMAL };

        // We will end up with two transitions.
        // The first one happens right before we start subpass #0, where
        // UNDEFINED is transitioned into COLOR_ATTACHMENT_OPTIMAL.
        // The final layout in the render pass attachment states PRESENT_SRC_KHR, so we
        // will get a final transition from COLOR_ATTACHMENT_OPTIMAL to PRESENT_SRC_KHR.
        VkSubpassDescription subpass = { 0 };
        subpass.pipelineBindPoint = VK_PIPELINE_BIND_POINT_GRAPHICS;
        subpass.colorAttachmentCount = 1;
        subpass.pColorAttachments = &colorRef;

        // Create a dependency to external events.
        // We need to wait for the WSI semaphore to signal.
        // Only pipeline stages which depend on COLOR_ATTACHMENT_OUTPUT_BIT will
        // actually wait for the semaphore, so we must also wait for that pipeline stage.
        VkSubpassDependency dependency = { 0 };
        dependency.srcSubpass = VK_SUBPASS_EXTERNAL;
        dependency.dstSubpass = 0;
        dependency.srcStageMask = VK_PIPELINE_STAGE_COLOR_ATTACHMENT_OUTPUT_BIT;
        dependency.dstStageMask = VK_PIPELINE_STAGE_COLOR_ATTACHMENT_OUTPUT_BIT;

        // Since we changed the image layout, we need to make the memory visible to
        // color attachment to modify.
        dependency.srcAccessMask = 0;
        dependency.dstAccessMask = VK_ACCESS_COLOR_ATTACHMENT_READ_BIT | VK_ACCESS_COLOR_ATTACHMENT_WRITE_BIT;

        // Finally, create the renderpass.
        VkRenderPassCreateInfo rpInfo{ VK_STRUCTURE_TYPE_RENDER_PASS_CREATE_INFO };
        rpInfo.attachmentCount = 1;
        rpInfo.pAttachments = &attachment;
        rpInfo.subpassCount = 1;
        rpInfo.pSubpasses = &subpass;
        rpInfo.dependencyCount = 1;
        rpInfo.pDependencies = &dependency;
        VK_CHECK(vkCreateRenderPass(g_ctx.device, &rpInfo, g_ctx.allocator, &g_ctx.renderPass));
    }

    uint32_t imageCount;
    VK_CHECK(vkGetSwapchainImagesKHR(g_ctx.device, g_ctx.swapchain, &imageCount, nullptr));

    /// The swapchain images.
    std::vector<VkImage> swapchainImages(imageCount);
    VK_CHECK(vkGetSwapchainImagesKHR(g_ctx.device, g_ctx.swapchain, &imageCount, swapchainImages.data()));

    // Initialize per-frame resources.
    // Every swapchain image has its own command pool and fence manager.
    // This makes it very easy to keep track of when we can reset command buffers and such.
    g_ctx.frames.clear();
    g_ctx.frames.resize(imageCount);

    for (size_t i = 0; i < imageCount; i++)
    {
        Frame& frame = g_ctx.frames[i];
        frame.image = swapchainImages[i];

        // Create an image view which we can render into.
        {
            VkImageViewCreateInfo viewInfo{ VK_STRUCTURE_TYPE_IMAGE_VIEW_CREATE_INFO };
            viewInfo.viewType = VK_IMAGE_VIEW_TYPE_2D;
            viewInfo.format = g_ctx.format;
            viewInfo.image = frame.image;
            viewInfo.subresourceRange.levelCount = 1;
            viewInfo.subresourceRange.layerCount = 1;
            viewInfo.subresourceRange.aspectMask = VK_IMAGE_ASPECT_COLOR_BIT;
            viewInfo.components.r = VK_COMPONENT_SWIZZLE_R;
            viewInfo.components.g = VK_COMPONENT_SWIZZLE_G;
            viewInfo.components.b = VK_COMPONENT_SWIZZLE_B;
            viewInfo.components.a = VK_COMPONENT_SWIZZLE_A;
            VK_CHECK(vkCreateImageView(g_ctx.device, &viewInfo, g_ctx.allocator, &frame.imageView));
        }

        // Create a fence to synchronize
        {
            VkFenceCreateInfo fenceInfo{ VK_STRUCTURE_TYPE_FENCE_CREATE_INFO };
            fenceInfo.flags = VK_FENCE_CREATE_SIGNALED_BIT;
            VK_CHECK(vkCreateFence(g_ctx.device, &fenceInfo, g_ctx.allocator, &frame.inFlightFence));
        }

        // Create a command buffer
        {
            VkCommandBufferAllocateInfo cmdBuffInfo{ VK_STRUCTURE_TYPE_COMMAND_BUFFER_ALLOCATE_INFO };
            cmdBuffInfo.commandPool = g_ctx.commandPool;
            cmdBuffInfo.level = VK_COMMAND_BUFFER_LEVEL_PRIMARY;
            cmdBuffInfo.commandBufferCount = 1;
            VK_CHECK(vkAllocateCommandBuffers(g_ctx.device, &cmdBuffInfo, &frame.commandBuffer));
        }

        // Build the framebuffer.
        {
            VkFramebufferCreateInfo fbInfo{ VK_STRUCTURE_TYPE_FRAMEBUFFER_CREATE_INFO };
            fbInfo.renderPass = g_ctx.renderPass;
            fbInfo.attachmentCount = 1;
            fbInfo.pAttachments = &frame.imageView;
            fbInfo.width = g_ctx.extent.width;
            fbInfo.height = g_ctx.extent.height;
            fbInfo.layers = 1;
            VK_CHECK(vkCreateFramebuffer(g_ctx.device, &fbInfo, g_ctx.allocator, &frame.framebuffer));
        }

        // Transition swap chain image to present source layout
        {
            VkImageMemoryBarrier barrier = {};
            barrier.sType = VK_STRUCTURE_TYPE_IMAGE_MEMORY_BARRIER;
            barrier.oldLayout = VK_IMAGE_LAYOUT_UNDEFINED;
            barrier.newLayout = VK_IMAGE_LAYOUT_PRESENT_SRC_KHR;
            barrier.srcQueueFamilyIndex = VK_QUEUE_FAMILY_IGNORED;
            barrier.dstQueueFamilyIndex = VK_QUEUE_FAMILY_IGNORED;
            barrier.image = frame.image;
            barrier.subresourceRange.aspectMask = VK_IMAGE_ASPECT_COLOR_BIT;
            barrier.subresourceRange.baseMipLevel = 0;
            barrier.subresourceRange.levelCount = 1;
            barrier.subresourceRange.baseArrayLayer = 0;
            barrier.subresourceRange.layerCount = 1;

            VkCommandBufferBeginInfo beginInfo = {};
            beginInfo.sType = VK_STRUCTURE_TYPE_COMMAND_BUFFER_BEGIN_INFO;
            beginInfo.flags = VK_COMMAND_BUFFER_USAGE_ONE_TIME_SUBMIT_BIT;

            vkBeginCommandBuffer(frame.commandBuffer, &beginInfo);
            vkCmdPipelineBarrier(frame.commandBuffer,
                VK_PIPELINE_STAGE_TOP_OF_PIPE_BIT, VK_PIPELINE_STAGE_COLOR_ATTACHMENT_OUTPUT_BIT,
                0, 0, nullptr, 0, nullptr, 1, &barrier);

            // End recording the command buffer
            vkEndCommandBuffer(frame.commandBuffer);

            // Submit the command buffer
            VkSubmitInfo submitInfo = {};
            submitInfo.sType = VK_STRUCTURE_TYPE_SUBMIT_INFO;
            submitInfo.commandBufferCount = 1;
            submitInfo.pCommandBuffers = &frame.commandBuffer;

            vkQueueSubmit(g_ctx.graphicsQueue, 1, &submitInfo, VK_NULL_HANDLE);
            vkQueueWaitIdle(g_ctx.graphicsQueue); // Wait for the command buffer to complete execution
        }
    }
}

// -----------------------------------------------------
// Graphics Vulkan Backend Implementation
// -----------------------------------------------------

bool Graphics::Initialize(void* device)
{
#ifdef WINDOW_GLFW_BACKEND
    GLFWwindow* glfwWindow = (GLFWwindow*)device;

    uint32_t extensionsCount = 0;
    const char** extensions = glfwGetRequiredInstanceExtensions(&extensionsCount);

    InitializeInstance(extensionsCount, extensions);

    VK_CHECK(glfwCreateWindowSurface(g_ctx.instance, glfwWindow, g_ctx.allocator, &g_ctx.surface));
#else

    Log::Error("`Window backend not supported!");
    return false;
#endif

    int w, h;
    Window::GetSize(&w, &h);

    InitializeDevice();
    InitializeSwapchain(w, h);

    int ret = glslang_initialize_process();
    // TODO: Check for proper glslang init

    return true;
}

// TODO: These should be implemented in the interface API
static void DestroyShader(GraphicsHandle shader);
static void DestroyBuffer(GraphicsHandle buffer);
static void DestroyTexture(GraphicsHandle texture);
static void DestroyPipeline(GraphicsHandle pipeline);

void Graphics::Shutdown()
{
    // Don't release anything until the GPU is completely idle.
    vkDeviceWaitIdle(g_ctx.device);

    // Wait until device is idle before teardown.
    vkQueueWaitIdle(g_ctx.graphicsQueue);
    
    // Terminate glslang processes
    glslang_finalize_process();

    // Clean up shader resources
    for (const auto& entry : g_shaders)
        DestroyShader(entry.key);
    g_shaders.Clear();

    // Clean up buffer resources
    for (const auto& entry : g_buffers)
        DestroyBuffer(entry.key);
    g_buffers.Clear();

    // Clean up texture resources
    for (const auto& entry : g_textures)
        DestroyTexture(entry.key);
    g_textures.Clear();

    // Clean up pipeline resources
    for (const auto& entry : g_pipelines)
        DestroyPipeline(entry.key);
    g_pipelines.Clear();

    // Release frame data
    for (auto& frame : g_ctx.frames)
    {
        if (frame.imageView != VK_NULL_HANDLE)
        {
            vkDestroyImageView(g_ctx.device, frame.imageView, g_ctx.allocator);
            frame.imageView = VK_NULL_HANDLE;
        }

        if (frame.framebuffer != VK_NULL_HANDLE)
        {
            vkDestroyFramebuffer(g_ctx.device, frame.framebuffer, g_ctx.allocator);
            frame.framebuffer = VK_NULL_HANDLE;
        }

        if (frame.inFlightFence != VK_NULL_HANDLE)
        {
            vkDestroyFence(g_ctx.device, frame.inFlightFence, g_ctx.allocator);
            frame.inFlightFence = VK_NULL_HANDLE;
        }

        if (frame.commandBuffer != VK_NULL_HANDLE)
        {
            vkFreeCommandBuffers(g_ctx.device, g_ctx.commandPool, 1, &frame.commandBuffer);
            frame.commandBuffer = VK_NULL_HANDLE;
        }

        if (frame.acquireSemaphore != VK_NULL_HANDLE)
        {
            vkDestroySemaphore(g_ctx.device, frame.acquireSemaphore, g_ctx.allocator);
            frame.acquireSemaphore = VK_NULL_HANDLE;
        }

        if (frame.presentSemaphore != VK_NULL_HANDLE)
        {
            vkDestroySemaphore(g_ctx.device, frame.presentSemaphore, g_ctx.allocator);
            frame.presentSemaphore = VK_NULL_HANDLE;
        }
    }
    g_ctx.frames.clear();
    
    // Destroy semaphores
    for (auto semaphore : g_ctx.semaphores)
    {
        vkDestroySemaphore(g_ctx.device, semaphore, g_ctx.allocator);
    }

    // Destroy render pass
    if (g_ctx.renderPass != VK_NULL_HANDLE)
    {
        vkDestroyRenderPass(g_ctx.device, g_ctx.renderPass, g_ctx.allocator);
        g_ctx.renderPass = VK_NULL_HANDLE;
    }

    // Destroy swapchain
    if (g_ctx.swapchain != VK_NULL_HANDLE)
    {
        vkDestroySwapchainKHR(g_ctx.device, g_ctx.swapchain, g_ctx.allocator);
        g_ctx.swapchain = VK_NULL_HANDLE;
    }

    // Destroy surface
    if (g_ctx.surface != VK_NULL_HANDLE)
    {
        vkDestroySurfaceKHR(g_ctx.instance, g_ctx.surface, g_ctx.allocator);
        g_ctx.surface = VK_NULL_HANDLE;
    }

    // Destroy descriptor pool
    if (g_ctx.descriptorPool != VK_NULL_HANDLE)
    {
        vkDestroyDescriptorPool(g_ctx.device, g_ctx.descriptorPool, g_ctx.allocator);
        g_ctx.descriptorPool = VK_NULL_HANDLE;
    }

    // Destroy command pool
    if (g_ctx.commandPool != VK_NULL_HANDLE)
    {
        vkDestroyCommandPool(g_ctx.device, g_ctx.commandPool, g_ctx.allocator);
        g_ctx.commandPool = VK_NULL_HANDLE;
    }

#ifdef GRAPHICS_VK_DEBUG_REPORT
    // Release debug callback
    if (g_ctx.debugCallback != VK_NULL_HANDLE)
    {
        auto vkDestroyDebugReportCallbackEXT = (PFN_vkDestroyDebugReportCallbackEXT)vkGetInstanceProcAddr(g_ctx.instance, "vkDestroyDebugReportCallbackEXT");
        ENGINE_ASSERT(vkDestroyDebugReportCallbackEXT != NULL);

        vkDestroyDebugReportCallbackEXT(g_ctx.instance, g_ctx.debugCallback, g_ctx.allocator);
        g_ctx.debugCallback = VK_NULL_HANDLE;
    }
#endif

    // Destroy device
    if (g_ctx.device != VK_NULL_HANDLE)
    {
        vkDestroyDevice(g_ctx.device, g_ctx.allocator);
        g_ctx.device = VK_NULL_HANDLE;
    }

    // Destroy instance
    if (g_ctx.instance != VK_NULL_HANDLE)
    {
        vkDestroyInstance(g_ctx.instance, g_ctx.allocator);
        g_ctx.instance = VK_NULL_HANDLE;
    }
}

void Graphics::Reload()
{
    // TODO: No implementation
}

void Graphics::NewFrame()
{
    // Make sure we have a semaphore available for synchronization
    VkSemaphore acquireSemaphore;
    if (g_ctx.semaphores.empty())
    {
        VkSemaphoreCreateInfo info = { VK_STRUCTURE_TYPE_SEMAPHORE_CREATE_INFO };
        VK_CHECK(vkCreateSemaphore(g_ctx.device, &info, g_ctx.allocator, &acquireSemaphore));
    }
    else
    {
        acquireSemaphore = g_ctx.semaphores.back();
        g_ctx.semaphores.pop_back();
    }

    // Acquire next frame
    VkResult res = vkAcquireNextImageKHR(g_ctx.device, g_ctx.swapchain, UINT64_MAX, acquireSemaphore, VK_NULL_HANDLE, &g_ctx.currentFrame);
    if (res != VK_SUCCESS)
    {
        g_ctx.semaphores.push_back(acquireSemaphore);
        
        // Handle outdated error in acquire.
        //if (res == VK_SUBOPTIMAL_KHR || res == VK_ERROR_OUT_OF_DATE_KHR)
        //{
        //    resize(context.swapchain_dimensions.width, context.swapchain_dimensions.height);
        //    res = acquire_next_image(context, &index);
        //}

        vkQueueWaitIdle(g_ctx.graphicsQueue);
        return;
    }

    auto& frame = g_ctx.frames[g_ctx.currentFrame];

    // If we have outstanding fences for this swapchain image, wait for them to complete first.
    // After begin frame returns, it is safe to reuse or delete resources which
    // were used previously.
    //
    // We wait for fences which completes N frames earlier, so we do not stall,
    // waiting for all GPU work to complete before this returns.
    // Normally, this doesn't really block at all,
    // since we're waiting for old frames to have been completed, but just in case.
    if (frame.inFlightFence != VK_NULL_HANDLE)
    {
        vkWaitForFences(g_ctx.device, 1, &frame.inFlightFence, true, UINT64_MAX);
        vkResetFences(g_ctx.device, 1, &frame.inFlightFence);
    }

    // Reset acquired frame commands
    if (frame.commandBuffer != VK_NULL_HANDLE)
    {
        vkResetCommandBuffer(frame.commandBuffer, VK_COMMAND_BUFFER_RESET_RELEASE_RESOURCES_BIT);
    }

    // Recycle the old semaphore back into the semaphore manager.
    VkSemaphore oldSemaphore = frame.acquireSemaphore;
    if (oldSemaphore != VK_NULL_HANDLE)
    {
        g_ctx.semaphores.push_back(oldSemaphore);
    }
    frame.acquireSemaphore = acquireSemaphore;

    // We will only submit this once before it's recycled.
    VkCommandBufferBeginInfo beginInfo{ VK_STRUCTURE_TYPE_COMMAND_BUFFER_BEGIN_INFO };
    beginInfo.flags = VK_COMMAND_BUFFER_USAGE_ONE_TIME_SUBMIT_BIT;

    // Begin command recording
    vkBeginCommandBuffer(frame.commandBuffer, &beginInfo);

    VkViewport vp{};
    vp.width = static_cast<float>(g_ctx.extent.width);
    vp.height = static_cast<float>(g_ctx.extent.height);
    vp.minDepth = 0.0f;
    vp.maxDepth = 1.0f;
    // Set viewport dynamically
    vkCmdSetViewport(frame.commandBuffer, 0, 1, &vp);

    VkRect2D scissor{};
    scissor.extent = g_ctx.extent;
    // Set scissor dynamically
    vkCmdSetScissor(frame.commandBuffer, 0, 1, &scissor);
}

void Graphics::EndFrame()
{
    auto& frame = g_ctx.frames[g_ctx.currentFrame];

    // Check for active render pass and complete it.
    if (g_ctx.bHasActiveRenderPass)
    {
        vkCmdEndRenderPass(frame.commandBuffer);
        g_ctx.bHasActiveRenderPass = false;
    }

    // Complete the command buffer.
    VK_CHECK(vkEndCommandBuffer(frame.commandBuffer));

    // Submit it to the queue with a present semaphore.
    if (frame.presentSemaphore == VK_NULL_HANDLE)
    {
        VkSemaphoreCreateInfo semaphore_info{ VK_STRUCTURE_TYPE_SEMAPHORE_CREATE_INFO };
        VK_CHECK(vkCreateSemaphore(g_ctx.device, &semaphore_info, g_ctx.allocator, &frame.presentSemaphore));
    }

    VkPipelineStageFlags waitStage{ VK_PIPELINE_STAGE_COLOR_ATTACHMENT_OUTPUT_BIT };

    VkSubmitInfo info{ VK_STRUCTURE_TYPE_SUBMIT_INFO };
    info.commandBufferCount = 1;
    info.pCommandBuffers = &frame.commandBuffer;
    info.waitSemaphoreCount = 1;
    info.pWaitSemaphores = &frame.acquireSemaphore;
    info.pWaitDstStageMask = &waitStage;
    info.signalSemaphoreCount = 1;
    info.pSignalSemaphores = &frame.presentSemaphore;

    // Submit command buffer to present queue
    VK_CHECK(vkQueueSubmit(g_ctx.presentQueue, 1, &info, frame.inFlightFence));

    VkPresentInfoKHR present{ VK_STRUCTURE_TYPE_PRESENT_INFO_KHR };
    present.swapchainCount = 1;
    present.pSwapchains = &g_ctx.swapchain;
    present.pImageIndices = &g_ctx.currentFrame;
    present.waitSemaphoreCount = 1;
    present.pWaitSemaphores = &frame.presentSemaphore;
    // Present swapchain image
    VkResult res = vkQueuePresentKHR(g_ctx.presentQueue, &present);

    // Handle Outdated error in present.
    if (res == VK_SUBOPTIMAL_KHR || res == VK_ERROR_OUT_OF_DATE_KHR)
    {
        //resize(context.swapchain_dimensions.width, context.swapchain_dimensions.height);
    }
    else if (res != VK_SUCCESS)
    {
        Log::Error("Failed to present swapchain image.");
    }
}

TextureFormat Graphics::GetColorBufferFormat()
{
    return TextureFormat::UNKNOWN;
}

TextureFormat Graphics::GetDepthBufferFormat()
{
    return TextureFormat::UNKNOWN;
}

GraphicsHandle Graphics::GetCurrentBackBufferRT()
{
    return 0;
}

GraphicsHandle Graphics::GetDepthBuffer()
{
    return 0;
}

GraphicsHandle Graphics::CreateShader(const ShaderInfo& info)
{
    ShaderImpl shaderImpl;

    String source = VK_GLSL_VERSION;
    switch (info.shaderType)
    {
    case ShaderType::VERTEX:
        source += VK_GLSL_VERTEX;
        break;

    case ShaderType::PIXEL:
        source += VK_GLSL_PIXEL;
        break;
    }

    source += info.source;

    SpirVBinary spirv{};
    switch (info.shaderType)
    {
    case ShaderType::VERTEX:
        spirv = compileShaderToSPIRV_Vulkan(GLSLANG_STAGE_VERTEX, source.c_str(), "filename");
        break;
    
    case ShaderType::PIXEL:
        spirv = compileShaderToSPIRV_Vulkan(GLSLANG_STAGE_FRAGMENT, source.c_str(), "filename");
        break;
    
    default: ENGINE_ASSERT(false);
    }

    VkShaderModuleCreateInfo vert_info{ VK_STRUCTURE_TYPE_SHADER_MODULE_CREATE_INFO };
    vert_info.codeSize = spirv.size * sizeof(u32);
    vert_info.pCode = spirv.words;
    VK_CHECK(vkCreateShaderModule(g_ctx.device, &vert_info, g_ctx.allocator, &shaderImpl.shaderModule));

    // Store into global map
    static GraphicsHandle g_shaderHandleCounter = 0;
    GraphicsHandle shaderHandle = g_shaderHandleCounter++;
    g_shaders.Insert(shaderHandle, shaderImpl);
    return shaderHandle;
}

static void DestroyShader(GraphicsHandle shader)
{
    const auto& impl = g_shaders[shader];
    vkDestroyShaderModule(g_ctx.device, impl.shaderModule, g_ctx.allocator);
}

static void DestroyBuffer(GraphicsHandle buffer)
{
    const auto& impl = g_buffers[buffer];
    // TODO: Implement this
}

static VkFormat GetFormat(ValueType type, u32 num, bool normalize)
{
    switch (type)
    {

    case ValueType::FLOAT32:
        switch (num)
        {
        case 1: return normalize ? VK_FORMAT_R32_SFLOAT : VK_FORMAT_R32_SFLOAT;
        case 2: return normalize ? VK_FORMAT_R32G32_SFLOAT : VK_FORMAT_R32G32_SFLOAT;
        case 3: return normalize ? VK_FORMAT_R32G32B32_SFLOAT : VK_FORMAT_R32G32B32_SFLOAT;
        case 4: return normalize ? VK_FORMAT_R32G32B32A32_SFLOAT : VK_FORMAT_R32G32B32A32_SFLOAT;
        default: ENGINE_ASSERT(false);
        }

    case ValueType::UINT8:
        switch (num)
        {
        case 1: return normalize ? VK_FORMAT_R8_UNORM : VK_FORMAT_R8_UNORM;
        case 2: return normalize ? VK_FORMAT_R8G8_UNORM : VK_FORMAT_R8G8_UNORM;
        case 3: return normalize ? VK_FORMAT_R8G8B8_UNORM : VK_FORMAT_R8G8B8_UNORM;
        case 4: return normalize ? VK_FORMAT_R8G8B8A8_UNORM : VK_FORMAT_R8G8B8A8_UNORM;
        default: ENGINE_ASSERT(false);
        }

    default: ENGINE_ASSERT(false);
    }
}

static u32 GetValueSize(ValueType vt)
{
    switch (vt)
    {
    case ValueType::FLOAT32: return sizeof(f32);
        break;

    case ValueType::UINT8: return sizeof(u8);
        break;

    default:
        Log::Error("Value type not supported!");
        return 0;
    }
}

static VkPrimitiveTopology GetTopology(GeomTopology topology)
{
    switch (topology)
    {
    case GeomTopology::POINTS:
        return VK_PRIMITIVE_TOPOLOGY_POINT_LIST;

    case GeomTopology::LINES:
        return VK_PRIMITIVE_TOPOLOGY_LINE_LIST;

    case GeomTopology::TRIANGLES:
        return VK_PRIMITIVE_TOPOLOGY_TRIANGLE_LIST;

    default: ENGINE_ASSERT(false);
    }
}

static VkShaderStageFlags GetShaderStage(ShaderType shaderType)
{
    switch (shaderType)
    {
    case ShaderType::VERTEX:
        return VK_SHADER_STAGE_VERTEX_BIT;

    default: ENGINE_ASSERT(false);
    }
}

GraphicsHandle Graphics::CreatePipeline(const PipelineInfo& info)
{
    PipelineImpl impl;

    impl.samplers.resize(info.numImmutableSamplers);

    std::vector<VkDescriptorSetLayoutBinding> layoutBindings;
    layoutBindings.resize(info.numImmutableSamplers);

    for (u32 i = 0; i < info.numImmutableSamplers; i++)
    {
        const auto& sampler = info.immutableSamplers[i];

        VkSamplerCreateInfo samplerInfo{ VK_STRUCTURE_TYPE_SAMPLER_CREATE_INFO };
        samplerInfo.magFilter = VK_FILTER_LINEAR;// sampler.samplerInfo.magFilter;
        samplerInfo.minFilter = VK_FILTER_LINEAR;// sampler.samplerInfo.minFilter;
        samplerInfo.mipmapMode = VK_SAMPLER_MIPMAP_MODE_LINEAR;// sampler.samplerInfo.mipmapMode;
        samplerInfo.addressModeU = VK_SAMPLER_ADDRESS_MODE_REPEAT;// sampler.samplerInfo.addressModeU;
        samplerInfo.addressModeV = VK_SAMPLER_ADDRESS_MODE_REPEAT;// sampler.samplerInfo.addressModeV;
        samplerInfo.addressModeW = VK_SAMPLER_ADDRESS_MODE_REPEAT;// sampler.samplerInfo.addressModeW;
        samplerInfo.minLod = -1000;// sampler.samplerInfo.minLod;
        samplerInfo.maxLod = 1000;// sampler.samplerInfo.maxLod;
        samplerInfo.maxAnisotropy = 1.0f;// sampler.samplerInfo.maxAnisotropy;
        VK_CHECK(vkCreateSampler(g_ctx.device, &samplerInfo, g_ctx.allocator, &impl.samplers[i]));

        layoutBindings[i].descriptorType = VK_DESCRIPTOR_TYPE_COMBINED_IMAGE_SAMPLER;
        layoutBindings[i].descriptorCount = 1;
        layoutBindings[i].stageFlags = VK_SHADER_STAGE_FRAGMENT_BIT;
        layoutBindings[i].pImmutableSamplers = &impl.samplers[i];
    }

    VkDescriptorSetLayoutCreateInfo descLayoutInfo{ VK_STRUCTURE_TYPE_DESCRIPTOR_SET_LAYOUT_CREATE_INFO };
    descLayoutInfo.bindingCount = layoutBindings.size();
    descLayoutInfo.pBindings = layoutBindings.data();
    VK_CHECK(vkCreateDescriptorSetLayout(g_ctx.device, &descLayoutInfo, g_ctx.allocator, &impl.descriptorSetLayout));

    std::vector<VkPushConstantRange> pushConstants;
    pushConstants.resize(info.numConstants);

    for (u32 i = 0; i < info.numConstants; i++)
    {
        const auto& constant = info.constants[i];
        pushConstants[i].stageFlags = GetShaderStage(constant.shaderType);
        pushConstants[i].offset = constant.offset;
        pushConstants[i].size = constant.size;
    }

    VkDescriptorSetLayout setLayouts[1] = { impl.descriptorSetLayout };

    VkPipelineLayoutCreateInfo pipeLayoutInfo{ VK_STRUCTURE_TYPE_PIPELINE_LAYOUT_CREATE_INFO };
    pipeLayoutInfo.setLayoutCount = 1;
    pipeLayoutInfo.pSetLayouts = setLayouts;
    pipeLayoutInfo.pushConstantRangeCount = pushConstants.size();
    pipeLayoutInfo.pPushConstantRanges = pushConstants.data();
    VK_CHECK(vkCreatePipelineLayout(g_ctx.device, &pipeLayoutInfo, nullptr, &impl.pipelineLayout));
    
    std::vector<VkVertexInputAttributeDescription> attributeDesc;
    attributeDesc.resize(info.numElements);

    u32 relativeOffset = 0;
    for (u32 i = 0; i < info.numElements; i++)
    {
        const auto& elem = info.layoutElements[i];
        if (elem.relativeOffset > 0)
            relativeOffset = elem.relativeOffset;

        attributeDesc[i].location = elem.inputIndex;
        attributeDesc[i].binding = elem.bufferSlot;
        attributeDesc[i].format = GetFormat(elem.valueType, elem.numComponents, elem.isNormalized);
        attributeDesc[i].offset = relativeOffset;

        relativeOffset += elem.numComponents * GetValueSize(elem.valueType);
    }

    u32 vertBindingCount = info.numElements > 0 ? 1 : 0;

    // TODO: Do we always expect one vertex binding? Only one?
    VkVertexInputBindingDescription vertBinding[1] = {};
    vertBinding[0].binding = 0;
    vertBinding[0].stride = relativeOffset;
    vertBinding[0].inputRate = VK_VERTEX_INPUT_RATE_VERTEX;

    VkPipelineVertexInputStateCreateInfo vertexInput{ VK_STRUCTURE_TYPE_PIPELINE_VERTEX_INPUT_STATE_CREATE_INFO };
    vertexInput.vertexBindingDescriptionCount = vertBindingCount;
    vertexInput.pVertexBindingDescriptions = vertBinding;
    vertexInput.vertexAttributeDescriptionCount = attributeDesc.size();
    vertexInput.pVertexAttributeDescriptions = attributeDesc.data();

    // Specify we will use triangle lists to draw geometry.
    VkPipelineInputAssemblyStateCreateInfo inputAssembly{ VK_STRUCTURE_TYPE_PIPELINE_INPUT_ASSEMBLY_STATE_CREATE_INFO };
    inputAssembly.topology = GetTopology(info.geomTopology);

    // Specify rasterization state.
    VkPipelineRasterizationStateCreateInfo rasterizer{ VK_STRUCTURE_TYPE_PIPELINE_RASTERIZATION_STATE_CREATE_INFO };
    rasterizer.depthClampEnable = VK_FALSE;
    rasterizer.rasterizerDiscardEnable = VK_FALSE;
    rasterizer.polygonMode = VK_POLYGON_MODE_FILL;
    rasterizer.cullMode = VK_CULL_MODE_NONE;// VK_CULL_MODE_BACK_BIT;
    rasterizer.frontFace = VK_FRONT_FACE_COUNTER_CLOCKWISE;// VK_FRONT_FACE_CLOCKWISE;
    rasterizer.depthBiasEnable = VK_FALSE;
    //rasterizer.depthBiasConstantFactor;
    //rasterizer.depthBiasClamp;
    //rasterizer.depthBiasSlopeFactor;
    rasterizer.lineWidth = 1.0f;

    // Our attachment will write to all color channels, but no blending is enabled.
    VkPipelineColorBlendAttachmentState colorBlendAttachment{};
    colorBlendAttachment.blendEnable = VK_TRUE;
    colorBlendAttachment.srcColorBlendFactor = VK_BLEND_FACTOR_SRC_ALPHA;
    colorBlendAttachment.dstColorBlendFactor = VK_BLEND_FACTOR_ONE_MINUS_SRC_ALPHA;
    colorBlendAttachment.colorBlendOp = VK_BLEND_OP_ADD;
    colorBlendAttachment.srcAlphaBlendFactor = VK_BLEND_FACTOR_ONE;
    colorBlendAttachment.dstAlphaBlendFactor = VK_BLEND_FACTOR_ONE_MINUS_SRC_ALPHA;
    colorBlendAttachment.alphaBlendOp = VK_BLEND_OP_ADD;
    colorBlendAttachment.colorWriteMask = VK_COLOR_COMPONENT_R_BIT | VK_COLOR_COMPONENT_G_BIT | VK_COLOR_COMPONENT_B_BIT | VK_COLOR_COMPONENT_A_BIT;
    
    VkPipelineColorBlendStateCreateInfo colorBlendState{ VK_STRUCTURE_TYPE_PIPELINE_COLOR_BLEND_STATE_CREATE_INFO };
    colorBlendState.logicOpEnable = VK_FALSE;
    colorBlendState.logicOp = VK_LOGIC_OP_COPY;
    colorBlendState.attachmentCount = 1;
    colorBlendState.pAttachments = &colorBlendAttachment;
    colorBlendState.blendConstants[0] = 0.0f;
    colorBlendState.blendConstants[1] = 0.0f;
    colorBlendState.blendConstants[2] = 0.0f;
    colorBlendState.blendConstants[3] = 0.0f;

    // We will have one viewport and scissor box.
    VkViewport viewport{};
    viewport.x = 0.0f;
    viewport.y = 0.0f;
    viewport.width = (float)g_ctx.extent.width;
    viewport.height = (float)g_ctx.extent.height;
    viewport.minDepth = 0.0f;
    viewport.maxDepth = 1.0f;
    
    VkRect2D scissor{};
    scissor.offset.x = 0.0f;
    scissor.offset.y = 0.0f;
    scissor.extent = g_ctx.extent;

    VkPipelineViewportStateCreateInfo viewportState{ VK_STRUCTURE_TYPE_PIPELINE_VIEWPORT_STATE_CREATE_INFO };
    viewportState.viewportCount = 1;
    viewportState.pViewports = &viewport;
    viewportState.scissorCount = 1;
    viewportState.pScissors = &scissor;

    // Disable all depth testing.
    VkPipelineDepthStencilStateCreateInfo depthStencilState{ VK_STRUCTURE_TYPE_PIPELINE_DEPTH_STENCIL_STATE_CREATE_INFO };

    // No multisampling.
    VkPipelineMultisampleStateCreateInfo multisample{ VK_STRUCTURE_TYPE_PIPELINE_MULTISAMPLE_STATE_CREATE_INFO };
    multisample.rasterizationSamples = VK_SAMPLE_COUNT_1_BIT;
    multisample.sampleShadingEnable = VK_FALSE;
    //multisample.minSampleShading;
    //multisample.pSampleMask;
    //multisample.alphaToCoverageEnable;
    //multisample.alphaToOneEnable;

    // Specify that these states will be dynamic, i.e. not part of pipeline state object.
    std::array<VkDynamicState, 2> dynamics{ VK_DYNAMIC_STATE_VIEWPORT, VK_DYNAMIC_STATE_SCISSOR };

    VkPipelineDynamicStateCreateInfo dynamic{ VK_STRUCTURE_TYPE_PIPELINE_DYNAMIC_STATE_CREATE_INFO };
    dynamic.dynamicStateCount = dynamics.size();
    dynamic.pDynamicStates = dynamics.data();

    // Load our SPIR-V shaders.
    std::array<VkPipelineShaderStageCreateInfo, 2> shaderStages{};

    // Vertex stage of the pipeline
    shaderStages[0].sType = VK_STRUCTURE_TYPE_PIPELINE_SHADER_STAGE_CREATE_INFO;
    shaderStages[0].stage = VK_SHADER_STAGE_VERTEX_BIT;
    shaderStages[0].module = g_shaders[info.vertShader].shaderModule;
    shaderStages[0].pName = "main";

    // Fragment stage of the pipeline
    shaderStages[1].sType = VK_STRUCTURE_TYPE_PIPELINE_SHADER_STAGE_CREATE_INFO;
    shaderStages[1].stage = VK_SHADER_STAGE_FRAGMENT_BIT;
    shaderStages[1].module = g_shaders[info.pixelShader].shaderModule;
    shaderStages[1].pName = "main";

    VkGraphicsPipelineCreateInfo pipe{ VK_STRUCTURE_TYPE_GRAPHICS_PIPELINE_CREATE_INFO };
    pipe.flags = VkPipelineCreateFlags{};// bd->PipelineCreateFlags;
    pipe.stageCount = shaderStages.size();
    pipe.pStages = shaderStages.data();
    pipe.pVertexInputState = &vertexInput;
    pipe.pInputAssemblyState = &inputAssembly;
    //pipe.pTessellationState;
    pipe.pViewportState = &viewportState;
    pipe.pRasterizationState = &rasterizer;
    pipe.pMultisampleState = &multisample;
    pipe.pDepthStencilState = &depthStencilState;
    pipe.pColorBlendState = &colorBlendState;
    pipe.pDynamicState = &dynamic;

    // We need to specify the pipeline layout and the render pass description up front as well.
    pipe.layout = impl.pipelineLayout;
    pipe.renderPass = g_ctx.renderPass;
    pipe.subpass = 0;// subpass;
    //pipe.basePipelineHandle;
    //pipe.basePipelineIndex;

    VK_CHECK(vkCreateGraphicsPipelines(g_ctx.device, VK_NULL_HANDLE, 1, &pipe, nullptr, &impl.pipeline));
    
    // Store into global map
    static GraphicsHandle g_counter = 0;
    GraphicsHandle handle = g_counter++;
    g_pipelines.Insert(handle, impl);
    return handle;
}

static void DestroyPipeline(GraphicsHandle pipeline)
{
    const auto& impl = g_pipelines[pipeline];

    for (auto sampler : impl.samplers)
        vkDestroySampler(g_ctx.device, sampler, g_ctx.allocator);

    vkDestroyDescriptorSetLayout(g_ctx.device, impl.descriptorSetLayout, g_ctx.allocator);
    vkDestroyPipelineLayout(g_ctx.device, impl.pipelineLayout, g_ctx.allocator);
    vkDestroyPipeline(g_ctx.device, impl.pipeline, g_ctx.allocator);
}

void Graphics::ClearRenderTarget(const GraphicsHandle rt, const f32 clearColor[4])
{
    auto& frame = g_ctx.frames[g_ctx.currentFrame];

    // Transition swap chain image to transfer destination layout
    VkImageMemoryBarrier barrier{ VK_STRUCTURE_TYPE_IMAGE_MEMORY_BARRIER };
    barrier.srcAccessMask = 0; // No source access needed
    barrier.dstAccessMask = VK_ACCESS_TRANSFER_WRITE_BIT;
    barrier.oldLayout = VK_IMAGE_LAYOUT_PRESENT_SRC_KHR; // Previous layout
    barrier.newLayout = VK_IMAGE_LAYOUT_TRANSFER_DST_OPTIMAL; // New layout
    barrier.srcQueueFamilyIndex = VK_QUEUE_FAMILY_IGNORED;
    barrier.dstQueueFamilyIndex = VK_QUEUE_FAMILY_IGNORED;
    barrier.image = frame.image;
    barrier.subresourceRange.aspectMask = VK_IMAGE_ASPECT_COLOR_BIT;
    barrier.subresourceRange.baseMipLevel = 0;
    barrier.subresourceRange.levelCount = 1;
    barrier.subresourceRange.baseArrayLayer = 0;
    barrier.subresourceRange.layerCount = 1;

    vkCmdPipelineBarrier(frame.commandBuffer,
        VK_PIPELINE_STAGE_TOP_OF_PIPE_BIT, VK_PIPELINE_STAGE_TRANSFER_BIT,
        0, 0, nullptr, 0, nullptr, 1, &barrier);

    // Clear the swap chain image
    VkClearColorValue clearColorValue = { clearColor[0], clearColor[1], clearColor[2], clearColor[3] };
    VkImageSubresourceRange colorRange = { VK_IMAGE_ASPECT_COLOR_BIT, 0, 1, 0, 1 };
    vkCmdClearColorImage(frame.commandBuffer, frame.image, VK_IMAGE_LAYOUT_TRANSFER_DST_OPTIMAL, &clearColorValue, 1, &colorRange);

    // Transition swap chain image back to present source layout
    barrier.srcAccessMask = VK_ACCESS_TRANSFER_WRITE_BIT;
    barrier.dstAccessMask = 0;
    barrier.oldLayout = VK_IMAGE_LAYOUT_TRANSFER_DST_OPTIMAL;
    barrier.newLayout = VK_IMAGE_LAYOUT_PRESENT_SRC_KHR;

    vkCmdPipelineBarrier(frame.commandBuffer,
        VK_PIPELINE_STAGE_TRANSFER_BIT, VK_PIPELINE_STAGE_BOTTOM_OF_PIPE_BIT,
        0, 0, nullptr, 0, nullptr, 1, &barrier);
}

void Graphics::ClearDepthStencil(const GraphicsHandle dt, ClearFlags flags, f32 depth, i32 stencil)
{
}

GraphicsHandle Graphics::CreateBuffer(const BufferInfo& info)
{
    return INVALID_GRAPHICS_HANDLE;
}

GraphicsHandle Graphics::CreateBuffer(const BufferInfo& info, const BufferData& data)
{
    return INVALID_GRAPHICS_HANDLE;
}

void Graphics::UpdateBuffer(const GraphicsHandle buffer, const BufferData& data)
{
}

static u32 GetMemoryType(VkMemoryPropertyFlags properties, u32 typeBits)
{
    VkPhysicalDeviceMemoryProperties prop;
    vkGetPhysicalDeviceMemoryProperties(g_ctx.physicalDevice, &prop);
    for (uint32_t i = 0; i < prop.memoryTypeCount; i++)
        if ((prop.memoryTypes[i].propertyFlags & properties) == properties && typeBits & (1 << i))
            return i;
    return 0xFFFFFFFF; // Unable to find memoryType
}

// TODO: Unsure if this is binding the texture to the pipeline, the code was taken from imgui implementation
static void BindTexture(const PipelineImpl& pipelineImpl, const TextureImpl& textureImpl)
{
    // Create Descriptor Set:
    VkDescriptorSet descriptorSet;
    {
        VkDescriptorSetAllocateInfo allocInfo = {};
        allocInfo.sType = VK_STRUCTURE_TYPE_DESCRIPTOR_SET_ALLOCATE_INFO;
        allocInfo.descriptorPool = g_ctx.descriptorPool;
        allocInfo.descriptorSetCount = 1;
        allocInfo.pSetLayouts = &pipelineImpl.descriptorSetLayout;
        VK_CHECK(vkAllocateDescriptorSets(g_ctx.device, &allocInfo, &descriptorSet));
    }

    // Update the Descriptor Set:
    {
        VkDescriptorImageInfo descImage{};
        descImage.sampler = pipelineImpl.samplers[0]; // TODO: Access by index?
        descImage.imageView = textureImpl.view;
        descImage.imageLayout = VK_IMAGE_LAYOUT_SHADER_READ_ONLY_OPTIMAL;
        VkWriteDescriptorSet writeDesc{ VK_STRUCTURE_TYPE_WRITE_DESCRIPTOR_SET };
        writeDesc.dstSet = descriptorSet;
        writeDesc.descriptorCount = 1;
        writeDesc.descriptorType = VK_DESCRIPTOR_TYPE_COMBINED_IMAGE_SAMPLER;
        writeDesc.pImageInfo = &descImage;
        vkUpdateDescriptorSets(g_ctx.device, 1, &writeDesc, 0, nullptr);
    }
}

GraphicsHandle Graphics::CreateTexture(const TextureInfo& info, const BufferData& data)
{
    TextureImpl impl;

    //unsigned char* pixels;
    //int width, height;
    //io.Fonts->GetTexDataAsRGBA32(&pixels, &width, &height);
    //size_t upload_size = width * height * 4 * sizeof(char);

    // Create the Image:
    {
        VkImageCreateInfo imgInfo{ VK_STRUCTURE_TYPE_IMAGE_CREATE_INFO };
        imgInfo.imageType = VK_IMAGE_TYPE_2D;
        imgInfo.format = VK_FORMAT_R8G8B8A8_UNORM;
        imgInfo.extent.width = info.width;
        imgInfo.extent.height = info.height;
        imgInfo.extent.depth = 1;
        imgInfo.mipLevels = 1;
        imgInfo.arrayLayers = 1;
        imgInfo.samples = VK_SAMPLE_COUNT_1_BIT;
        imgInfo.tiling = VK_IMAGE_TILING_OPTIMAL;
        imgInfo.usage = VK_IMAGE_USAGE_SAMPLED_BIT | VK_IMAGE_USAGE_TRANSFER_DST_BIT;
        imgInfo.sharingMode = VK_SHARING_MODE_EXCLUSIVE;
        imgInfo.initialLayout = VK_IMAGE_LAYOUT_UNDEFINED;
        VK_CHECK(vkCreateImage(g_ctx.device, &imgInfo, g_ctx.allocator, &impl.image));
        VkMemoryRequirements req;
        vkGetImageMemoryRequirements(g_ctx.device, impl.image, &req);
        VkMemoryAllocateInfo allocInfo{ VK_STRUCTURE_TYPE_MEMORY_ALLOCATE_INFO };
        allocInfo.allocationSize = req.size;
        allocInfo.memoryTypeIndex = GetMemoryType(VK_MEMORY_PROPERTY_DEVICE_LOCAL_BIT, req.memoryTypeBits);
        VK_CHECK(vkAllocateMemory(g_ctx.device, &allocInfo, g_ctx.allocator, &impl.memory));
        VK_CHECK(vkBindImageMemory(g_ctx.device, impl.image, impl.memory, 0));
    }

    // Create the Image View:
    {
        VkImageViewCreateInfo viewInfo{ VK_STRUCTURE_TYPE_IMAGE_VIEW_CREATE_INFO };
        viewInfo.image = impl.image;
        viewInfo.viewType = VK_IMAGE_VIEW_TYPE_2D;
        viewInfo.format = VK_FORMAT_R8G8B8A8_UNORM;
        viewInfo.subresourceRange.aspectMask = VK_IMAGE_ASPECT_COLOR_BIT;
        viewInfo.subresourceRange.levelCount = 1;
        viewInfo.subresourceRange.layerCount = 1;
        VK_CHECK(vkCreateImageView(g_ctx.device, &viewInfo, g_ctx.allocator, &impl.view));
    }

    //BindTexture(pipeline, texture); // ????
    
    // Create the Upload Buffer:
    /*{
        VkBufferCreateInfo bufferInfo{ VK_STRUCTURE_TYPE_BUFFER_CREATE_INFO };
        bufferInfo.size = data.dataSize;
        bufferInfo.usage = VK_BUFFER_USAGE_TRANSFER_SRC_BIT;
        bufferInfo.sharingMode = VK_SHARING_MODE_EXCLUSIVE;
        VK_CHECK(vkCreateBuffer(g_ctx.device, &bufferInfo, g_ctx.allocator, &g_ctx.uploadBuffer));

        VkMemoryRequirements req;
        vkGetBufferMemoryRequirements(g_ctx.device, g_ctx.uploadBuffer, &req);
        g_ctx.bufferMemoryAlignment = (g_ctx.bufferMemoryAlignment > req.alignment) ? g_ctx.bufferMemoryAlignment : req.alignment;
        
        VkMemoryAllocateInfo allocInfo{ VK_STRUCTURE_TYPE_MEMORY_ALLOCATE_INFO };
        allocInfo.allocationSize = req.size;
        allocInfo.memoryTypeIndex = GetMemoryType(VK_MEMORY_PROPERTY_HOST_VISIBLE_BIT, req.memoryTypeBits);
        VK_CHECK(vkAllocateMemory(g_ctx.device, &allocInfo, g_ctx.allocator, &g_ctx.uploadBufferMemory));
        VK_CHECK(vkBindBufferMemory(g_ctx.device, g_ctx.uploadBuffer, g_ctx.uploadBufferMemory, 0));
    }

    // Upload to Buffer:
    {
        char* map = nullptr;
        VK_CHECK(vkMapMemory(g_ctx.device, g_ctx.uploadBufferMemory, 0, data.dataSize, 0, (void**)(&map)));
        memcpy(map, data.pData, data.dataSize);
        VkMappedMemoryRange range[1] = {};
        range[0].sType = VK_STRUCTURE_TYPE_MAPPED_MEMORY_RANGE;
        range[0].memory = g_ctx.uploadBufferMemory;
        range[0].size = data.dataSize;
        VK_CHECK(vkFlushMappedMemoryRanges(g_ctx.device, 1, range));
        vkUnmapMemory(g_ctx.device, g_ctx.uploadBufferMemory);
    }

    auto commandBuffer = BeginCommand();

    // Copy to Image:
    {
        VkImageMemoryBarrier copy_barrier[1] = {};
        copy_barrier[0].sType = VK_STRUCTURE_TYPE_IMAGE_MEMORY_BARRIER;
        copy_barrier[0].dstAccessMask = VK_ACCESS_TRANSFER_WRITE_BIT;
        copy_barrier[0].oldLayout = VK_IMAGE_LAYOUT_UNDEFINED;
        copy_barrier[0].newLayout = VK_IMAGE_LAYOUT_TRANSFER_DST_OPTIMAL;
        copy_barrier[0].srcQueueFamilyIndex = VK_QUEUE_FAMILY_IGNORED;
        copy_barrier[0].dstQueueFamilyIndex = VK_QUEUE_FAMILY_IGNORED;
        copy_barrier[0].image = impl.image;
        copy_barrier[0].subresourceRange.aspectMask = VK_IMAGE_ASPECT_COLOR_BIT;
        copy_barrier[0].subresourceRange.levelCount = 1;
        copy_barrier[0].subresourceRange.layerCount = 1;
        vkCmdPipelineBarrier(commandBuffer, VK_PIPELINE_STAGE_HOST_BIT, VK_PIPELINE_STAGE_TRANSFER_BIT, 0, 0, nullptr, 0, nullptr, 1, copy_barrier);

        VkBufferImageCopy region = {};
        region.imageSubresource.aspectMask = VK_IMAGE_ASPECT_COLOR_BIT;
        region.imageSubresource.layerCount = 1;
        region.imageExtent.width = info.width;
        region.imageExtent.height = info.height;
        region.imageExtent.depth = 1;
        vkCmdCopyBufferToImage(commandBuffer, g_ctx.uploadBuffer, impl.image, VK_IMAGE_LAYOUT_TRANSFER_DST_OPTIMAL, 1, &region);

        VkImageMemoryBarrier use_barrier[1] = {};
        use_barrier[0].sType = VK_STRUCTURE_TYPE_IMAGE_MEMORY_BARRIER;
        use_barrier[0].srcAccessMask = VK_ACCESS_TRANSFER_WRITE_BIT;
        use_barrier[0].dstAccessMask = VK_ACCESS_SHADER_READ_BIT;
        use_barrier[0].oldLayout = VK_IMAGE_LAYOUT_TRANSFER_DST_OPTIMAL;
        use_barrier[0].newLayout = VK_IMAGE_LAYOUT_SHADER_READ_ONLY_OPTIMAL;
        use_barrier[0].srcQueueFamilyIndex = VK_QUEUE_FAMILY_IGNORED;
        use_barrier[0].dstQueueFamilyIndex = VK_QUEUE_FAMILY_IGNORED;
        use_barrier[0].image = impl.image;
        use_barrier[0].subresourceRange.aspectMask = VK_IMAGE_ASPECT_COLOR_BIT;
        use_barrier[0].subresourceRange.levelCount = 1;
        use_barrier[0].subresourceRange.layerCount = 1;
        vkCmdPipelineBarrier(commandBuffer, VK_PIPELINE_STAGE_TRANSFER_BIT, VK_PIPELINE_STAGE_FRAGMENT_SHADER_BIT, 0, 0, nullptr, 0, nullptr, 1, use_barrier);
    }

    EndCommand();*/
    
    // Store into global map
    static GraphicsHandle g_counter = 0;
    GraphicsHandle handle = g_counter++;
    g_textures.Insert(handle, impl);
    return handle;
}

static void DestroyTexture(GraphicsHandle texture)
{
    const TextureImpl& impl = g_textures[texture];
    vkDestroyImage(g_ctx.device, impl.image, g_ctx.allocator);
    //vkDeallocateMemory(impl.memory);
    vkDestroyImageView(g_ctx.device, impl.view, g_ctx.allocator);
    //vkDestroyDescriptorSetLayout(g_ctx.device, impl.descriptorSet, g_ctx.allocator);
}

void Graphics::SetStaticVariable(const GraphicsHandle pipeline, ShaderType shaderType, const char* name, const GraphicsHandle buffer)
{
}

void Graphics::SetPipeline(const GraphicsHandle pipeline)
{
    auto& frame = g_ctx.frames[g_ctx.currentFrame];

    if (g_ctx.bHasActiveRenderPass)
    {
        vkCmdEndRenderPass(frame.commandBuffer);
    }

    // Begin the render pass.
    VkRenderPassBeginInfo rpBeginInfo{ VK_STRUCTURE_TYPE_RENDER_PASS_BEGIN_INFO };
    rpBeginInfo.renderPass = g_ctx.renderPass;
    rpBeginInfo.framebuffer = frame.framebuffer;
    rpBeginInfo.renderArea.extent = g_ctx.extent;

    VkClearValue clearValues[2]{};
    clearValues[0] = { 0.0f, 0.0f, 0.0f, 1.0f };
    clearValues[1] = { 1.0f, 0 };
    rpBeginInfo.clearValueCount = 2;
    rpBeginInfo.pClearValues = clearValues;
    
    // We will add draw commands in the same command buffer.
    vkCmdBeginRenderPass(frame.commandBuffer, &rpBeginInfo, VK_SUBPASS_CONTENTS_INLINE);

    // Bind the graphics pipeline.
    auto& impl = g_pipelines[pipeline];
    vkCmdBindPipeline(frame.commandBuffer, VK_PIPELINE_BIND_POINT_GRAPHICS, impl.pipeline);

    g_ctx.bHasActiveRenderPass = true;
}

void Graphics::SetVertexBuffers(i32 i, i32 count, const GraphicsHandle* pBuffers, const u64* offset)
{
}

void Graphics::SetIndexBuffer(const GraphicsHandle buffer, i32 i)
{
}

void Graphics::Draw(const DrawAttribs& attribs)
{
    ENGINE_ASSERT(g_ctx.bHasActiveRenderPass, "Render pass must be active before attempting to draw!");

    auto& frame = g_ctx.frames[g_ctx.currentFrame];

    // Draw three vertices with one instance.
    vkCmdDraw(frame.commandBuffer, 3, 1, 0, 0);
}

void Graphics::DrawIndexed(const DrawIndexedAttribs& attribs)
{
}

#endif // GRAPHICS_VK_BACKEND
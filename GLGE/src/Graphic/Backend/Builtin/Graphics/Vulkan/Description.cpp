/**
 * @file Description.cpp
 * @author DM8AT
 * @brief implement the description for the vulkan backend
 * @version 0.1
 * @date 2026-04-14
 * 
 * @copyright Copyright (c) 2026
 * 
 */
//add the vulkan description
#include "Graphic/Backend/Builtin/Graphics/Vulkan/Description.h"

//add instances
#include "Graphic/Backend/Builtin/Graphics/Vulkan/Instance.h"
//add windows
#include "Graphic/Backend/Builtin/Graphics/Vulkan/Window.h"
//add command buffers
#include "Graphic/Backend/Builtin/Graphics/Vulkan/CommandBuffer.h"
//add images
#include "Graphic/Backend/Builtin/Graphics/Vulkan/Image.h"
//add textures
#include "Graphic/Backend/Builtin/Graphics/Vulkan/Texture.h"
//add shaders
#include "Graphic/Backend/Builtin/Graphics/Vulkan/Shader.h"
//add resource sets
#include "Graphic/Backend/Builtin/Graphics/Vulkan/ResourceSet.h"
//add framebuffers
#include "Graphic/Backend/Builtin/Graphics/Vulkan/Framebuffer.h"
//add buffers
#include "Graphic/Backend/Builtin/Graphics/Vulkan/Buffer.h"
//add mesh pools
#include "Graphic/Backend/Builtin/Graphics/Vulkan/MeshPool.h"

//add device evaluation
#include "DeviceEvaluation.h"

//use the builtin namespace
using namespace GLGE::Graphic::Builtin::Graphics;

//add vulkan
#include <vulkan/vulkan.h>

//add translation functions
#include "Translator.h"

Vulkan::Vulkan() 
 : Description(GLGE::Graphic::Backend::Graphic::CommandTable({
        std::pair{GLGE::Graphic::COMMAND_CLEAR, GLGE::Graphic::Backend::Graphic::CommandTable::TableEntry::create<GLGE::Graphic::RenderTarget, GLGE::u8, GLGE::vec4, GLGE::f32, GLGE::u32>(VkImpl::clear)},
        std::pair{GLGE::Graphic::COMMAND_COPY, GLGE::Graphic::Backend::Graphic::CommandTable::TableEntry::create<GLGE::Graphic::RenderTarget, GLGE::u8, GLGE::Graphic::RenderTarget, GLGE::u8, bool, bool>(VkImpl::copy)},
        std::pair{GLGE::Graphic::COMMAND_DISPATCH_COMPUTE, GLGE::Graphic::Backend::Graphic::CommandTable::TableEntry::create<GLGE::Graphic::Shader*, GLGE::uvec3>(VkImpl::dispatchCompute)}
    }))
{initialize();}

const char* Vulkan::getName() const noexcept
{return "GLGE_GRAPHIC_VULKAN_BUILTIN";}

GLGE::Version Vulkan::getVersion() const noexcept
{return GLGE::Version(0,1,0);}

GLGE::Reference<GLGE::Graphic::Backend::Graphic::Instance> Vulkan::createInstance(GLGE::Graphic::Instance* instance)
{return GLGE::Reference<GLGE::Graphic::Backend::Graphic::Instance>(new GLGE::Graphic::Backend::Graphic::Vulkan::Instance(instance), false);}

GLGE::Reference<GLGE::Graphic::Backend::Graphic::Window> Vulkan::createWindow(GLGE::Graphic::Window* window)
{return GLGE::Reference<GLGE::Graphic::Backend::Graphic::Window>(new GLGE::Graphic::Backend::Graphic::Vulkan::Window(window), false);}

GLGE::Reference<GLGE::Graphic::Backend::Graphic::CommandBuffer> Vulkan::createCommandBuffer([[maybe_unused]]GLGE::Graphic::RenderPipeline* renderPipeline)
{return GLGE::Reference<GLGE::Graphic::Backend::Graphic::CommandBuffer>(new GLGE::Graphic::Backend::Graphic::Vulkan::CommandBuffer(renderPipeline), false);}

GLGE::Reference<GLGE::Graphic::Backend::Graphic::Sampler> Vulkan::createSampler([[maybe_unused]] const GLGE::Graphic::SamplerCPU& sampler, [[maybe_unused]] GLGE::Graphic::Backend::Graphic::Instance* instance)
{return GLGE::Reference<GLGE::Graphic::Backend::Graphic::Sampler>(nullptr, false);}

GLGE::Reference<GLGE::Graphic::Backend::Graphic::Texture> Vulkan::createTexture([[maybe_unused]] const GLGE::uvec2& size, [[maybe_unused]] GLGE::u16 mipCount, [[maybe_unused]] GLGE::Graphic::PixelFormat format, GLGE::Graphic::Backend::Graphic::Instance* instance)
{return GLGE::Reference<GLGE::Graphic::Backend::Graphic::Texture>(new GLGE::Graphic::Backend::Graphic::Vulkan::Texture(size, mipCount, format, instance), false);}

GLGE::Reference<GLGE::Graphic::Backend::Graphic::Image> Vulkan::createImage([[maybe_unused]] const GLGE::uvec2& size, [[maybe_unused]] GLGE::Graphic::PixelFormat format, [[maybe_unused]] GLGE::u8 samples, GLGE::Graphic::Backend::Graphic::Instance* instance)
{return GLGE::Reference<GLGE::Graphic::Backend::Graphic::Image>(new GLGE::Graphic::Backend::Graphic::Vulkan::Image(size, format, samples, instance), false);}

GLGE::Reference<GLGE::Graphic::Backend::Graphic::Framebuffer> Vulkan::createFramebuffer([[maybe_unused]] u8 colorAttachmentCount, [[maybe_unused]] Reference<GLGE::Graphic::Backend::Graphic::Image> const* colorAttachments, 
                                                                                        [[maybe_unused]] u8 depthAttachmentCount, [[maybe_unused]] Reference<GLGE::Graphic::Backend::Graphic::Image> const* depthAttachments, 
                                                                                        [[maybe_unused]] u8 stencilAttachmentCount, [[maybe_unused]] Reference<GLGE::Graphic::Backend::Graphic::Image> const* stencilAttachments, 
                                                                                        [[maybe_unused]] bool combinedDepthStencil, GLGE::Graphic::Backend::Graphic::Instance* instance)
{return GLGE::Reference<GLGE::Graphic::Backend::Graphic::Framebuffer>(new GLGE::Graphic::Backend::Graphic::Vulkan::Framebuffer(colorAttachmentCount, colorAttachments, depthAttachmentCount, depthAttachments, stencilAttachmentCount, stencilAttachments, combinedDepthStencil, instance), false);}

GLGE::Reference<GLGE::Graphic::Backend::Graphic::ResourceSet> Vulkan::createResourceSet([[maybe_unused]]GLGE::Graphic::ResourceSet* set)
{return GLGE::Reference<GLGE::Graphic::Backend::Graphic::ResourceSet>(new GLGE::Graphic::Backend::Graphic::Vulkan::ResourceSet(set), false);}

GLGE::Reference<GLGE::Graphic::Backend::Graphic::Shader> Vulkan::createShader([[maybe_unused]] GLGE::Graphic::Shader* frontend)
{return GLGE::Reference<GLGE::Graphic::Backend::Graphic::Shader>(new GLGE::Graphic::Backend::Graphic::Vulkan::Shader(frontend), false);}

GLGE::Reference<GLGE::Graphic::Backend::Graphic::Buffer> Vulkan::createBuffer([[maybe_unused]] GLGE::Graphic::Backend::Graphic::Buffer::Type type, [[maybe_unused]] const void* initial, [[maybe_unused]] size_t size, [[maybe_unused]] GLGE::Graphic::Backend::Graphic::Buffer::Usage usage, GLGE::Graphic::Backend::Graphic::Instance* instance)
{return GLGE::Reference<GLGE::Graphic::Backend::Graphic::Buffer>(new GLGE::Graphic::Backend::Graphic::Vulkan::Buffer(type, initial, size, instance, usage), false);}

GLGE::Reference<GLGE::Graphic::Backend::Graphic::SampledTexture> Vulkan::createSampledTexture([[maybe_unused]] const Reference<GLGE::Graphic::Backend::Graphic::Texture>& texture, [[maybe_unused]] const Reference<GLGE::Graphic::Backend::Graphic::Sampler>& sampler)
{return GLGE::Reference<GLGE::Graphic::Backend::Graphic::SampledTexture>(nullptr, false);}

GLGE::Reference<GLGE::Graphic::Backend::Graphic::SampledTexture> Vulkan::createSampledTexture([[maybe_unused]] const Reference<GLGE::Graphic::Backend::Graphic::Image>& image, [[maybe_unused]] const Reference<GLGE::Graphic::Backend::Graphic::Sampler>& sampler)
{return GLGE::Reference<GLGE::Graphic::Backend::Graphic::SampledTexture>(nullptr, false);}

GLGE::Reference<GLGE::Graphic::Backend::Graphic::MeshPool> Vulkan::createMeshPool(GLGE::Graphic::Instance* instance) 
{return GLGE::Reference<GLGE::Graphic::Backend::Graphic::MeshPool>(new GLGE::Graphic::Backend::Graphic::Vulkan::MeshPool(instance), false);}

GLGE::Reference<GLGE::Graphic::Backend::Graphic::VertexLayout> Vulkan::createVertexLayout(const VertexAttribute* ptr, size_t size, size_t stride, Reference<GLGE::Graphic::Backend::Graphic::MeshPool> pool)
{return GLGE::Reference<GLGE::Graphic::Backend::Graphic::VertexLayout>(nullptr, false);}

GLGE::Reference<GLGE::Graphic::Backend::Graphic::Material> Vulkan::createMaterial(Reference<GLGE::Graphic::Backend::Graphic::Shader> shader, Reference<GLGE::Graphic::Backend::Graphic::VertexLayout> layout, GLGE::Graphic::Backend::Graphic::Material::CullMode cullMode, GLGE::Graphic::Backend::Graphic::Material::DepthMode depthMode, bool depthWrite)
{return GLGE::Reference<GLGE::Graphic::Backend::Graphic::Material>(nullptr, false);}

GLGE::Reference<GLGE::Graphic::Backend::Graphic::Renderer> Vulkan::createRenderer(World& world, Object* camera, RenderTarget target)
{return GLGE::Reference<GLGE::Graphic::Backend::Graphic::Renderer>(nullptr, false);}

GLGE::Graphic::GraphicAPI Vulkan::getAPI() const noexcept
{return GLGE::Graphic::GraphicAPI::VULKAN;}

GLGE::Version Vulkan::getAPIVersion() const noexcept
{return GLGE::Version(1, 0, 0);}

/**
 * @brief get the vendor name from a vendor ID
 * 
 * @param vendorID the vendor ID to quarry
 * @return `const char*` the name of the vendor
 */
static const char* __vendor_name(uint32_t vendorID) {
    //just use a switch statement
    switch (vendorID) {
        case 0x10DE: return "NVIDIA Corporation";
        case 0x1002: return "AMD";
        case 0x8086: return "Intel";
        case 0x17CB: return "Microsoft Corporation";
        default:     return "Unknown";
    }
}

void Vulkan::initialize() {
    //create a temporary vulkan instance
    VkApplicationInfo appInfo {
        .sType = VK_STRUCTURE_TYPE_APPLICATION_INFO,
        .pNext = nullptr,
        .pApplicationName = "TEMP_INSTANCE",
        .applicationVersion = VK_MAKE_VERSION(1,0,0),
        .pEngineName = "GLGE_VK_DEFAULT",
        .engineVersion = VK_MAKE_VERSION(0,1,0),
        .apiVersion = VK_API_VERSION_1_0
    };

    //create the instance
    const char* extensions[] = {VK_KHR_GET_PHYSICAL_DEVICE_PROPERTIES_2_EXTENSION_NAME};
    VkInstanceCreateInfo instCreate {
        .sType = VK_STRUCTURE_TYPE_INSTANCE_CREATE_INFO,
        .pNext = nullptr,
        .flags = 0,
        .pApplicationInfo = &appInfo,
        .enabledLayerCount = 0,
        .ppEnabledLayerNames = nullptr,
        .enabledExtensionCount = sizeof(extensions)/sizeof(*extensions),
        .ppEnabledExtensionNames = extensions
    };
    VkInstance instance = VK_NULL_HANDLE;
    if (vkCreateInstance(&instCreate, nullptr, &instance) != VK_SUCCESS) 
    {throw Exception("Failed to create dummy vulkan instance: Vulkan is likely not available or not available enough", "Vulkan::initialize");}

    //get the GPUs
    u32 gpuCount = 0;
    vkEnumeratePhysicalDevices(instance, &gpuCount, nullptr);
    if (gpuCount == 0) {throw Exception("No GPU that can run vulkan was found", "Vulkan::initialize");}
    std::vector<VkPhysicalDevice> gpus(gpuCount);
    vkEnumeratePhysicalDevices(instance, &gpuCount, gpus.data());

    //get the best GPU
    VkPhysicalDevice best = __pick_strongest_device(gpus, instance);

    //load the getter function
    PFN_vkGetPhysicalDeviceProperties2 getProps2 =(PFN_vkGetPhysicalDeviceProperties2)vkGetInstanceProcAddr(instance, "vkGetPhysicalDeviceProperties2");
    if (!getProps2) 
    {getProps2 = (PFN_vkGetPhysicalDeviceProperties2)vkGetInstanceProcAddr(instance, "vkGetPhysicalDeviceProperties2KHR");}
    if (getProps2) {
        //get extended information
        VkPhysicalDeviceProperties2 props2{};
        props2.sType = VK_STRUCTURE_TYPE_PHYSICAL_DEVICE_PROPERTIES_2;
        VkPhysicalDeviceDriverProperties driverProps{};
        driverProps.sType = VK_STRUCTURE_TYPE_PHYSICAL_DEVICE_DRIVER_PROPERTIES;
        props2.pNext = &driverProps;

        //get and store the data
        getProps2(best, &props2);
        m_driverVersion = std::to_string(VK_API_VERSION_MAJOR(props2.properties.apiVersion)) + "." + std::to_string(VK_API_VERSION_MINOR(props2.properties.apiVersion)) + "." + 
                        std::to_string(VK_API_VERSION_PATCH(props2.properties.apiVersion)) + " " + ((driverProps.driverName[0] == 0) ? "NO_NAME" : driverProps.driverName) + " " + 
                        ((driverProps.driverInfo[0] == 0) ? "NO_INFO" : driverProps.driverInfo);
        m_gpuName = props2.properties.deviceName;
        m_vendorName = __vendor_name(props2.properties.vendorID);
    } else {
        //fallback

        //get information about the GPU
        VkPhysicalDeviceProperties props;
        vkGetPhysicalDeviceProperties(best, &props);

        //store information about the GPU (store default driver version string)
        m_gpuName = props.deviceName;
        m_vendorName = __vendor_name(props.vendorID);
        m_driverVersion = std::to_string(VK_API_VERSION_MAJOR(props.apiVersion)) + "." + std::to_string(VK_API_VERSION_MINOR(props.apiVersion)) + "." + 
                        std::to_string(VK_API_VERSION_PATCH(props.apiVersion)) + " NO_NAME NO_INFO";
    }

    //clean up
    vkDestroyInstance(instance, nullptr);
}
/**
 * @file Instance.cpp
 * @author DM8AT
 * @brief implement the vulkan instance
 * @version 0.1
 * @date 2026-04-14
 * 
 * @copyright Copyright (c) 2026
 * 
 */
//include the instance
#include "Graphic/Backend/Builtin/Graphics/Vulkan/Instance.h"
//add graphic instances
#include "Graphic/Instance.h"

//add vulkan
#include "vulkan/vulkan.h"

//add the vulkan contract
#include "Graphic/Backend/Video/APIContracts/Vulkan.h"

//add device evaluation
#include "DeviceEvaluation.h"

//for bit counting
#include <bit>
//add normal maps for maps with iterators
#include <map>

//implement vma
#define VMA_IMPLEMENTATION
#include "vk_mem_alloc.h"

//use the vulkan backend namespace
using namespace GLGE::Graphic::Backend::Graphic::Vulkan;

/**
 * @brief select the queue from the family set that supports the required bits, but the least extra bits
 * 
 * @param families the families to select from
 * @param required the required bits
 * @return `GLGE::u32` the index of the best fitting queue or `UINT32_MAX` if no queue supports the selected bit
 */
static GLGE::u32 __selectMinimalFit(const std::vector<VkQueueFamilyProperties>& families, VkQueueFlags required) {
    //store information about the best fitting family
    GLGE::u32 bestFit = UINT32_MAX;
    GLGE::u32 bestExtraBits = UINT32_MAX;
    //search the best fitting family
    for (GLGE::u32 i = 0; i < families.size(); ++i) {
        //alias
        const auto& family = families[i];
        //check if the queue even fits
        if (family.queueFlags & required) {
            //flag is supported

            //get the amount of extra bits
            GLGE::u32 extra = std::popcount<GLGE::u32>(family.queueFlags & ~required);

            //if the amount of extra bits is less than the current best, select this
            if (extra < bestExtraBits) {
                bestFit = i;
                bestExtraBits = extra;
            }
        }
    }

    //return the best fit
    return bestFit;
}

Instance::Instance(GLGE::Graphic::Instance* instance)
 : GLGE::Graphic::Backend::Graphic::Instance(instance)
{
    //create the application information
    VkApplicationInfo appInfo {};
    appInfo.sType = VK_STRUCTURE_TYPE_APPLICATION_INFO;
    //dynamically pull the requested application version
    auto version = instance->getGraphicDescription()->getAPIVersion();
    appInfo.apiVersion = VK_MAKE_API_VERSION(0, version.getMajor(), version.getMinor(), version.getPatch());
    //use the instance's name as the app name
    appInfo.pApplicationName = instance->getInstance()->getName().data();
    //use the instance's version as the app version
    appInfo.applicationVersion = VK_MAKE_VERSION(instance->getInstance()->getVersion().getMajor(), instance->getInstance()->getVersion().getMinor(), instance->getInstance()->getVersion().getPatch());
    //the engine is the description's name
    appInfo.pEngineName = instance->getGraphicBackendName();
    //the engine version is pulled from the description too
    appInfo.engineVersion = VK_MAKE_VERSION(instance->getGraphicBackendVersion().getMajor(), instance->getGraphicBackendVersion().getMinor(), instance->getGraphicBackendVersion().getPatch());

    //get the required instance extensions
    std::vector<const char*> instExt;
    instance->getVideoBackendInstance()->getContract<GLGE::Graphic::Backend::Video::Contracts::Vulkan>()->getRequiredInstanceExtensions(instExt);
    instExt.push_back(VK_KHR_GET_PHYSICAL_DEVICE_PROPERTIES_2_EXTENSION_NAME);

    //get all available instance extensions
    u32 instExtCount = 0;
    vkEnumerateInstanceExtensionProperties(nullptr, &instExtCount, nullptr);
    std::vector<VkExtensionProperties> availableInstExt(instExtCount);
    vkEnumerateInstanceExtensionProperties(nullptr, &instExtCount, availableInstExt.data());

    //check that all extensions are available
    u32 instExtFoundCount = 0;
    for (const auto& ext : instExt) {
        for (const auto& availableExt : availableInstExt) {
            if (strcmp(ext, availableExt.extensionName) == 0) {
                //extension was found -> increment the found counter and stop this loop
                ++instExtFoundCount;
                break;
            }
        }
    }

    //if not all extensions where found, throw an error
    if (instExtFoundCount != instExt.size()) {
        //store the exception message stream
        std::stringstream stream;
        stream << "\nFailed to find all required instance extensions. The following extensions were requested, but not supported:\n";
        
        for (const auto& ext : instExt) {
            bool found = false;
            for (const auto& availableExt : availableInstExt) {
                if (strcmp(ext, availableExt.extensionName) == 0) {
                    //extension was found -> it cannot be missing, so stop
                    found = true;
                    break;
                }
            }

            if (!found) 
            {stream << "    - " << ext << "\n";}
        }

        //throw the exception
        throw Exception(stream.str(), "GLGE::Graphic::Backend::Graphic::Vulkan::Instance");
    }

    //get the required instance extensions
    std::vector<const char*> instLayers;

    //get all available device extensions
    u32 instLayerCount = 0;
    vkEnumerateInstanceLayerProperties(&instLayerCount, nullptr);
    std::vector<VkLayerProperties> availableInstLayer(instLayerCount);
    vkEnumerateInstanceLayerProperties(&instLayerCount, availableInstLayer.data());

    //store if the vulkan validation layer exists
    const char *const validationLayerName = "VK_LAYER_KHRONOS_validation";
    bool hasValidation = false;

    //check if the validation layer exists
    for (const auto& availableLayer : availableInstLayer) {
        //check for the validation layer
        if (strcmp(availableLayer.layerName, validationLayerName) == 0)
        {hasValidation = true;}
    }

    //in debug add the validation layer if it exists
    #if GLGE_DEBUG
    if (hasValidation)
    {instLayers.push_back(validationLayerName);}
    #endif

    //check that all layers are available
    u32 instLayerFoundCount = 0;
    for (const auto& layer : instLayers) {
        for (const auto& availableLayer : availableInstLayer) {
            if (strcmp(layer, availableLayer.layerName) == 0) {
                //layer was found -> increment the found counter and stop this loop
                ++instLayerFoundCount;
                break;
            }
        }
    }

    //if not all layers where found, throw an error
    if (instLayerFoundCount != instLayers.size()) {
        //store the exception message stream
        std::stringstream stream;
        stream << "\nFailed to find all required instance layers. The following layers were requested, but not supported:\n";
        
        for (const auto& layer : instLayers) {
            bool found = false;
            for (const auto& availableLayer : availableInstLayer) {
                if (strcmp(layer, availableLayer.layerName) == 0) {
                    //layer was found -> it cannot be missing, so stop
                    found = true;
                    break;
                }
            }

            if (!found) 
            {stream << "    - " << layer << "\n";}
        }

        //throw the exception
        throw Exception(stream.str(), "GLGE::Graphic::Backend::Graphic::Vulkan::Instance");
    }

    //fill out the instance create info
    VkInstanceCreateInfo instCreate {};
    instCreate.sType = VK_STRUCTURE_TYPE_INSTANCE_CREATE_INFO;
    instCreate.pApplicationInfo = &appInfo;
    instCreate.enabledExtensionCount = instExt.size();
    instCreate.ppEnabledExtensionNames = instExt.data();
    instCreate.enabledLayerCount = instLayers.size();
    instCreate.ppEnabledLayerNames = instLayers.data();

    //create the instance
    if (vkCreateInstance(&instCreate, nullptr, reinterpret_cast<VkInstance*>(&m_vkInstance)) != VK_SUCCESS)
    {throw GLGE::Exception("Failed to create the vulkan instance", "GLGE::Graphic::Backend::Graphic::Vulkan::Instance::Instance");}

    //get all available GPUs
    u32 deviceCount = 0;
    vkEnumeratePhysicalDevices(reinterpret_cast<VkInstance>(m_vkInstance), &deviceCount, nullptr);
    std::vector<VkPhysicalDevice> devices(deviceCount);
    vkEnumeratePhysicalDevices(reinterpret_cast<VkInstance>(m_vkInstance), &deviceCount, devices.data());

    //get and store the best GPU
    m_physicalDevice = __pick_strongest_device(devices, reinterpret_cast<VkInstance>(m_vkInstance));

    //store the required device extensions
    std::vector<const char*> devExt = {
        //this extension is required for shader functionality compatibility
        VK_KHR_SHADER_DRAW_PARAMETERS_EXTENSION_NAME,
        //required for descriptor updates
        VK_EXT_DESCRIPTOR_INDEXING_EXTENSION_NAME,
        VK_KHR_MAINTENANCE_3_EXTENSION_NAME
    };
    instance->getVideoBackendInstance()->getContract<GLGE::Graphic::Backend::Video::Contracts::Vulkan>()->getRequiredDeviceExtensions(devExt);

    //get all available device extensions
    u32 devExtCount = 0;
    vkEnumerateDeviceExtensionProperties(reinterpret_cast<VkPhysicalDevice>(m_physicalDevice), nullptr, &devExtCount, nullptr);
    std::vector<VkExtensionProperties> availableDevExt(devExtCount);
    vkEnumerateDeviceExtensionProperties(reinterpret_cast<VkPhysicalDevice>(m_physicalDevice), nullptr, &devExtCount, availableDevExt.data());

    //check that all extensions are available
    u32 devExtFoundCount = 0;
    for (const auto& ext : devExt) {
        for (const auto& availableExt : availableDevExt) {
            if (strcmp(ext, availableExt.extensionName) == 0) {
                //extension was found -> increment the found counter and stop this loop
                ++devExtFoundCount;
                break;
            }
        }
    }

    //if not all extensions where found, throw an error
    if (devExtFoundCount != devExt.size()) {
        //store the exception message stream
        std::stringstream stream;
        stream << "\nFailed to find all required extensions. The following extensions were requested, but not supported:\n";
        
        for (const auto& ext : devExt) {
            bool found = false;
            for (const auto& availableExt : availableDevExt) {
                if (strcmp(ext, availableExt.extensionName) == 0) {
                    //extension was found -> it cannot be missing, so stop
                    found = true;
                    break;
                }
            }

            if (!found) 
            {stream << "    - " << ext << "\n";}
        }

        //throw the exception
        throw Exception(stream.str(), "GLGE::Graphic::Backend::Graphic::Vulkan::Instance");
    }

    //get queue capabilities
    u32 queueFamilyCount = 0;
    vkGetPhysicalDeviceQueueFamilyProperties(reinterpret_cast<VkPhysicalDevice>(m_physicalDevice), &queueFamilyCount, nullptr);
    std::vector<VkQueueFamilyProperties> queueFamilies(queueFamilyCount);
    vkGetPhysicalDeviceQueueFamilyProperties(reinterpret_cast<VkPhysicalDevice>(m_physicalDevice), &queueFamilyCount, queueFamilies.data());

    //get the different queue families
    u32 graphicsFamily = __selectMinimalFit(queueFamilies, VK_QUEUE_GRAPHICS_BIT | VK_QUEUE_COMPUTE_BIT); //the graphics queue must be compute-capable to run inline compute, e.g. for indirect rendering
    u32 transferFamily = __selectMinimalFit(queueFamilies, VK_QUEUE_TRANSFER_BIT);
    u32 computeFamily  = __selectMinimalFit(queueFamilies, VK_QUEUE_COMPUTE_BIT);

    //check if all queues were found
    if (graphicsFamily == UINT32_MAX)
    {throw Exception("Failed to find a graphics queue (How did we get here?)", "GLGE::Graphic::Backend::Graphic::Vulkan::Instance");}
    if (transferFamily == UINT32_MAX) {
        //fallback to the graphics family, since it is required to support transfer implicitly
        transferFamily = graphicsFamily;
    }
    if (computeFamily  == UINT32_MAX)
    {throw Exception("Failed to find a compute queue", "GLGE::Graphic::Backend::Graphic::Vulkan::Instance");}

    //for each queue count up how much it is requested

    /**
     * @brief a structure to store the amount of queues requested for each family
     * 
     * This is done with a structure because it allows me to define a clear default for uint32_t's
     */
    struct RequestCount {u32 count = 0;};

    //store all requests in a map
    //during this process also make sure that all queues actually exist
    std::map<u32, u32> familyFreeMap;
    familyFreeMap[graphicsFamily] = queueFamilies[graphicsFamily].queueCount;
    familyFreeMap[transferFamily] = queueFamilies[transferFamily].queueCount;
    familyFreeMap[computeFamily ] = queueFamilies[computeFamily ].queueCount;
    std::map<u32, RequestCount> familyRequestMap;
    u32 graphicsQueueIdx = familyRequestMap[graphicsFamily].count;
    familyRequestMap[graphicsFamily].count++;
    --familyFreeMap[graphicsFamily];
    u32 transferQueueIdx = familyRequestMap[transferFamily].count;
    //the transfer queue family must have remaining queues
    if (familyFreeMap[transferFamily] == 0)
    {throw Exception("To few queues available - Failed to assign a transfer queue", "GLGE::Graphic::Backend::Graphic::Vulkan::Instance");}
    u32 transferQueueCount = (transferFamily == computeFamily) ? (familyFreeMap[transferFamily] / 2) : familyFreeMap[transferFamily];
    familyRequestMap[transferFamily].count += transferQueueCount;
    familyFreeMap[transferFamily] -= transferQueueCount;
    //the compute queue family must have remaining queues
    if (familyFreeMap[computeFamily] == 0)
    {throw Exception("To few queues available - Failed to assign a compute queue", "GLGE::Graphic::Backend::Graphic::Vulkan::Instance");}
    u32 computeQueueIdx  = familyRequestMap[computeFamily].count;
    u32 computeQueueCount = familyFreeMap[computeFamily];
    familyRequestMap[computeFamily].count += computeQueueCount;
    familyFreeMap[computeFamily] -= computeQueueCount;

    //create a vector of floats for the biggest queue count
    std::vector<float> priorities(glm::max<u32>(1, glm::max<u32>(transferQueueCount, computeQueueCount)), 1.f);

    //fill out queue create infos for each queue
    std::vector<VkDeviceQueueCreateInfo> queueCreates;
    queueCreates.reserve(familyRequestMap.size());
    for (const auto& [family, request] : familyRequestMap) {
        //create the queue create info
        VkDeviceQueueCreateInfo create{};
        create.sType = VK_STRUCTURE_TYPE_DEVICE_QUEUE_CREATE_INFO;
        create.queueFamilyIndex = family;
        create.pQueuePriorities = priorities.data();
        create.queueCount = request.count;

        //queue the queue create info
        queueCreates.push_back(create);
    }

    VkPhysicalDeviceDescriptorIndexingFeatures indexing = {};
    indexing.sType = VK_STRUCTURE_TYPE_PHYSICAL_DEVICE_DESCRIPTOR_INDEXING_FEATURES;
    VkPhysicalDeviceFeatures2 features2 = {};
    features2.sType = VK_STRUCTURE_TYPE_PHYSICAL_DEVICE_FEATURES_2;
    features2.pNext = &indexing;
    vkGetPhysicalDeviceFeatures2(reinterpret_cast<VkPhysicalDevice>(m_physicalDevice), &features2);

    //check if everything required is supported
    if (!features2.features.shaderStorageImageMultisample)
    {throw Exception("The device must support storage image multi-sample", "GLGE::Graphic::Backend::Graphic::Vulkan::Instance");}
    if (!(indexing.descriptorBindingStorageImageUpdateAfterBind && indexing.descriptorBindingSampledImageUpdateAfterBind && indexing.descriptorBindingStorageBufferUpdateAfterBind && indexing.descriptorBindingUniformBufferUpdateAfterBind))
    {throw Exception("The device must support all required update after binding types for descriptor sets", "GLGE::Graphic::Backend::Graphic::Vulkan::Instance");}
    if (!features2.features.multiDrawIndirect)
    {throw Exception("The multi draw indirect feature is required", "GLGE::Graphic::Backend::Graphic::Vulkan::Instance");}
    if (!features2.features.sampleRateShading)
    {throw Exception("The sample rate shading feature is required", "GLGE::Graphic::Backend::Graphic::Vulkan::Instance");}

    //enable specific features
    VkPhysicalDeviceFeatures devFeatures {};
    devFeatures.shaderStorageImageMultisample = VK_TRUE;
    devFeatures.multiDrawIndirect = VK_TRUE;
    devFeatures.sampleRateShading = VK_TRUE;
    //enable descriptor update after binding
    indexing = {};
    indexing.sType = VK_STRUCTURE_TYPE_PHYSICAL_DEVICE_DESCRIPTOR_INDEXING_FEATURES;
    indexing.descriptorBindingSampledImageUpdateAfterBind = VK_TRUE;
    indexing.descriptorBindingStorageImageUpdateAfterBind = VK_TRUE;
    indexing.descriptorBindingStorageBufferUpdateAfterBind = VK_TRUE;
    indexing.descriptorBindingUniformBufferUpdateAfterBind = VK_TRUE;

    //create the actual device
    VkDeviceCreateInfo devCreateInfo {};
    devCreateInfo.sType = VK_STRUCTURE_TYPE_DEVICE_CREATE_INFO;
    devCreateInfo.pNext = &indexing;
    devCreateInfo.enabledExtensionCount = devExt.size();
    devCreateInfo.ppEnabledExtensionNames = devExt.data();
    devCreateInfo.queueCreateInfoCount = queueCreates.size();
    devCreateInfo.pQueueCreateInfos = queueCreates.data();
    devCreateInfo.pEnabledFeatures = &devFeatures;
    if (vkCreateDevice(reinterpret_cast<VkPhysicalDevice>(m_physicalDevice), &devCreateInfo, nullptr, reinterpret_cast<VkDevice*>(&m_device)) != VK_SUCCESS)
    {throw Exception("Failed to create the vulkan device", "GLGE::Graphic::Backend::Graphic::Vulkan::Instance");}

    //get the queues
    //graphics
    m_graphicsQueue.familyIdx = graphicsFamily;
    m_graphicsQueue.queueCount = 1;
    m_graphicsQueue.queues = new std::pair<void*, std::mutex>[1];
    vkGetDeviceQueue(reinterpret_cast<VkDevice>(m_device), graphicsFamily, graphicsQueueIdx, reinterpret_cast<VkQueue*>(&m_graphicsQueue.queues[0]));
    //transfer
    m_transferQueue.familyIdx = transferFamily;
    m_transferQueue.queueCount = transferQueueCount;
    m_transferQueue.queues = new std::pair<void*, std::mutex>[transferQueueCount];
    for (size_t i = 0; i < transferQueueCount; ++i) 
    {vkGetDeviceQueue(reinterpret_cast<VkDevice>(m_device), m_transferQueue.familyIdx, transferQueueIdx + i, reinterpret_cast<VkQueue*>(&m_transferQueue.queues[i]));}
    //compute
    m_computeQueue.familyIdx = computeFamily;
    m_computeQueue.queueCount = computeQueueCount;
    m_computeQueue.queues = new std::pair<void*, std::mutex>[computeQueueCount];
    for (size_t i = 0; i < computeQueueCount; ++i) 
    {vkGetDeviceQueue(reinterpret_cast<VkDevice>(m_device), m_computeQueue.familyIdx, computeQueueIdx + i, reinterpret_cast<VkQueue*>(&m_computeQueue.queues[i]));}

    //create single-use command pools
    VkCommandPoolCreateInfo cmdPoolCreate {};
    cmdPoolCreate.sType = VK_STRUCTURE_TYPE_COMMAND_POOL_CREATE_INFO;
    cmdPoolCreate.queueFamilyIndex = m_graphicsQueue.familyIdx;
    if (vkCreateCommandPool(reinterpret_cast<VkDevice>(m_device), &cmdPoolCreate, nullptr, reinterpret_cast<VkCommandPool*>(&m_graphicsQueue.singleUsePool)) != VK_SUCCESS)
    {throw Exception("Failed to create the graphics single use command pool", "GLGE::Graphic::Backend::Graphic::Vulkan::Instance");}
    cmdPoolCreate.queueFamilyIndex = m_transferQueue.familyIdx;
    if (vkCreateCommandPool(reinterpret_cast<VkDevice>(m_device), &cmdPoolCreate, nullptr, reinterpret_cast<VkCommandPool*>(&m_transferQueue.singleUsePool)) != VK_SUCCESS)
    {throw Exception("Failed to create the transfer single use command pool", "GLGE::Graphic::Backend::Graphic::Vulkan::Instance");}
    cmdPoolCreate.queueFamilyIndex = m_computeQueue.familyIdx;
    if (vkCreateCommandPool(reinterpret_cast<VkDevice>(m_device), &cmdPoolCreate, nullptr, reinterpret_cast<VkCommandPool*>(&m_computeQueue.singleUsePool)) != VK_SUCCESS)
    {throw Exception("Failed to create the compute single use command pool", "GLGE::Graphic::Backend::Graphic::Vulkan::Instance");}

    //create the VMA allocator
    VmaAllocatorCreateInfo vmaAllocCreate {};
    vmaAllocCreate.physicalDevice = reinterpret_cast<VkPhysicalDevice>(m_physicalDevice);
    vmaAllocCreate.device = reinterpret_cast<VkDevice>(m_device);
    vmaAllocCreate.instance = reinterpret_cast<VkInstance>(m_vkInstance);
    vmaAllocCreate.vulkanApiVersion = appInfo.apiVersion;
    vmaCreateAllocator(&vmaAllocCreate, reinterpret_cast<VmaAllocator*>(&m_allocator));

    //load the device limits
    VkPhysicalDeviceProperties props;
    vkGetPhysicalDeviceProperties(reinterpret_cast<VkPhysicalDevice>(m_physicalDevice), &props);
    
    //store the limits
    m_maxSampleCount = glm::min<i32>(static_cast<i32>(props.limits.framebufferColorSampleCounts+1), static_cast<i32>(props.limits.framebufferDepthSampleCounts+1));
    m_maxSampleCount = glm::min<i32>(m_maxSampleCount, static_cast<i32>(props.limits.framebufferStencilSampleCounts+1));
    m_maxSampleCount = glm::min<i32>(m_maxSampleCount, static_cast<i32>(VK_SAMPLE_COUNT_64_BIT));
}

Instance::~Instance() {
    //destroy the graphic backend
    m_instance->onGraphicBackendDestroy();

    //destroy the memory allocator
    vmaDestroyAllocator(reinterpret_cast<VmaAllocator>(m_allocator));

    //destroy the command pools
    vkDestroyCommandPool(reinterpret_cast<VkDevice>(m_device), reinterpret_cast<VkCommandPool>(m_graphicsQueue.singleUsePool), nullptr);
    vkDestroyCommandPool(reinterpret_cast<VkDevice>(m_device), reinterpret_cast<VkCommandPool>(m_transferQueue.singleUsePool), nullptr);
    vkDestroyCommandPool(reinterpret_cast<VkDevice>(m_device), reinterpret_cast<VkCommandPool>(m_computeQueue.singleUsePool),  nullptr);

    //destroy the vulkan device
    vkDestroyDevice(reinterpret_cast<VkDevice>(m_device), nullptr);
    //destroy the vulkan instance
    vkDestroyInstance(reinterpret_cast<VkInstance>(m_vkInstance), nullptr);
}

void Instance::onBind() {
    //initialize the graphic backend
    m_instance->onGraphicBackendInit();
}

void Instance::onUnbind() {

}

void Instance::onRegisterWindow(GLGE::Graphic::Backend::Graphic::Window* window) {
    
}

void Instance::onRemoveWindow(GLGE::Graphic::Backend::Graphic::Window* window) {
    
}
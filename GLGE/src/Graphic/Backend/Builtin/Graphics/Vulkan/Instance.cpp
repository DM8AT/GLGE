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

//use the vulkan backend namespace
using namespace GLGE::Graphic::Backend::Graphic::Vulkan;

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

    //fill out the instance create info
    VkInstanceCreateInfo instCreate {};
    instCreate.sType = VK_STRUCTURE_TYPE_INSTANCE_CREATE_INFO;
    instCreate.pApplicationInfo = &appInfo;
    instCreate.enabledExtensionCount = instExt.size();
    instCreate.ppEnabledExtensionNames = instExt.data();

    //create the instance
    if (vkCreateInstance(&instCreate, nullptr, reinterpret_cast<VkInstance*>(&m_instance)) != VK_SUCCESS)
    {throw GLGE::Exception("Failed to create the vulkan instance", "GLGE::Graphic::Backend::Graphic::Vulkan::Instance::Instance");}

    //get all available GPUs
    u32 deviceCount = 0;
    vkEnumeratePhysicalDevices(reinterpret_cast<VkInstance>(m_instance), &deviceCount, nullptr);
    std::vector<VkPhysicalDevice> devices(deviceCount);
    vkEnumeratePhysicalDevices(reinterpret_cast<VkInstance>(m_instance), &deviceCount, devices.data());

    //get and store the best GPU
    m_physicalDevice = __pick_strongest_device(devices, reinterpret_cast<VkInstance>(m_instance));
}

Instance::~Instance() {
    //destroy the vulkan instance
    vkDestroyInstance(reinterpret_cast<VkInstance>(m_instance), nullptr);
}

void Instance::onBind() {
    
}

void Instance::onUnbind() {

}

void Instance::onRegisterWindow(GLGE::Graphic::Backend::Graphic::Window* window) {
    
}

void Instance::onRemoveWindow(GLGE::Graphic::Backend::Graphic::Window* window) {
    
}
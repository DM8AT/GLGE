/**
 * @file DeviceEvaluation.h
 * @author DM8AT
 * @brief 
 * @version 0.1
 * @date 2026-05-04
 * 
 * @copyright Copyright (c) 2026
 * 
 */
#pragma once

//add vulkan
#include "vulkan/vulkan.h"
//add vectors
#include <vector>

/**
 * @brief a helper function to score the type of a GPU
 * 
 * @param type the type of the GPU
 * @return `uint32_t` the score given to the type
 */
static uint32_t __device_type_score(VkPhysicalDeviceType type) {
    //switch over the types and give pack a score
    switch (type) {
        case VK_PHYSICAL_DEVICE_TYPE_DISCRETE_GPU:       return static_cast<uint32_t>(4);
        case VK_PHYSICAL_DEVICE_TYPE_INTEGRATED_GPU:     return static_cast<uint32_t>(3);
        case VK_PHYSICAL_DEVICE_TYPE_VIRTUAL_GPU:        return static_cast<uint32_t>(2);
        case VK_PHYSICAL_DEVICE_TYPE_CPU:                return static_cast<uint32_t>(1);
        default:                                         return static_cast<uint32_t>(0);
    }
}

/**
 * @brief Simple compute/graphics capability heuristic
 * 
 * @param p the physical device properties to use for approximation
 * @return `uint64_t` the approximated score
 */
static uint64_t __compute_power_score(const VkPhysicalDeviceProperties& p) {
    //Limits proxy: maxComputeSharedMemorySize + maxComputeWorkGroupInvocations
    uint64_t proxy = static_cast<uint64_t>(p.limits.maxComputeSharedMemorySize) +
                     static_cast<uint64_t>(p.limits.maxComputeWorkGroupInvocations);

    return proxy;
}

/**
 * @brief a helper function to pick the strongest device
 * 
 * @param devices a vector of devices to pick from
 * @param instance the instance to use
 * @return `VkPhysicalDevice` the strongest device or `VK_NULL_HANDLE` if none is available
 */
static VkPhysicalDevice __pick_strongest_device(const std::vector<VkPhysicalDevice>& devices, VkInstance instance) {
    //store the current best device
    VkPhysicalDevice best_device = VK_NULL_HANDLE;
    uint64_t best_score = 0;
    //iterate the device list
    for (auto device : devices) {
        VkPhysicalDeviceProperties props{};
        vkGetPhysicalDeviceProperties(device, &props);

        //Primary: device type ranking
        uint64_t score = __device_type_score(props.deviceType) * 1000000;

        //Secondary: approximate compute capability
        score += __compute_power_score(props);

        //if this is the best, store it
        if (score > best_score) {
            best_score = score;
            best_device = device;
        }
    }

    //return the device
    return best_device;
}
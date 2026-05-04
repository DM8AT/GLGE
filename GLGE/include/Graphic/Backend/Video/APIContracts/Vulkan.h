/**
 * @file Vulkan.h
 * @author DM8AT
 * @brief define the contract for the vulkan graphic backend
 * @version 0.1
 * @date 2026-05-04
 * 
 * @copyright Copyright (c) 2026
 * 
 */
//header guard
#ifndef _GLGE_GRAPHIC_BACKEND_VIDEO_CONTRACTS_VULKAN_
#define _GLGE_GRAPHIC_BACKEND_VIDEO_CONTRACTS_VULKAN_

//add the contract base
#include "Contracts.h"
//add default
#include "Core/Common.h"

//dummy definitions of required classes
namespace GLGE::Graphic::Backend::Video 
{class Window;}

//use the backend video namespace
namespace GLGE::Graphic::Backend::Video::Contracts {

    /**
     * @brief define the contract base class for all OpenGL contracts
     */
    class Vulkan : public BaseContract {
    public:

        /**
         * @brief Construct a new Vulkan contract
         * 
         * @param instance a pointer to the video instance that owns the contract
         */
        Vulkan(GLGE::Graphic::Backend::Video::Instance* instance)
         : BaseContract(instance)
        {}

        /**
         * @brief Destroy the Vulkan contract
         */
        virtual ~Vulkan() = default;

        /**
         * @brief Get the required instance extensions
         * 
         * @param extensions a vector to append the instance extensions to
         * 
         * @warning do NOT assume that the vector is empty to begin with
         */
        virtual void getRequiredInstanceExtensions(std::vector<const char*>& extensions) = 0;

        /**
         * @brief Get the required device extensions
         * 
         * @param extensions a vector to append the instance extensions to
         * 
         * @warning do NOT assume that the vector is empty to begin with
         */
        virtual void getRequiredDeviceExtensions(std::vector<const char*>& extensions) = 0;

    };

}


#endif
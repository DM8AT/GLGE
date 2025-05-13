/**
 * @file GLGEApi.h
 * @author DM8AT
 * @brief The API sub-module is used to structure the fallback of APIs to select the best supported graphics API
 * @version 0.1
 * @date 2025-04-12
 * 
 * @copyright Copyright (c) 2025
 * 
 */

//header guard
#ifndef _GLGE_GRAPHIC_API_
#define _GLGE_GRAPHIC_API_

//include the settings
#include "../GLGESettings.h"
//include the utilities
#include "../GLGEUtil.h"

//check if this is C++
#if GLGE_CPP
extern "C" {
#endif //C++ section

/**
 * @brief store a list of potentially supported graphic apis
 */
typedef enum e_APIs {
    /**
     * @brief say that no graphics will be done
     */
    API_NONE = 0,
    /**
     * @brief say that OpenGL should be used for graphics and that OpenGL 4.6 should be used as maximum
     */
    API_OPENGL_4_6,
    /**
     * @brief say that OpenGL should be used for graphics and that a compute ray tracer used
     */
    API_OPENGL_4_6_RT,
    /**
     * @brief say that vulkan 1.2 should be used in a simple implementation
     */
    API_VULKAN_1_2_DEFAULT,
    /**
     * @brief say that vulkan 1.2 should be used with indirect drawing
     */
    API_VULKAN_1_2_INDIRECT,
    /**
     * @brief say that vulkan 1.2 should be used with ray tracing
     * @warning even if the engine supports this, it is not given that the graphics card supports it. The graphics card must support the vulkan ray tracing extension. 
     */
    API_VULKAN_1_2_RT,
    /**
     * @brief say that a software rasterizer should be used
     */
    API_SOFTWARE,
    /**
     * @brief say that a software ray tracer should be used
     */
    API_SOFTWARE_RT,
    /**
     * @brief say that a fallback error occoured
     */
    API_FALLBACK_ERROR
} APIs;

/**
 * @brief get if a API is supported
 * 
 * @param api the api to check
 * @return true : the API is supported
 * @return false : the API is not supported
 */
bool GLGE_C_FUNC(getAPISupport)(APIs api) GLGE_FUNC_NOEXCEPT;

/**
 * @brief treverse all inputed graphics apis to select the best supported one. If no grpahic api is supported, `API_FALLBACK_ERROR` will be outputed. 
 * 
 * @param fallbackList
 * @param fallbackCount 
 */
APIs GLGE_C_FUNC(getBestGraphicAPI)(APIs* fallbackList, uint32_t fallbackCount) GLGE_FUNC_NOEXCEPT;

/**
 * @brief convert an API identifyer into a string
 * 
 * @param api the API identifyer
 * @return the API as a string or 0 if the API is invalid
 */
const char* GLGE_C_FUNC(apiToString)(APIs api) GLGE_FUNC_NOEXCEPT;

//check if this is C++
#if GLGE_CPP
}

/**
 * @brief print an API identifyer to a default output
 * 
 * @param os the output stream to print to
 * @param api the API enum to print
 * @return std::ostream& the output stream with the API identifyer
 */
inline std::ostream& operator<<(std::ostream& os, APIs api) noexcept {return os << GLGE_C_FUNC(apiToString)(api);}

#endif //C++ section

#endif
/**
 * @file GLGEApi.cpp
 * @author DM8AT
 * @brief implement the api selection methodes
 * @version 0.1
 * @date 2025-04-12
 * 
 * @copyright Copyright (c) 2025
 * 
 */

//include the API header
#include "GLGEApi.h"
//include the list of supported APIs
#include "GLGEImplementationList.h"

/**
 * @brief store a lookup table of all APIs
 */
static std::string_view apiLookupTable[] = {
    "API_NONE",
    "API_OPENGL_3_3",
    "API_OPENGL_4_6",
    "API_OPENGL_4_6_RT",
    "API_VULKAN_1_2_DEFAULT",
    "API_VULKAN_1_2_INDIRECT",
    "API_VULKAN_1_2_RT",
    "API_SOFTWARE",
    "API_SOFTWARE_RT",
    "API_FALLBACK_ERROR"
};

/**
 * @brief get if a API is supported
 * 
 * @param api the api to check
 * @return true : the API is supported
 * @return false : the API is not supported
 */
bool GLGE_C_FUNC(getAPISupport)(APIs api) GLGE_FUNC_NOEXCEPT
{
    //switch over the API
    switch (api)
    {
    //define a helper macro to check if a API is supported
    #define __GLGE_SWITCH_HELPER(api, apiCheck) \
    case api: \
        /*check if the API is supported*/ \
        if (apiCheck == GLGE_API_DEFINED) \
        { \
            /*return that the API is supported*/ \
            return true; \
        } \
        break;

    //check if headless is supported
    __GLGE_SWITCH_HELPER(API_NONE, GLGE_API_SUPPORTS_API_NONE)
    //check if opengl 3.3 is supported
    __GLGE_SWITCH_HELPER(API_OPENGL_3_3, GLGE_API_SUPPORTS_API_OPENGL_3_3)
    //check if opengl 4.6 is supported
    __GLGE_SWITCH_HELPER(API_OPENGL_4_6, GLGE_API_SUPPORTS_API_OPENGL_4_6)
    //check if opengl 4.6 ray tracing is supported
    __GLGE_SWITCH_HELPER(API_OPENGL_4_6_RT, GLGE_API_SUPPORTS_API_OPENGL_4_6_RT)
    //check if vulkan 1.2 default is supported
    __GLGE_SWITCH_HELPER(API_VULKAN_1_2_DEFAULT, GLGE_API_SUPPORTS_API_VULKAN_1_2_DEFAULT)
    //check if vulkan 1.2 indirect is supported
    __GLGE_SWITCH_HELPER(API_VULKAN_1_2_INDIRECT, GLGE_API_SUPPORTS_API_VULKAN_1_2_INDIRECT)
    //check if vulkan 1.2 ray traing is supported
    __GLGE_SWITCH_HELPER(API_VULKAN_1_2_RT, GLGE_API_SUPPORTS_API_VULKAN_1_2_RT)
    //check if software rasterization is supported
    __GLGE_SWITCH_HELPER(API_SOFTWARE, GLGE_API_SUPPORTS_API_SOFTWARE)
    //check if software ray tracing is supported
    __GLGE_SWITCH_HELPER(API_SOFTWARE_RT, GLGE_API_SUPPORTS_API_SOFTWARE_RT)
    
    //undefine the helper macro
    #undef __GLGE_SWITCH_HELPER

    default:
        break;
    }
    //say that the API is not supported
    return false;
}

/**
 * @brief treverse all inputed graphics apis to select the best supported one. If no grpahic api is supported, `API_FALLBACK_ERROR` will be outputed. 
 * 
 * @param fallbackList
 * @param fallbackCount 
 */
APIs GLGE_C_FUNC(getBestGraphicAPI)(APIs* fallbackList, uint32_t fallbackCount) GLGE_FUNC_NOEXCEPT
{
    //store the selected graphics API
    APIs best = API_FALLBACK_ERROR;
    //iterate over the fallback list
    for (uint32_t i = 0; i < fallbackCount; ++i)
    {
        //check if the API is supported
        if (GLGE_C_FUNC(getAPISupport)(fallbackList[i]))
        {
            //store this as the best API
            best = fallbackList[i];
            //stop the loop
            break;
        }
    }
    //return the best API
    return best;
}

/**
 * @brief convert an API identifyer into a string
 * 
 * @param api the API identifyer
 * @return the API as a string or 0 if the API is invalid
 */
const char* GLGE_C_FUNC(apiToString)(APIs api) GLGE_FUNC_NOEXCEPT
{
    //sanity-check the input
    if (((uint32_t)api) >= (uint32_t)(sizeof(apiLookupTable) / sizeof(*apiLookupTable)))
    {
        //return 0 for out of range
        return 0;
    }
    //return the correct position in the LUT
    return apiLookupTable[(uint32_t)api].data();
}
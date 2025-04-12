/**
 * @file GLGEImplementationList.h
 * @author DM8AT
 * @brief define a list of the states of all graphic apis
 * @version 0.1
 * @date 2025-04-12
 * 
 * @copyright Copyright (c) 2025
 * 
 */

//header guard
#ifndef _GLGE_GRAPHIC_IMPLEMENTATION_LIST_
#define _GLGE_GRAPHIC_IMPLEMENTATION_LIST_

//define the value that means undefined
#define GLGE_API_UNDEFINED 0
//define the value that means defined
#define GLGE_API_DEFINED 1

//say that the headless api is not defined
#define GLGE_API_SUPPORTS_API_NONE GLGE_API_UNDEFINED
//say that the OpenGL 3.3 api is not defined
#define GLGE_API_SUPPORTS_API_OPENGL_3_3 GLGE_API_UNDEFINED
//say that the OpenGL 4.6 api is not defined
#define GLGE_API_SUPPORTS_API_OPENGL_4_6 GLGE_API_UNDEFINED
//say that the OpenGL 4.6 Ray Tracing api is not defined
#define GLGE_API_SUPPORTS_API_OPENGL_4_6_RTX GLGE_API_UNDEFINED
//say that the Vulkan 1.2 default api is not defined
#define GLGE_API_SUPPORTS_API_VULKAN_1_2_DEFAULT GLGE_API_UNDEFINED
//say that the Vulkan 1.2 indirect api is not defined
#define GLGE_API_SUPPORTS_API_VULKAN_1_2_INDIRECT GLGE_API_UNDEFINED
//say that the Vulkan 1.2 Ray Tracing api is not defined
#define GLGE_API_SUPPORTS_API_VULKAN_1_2_RTX GLGE_API_UNDEFINED
//say that the software rasterization api is not defined
#define GLGE_API_SUPPORTS_API_SOFTWARE GLGE_API_DEFINED
//say that the software ray tracing api is not defined
#define GLGE_API_SUPPORTS_API_SOFTWARE_RTX GLGE_API_UNDEFINED

#endif
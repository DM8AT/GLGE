/**
 * @file ExamplePluginContract.h
 * @author DM8AT
 * @brief define a stable contract between an example and the example loading system
 * @version 0.1
 * @date 2026-05-18
 *
 * @copyright Copyright (c) 2026
 *
 */
#ifndef _EXAMPLE_PLUGIN_CONTRACT_
#define _EXAMPLE_PLUGIN_CONTRACT_

//set a define so the functions are visible in dynamic libraries
#if defined(_WIN32)
    #if defined(EXAMPLE_PLUGIN_BUILD)
        #define GLGE_EXAMPLE_PLUGIN_API __declspec(dllexport)
    #else
        #define GLGE_EXAMPLE_PLUGIN_API __declspec(dllimport)
    #endif
#else
    #define GLGE_EXAMPLE_PLUGIN_API __attribute__((visibility("default")))
#endif

/**
 * @brief Example entry point. Backend names match the launcher registry ("OpenGL", "Vulkan", "SDL3", ...).
 *
 * Implementations must create backend Description objects inside the plugin module, else the systems don't work together correctly. 
 */
typedef unsigned char (*PFN_ExampleFunc)(const char* graphicBackendName, const char* videoBackendName);

/**
 * @brief Registry implemented by the launcher; plugins only call `addExample`.
 */
class ExampleRegistry {
public:
    virtual ~ExampleRegistry() = default;
    virtual void addExample(const char* displayName, PFN_ExampleFunc entry) = 0;
};

#define EXAMPLE_SYS_STRINGIFY_HELPER(x) #x
#define EXAMPLE_SYS_STRINGIFY(x) EXAMPLE_SYS_STRINGIFY_HELPER(x)

#define EXAMPLE_SYS_REGISTER_EXAMPLE_PLUGIN example_registerExamplePlugin
#define EXAMPLE_SYS_REGISTER_EXAMPLE_PLUGIN_SYM EXAMPLE_SYS_STRINGIFY(EXAMPLE_SYS_REGISTER_EXAMPLE_PLUGIN)

typedef void* ExampleRegistryPtr;
using PFN_registerExamplePlugin = void (*)(ExampleRegistryPtr);

#endif

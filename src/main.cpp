/**
 * @file main.cpp
 * @author DM8AT
 * @brief define the main selector for the examples
 * @version 0.1
 * @date 2026-04-14
 * 
 * @copyright Copyright (c) 2026
 * 
 */
//add examples
#include "Examples/Examples.h"

//add default I/O
#include <iostream>

/**
 * @brief define if the configuration is changed when an example fails or if the failure is passed
 */
constexpr bool RETRY = false;

/**
 * @brief define a function pointer to a graphic backend loader function
 */
typedef GLGE::Graphic::Backend::Graphic::Description* (*PFN_GraphicBackendLoader)();

/**
 * @brief store all graphic backend
 */
std::vector<std::pair<const char*, PFN_GraphicBackendLoader>> GRAPHIC_BACKEND_MAP = {
    std::pair("OpenGL", []() -> GLGE::Graphic::Backend::Graphic::Description* {return new GLGE::Graphic::Builtin::Graphics::OpenGL();}),
    std::pair("Vulkan", []() -> GLGE::Graphic::Backend::Graphic::Description* {return new GLGE::Graphic::Builtin::Graphics::Vulkan();})
};

/**
 * @brief define a function pointer to a video backend loader function
 */
typedef GLGE::Graphic::Backend::Video::Description* (*PFN_VideoBackendLoader)();

/**
 * @brief store all graphic backend
 */
std::vector<std::pair<const char*, PFN_VideoBackendLoader>> VIDEO_BACKEND_MAP = {
    std::pair("SDL3", []() -> GLGE::Graphic::Backend::Video::Description* {return new GLGE::Graphic::Builtin::Video::SDL3();})
};

/**
 * @brief a helper function to stringify a list of elements in a structured way
 * 
 * @tparam T ignored
 * @param list the list to print
 * @param align the size of the begin block, padded with spaces
 * @return `std::string` the string containing the map
 */
template <typename T> 
static std::string structureMapToString(const std::vector<std::pair<const char*, T>>& list, size_t align = 6) {
    //create the initial stream
    std::stringstream stream;
    //keep track of the current element ID
    size_t id = 0;
    //iterate over all elements
    for (const auto& [name, ptr] : list) {
        //create the beginning section of the element
        std::string begin("[");
        begin += std::to_string(id);
        begin += "]";
        begin.resize(align, ' ');
        stream << begin << " " << name << "\n";
        ++id;
    }
    //return the final stream
    return stream.str();
}

int main() {
    //print all loaded examples
    std::cout << "Loaded examples:\n";
    std::cout << structureMapToString(EXAMPLES);
    std::cout << "Please enter the number of the example to run:\n";
    //get the example to run
    size_t exampleId = UINT64_MAX;
    std::cin >> exampleId;
    if (exampleId > EXAMPLES.size())
    {throw GLGE::Exception("The selected example does not map to a valid example ID", "main");}

    //print all loaded graphic backends
    std::cout << "Loaded graphic backends:\n";
    std::cout << structureMapToString(GRAPHIC_BACKEND_MAP);
    //get the graphic API to use
    size_t graphicApiId = UINT64_MAX;
    std::cin >> graphicApiId;
    if (graphicApiId > GRAPHIC_BACKEND_MAP.size())
    {throw GLGE::Exception("The selected graphic API ID does not map to a valid graphic API", "main");}

    //print all loaded video backends
    std::cout << "Loaded video backends:\n";
    std::cout << structureMapToString(VIDEO_BACKEND_MAP);
    //get the video api to use
    size_t videoApiId = UINT64_MAX;
    std::cin >> videoApiId;
    if (videoApiId > VIDEO_BACKEND_MAP.size())
    {throw GLGE::Exception("The selected video API ID does not map to a valid video API", "main");}

    //setup for potential looping
    bool success = false;
    size_t initalGraphicAPI = graphicApiId;
    size_t initialVideoAPI = videoApiId;
    //helper
    auto stepHelper = [&]() {
        //re-trying in a new configuration.
        ++graphicApiId;
        if (graphicApiId > GRAPHIC_BACKEND_MAP.size()) 
        {graphicApiId = 0;}
        if (graphicApiId == initalGraphicAPI)
        {++videoApiId;}
        if (videoApiId > VIDEO_BACKEND_MAP.size()) 
        {videoApiId = 0;}
        if (videoApiId == initialVideoAPI)
        {throw GLGE::Exception("Failed to find a configuration that can run the selected example", "main");}
    };
    //if `RETRY` is enabled try to loop when the example crashes
    while (!success) {
        try {
            //create the APIs and run the code, storing the return value
            unsigned char ret = (*(EXAMPLES[exampleId].second))((*(GRAPHIC_BACKEND_MAP[graphicApiId].second))(), (*(VIDEO_BACKEND_MAP[videoApiId].second))());
            //validate success
            if (ret == 0 || !RETRY) {
                success = true;
            } else {
                //print that a return value larger than 0 was gotten
                std::cout << "[FATAL ERROR] The selected example returned a non-zero return value when running in the current configuration. Return value:\n    "
                          << static_cast<int>(ret) << "\n";
                //step
                stepHelper();
            }
        } catch (std::exception exception) {
            if (RETRY) {
                //print it but ignore it
                std::cout << "[FATAL ERROR] Failed to run the selected example in the current configuration. An exception was thrown. What:\n";
                std::cout << exception.what() << "\n";
                //step
                stepHelper();
            } else {
                //just re-throw the exception
                throw;
            }
        }
    }

    //sane exit
    return 0;
}
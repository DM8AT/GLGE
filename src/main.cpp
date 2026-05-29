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
//add examples and the example plugin loader
#include "Examples/Examples.h"
#include "Examples/ExamplePluginLoader.h"
//add the test system
#include "Testing/TestLauncher.h"

//add default I/O
#include <iostream>
#include <sstream>
#include <stdexcept>
#include <string>

/**
 * @brief Directory scanned for example modules (`.so` / `.dll` / `.dylib`) exporting `EXAMPLE_SYS_REGISTER_EXAMPLE_PLUGIN`.
 */
constexpr const char* const EXAMPLE_PLUGIN_DIRECTORY = "plugins";
/**
 * @brief store all examples
 */
std::vector<std::pair<const char*, PFN_ExampleFunc>> EXAMPLES;

/**
 * @brief define if the configuration is changed when an example fails or if the failure is passed
 */
constexpr bool RETRY = false;

/**
 * @brief store all graphic backends
 * 
 * The first element is the display name, the second the identification name
 */
std::vector<std::pair<const char*, const char*>> GRAPHIC_BACKEND_MAP = {
    std::pair("OpenGL 4.6 core", "OpenGL"),
    std::pair("Vulkan 1.0", "Vulkan")
};

/**
 * @brief store all video backends
 * 
 * The first element is the display name, the second the identification name
 */
std::vector<std::pair<const char*, const char*>> VIDEO_BACKEND_MAP = {
    std::pair("SDL 3", "SDL3")
};

/**
 * @brief a function that is responsible for gathering which example to run and to try and run it
 */
void uiSelector() {
    //sanity check that all lists are filled
    if (EXAMPLES.size() == 0)
    {throw std::runtime_error("Failed to run the example launcher - the example list is empty");}
    if (GRAPHIC_BACKEND_MAP.size() == 0)
    {throw std::runtime_error("Failed to run the example launcher - the graphic backend list is empty");}
    if (VIDEO_BACKEND_MAP.size() == 0)
    {throw std::runtime_error("Failed to run the example launcher - the video backend list is empty");}

    //print all loaded examples
    std::cout << "Loaded examples:\n";
    std::cout << structureMapToString(EXAMPLES);
    //get the example to run
    size_t exampleId = SIZE_MAX;
    //auto-select 0 if only one example exists
    if (EXAMPLES.size() == 1) 
    {exampleId = 0;}
    else {
        //else, quarry which example to use from the user
        std::cout << "Please enter the number of the example to run:\n";
        exampleId = readSizeT();
        if (exampleId >= EXAMPLES.size())
        {throw std::invalid_argument("The selected example does not map to a valid example ID");}
    }

    //print all loaded graphic backends
    std::cout << "Loaded graphic backends:\n";
    std::cout << structureMapToString(GRAPHIC_BACKEND_MAP);
    //get the graphic API to use
    size_t graphicApiId = SIZE_MAX;
    //auto-select 0 if only one graphic backend exists
    if (GRAPHIC_BACKEND_MAP.size() == 1)
    {graphicApiId = 0;}
    else {
        //else, quarry which graphic backend to use from the user
        std::cout << "Please enter the number of the graphic API to use:\n";
        graphicApiId = readSizeT();
        if (graphicApiId >= GRAPHIC_BACKEND_MAP.size())
        {throw std::invalid_argument("The selected graphic API ID does not map to a valid graphic API");}
    }

    //print all loaded video backends
    std::cout << "Loaded video backends:\n";
    std::cout << structureMapToString(VIDEO_BACKEND_MAP);
    //get the video api to use
    size_t videoApiId = SIZE_MAX;
    //auto-select 0 if only one video backend exists
    if (VIDEO_BACKEND_MAP.size() == 1)
    {videoApiId = 0;}
    else {
        //else, quarry which video backend to use from the user
        std::cout << "Please enter the number of the video API to use:\n";
        videoApiId = readSizeT();
        if (videoApiId >= VIDEO_BACKEND_MAP.size())
        {throw std::invalid_argument("The selected video API ID does not map to a valid video API");}
    }

    //setup for potential looping
    bool success = false;
    size_t initialGraphicAPI = graphicApiId;
    size_t initialVideoAPI = videoApiId;
    //helper
    auto stepHelper = [&]() {
        //re-trying in a new configuration.
        ++graphicApiId;
        if (graphicApiId >= GRAPHIC_BACKEND_MAP.size()) 
        {graphicApiId = 0;}
        if (graphicApiId == initialGraphicAPI)
        {++videoApiId;}
        if (videoApiId >= VIDEO_BACKEND_MAP.size()) 
        {videoApiId = 0;}
        if (videoApiId == initialVideoAPI && graphicApiId == initialGraphicAPI)
        {throw std::runtime_error("Failed to find a configuration that can run the selected example");}
    };
    //if `RETRY` is enabled try to loop when the example crashes
    while (!success) {
        try {
            const char* graphicBackendName = GRAPHIC_BACKEND_MAP[graphicApiId].second;
            const char* videoBackendName = VIDEO_BACKEND_MAP[videoApiId].second;

            std::cout << "Current configuration:\n    Graphic API: " << graphicBackendName << "\n    Video API: " << videoBackendName << "\n";

            std::cout << "------------------LAUNCHED EXAMPLE------------------\n";

            unsigned char ret = (*(EXAMPLES[exampleId].second))(graphicBackendName, videoBackendName);

            //print that the example closed via return
            std::cout << "------------------EXAMPLE RETURNED------------------\n";

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
        } catch (const std::exception& exception) {
            //print that the example closed via exception
            std::cout << "------------------EXAMPLE THREW EXCEPTION------------------\n";
            //check if retries are allowed
            if (RETRY) {
                //print it but ignore it
                std::cout << "[ERROR] Failed to run the selected example in the current configuration. An exception was thrown. What:\n";
                std::cout << exception.what() << "\n";
                //step
                stepHelper();
            } else {
                //just re-throw the exception
                throw;
            }
        }
    }
}

int runExamples() {
    //load all dynamic examples
    loadExamplePluginsFromDirectory(EXAMPLE_PLUGIN_DIRECTORY);

    //store if the program is active
    bool active = true;
    //store what to return
    int retValue = 0;
    //loop while the program is active
    while (active) {
        //run the selector
        try {
            uiSelector();
            active = false;
        } catch (const std::invalid_argument& exception) {
            //input error - print and re-try
            std::cout << "----------------\n" << exception.what() << "\n----------------\n";
        } catch (...) {
            //re-throw
            throw;
        }
    }
    //return the return value
    return retValue;
}

int main(void) {
    //print what the numbers mean
    std::cout << "[0]     Run Examples - Launches the example selector to run any provided example\n";
    std::cout << "[1]     Run Unit Tests - Launches the test launcher to select a suite or a list of suites to run\n";

    //get the selected number
    size_t selected = SIZE_MAX;
    while (selected == SIZE_MAX) {
        try {
            //try to read a size_t (this may throw)
            selected = readSizeT();
        } catch (const std::invalid_argument& exception) {
            //input error - print and re-try
            std::cout << "----------------\n" << exception.what() << "\n----------------\n";
        } catch (...) {
            //re-throw
            throw;
        }
        //sanity check the range
        if (selected > 1) {
            //print an error
            std::cout << "----------------\nPlease input either 0 or 1\n----------------\n";
            selected = SIZE_MAX;
        }
    }

    //run the according function
    if (selected == 0)
    {return runExamples();}
    else if (selected == 1)
    {return launchUnitTests();}
    else 
    {return 0xf1;} //what?
}
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
typedef std::unique_ptr<GLGE::Graphic::Backend::Graphic::Description> (*PFN_GraphicBackendLoader)();

/**
 * @brief store all graphic backend
 */
std::vector<std::pair<const char*, PFN_GraphicBackendLoader>> GRAPHIC_BACKEND_MAP = {
    std::pair("OpenGL", []() -> std::unique_ptr<GLGE::Graphic::Backend::Graphic::Description> {return std::make_unique<GLGE::Graphic::Builtin::Graphics::OpenGL>();}),
    std::pair("Vulkan", []() -> std::unique_ptr<GLGE::Graphic::Backend::Graphic::Description> {return std::make_unique<GLGE::Graphic::Builtin::Graphics::Vulkan>();})
};

/**
 * @brief define a function pointer to a video backend loader function
 */
typedef std::unique_ptr<GLGE::Graphic::Backend::Video::Description> (*PFN_VideoBackendLoader)();

/**
 * @brief store all graphic backend
 */
std::vector<std::pair<const char*, PFN_VideoBackendLoader>> VIDEO_BACKEND_MAP = {
    std::pair("SDL3", []() -> std::unique_ptr<GLGE::Graphic::Backend::Video::Description> {return std::make_unique<GLGE::Graphic::Builtin::Video::SDL3>();})
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
    for (const auto& [name, _] : list) {
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

/**
 * @brief a helper function to read a size_t from the console
 * 
 * @return `size_t` the read value
 */
static size_t readSizeT() {
    //get the whole line
    std::string line;
    //loop until something is inputted
    while (line.empty()) {
        if (!std::getline(std::cin, line))
        {throw GLGE::Exception("The input stream closed", "readSizeT");}
    }

    //convert the line to an input string stream
    std::istringstream iss(line);

    //check if the initial value is a number
    long long value;
    if (!(iss >> value))  {
        //throw an error
        throw std::invalid_argument("Please input only numbers");
    }

    //check if the value is negative (invalid size_t)
    if (value < 0) {
        //input state is still valid
        throw std::invalid_argument("Please input only positive numbers");
    }

    //check for too many input items
    std::string extra;
    if (iss >> extra) {
        //throw an error
        throw std::invalid_argument("Please input only a single number");
    }

    //return the valid number
    return size_t(value);
}

/**
 * @brief a function that is responsible for gathering which example to run and to try and run it
 */
void uiSelector() {
    //sanity check that all lists are filled
    if (EXAMPLES.size() == 0)
    {throw GLGE::Exception("Failed to run the example launcher - the example list is empty", "uiSelector");}
    if (GRAPHIC_BACKEND_MAP.size() == 0)
    {throw GLGE::Exception("Failed to run the example launcher - the graphic backend list is empty", "uiSelector");}
    if (VIDEO_BACKEND_MAP.size() == 0)
    {throw GLGE::Exception("Failed to run the example launcher - the video backend list is empty", "uiSelector");}

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
        {throw GLGE::Exception("Failed to find a configuration that can run the selected example", "uiSelector");}
    };
    //if `RETRY` is enabled try to loop when the example crashes
    while (!success) {
        try {
            //create the backend descriptors
            auto gDescr = ((*(GRAPHIC_BACKEND_MAP[graphicApiId].second))());
            auto vDescr = (*(VIDEO_BACKEND_MAP[videoApiId].second))();

            //log the selected configuration
            std::cout << "Current configuration:\n    Graphic API: " << gDescr->getName() << "\n    Video API: " << vDescr->getName() << "\n";

            //log a clear separation between what the launcher printed and what the example printed
            std::cout << "------------------LAUNCHED EXAMPLE------------------\n";

            //create the APIs and run the code, storing the return value
            unsigned char ret = (*(EXAMPLES[exampleId].second))(gDescr.get(), vDescr.get());

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

int main(void) {
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
        } catch (const std::exception& exception) {
            //print the fatal error
            std::cout << "[FATAL ERROR] A fatal error was thrown during selection:\n" << exception.what() << "\n";
            //set the return value to 256 and stop
            retValue = 0xff;
            active = false;
        }
    }
    //return the return value
    return retValue;
}
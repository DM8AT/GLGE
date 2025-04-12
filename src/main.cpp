#include "GLGE/GLGE.h"

int main()
{
    //store all APIs that might work
    std::vector<APIs> apiList {
        API_VULKAN_1_2_INDIRECT,
        API_VULKAN_1_2_DEFAULT,
        API_OPENGL_4_6,
        API_OPENGL_3_3,
        API_SOFTWARE
    };
    //get the best API
    APIs best = getBestGraphicAPI(apiList.data(), apiList.size());
    //check if an API was found
    if (best == API_FALLBACK_ERROR)
    {
        //print an error
        std::cerr << "[ERROR] No good graphic API was found\n";
        //return an error
        return EXIT_FAILURE;
    }

    //print the selected API
    std::cout << "Selected Graphics API: " << best << "\n";
    //create an instance with the best API
    Instance instance("Main Instance", best);

    //print the instance
    std::cout << instance << "\n";
}
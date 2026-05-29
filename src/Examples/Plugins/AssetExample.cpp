/**
 * @file AssetExample.cpp
 * @author DM8AT
 * @brief an example that makes extensive use of the asset system
 * @version 0.1
 * @date 2026-05-29
 * 
 * @copyright Copyright (c) 2026
 * 
 */
//add the example system
#include "ExamplePluginContract.h"
#include "ExampleBackendFactory.h"

void printHelper(GLGE::AssetHandle<GLGE::CompoundAsset> asset, uint64_t depth) {
    GLGE::CompoundAsset& ass = *asset.reference();
    for (const auto& entry : ass) {
        std::cout << std::string(depth*4, ' ') << entry.path() << "\n";
        if (entry.is_directory()) {
            GLGE::AssetHandle<GLGE::CompoundAsset> nextAsset = asset.reference()->open<GLGE::CompoundAsset>(entry.path());
            printHelper(nextAsset, depth+1);
        }
    }
}

GLGE::u8 assetExample(const char *graphicBackendName, const char *videoBackendName) {
    //initialize
    GLGE::Instance::init();

    //the descriptions need to be created here using the provided names
    auto gDescr = createGraphicBackendDescription(graphicBackendName);
    auto vDescr = createVideoBackendDescription(videoBackendName);

    //create the instance
    GLGE::Graphic::Instance gInst(gDescr.get(), vDescr.get());
    GLGE::Instance inst("Asset example", GLGE::Version(1,0,0), std::pair{"Graphic", &gInst});

    auto ass = inst.assets().load<GLGE::CompoundAsset>("assets.gcmp");
    printHelper(ass, 0);

    auto img = ass.reference()->open<GLGE::Graphic::Asset::ImageCPU>("imgs/img");
    if (img) 
    {img.reference()->export_as("Img.png", GLGE::Graphic::Asset::ImageCPU::PNG);}

    return 0;
}

/**
 * @brief define the function that is used to register the example
 * 
 * @param ptr a pointer to the example registry
 */
extern "C" GLGE_EXAMPLE_PLUGIN_API void EXAMPLE_SYS_REGISTER_EXAMPLE_PLUGIN(ExampleRegistryPtr ptr) {
    //get the example registry
    auto* reg = reinterpret_cast<ExampleRegistry*>(ptr);

    //add the example
    reg->addExample("Asset Example - This example makes extensive use of the asset API.",
                    &assetExample);
}
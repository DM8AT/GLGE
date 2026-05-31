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
    for (const auto& entry : *asset.reference()) {
        std::cout << std::string(depth*4, ' ') << entry.path() << "\n";
        if (entry.is_directory()) {
            GLGE::AssetHandle<GLGE::CompoundAsset> nextAsset = asset.reference()->open<GLGE::CompoundAsset>(entry.path());
            printHelper(nextAsset, depth+1);
        }
    }
}

class ExampleComponent : public GLGE::SerializableComponent {
public:

    /**
     * @brief Construct a new Example Component
     * 
     * All components must be default-constructable
     */
    ExampleComponent() = default;

    /**
     * @brief Construct a new Example Component
     * 
     * @param v the value to initialize with
     */
    ExampleComponent(GLGE::u64 v) : value(v) {}

    /**
     * @brief load the serializable component
     * 
     * @param entity the entity to load to
     * @param buffer the buffer to load from
     * @param world the context of the world to load to
     */
    virtual void load(const std::span<const GLGE::u8>& buffer) override {
        //store the number
        memcpy(&value, buffer.data(), sizeof(value));
    }

    /**
     * @brief store the serializable asset
     * 
     * @param buffer the buffer to write the serializable data to
     * @param world a pointer to the world to store from
     */
    virtual void store(std::vector<GLGE::u8>& buffer) override {
        //add the number
        GLGE::u8 data[8] {};
        memcpy(data, &value, sizeof(value));
        buffer.insert(buffer.end(), data, data+8);
    }

    GLGE::u64 value = 0;

};

GLGE::u8 assetExample(const char *graphicBackendName, const char *videoBackendName) {
    //initialize
    GLGE::Instance::init();

    //this registration can be done at any time. 
    GLGE::WorldAsset::getComponentRegistry().addType<ExampleComponent>();

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

    auto worldAss = ass.reference()->open<GLGE::WorldAsset>("worlds/Hello World");
    GLGE::World& world = worldAss.reference()->world();
    world.printTree(world.getRoot());

    GLGE::Object first = world.getAllWithName("First").front();
    std::cout << "Value from \"First\": " << world.get<ExampleComponent>(first)->value << "\n";
    GLGE::Object third = world.getAllWithName("Third").front();
    std::cout << "Value from \"Third\": " << world.get<ExampleComponent>(third)->value << "\n";

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
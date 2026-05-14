/**
 * @file vk_example1.cpp
 * @author DM8AT
 * @brief define the first Vulkan example
 * @version 0.1
 * @date 2026-04-14
 * 
 * @copyright Copyright (c) 2026
 * 
 */
//add the example
#include "vk_example1.h"

//add GLGE
#define GLGE_NO_THANKS_MSG
#include "GLGE.h"

void vk_example1() {
    //initialize GLGE
    GLGE::Instance::init();

    //setup the backends
    GLGE::Graphic::Builtin::Graphics::Vulkan graphicDesc;
    GLGE::Graphic::Builtin::Video::SDL3 videoDesc;
    //store the graphic instance
    GLGE::Graphic::Instance gInst(&graphicDesc, &videoDesc);
    
    std::cout << "Using graphic backend " << gInst.getGraphicBackendName() << "\n";
    std::cout << "Using video backend " << gInst.getVideoBackendName() << "\n";

    //create the instance
    GLGE::Instance inst("Vulkan instance", GLGE::Version(0, 1, 0), std::pair{"Graphic instance", &gInst});
    inst.mainLimiter().setLimit(144);

    GLGE::Graphic::Window win("Hello from SDL3", {600, 600});
    win.setVSyncMode(GLGE::Graphic::VSYNC_ENABLED);

    std::cout << "Selected GPU: "           << gInst.getGPUName()          << "\n";
    std::cout << "    GPU Vendor: "         << gInst.getGPUVendorName()    << "\n";
    std::cout << "    GPU Driver Version: " << gInst.getGPUDriverVersion() << "\n";

    auto imgData = inst.assets().load<GLGE::Graphic::Asset::ImageCPU>("assets/textures/grass.png", GLGE::Graphic::Asset::ImageCPU::PNG);
    auto othData = inst.assets().load<GLGE::Graphic::Asset::ImageCPU>("assets/textures/cubeTexture.png", GLGE::Graphic::Asset::ImageCPU::PNG);
    GLGE::Graphic::Image img(*imgData.reference());
    img.write(*othData.reference(), {32,32});
    GLGE::Graphic::Texture tex(GLGE::Graphic::TextureCPU(*imgData.reference(), 0));

    GLGE::Graphic::Image oth(win.getSize(), GLGE::Graphic::PIXEL_FORMAT_RGBA_16_FLOAT);
    GLGE::Graphic::Framebuffer fbuff({&oth}, {}, {});

    GLGE::Graphic::ImageCPU newImg;
    img.read(newImg);
    GLGE::Graphic::Asset::ImageCPU cpuImgAsset;
    cpuImgAsset.image() = newImg;
    cpuImgAsset.export_as("img.png", GLGE::Graphic::Asset::ImageCPU::PNG);

    GLGE::u32 data[] = {0,1,2,3,4,5,6,7,8,9};
    GLGE::Graphic::Buffer buff(GLGE::Graphic::Buffer::Type::STORAGE, data, sizeof(data), GLGE::Graphic::Buffer::Usage::CPU_GPU);

    GLGE::Graphic::Shader simple({std::pair{"Compute", "assets/shader/simple.comp.spv"}});
    GLGE::Graphic::ResourceSet simpleSet(simple.getSet(0), std::pair{"imgOutput", &oth});
    simple.setResources(0, &simpleSet);
    
    GLGE::Graphic::VertexLayout layout {{
        GLGE::Graphic::VertexAttribute(GLGE::Graphic::VertexAttribute::Type::Position, GLGE::Graphic::VertexAttribute::Format::vec3, 0,  0),
        GLGE::Graphic::VertexAttribute(GLGE::Graphic::VertexAttribute::Type::UV,       GLGE::Graphic::VertexAttribute::Format::vec2, 12, 1),
        GLGE::Graphic::VertexAttribute(GLGE::Graphic::VertexAttribute::Type::Normal,   GLGE::Graphic::VertexAttribute::Format::vec3, 20, 2),
        GLGE::Graphic::VertexAttribute(GLGE::Graphic::VertexAttribute::Type::Tangent,  GLGE::Graphic::VertexAttribute::Format::vec3, 32, 3),
        GLGE::Graphic::VertexAttribute(GLGE::Graphic::VertexAttribute::Type::Color,    GLGE::Graphic::VertexAttribute::Format::vec4, 44, 4)
    }, 60};
    GLGE::AssetHandle<GLGE::Graphic::Asset::Mesh> cube_mesh = inst.assets().load<GLGE::Graphic::Asset::Mesh>("assets/meshes/Cube.fbx", GLGE::Graphic::Asset::Mesh::ASSIMP);
    GLGE::World world("Scene 1");

    GLGE::Graphic::RenderTarget window(&win);
    GLGE::Graphic::RenderTarget fbuffTarget(&fbuff);

    auto pipe = GLGE::Graphic::RenderPipeline::create(&win, 
        std::pair{"Clear", GLGE::Graphic::Command(GLGE::Graphic::COMMAND_CLEAR, window, GLGE::u8(0), GLGE::vec4(GLGE::vec3(0.4f),1), GLGE::f32(1), GLGE::u32(0))},
        std::pair{"Compute", GLGE::Graphic::Command(GLGE::Graphic::COMMAND_DISPATCH_COMPUTE, &simple, GLGE::uvec3(glm::ceil(oth.getSize().x/16.f), glm::ceil(oth.getSize().y/16.f), 1))},
        std::pair{"Present", GLGE::Graphic::Command(GLGE::Graphic::COMMAND_COPY, fbuffTarget, GLGE::u8(0), window, GLGE::u8(0), false, false)}
    );
    pipe.record();

    while (!win.isClosingRequested()) {
        //start the tick
        inst.startMainTick();

        //handle resizing
        if (win.didResize()) {
            oth.resizeAndClear(win.getResolution());
            *pipe.getCommand("Compute")->access<GLGE::uvec3>(sizeof(void*)) = GLGE::uvec3(glm::ceil(oth.getSize().x/16.f), glm::ceil(oth.getSize().y/16.f), 1);
            pipe.record();
        }

        //draw
        pipe.play();

        //end the tick
        inst.endMainTick();
    }

}
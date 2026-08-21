/**
 * @file NewMeshExample.cpp
 * @author DM8AT
 * @brief An example for the new mesh storage system
 * @version 0.1
 * @date 2026-08-14
 * 
 * @copyright Copyright (c) 2026
 * 
 */
//add the example system
#include "ExamplePluginContract.h"
#include "ExampleBackendFactory.h"

struct VertAttr_UV {
    GLGE::u8 _unused = 0;
};

GLGE::u8 newMeshExample(const char *graphicBackendName, const char *videoBackendName) {
    //initialize
    GLGE::Instance::init();

    //the descriptions need to be created here using the provided names
    auto gDescr = createGraphicBackendDescription(graphicBackendName);
    auto vDescr = createVideoBackendDescription(videoBackendName);

    //create the instance
    GLGE::Graphic::Instance gInst(gDescr.get(), vDescr.get());
    GLGE::Instance inst("New Mesh example", GLGE::Version(1,0,0), std::pair{"Graphic", &gInst});

    //create a window to make the graphic instance valid
    GLGE::Graphic::WindowSettings settings;
    GLGE::Graphic::Window win("Window", {600, 600}, settings);

    auto ass = inst.assets().load<GLGE::MeshAsset>("assets/meshes/Cube.fbx", GLGE::MeshAsset::ASSIMP);
    
    //print information about the layout of the mesh
    for (const auto& attr : ass.reference()->getMesh()->getLayout()) 
    {std::cout << "Found attribute with usage: " << attr.usage << " at offset " << attr.offset << " with type " << static_cast<GLGE::u32>(attr.type) << "\n";}

    GLGE::Graphic::VertexLayout layout(ass.reference()->getMesh()->getLayout(), 
        std::pair{GLGE::VertexAttribute::Position {}, GLGE::u64(0)},    //Pos    -> stream 0
        std::pair{GLGE::VertexAttribute::Normal {}, GLGE::u64(1)}       //Normal -> stream 1
    );

    GLGE::Graphic::Mesh cube{*ass.reference()->getMesh(), layout};

    GLGE::Graphic::Image renCol0(win.getResolution(), GLGE::Graphic::PIXEL_FORMAT_RGBA_8_UNORM);
    GLGE::Graphic::Image renDepth(win.getResolution(), GLGE::Graphic::PIXEL_FORMAT_DEPTH_32_FLOAT);
    GLGE::Graphic::Framebuffer renderFBuff({&renCol0}, {&renDepth}, {});
    GLGE::Graphic::RenderTarget renderTarget(&renderFBuff);

    GLGE::World world("Scene 1");
    GLGE::Object camera = world.create<GLGE::Graphic::Component::Camera, GLGE::Transform>(
        "Camera", 
        GLGE::Graphic::Component::Camera{90,0.1,1000, GLGE::vec3(0,0,0)}, 
        GLGE::Transform{GLGE::vec3(-3,0,0), GLGE::Quaternion(GLGE::vec3(0,0,0)), GLGE::vec3(1,1,1)}
    );
    GLGE::Graphic::Renderer renderer(world, &camera, renderTarget);

    GLGE::Graphic::Shader meshShader {
        std::pair{"Vertex", "assets/shader/simple.vert.spv"},
        std::pair{"Fragment", "assets/shader/simple.frag.spv"}
    };
    GLGE::Graphic::ResourceSet renderSet(meshShader.getSet(0), std::pair{"cam", renderer.getCameraBuffer()}, std::pair{"transforms", renderer.getTransformBuffer()}, 
        std::pair{"pointLights", renderer.getPointLightBuffer()}, std::pair{"spotLights", renderer.getSpotLightBuffer()}, std::pair{"directionalLights", renderer.getDirectionalLightBuffer()}
    );
    meshShader.setResources(0, &renderSet);

    GLGE::Graphic::Material mat(meshShader, layout, renderFBuff, GLGE::Graphic::Material::CullMode::BACK, GLGE::Graphic::Material::DepthMode::DEPTH_COMPARE_LESS, true);

    GLGE::Object cubeObj = world.create<GLGE::Transform, GLGE::Graphic::Component::Renderable>("Cube", 
        GLGE::Transform({0,0,0}, {1,0,0,0}, GLGE::vec3{0.5f}),
        GLGE::Graphic::Component::Renderable(&cube, &mat, true)
    );

    GLGE::Graphic::RenderTarget target(&win);
    auto pipe = GLGE::Graphic::RenderPipeline::create(&win, 
        std::pair{"Clear", GLGE::Graphic::Command(GLGE::Graphic::COMMAND_CLEAR, renderTarget, GLGE::u8(0), GLGE::vec4(GLGE::vec3(0.4f),1), GLGE::f32(1), GLGE::u32(0))},
        //TODO: Implement draw command
        std::pair{"Flush", GLGE::Graphic::Command(GLGE::Graphic::COMMAND_COPY, renderTarget, GLGE::u8(0), target, GLGE::u8(0), false, false)}
    );
    pipe.record();

    inst.start();

    while (!win.isClosingRequested()) {
        inst.startMainTick();

        if (win.didResize()) {
            renderFBuff.resize(win.getResolution());
            pipe.record();
        }

        pipe.play();

        inst.endMainTick();
    }

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
    reg->addExample("New Mesh Example - An example to test the new mesh system",
                    &newMeshExample);
}
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

struct FirstPersonController {GLGE::u8 filler;};

static void updateFirstPersonController(GLGE::Transform& transform, GLGE::Graphic::Component::Camera& camera, FirstPersonController) {
    GLGE::Instance* inst = GLGE::Instance::getCurrentInstance();

    float speed = 0.0125 * inst->mainLimiter().getCurrentDeltaTime();
    //compute forward, right and up vector
    GLGE::Quaternion quat(camera.eulerAngles);
    GLGE::vec3 forward = quat * GLGE::vec3(0,0,-speed);
    GLGE::vec3 right = quat * GLGE::vec3(speed,0,0);
    GLGE::vec3 up = quat * GLGE::vec3(0,speed,0);

    if (inst->getKeyboard().pressed()['w'])
    {transform.pos += forward;}
    if (inst->getKeyboard().pressed()['s'])
    {transform.pos -= forward;}
    if (inst->getKeyboard().pressed()['a'])
    {transform.pos -= right;}
    if (inst->getKeyboard().pressed()['d'])
    {transform.pos += right;}
    if (inst->getKeyboard().pressed()[GLGE::Key::Space])
    {transform.pos += up;}
    if (inst->getKeyboard().pressed()[GLGE::Key::LeftShift])
    {transform.pos -= up;}

    float rotSpeed = glm::radians(0.125) * inst->mainLimiter().getCurrentDeltaTime();
    if (inst->getKeyboard().pressed()[GLGE::Key::ArrowLeft])
    {camera.eulerAngles.y += rotSpeed;}
    if (inst->getKeyboard().pressed()[GLGE::Key::ArrowRight])
    {camera.eulerAngles.y -= rotSpeed;}
    if (inst->getKeyboard().pressed()[GLGE::Key::ArrowUp])
    {camera.eulerAngles.x += rotSpeed;}
    if (inst->getKeyboard().pressed()[GLGE::Key::ArrowDown])
    {camera.eulerAngles.x -= rotSpeed;}
    if (inst->getKeyboard().pressed()[GLGE::Key::PageUp])
    {camera.eulerAngles.z += rotSpeed;}
    if (inst->getKeyboard().pressed()[GLGE::Key::PageDown])
    {camera.eulerAngles.z -= rotSpeed;}
}

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

    auto ass = inst.assets().load<GLGE::MeshAsset>("assets/meshes/Suzanne.glb", GLGE::MeshAsset::ASSIMP);
    GLGE::Mesh& mesh = *ass.reference()->getMesh();

    GLGE::Graphic::VertexLayout layout(mesh.getLayout(), 
        std::pair{GLGE::VertexAttribute::Position {}, GLGE::u64(0)},    //Pos    -> stream 0
        std::pair{GLGE::VertexAttribute::UV {},       GLGE::u64(1)},    //UV     -> stream 1
        std::pair{GLGE::VertexAttribute::Normal {},   GLGE::u64(1)}     //Normal -> stream 1
    );

    GLGE::Graphic::Mesh cube{mesh, layout};

    GLGE::Graphic::Image renCol0(win.getResolution(), GLGE::Graphic::PIXEL_FORMAT_RGBA_8_UNORM);
    GLGE::Graphic::Image renDepth(win.getResolution(), GLGE::Graphic::PIXEL_FORMAT_DEPTH_32_FLOAT);
    GLGE::Graphic::Framebuffer renderFBuff({&renCol0}, {&renDepth}, {});
    GLGE::Graphic::RenderTarget renderTarget(&renderFBuff);

    GLGE::World world("Scene 1");
    GLGE::Object camera = world.create<GLGE::Graphic::Component::Camera, GLGE::Transform, FirstPersonController>(
        "Camera", 
        GLGE::Graphic::Component::Camera{90,0.1,1000, GLGE::vec3(0,0,0)}, 
        GLGE::Transform{GLGE::vec3(0,0,10), GLGE::Quaternion(GLGE::vec3(0,0,0)), GLGE::vec3(1,1,1)},
        FirstPersonController {}
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

    GLGE::Object cubeObj = world.create<GLGE::Transform, GLGE::Graphic::Component::Renderable, GLGE::MeshComponent>("Cube", 
        GLGE::Transform({0,0,0}, GLGE::Quaternion({0,0,0}), GLGE::vec3{0.5f}),
        GLGE::Graphic::Component::Renderable(&cube, &mat, true),
        GLGE::MeshComponent {&mesh}
    );
    GLGE::Object skylight = world.create<GLGE::Transform, GLGE::Graphic::Component::DirectionalLight>("Skylight",
        GLGE::Transform({0,0,0}, GLGE::Quaternion({0, glm::radians(45.f), glm::radians(45.f)}), {1,1,1}),
        GLGE::Graphic::Component::DirectionalLight{GLGE::vec3(0.908, 0.912, 0.894), 1.f, 0}
    );

    GLGE::Graphic::RenderTarget target(&win);
    auto pipe = GLGE::Graphic::RenderPipeline::create(&win, 
        std::pair{"Clear",  GLGE::Graphic::Command(GLGE::Graphic::COMMAND_CLEAR, renderTarget, GLGE::u8(0), GLGE::vec4(GLGE::vec3(0.4f),1), GLGE::f32(1), GLGE::u32(0))},
        std::pair{"Render", GLGE::Graphic::Command(GLGE::Graphic::COMMAND_DRAW_WORLD, &renderer)},
        std::pair{"Flush",  GLGE::Graphic::Command(GLGE::Graphic::COMMAND_COPY, renderTarget, GLGE::u8(0), target, GLGE::u8(0), false, false)}
    );
    pipe.record();

    GLGE::System::BakeTransforms(world);

    //ray casting test
    GLGE::Ray r {
        {0,0,10},
        {0,0,-1}
    };
    auto hit = GLGE::System::CastRay(world, r);
    std::cout << "Has Hit?: " << (hit.has_value() ? "true" : "false") << "\n";
    if (hit.has_value()) 
    {std::cout << "Hit:\n    Position: " << hit->position << "\n    Distance: " << hit->distance << "\n";}

    inst.start();

    while (!win.isClosingRequested()) {
        inst.startMainTick();

        if (win.didResize()) {
            renderFBuff.resize(win.getResolution());
            pipe.record();
        }

        //update transforms
        world.each<GLGE::Transform, GLGE::Graphic::Component::Camera, FirstPersonController>(updateFirstPersonController);
        GLGE::System::BakeTransforms(world);
        renderer.update();

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
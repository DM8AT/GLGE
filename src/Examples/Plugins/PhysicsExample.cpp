/**
 * @file PhysicsExample.cpp
 * @author DM8AT
 * @brief implement a simple physics example
 * @version 0.1
 * @date 2026-06-01
 * 
 * @copyright Copyright (c) 2026
 * 
 */
//add the example system
#include "ExamplePluginContract.h"
#include "ExampleBackendFactory.h"

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

GLGE::u8 physicExample(const char *graphicBackendName, const char *videoBackendName) {
    //initialize
    GLGE::Instance::init();

    //the descriptions need to be created here using the provided names
    auto gDescr = createGraphicBackendDescription(graphicBackendName);
    auto vDescr = createVideoBackendDescription(videoBackendName);
    GLGE::Physic::Builtin::Jolt jolt;

    //create the instance
    GLGE::Graphic::Instance gInst(gDescr.get(), vDescr.get());
    GLGE::Physic::Instance  pInst(&jolt);
    GLGE::Instance inst("Physic example", GLGE::Version(1,0,0), std::pair{"Graphic", &gInst}, std::pair{"Physic", &pInst});

    //open a window
    GLGE::Graphic::Window win("Physic Example", {600, 600});
    GLGE::Graphic::RenderTarget window(&win);

    //create the new ECS world
    GLGE::World world("World");

    GLGE::Physic::SphereCollider sphere = 0.5;
    GLGE::Physic::BoxCollider    plane = GLGE::vec3{200, 10, 200};
    GLGE::Physic::BoxCollider    box = GLGE::vec3{1};

    GLGE::Physic::World physicsWorld(world, GLGE::vec3(0,-9.81,0), 5E5, 2ull*1024ull*1024ull*1024ull);
    GLGE::Physic::SimulatedEntity floor = physicsWorld.createEntity(plane);
    GLGE::Physic::SimulatedEntity camBody = physicsWorld.createEntity(sphere, GLGE::Physic::SimulatedEntity::BodyType::KINEMATIC);

    //load a cube model
    auto cube = inst.assets().load<GLGE::Graphic::Asset::Mesh>("assets/meshes/Cube.fbx", GLGE::Graphic::Asset::Mesh::Format::ASSIMP);
    auto cubeRef = cube.reference();

    GLGE::Object camera = world.create<GLGE::Transform, GLGE::Graphic::Component::Camera, FirstPersonController, GLGE::Physic::RigidBody>("Camera", 
        GLGE::Transform(GLGE::vec3(0, 1.5, 3)), 
        GLGE::Graphic::Component::Camera {
            .FOV = 90,
            .clip_near = 0.1,
            .clip_far = 1000.,
            .eulerAngles = GLGE::vec3(0)
        },
        FirstPersonController{},
        GLGE::Physic::RigidBody{&camBody}
    );

    //create a simple render target
    GLGE::Graphic::Image targetImg(win.getSize(), GLGE::Graphic::PIXEL_FORMAT_RGBA_8_UNORM);
    GLGE::Graphic::Image targetDep(win.getSize(), GLGE::Graphic::PIXEL_FORMAT_DEPTH_32_FLOAT);
    GLGE::Graphic::Framebuffer fbuff({&targetImg}, {&targetDep});
    GLGE::Graphic::RenderTarget target(&fbuff);

    //define a vertex layout
    GLGE::Graphic::VertexLayout layout {{
        GLGE::Graphic::VertexAttribute(GLGE::Graphic::VertexAttribute::Type::Position, GLGE::Graphic::VertexAttribute::Format::vec3, 0,  0),
        GLGE::Graphic::VertexAttribute(GLGE::Graphic::VertexAttribute::Type::UV,       GLGE::Graphic::VertexAttribute::Format::vec2, 12, 1),
        GLGE::Graphic::VertexAttribute(GLGE::Graphic::VertexAttribute::Type::Normal,   GLGE::Graphic::VertexAttribute::Format::vec3, 20, 2),
        GLGE::Graphic::VertexAttribute(GLGE::Graphic::VertexAttribute::Type::Tangent,  GLGE::Graphic::VertexAttribute::Format::vec3, 32, 3),
        GLGE::Graphic::VertexAttribute(GLGE::Graphic::VertexAttribute::Type::Color,    GLGE::Graphic::VertexAttribute::Format::vec4, 44, 4)
    }, 60};

    //create the renderer
    GLGE::Graphic::Renderer renderer(world, &camera, target);

    //load default shader
    GLGE::Graphic::Shader shader({
        std::pair{"Vertex",   "assets/shader/simple.vert.spv"},
        std::pair{"Fragment", "assets/shader/simple.frag.spv"}
    });
    GLGE::Graphic::ResourceSet renderSet(shader.getSet(0), std::pair{"cam", renderer.getCameraBuffer()}, std::pair{"transforms", renderer.getTransformBuffer()}, 
        std::pair{"pointLights", renderer.getPointLightBuffer()}, std::pair{"spotLights", renderer.getSpotLightBuffer()}, std::pair{"directionalLights", renderer.getDirectionalLightBuffer()}
    );
    shader.setResources(0, &renderSet);

    GLGE::Graphic::Material mat(shader, layout, fbuff, GLGE::Graphic::Material::CullMode::BACK, GLGE::Graphic::Material::DepthMode::DEPTH_COMPARE_LESS, true);

    GLGE::Object floorObj = world.create<GLGE::Transform, GLGE::Physic::RigidBody, GLGE::Graphic::Component::Renderable>("Floor", 
        GLGE::Transform(GLGE::vec3(0,-5,0), {1,0,0,0}, GLGE::vec3(100,5,100)), 
        GLGE::Physic::RigidBody{.entity = &floor}, 
        GLGE::Graphic::Component::Renderable {
            .mesh = &cubeRef->mesh(),
            .material = &mat,
            .enabled = true
        }
    );

    const size_t cubeCount = 20000;
    std::vector<std::pair<GLGE::Object, GLGE::Physic::SimulatedEntity>> cubes(cubeCount);
    for (size_t i = 0; i < cubeCount; ++i) {
        //create the elements
        cubes[i].second = physicsWorld.createEntity(box, GLGE::Physic::SimulatedEntity::BodyType::DYNAMIC, 1.f, 0.1, 0.f, false);
        cubes[i].first = world.create<GLGE::Transform, GLGE::Physic::RigidBody, GLGE::Graphic::Component::Renderable>("Cube", 
            GLGE::Transform(GLGE::vec3(0,2+i,0), {1,0,0,0}, GLGE::vec3(0.5,0.5,0.5)), 
            GLGE::Physic::RigidBody{.entity = &cubes[i].second}, 
            GLGE::Graphic::Component::Renderable {
                .mesh = &cubeRef->mesh(),
                .material = &mat,
                .enabled = true
            }
        );
    }

    GLGE::Object skylight = world.create<GLGE::Graphic::Component::DirectionalLight, GLGE::Transform>("Skylight",
        GLGE::Graphic::Component::DirectionalLight {.color = GLGE::vec3(1), .intensity = 0.8, .flags = 0}, 
        GLGE::Transform {{0,0,0}, GLGE::vec3{glm::radians(-12.5f), 0, glm::radians(20.f)}, {1,1,1}}
    );

    //update transforms
    GLGE::System::BakeTransforms(world);
    //define how a frame is structured
    GLGE::Graphic::RenderPipeline pipe = GLGE::Graphic::RenderPipeline::create(&win,
        std::pair{"Clear", GLGE::Graphic::Command(GLGE::Graphic::COMMAND_CLEAR, target, GLGE::u8(0), GLGE::vec4(0.5,0.5,0.5,1), 1.f, GLGE::u32(0))},
        std::pair{"Render", GLGE::Graphic::Command(GLGE::Graphic::COMMAND_DRAW_WORLD, &renderer)},
        std::pair{"Copy", GLGE::Graphic::Command(GLGE::Graphic::COMMAND_COPY, target, GLGE::u8(0), window, GLGE::u8(0), false, false)}
    );
    pipe.record();

    inst.start();

    while (!win.isClosingRequested()) {
        inst.startMainTick();
        //handle resizing
        if (win.didResize()) {
            fbuff.resize(win.getResolution());
            pipe.record();
        }

        //camera movement
        world.each<GLGE::Transform, GLGE::Graphic::Component::Camera, FirstPersonController>(updateFirstPersonController);
        //update transforms
        GLGE::System::BakeTransforms(world);
        //update physics
        physicsWorld.update();
        //update the renderer
        renderer.update();

        pipe.play();

        inst.endMainTick();
    }

    inst.shutdown();

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
    reg->addExample("Physics Example - This example shows that physics work.",
                    &physicExample);
}
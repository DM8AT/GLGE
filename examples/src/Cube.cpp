/**
 * @file Cube.cpp
 * @author DM8AT
 * @brief A simple example to show how a 3D cube is rendered
 * @version 0.1
 * @date 2026-09-16
 * 
 * @copyright Copyright (c) 2026
 * 
 */
#define GLGE_NO_THANKS_MSG
#include "GLGE/GLGE.h"

int main(void) {
    GLGE::Instance::init();

    GLGE::Graphic::Instance gInst(new GLGE::Graphic::Builtin::Graphics::OpenGL(), new GLGE::Graphic::Builtin::Video::SDL3());
    GLGE::Instance inst("Instance", {0,1,0}, std::pair{"Graphic", &gInst});

    GLGE::Graphic::Window win {"Window", {600, 600}};
    GLGE::Graphic::Image img(win.getResolution(), GLGE::Graphic::PIXEL_FORMAT_RGBA_8_UNORM);
    GLGE::Graphic::Image dep(win.getResolution(), GLGE::Graphic::PIXEL_FORMAT_DEPTH_32_FLOAT);
    GLGE::Graphic::Framebuffer fbuff({&img}, {&dep});
    GLGE::Graphic::RenderTarget target {&win};
    GLGE::Graphic::RenderTarget fbuffTarget {&fbuff};

    GLGE::World world("Scene");
    GLGE::Object camera = world.create<GLGE::Graphic::Component::Camera, GLGE::Transform>("Camera", GLGE::Graphic::Component::Camera{90.f, 0.1f, 1000.f, GLGE::vec3{0}}, GLGE::Transform({0,0,4}));
    GLGE::Graphic::Renderer renderer(world, &camera, fbuffTarget);

    struct Vertex {
        GLGE::vec3 pos;
        GLGE::vec3 normal;
        GLGE::vec2 tex;
    };
    const Vertex vertices[] = {
        //Front (+Z)
        Vertex {{-1,-1,  1}, { 0, 0, 1}, {0,0}},
        Vertex {{ 1,-1,  1}, { 0, 0, 1}, {1,0}},
        Vertex {{ 1, 1,  1}, { 0, 0, 1}, {1,1}},
        Vertex {{-1, 1,  1}, { 0, 0, 1}, {0,1}},
        //Back (-Z)
        Vertex {{ 1,-1, -1}, { 0, 0,-1}, {0,0}},
        Vertex {{-1,-1, -1}, { 0, 0,-1}, {1,0}},
        Vertex {{-1, 1, -1}, { 0, 0,-1}, {1,1}},
        Vertex {{ 1, 1, -1}, { 0, 0,-1}, {0,1}},
        //Left (-X)
        Vertex {{-1,-1, -1}, {-1, 0, 0}, {0,0}},
        Vertex {{-1,-1,  1}, {-1, 0, 0}, {1,0}},
        Vertex {{-1, 1,  1}, {-1, 0, 0}, {1,1}},
        Vertex {{-1, 1, -1}, {-1, 0, 0}, {0,1}},
        //Right (+X)
        Vertex {{ 1,-1,  1}, { 1, 0, 0}, {0,0}},
        Vertex {{ 1,-1, -1}, { 1, 0, 0}, {1,0}},
        Vertex {{ 1, 1, -1}, { 1, 0, 0}, {1,1}},
        Vertex {{ 1, 1,  1}, { 1, 0, 0}, {0,1}},
        //Top (+Y)
        Vertex {{-1, 1,  1}, {0, 1, 0}, {0,0}},
        Vertex {{ 1, 1,  1}, {0, 1, 0}, {1,0}},
        Vertex {{ 1, 1, -1}, {0, 1, 0}, {1,1}},
        Vertex {{-1, 1, -1}, {0, 1, 0}, {0,1}},
        //Bottom (-Y)
        Vertex {{-1,-1, -1}, {0,-1, 0}, {0,0}},
        Vertex {{ 1,-1, -1}, {0,-1, 0}, {1,0}},
        Vertex {{ 1,-1,  1}, {0,-1, 0}, {1,1}},
        Vertex {{-1,-1,  1}, {0,-1, 0}, {0,1}},
    };
    const GLGE::Triangle indices[] = {
        //Front (+Z)
        GLGE::Triangle{0,  1,  2},
        GLGE::Triangle{0,  2,  3},

        //Back (-Z)
        GLGE::Triangle{4,  5,  6},
        GLGE::Triangle{4,  6,  7},

        //Left (-X)
        GLGE::Triangle{8,  9, 10},
        GLGE::Triangle{8, 10, 11},

        //Right (+X)
        GLGE::Triangle{12, 13, 14},
        GLGE::Triangle{12, 14, 15},

        //Top (+Y)
        GLGE::Triangle{16, 17, 18},
        GLGE::Triangle{16, 18, 19},

        //Bottom (-Y)
        GLGE::Triangle{20, 21, 22},
        GLGE::Triangle{20, 22, 23},
    };

    GLGE::Mesh::VertexLayout layout {{
        GLGE::Mesh::VertexAttribute {GLGE::Mesh::VertexAttribute::Type::vec3, GLGE::VertexAttribute::Position {}, offsetof(Vertex, Vertex::pos)},
        GLGE::Mesh::VertexAttribute {GLGE::Mesh::VertexAttribute::Type::vec3, GLGE::VertexAttribute::Normal {},   offsetof(Vertex, Vertex::normal)},
        GLGE::Mesh::VertexAttribute {GLGE::Mesh::VertexAttribute::Type::vec2, GLGE::VertexAttribute::UV {},       offsetof(Vertex, Vertex::tex)},
    }, sizeof(Vertex)};

    GLGE::Mesh cubeMesh(1, layout);
    cubeMesh.addLOD(GLGE::Mesh::LOD{vertices, sizeof(vertices)/sizeof(*vertices), std::vector<GLGE::Triangle>{indices, indices + (sizeof(indices)/sizeof(*indices))}, layout, 0.f, true});
    GLGE::Graphic::VertexLayout gLayout{layout, std::pair{GLGE::VertexAttribute::Position {}, GLGE::u64{0}}, std::pair{GLGE::VertexAttribute::UV {}, GLGE::u64{1}}, std::pair{GLGE::VertexAttribute::Normal {}, GLGE::u64{2}}};
    GLGE::Graphic::Mesh cubeGMesh(cubeMesh, gLayout);

    GLGE::Graphic::Shader shader({
        std::pair{"Vertex",   GLGE::Graphic::Shader::Source("examples/assets/shader/simple.vert.spv")},
        std::pair{"Fragment", GLGE::Graphic::Shader::Source("examples/assets/shader/simple.frag.spv")}
    });
    GLGE::Graphic::ResourceSet resources(shader.getSet(0), std::pair{"cam", renderer.getCameraBuffer()}, std::pair{"transforms", renderer.getTransformBuffer()}, 
        std::pair{"pointLights", renderer.getPointLightBuffer()}, std::pair{"spotLights", renderer.getSpotLightBuffer()}, std::pair{"directionalLights", renderer.getDirectionalLightBuffer()}
    );
    shader.setResources(0, &resources);

    GLGE::Graphic::Material mat(shader, gLayout, fbuff, GLGE::Graphic::Material::CullMode::BACK, GLGE::Graphic::Material::DepthMode::DEPTH_COMPARE_LESS, true);

    GLGE::Object cube = world.create<GLGE::Graphic::Component::Renderable, GLGE::Transform>("Cube", 
        GLGE::Graphic::Component::Renderable {&cubeGMesh, &mat, true}, 
        GLGE::Transform({0,0,0}, GLGE::Quaternion{GLGE::vec3{glm::radians(45.f), glm::radians(45.f),0.f}})
    );
    GLGE::Object sun = world.create<GLGE::Graphic::Component::DirectionalLight, GLGE::Transform>("Sun", 
        GLGE::Graphic::Component::DirectionalLight {GLGE::vec3{1, 0.98, 0.9}, 1.f, 0}, 
        GLGE::Transform({0,0,0}, GLGE::Quaternion(GLGE::vec3{glm::radians(-45.f),0,0}))
    );

    GLGE::Graphic::CommandStream stream {
        std::pair{"Clear", std::make_unique<GLGE::Graphic::Cmd::Clear>(fbuff, 0, GLGE::vec4{0.5,0.5,0.5,1})},
        std::pair{"Draw",  std::make_unique<GLGE::Graphic::Cmd::Render>(renderer)},
        std::pair{"Flip",  std::make_unique<GLGE::Graphic::Cmd::Copy>(fbuff, win, 0)}
    };
    GLGE::Graphic::CommandExecutor exec(&win);

    inst.start();

    while (!win.isClosingRequested()) {
        inst.startMainTick();

        if (win.didResize()) {
            fbuff.resize(win.getResolution());
        }

        GLGE::System::BakeTransforms(world);
        renderer.update();
        exec.dispatch(stream);

        inst.endMainTick();
    }
}
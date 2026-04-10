/**
 * @file Renderer.cpp
 * @author DM8AT
 * @brief implement the OpenGL renderer backend
 * @version 0.1
 * @date 2026-04-05
 * 
 * @copyright Copyright (c) 2026
 * 
 */
//include the renderer override
#include "Graphic/Backend/Builtin/Graphics/OpenGL/Renderer.h"
//add mappings
#include "__Mapper.h"
//add components
#include "Graphic/Components.h"

//get the window and framebuffer frontends and backend implementations
#include "Graphic/Window.h"
#include "Graphic/Framebuffer.h"
#include "Graphic/Backend/Builtin/Graphics/OpenGL/Window.h"
#include "Graphic/Backend/Builtin/Graphics/OpenGL/Framebuffer.h"

//add OpenGL instances
#include "Graphic/Backend/Builtin/Graphics/OpenGL/Instance.h"

//add OpenGL materials and meshes
#include "Graphic/Material.h"
#include "Graphic/Mesh.h"
#include "Graphic/Backend/Builtin/Graphics/OpenGL/Material.h"
#include "Graphic/Backend/Builtin/Graphics/OpenGL/MeshPool.h"
#include "Graphic/Backend/Video/Window.h"

//add OpenGL buffers
#include "Graphic/Backend/Builtin/Graphics/OpenGL/Buffer.h"

//add the frontend buffer
#include "Graphic/Buffer.h"

/**
 * @brief store the structure of an element indirect draw command
 */
struct DrawElementsIndirectCommand {
    /**
     * @brief store the amount of indices to use
     */
    GLGE::u32 count;
    /**
     * @brief store the amount of instances to draw
     */
    GLGE::u32 instanceCount;
    /**
     * @brief store the first index in the global buffer
     */
    GLGE::u32 firstIndex;
    /**
     * @brief store the base vertex
     */
    GLGE::i32 baseVertex;
    /**
     * @brief store the base instance
     */
    GLGE::u32 baseInstance;
};

static constexpr GLGE::u64 __compressQuaternion(const GLGE::Quaternion& quaternion) {
    //pack a float to 16 bits
    auto packFloat = [](float val) -> GLGE::u64
    {return static_cast<GLGE::u64>(static_cast<GLGE::u16>((glm::clamp(val, -1.f, 1.f) * 0.5 + 0.5) * GLGE::f32(UINT16_MAX)));};

    //pack all elements of the quaternion
    return packFloat(quaternion.x)      | (packFloat(quaternion.y)<<16) | 
          (packFloat(quaternion.z)<<32) | (packFloat(quaternion.w)<<48);
}

GLGE::Graphic::Backend::Graphic::OpenGL::Renderer::Renderer(World& world, Object* camera, RenderTarget target) 
 : GLGE::Graphic::Backend::Graphic::Renderer(world, camera, target)
{
    m_cameraBuffer = new GLGE::Graphic::Buffer(GLGE::Graphic::Buffer::Type::UNIFORM, nullptr, sizeof(CameraData), GLGE::Graphic::Buffer::Usage::STREAMING_UPLOAD);
    m_transformBuffer = new GLGE::Graphic::Buffer(GLGE::Graphic::Buffer::Type::STORAGE, nullptr, sizeof(TransformData), GLGE::Graphic::Buffer::Usage::STREAMING_UPLOAD);
}

GLGE::Graphic::Backend::Graphic::OpenGL::Renderer::~Renderer() 
{
    //if a command buffer exists, delete it
    if (m_cmdBuffer) 
    {glDeleteBuffers(1, &m_cmdBuffer);}
    //clean up the buffers
    delete m_cameraBuffer; m_cameraBuffer = nullptr;
    delete m_transformBuffer; m_transformBuffer = nullptr;
}

void GLGE::Graphic::Backend::Graphic::OpenGL::Renderer::record(CommandBuffer& cmdBuff) {
    //store all objects sorted by the materials
    std::unordered_map<GLGE::Graphic::Material*, std::vector<std::pair<Mesh*, Object>>> objs;
    size_t total = 0;
    auto reg = [&objs, &total](Tiny::ECS::Entity ent, const Component::Renderable& render) {
        //only add the mesh if it is enabled
        if (render.enabled) 
        {objs[render.material].emplace_back(render.mesh, Object(ent)); ++total;}
    };
    m_world->each<Component::Renderable>(reg);
    
    //prepare the indirect draw commands and flatten the entities IN THE SAME ORDER
    m_entities.clear();
    m_entities.reserve(total);
    std::vector<DrawElementsIndirectCommand> drawCommands;
    drawCommands.reserve(total);
    u32 meshIdx = 0;
    for (const auto& [mat, meshes] : objs) {
        for (const auto& [mesh, obj] : meshes) {
            //get the base LOD
            const auto& lod = mesh->getLODInfo(0);
            const auto& idx = mesh->getIndexSection();
            const auto& vtx = mesh->getVertexSection();
            //create the draw information
            drawCommands.emplace_back(
                static_cast<u32>(lod.index.count),
                1,
                static_cast<u32>(idx.offset/idx.size + lod.index.offset/lod.index.size),  //due to alignment this division is guaranteed to produce a valid integer
                static_cast<i32>(vtx.offset/vtx.size + lod.vertex.offset/lod.vertex.size), //see above
                meshIdx
            );
            //store the object
            m_entities.push_back(obj);
            //advance the mesh
            ++meshIdx;
        }
    }
    //re-create the corresponding GPU buffer
    if (m_cmdBuffer) 
    {glDeleteBuffers(1, &m_cmdBuffer);}
    glCreateBuffers(1, &m_cmdBuffer);
    glNamedBufferStorage(m_cmdBuffer, drawCommands.size()*sizeof(*drawCommands.data()), drawCommands.data(), 0);
    //resize the transform buffer
    m_transformBuffer->resize(sizeof(TransformData) * total);

    //a static helper function that invokes the draw
    static void (*drawer)(DrawElementsIndirectCommand*, size_t) = [](DrawElementsIndirectCommand* start, size_t count) 
    {glMultiDrawElementsIndirect(GL_TRIANGLES, GL_UNSIGNED_INT, start, count, 0);};

    //a static helper function to bind the render target
    static void (*target_select)(u32, u32, GLGE::Graphic::Backend::Video::Window*, void*) = [](u32 target, u32 targetCount, GLGE::Graphic::Backend::Video::Window* win, void* context) {
        if (target == 0) 
        {win->onMakeCurrent(context);}
        else {
            //activate all color attachments
            static constexpr const GLenum bufs[16] = {GL_COLOR_ATTACHMENT0, GL_COLOR_ATTACHMENT1, GL_COLOR_ATTACHMENT2, GL_COLOR_ATTACHMENT3, GL_COLOR_ATTACHMENT4, GL_COLOR_ATTACHMENT5, GL_COLOR_ATTACHMENT6, GL_COLOR_ATTACHMENT7, GL_COLOR_ATTACHMENT8, GL_COLOR_ATTACHMENT9, GL_COLOR_ATTACHMENT10, GL_COLOR_ATTACHMENT11, GL_COLOR_ATTACHMENT12, GL_COLOR_ATTACHMENT13, GL_COLOR_ATTACHMENT14, GL_COLOR_ATTACHMENT15};
            glNamedFramebufferDrawBuffers(target, targetCount, bufs);
        }
        glBindFramebuffer(GL_FRAMEBUFFER, target);
    };

    //a static helper function to bind a draw indirect buffer
    static void (*prepare_draw)(u32) = [](u32 indirectBuffer) 
    {glBindBuffer(GL_DRAW_INDIRECT_BUFFER, indirectBuffer);};

    //get the target
    u32 target = 0;
    u32 targetCount = 1;
    GLGE::Graphic::Backend::Video::Window* window = nullptr;
    void* context = nullptr;
    switch (m_target.getType()) {
    case GLGE::Graphic::RenderTarget::WINDOW: {
            auto win = reinterpret_cast<GLGE::Graphic::Window*>(m_target.getTarget());
            window = win->getVideoWindow();
            context = reinterpret_cast<GLGE::Graphic::Backend::Graphic::OpenGL::Instance*>(win->getGraphicInstance()->getGraphicBackendInstance().get())->getContext();
        } break;
    case GLGE::Graphic::RenderTarget::FRAMEBUFFER:
        target = reinterpret_cast<GLGE::Graphic::Backend::Graphic::OpenGL::Framebuffer*>(reinterpret_cast<GLGE::Graphic::Framebuffer*>(m_target.getTarget())->getBackend().get())->getHandle();
        targetCount = reinterpret_cast<GLGE::Graphic::Framebuffer*>(m_target.getTarget())->getBackend()->getColorAttachmentCount();
        break;
    
    default:
        throw Exception("Invalid render target type", "GLGE::Graphic::Backend::Graphic::OpenGL::Renderer::render");
        break;
    }
    
    //record the target selection
    cmdBuff.addCommand(target_select, target, targetCount, window, context);
    //prepare the drawing
    cmdBuff.addCommand(prepare_draw, m_cmdBuffer);

    //store a pointer on the start of the draw list
    DrawElementsIndirectCommand* ptr = 0;
    //iterate over all materials to draw them one by one
    for (auto& [mat, meshes] : objs) {
        //bind the material
        GLGE::Graphic::Backend::Graphic::OpenGL::Material* material = reinterpret_cast<GLGE::Graphic::Backend::Graphic::OpenGL::Material*>(mat->getBackend().get());
        material->bind(&cmdBuff, 0);

        //draw the mesh
        cmdBuff.addCommand(drawer, ptr, meshes.size());
        //advance the pointer
        ptr += meshes.size();
    }
}

void GLGE::Graphic::Backend::Graphic::OpenGL::Renderer::update() {
    //update the camera data depending on if a camera exists
    bool isCam = false;
    Component::Camera* cam = nullptr;
    //store the full camera data
    CameraData data;
    if (m_camera) {
        cam = m_world->get<Component::Camera>(*m_camera);
        isCam = bool(cam);
    }
    if (isCam) {
        //compute the aspect
        f32 aspect = 1.f;
        switch (m_target.getType())
        {
        case GLGE::Graphic::RenderTarget::WINDOW: {
                auto size = reinterpret_cast<GLGE::Graphic::Window*>(m_target.getTarget())->getSize();
                aspect = f32(size.x) / f32(size.y);
            } break;
        case GLGE::Graphic::RenderTarget::FRAMEBUFFER: {
                auto size = reinterpret_cast<GLGE::Graphic::Framebuffer*>(m_target.getTarget())->getBackend()->getColorAttachment(0)->getSize();
                aspect = f32(size.x) / f32(size.y);
            } break;
        
        default:
            break;
        }
        //store the settings
        data.fov = glm::radians(cam->FOV);
        data.proj_near = cam->near;
        data.proj_far  = cam->far;
        //compute the projection matrix
        data.projection = glm::perspective(data.fov, aspect, data.proj_near, data.proj_far);

        //get the position (this can be Transform, Transform2D or none)
        Transform* transf = m_world->get<Transform>(*m_camera);

        if (transf) {
            //transform in use

            //copy position
            data.position = transf->pos;

            //compress the rotation in a quaternion
            Quaternion q(vec3(0, cam->eulerAngles.y, 0));
            q *= Quaternion(vec3(cam->eulerAngles.x, 0, 0));
            q *= Quaternion(vec3(0, 0, cam->eulerAngles.z));
            u64 quat = __compressQuaternion(q);
            data.compressedQuaternion_i_j = *reinterpret_cast<GLGE::u32*>(&quat);
            data.compressedQuaternion_k_w = *(reinterpret_cast<GLGE::u32*>(&quat)+1);
        } else {
            //check transform 2D
            Transform2D* transf2d = m_world->get<Transform2D>(*m_camera);
            if (transf2d) {
                //use transform 2d
                data.position.x = transf2d->pos.x;
                data.position.y = transf2d->pos.y;

                //compress the rotation in a quaternion
                Quaternion q(vec3(0, 0, cam->eulerAngles.z));
                u64 quat = __compressQuaternion(q);
                data.compressedQuaternion_i_j = *reinterpret_cast<GLGE::u32*>(&quat);
                data.compressedQuaternion_k_w = *(reinterpret_cast<GLGE::u32*>(&quat)+1);
            } else {
                //use default values (position = {0,0,0}, rotation = {w:1, rest:0})
                data.position = {0,0,0};
                constexpr u64 quat = __compressQuaternion(Quaternion(1,0,0,0));
                data.compressedQuaternion_i_j = *reinterpret_cast<const GLGE::u32*>(&quat);
                data.compressedQuaternion_k_w = *(reinterpret_cast<const GLGE::u32*>(&quat)+1);
            }
        }
    } else {
        //no camera, set the projection to normal (use default constructor)

        //use default position values (position = {0,0,0}, rotation = {w:1, rest:0})
        data.position = {0,0,0};
        constexpr u64 quat = __compressQuaternion(Quaternion(1,0,0,0));
        data.compressedQuaternion_i_j = *reinterpret_cast<const GLGE::u32*>(&quat);
        data.compressedQuaternion_k_w = *(reinterpret_cast<const GLGE::u32*>(&quat)+1);
    }
    //update the camera data
    m_cameraBuffer->write(&data, sizeof(data), 0);

    //iterate over all known objects and update the transforms
    for (size_t i = 0; i < m_entities.size(); ++i) {
        //store the data to write
        TransformData data  {
            .position {0,0,0},
            .compressedQuaternion_i_j = 2147450879,
            .compressedQuaternion_k_w = 4294934527,
            .scale {1,1,1}
        };

        //try to get the transform
        Transform* transf = m_world->get<Transform>(m_entities[i]);

        //check if a transform was found
        if (transf) {
            //use the 3D transform
            data.position = transf->pos;
            data.scale    = transf->scale;
            u64 quat = __compressQuaternion(transf->rot);
            data.compressedQuaternion_i_j = *reinterpret_cast<const GLGE::u32*>(&quat);
            data.compressedQuaternion_k_w = *(reinterpret_cast<const GLGE::u32*>(&quat)+1);
        } else {
            //try to load a 2D transform
            Transform2D* transf2d = m_world->get<Transform2D>(m_entities[i]);

            //if a 2D transform was found, use it
            if (transf2d) {
                data.position = vec3(transf2d->pos.x, transf2d->pos.y, 0);
                data.scale    = vec3(transf2d->scale.x, transf2d->scale.y, 1);
                u64 quat = __compressQuaternion(Quaternion(vec3(0,0,transf2d->angle)));
                data.compressedQuaternion_i_j = *reinterpret_cast<const GLGE::u32*>(&quat);
                data.compressedQuaternion_k_w = *(reinterpret_cast<const GLGE::u32*>(&quat)+1);
            }

            //safe fallback allready provided
        }

        //write the data
        m_transformBuffer->write(&data, sizeof(data), sizeof(TransformData) * i);
    }
}
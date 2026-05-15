/**
 * @file Renderer.cpp
 * @author DM8AT
 * @brief implement the vulkan renderer
 * @version 0.1
 * @date 2026-05-14
 * 
 * @copyright Copyright (c) 2026
 * 
 */
//include the renderer override
#include "Graphic/Backend/Builtin/Graphics/Vulkan/Renderer.h"
//add components
#include "Graphic/Components.h"

//get the window and framebuffer frontends and backend implementations
#include "Graphic/Window.h"
#include "Graphic/Framebuffer.h"
#include "Graphic/Backend/Builtin/Graphics/Vulkan/Window.h"
#include "Graphic/Backend/Builtin/Graphics/Vulkan/Framebuffer.h"

//add vulkan command buffers
#include "Graphic/Backend/Builtin/Graphics/Vulkan/CommandBuffer.h"

//add Vulkan instances
#include "Graphic/Backend/Builtin/Graphics/Vulkan/Instance.h"

//add Vulkan materials and meshes
#include "Graphic/Material.h"
#include "Graphic/Mesh.h"
#include "Graphic/Backend/Builtin/Graphics/Vulkan/Material.h"
#include "Graphic/Backend/Builtin/Graphics/Vulkan/Shader.h"
#include "Graphic/Backend/Builtin/Graphics/Vulkan/MeshPool.h"
#include "Graphic/Backend/Video/Window.h"

//add Vulkan buffers
#include "Graphic/Backend/Builtin/Graphics/Vulkan/Buffer.h"
//add Vulkan resource sets
#include "Graphic/Backend/Builtin/Graphics/Vulkan/ResourceSet.h"

//add the frontend buffer
#include "Graphic/Buffer.h"

//add the Vulkan contract
#include "Graphic/Backend/Video/APIContracts/Vulkan.h"

//add shared stuff
#include "Shared.h"

static constexpr GLGE::u64 __compressQuaternion(const GLGE::Quaternion& quaternion) {
    //pack a float to 16 bits
    auto packFloat = [](float val) -> GLGE::u64
    {return static_cast<GLGE::u64>(static_cast<GLGE::u16>((glm::clamp(val, -1.f, 1.f) * 0.5 + 0.5) * GLGE::f32(UINT16_MAX)));};

    //pack all elements of the quaternion
    return packFloat(quaternion.x)      | (packFloat(quaternion.y)<<16) | 
          (packFloat(quaternion.z)<<32) | (packFloat(quaternion.w)<<48);
}

GLGE::Graphic::Backend::Graphic::Vulkan::Renderer::Renderer(World& world, Object* camera, RenderTarget target) 
 : GLGE::Graphic::Backend::Graphic::Renderer(world, camera, target)
{
    //transformation information
    m_cameraBuffer = new GLGE::Graphic::Buffer(GLGE::Graphic::Buffer::Type::UNIFORM, nullptr, sizeof(CameraData), GLGE::Graphic::Buffer::Usage::STREAMING_UPLOAD);
    m_transformBuffer = new GLGE::Graphic::Buffer(GLGE::Graphic::Buffer::Type::STORAGE, nullptr, sizeof(TransformData), GLGE::Graphic::Buffer::Usage::STREAMING_UPLOAD);

    //light information
    m_pointLightBuffer = new GLGE::Graphic::Buffer(GLGE::Graphic::Buffer::Type::STORAGE, nullptr, sizeof(PointLightData),       GLGE::Graphic::Buffer::Usage::STREAMING_UPLOAD);
    m_spotLightBuffer  = new GLGE::Graphic::Buffer(GLGE::Graphic::Buffer::Type::STORAGE, nullptr, sizeof(SpotLightData),        GLGE::Graphic::Buffer::Usage::STREAMING_UPLOAD);
    m_dirLightBuffer   = new GLGE::Graphic::Buffer(GLGE::Graphic::Buffer::Type::STORAGE, nullptr, sizeof(DirectionalLightData), GLGE::Graphic::Buffer::Usage::STREAMING_UPLOAD);

    //draw buffer
    m_drawBuffer       = new GLGE::Graphic::Buffer(GLGE::Graphic::Buffer::Type::STORAGE, nullptr, sizeof(VkDrawIndexedIndirectCommand), GLGE::Graphic::Buffer::Usage::GPU_ONLY);
}

GLGE::Graphic::Backend::Graphic::Vulkan::Renderer::~Renderer() {
    //clean up the buffers
    delete m_cameraBuffer; m_cameraBuffer = nullptr;
    delete m_transformBuffer; m_transformBuffer = nullptr;
    delete m_pointLightBuffer; m_pointLightBuffer = nullptr;
    delete m_spotLightBuffer; m_spotLightBuffer = nullptr;
    delete m_dirLightBuffer; m_dirLightBuffer = nullptr;
    delete m_drawBuffer; m_drawBuffer = nullptr;
}

void GLGE::Graphic::Backend::Graphic::Vulkan::Renderer::record(GLGE::Graphic::Backend::Graphic::CommandBuffer& cmdBuff) {
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
    std::vector<VkDrawIndexedIndirectCommand> drawCommands;
    drawCommands.reserve(total);
    u32 meshIdx = 0;
    for (const auto& [mat, meshes] : objs) {
        for (const auto& [mesh, obj] : meshes) {
            //get the base LOD
            const auto& lod = mesh->getLODInfo(0);
            const auto& idx = mesh->getIndexSection();
            const auto& vtx = mesh->getVertexSection();
            //create the draw information
            drawCommands.push_back(VkDrawIndexedIndirectCommand{
                    .indexCount = static_cast<u32>(lod.index.count),
                    .instanceCount = 1,
                    .firstIndex = static_cast<u32>(idx.offset/idx.size + lod.index.offset/lod.index.size), //due to alignment this division is guaranteed to produce a valid integer
                    .vertexOffset = static_cast<i32>(vtx.offset/vtx.size + lod.vertex.offset/lod.vertex.size), //see above
                    .firstInstance = meshIdx
                }
            );
            //store the object
            m_entities.push_back(obj);
            //advance the mesh
            ++meshIdx;
        }
    }
    //re-create the corresponding GPU buffer
    m_drawBuffer->resize(drawCommands.size()*sizeof(*drawCommands.data()));
    static_cast<GLGE::Graphic::Backend::Graphic::Vulkan::Buffer*>(m_drawBuffer->getBackendReference().get())->asyncUpload(drawCommands.data(), 0, drawCommands.size()*sizeof(*drawCommands.data()));
    //resize the transform buffer
    m_transformBuffer->resize(sizeof(TransformData) * total);

    //get the indirect buffer
    VkBuffer indirectBuffer = reinterpret_cast<VkBuffer>(static_cast<GLGE::Graphic::Backend::Graphic::Vulkan::Buffer*>(m_drawBuffer->getBackendReference().get())->getBuffer());
    //iterate over all command buffers
    auto cbuffs = static_cast<GLGE::Graphic::Backend::Graphic::Vulkan::CommandBuffer*>(&cmdBuff)->getBuffers();
    for (size_t i = 0; i < cbuffs.size(); ++i) {
        //extract the vulkan command buffer
        VkCommandBuffer cb = reinterpret_cast<VkCommandBuffer>(cbuffs[i]);

        //iterate over all materials
        for (auto& [mat, meshes] : objs) {
            //bind the material
            GLGE::Graphic::Backend::Graphic::Vulkan::Material* material = static_cast<GLGE::Graphic::Backend::Graphic::Vulkan::Material*>(mat->getBackend().get());
            //get the vulkan framebuffer
            auto* vkFbuff = static_cast<GLGE::Graphic::Backend::Graphic::Vulkan::Framebuffer*>(material->getFbuff().get());
            auto size = vkFbuff->getColorAttachmentCount() ? vkFbuff->getColorAttachment(0)->getSize() : vkFbuff->getDepthAttachment(0)->getSize();
            //get the shader frontend
            auto* shaderFrontend = material->getShader().get()->getFrontend();

            //extract the resource sets
            std::vector<VkDescriptorSet> sets;
            sets.reserve(shaderFrontend->getSetCount());
            for (size_t i = 0; i < shaderFrontend->getSetCount(); ++i) {
                if (!shaderFrontend->hasSet(i)) {continue;}
                auto* vkResourceSet = static_cast<GLGE::Graphic::Backend::Graphic::Vulkan::ResourceSet*>(shaderFrontend->getResources(0)->getBackend().get());
                sets.push_back(reinterpret_cast<VkDescriptorSet>(vkResourceSet->getDescriptorSet()));
            }

            //start the render pass
            VkRenderPassBeginInfo renPassBeg {};
            renPassBeg.sType = VK_STRUCTURE_TYPE_RENDER_PASS_BEGIN_INFO;
            renPassBeg.framebuffer = static_cast<VkFramebuffer>(vkFbuff->getFramebuffer());
            renPassBeg.renderPass = static_cast<VkRenderPass>(vkFbuff->getRenderPass());
            renPassBeg.renderArea.offset = {0,0};
            renPassBeg.renderArea.extent = {size.x, size.y};
            vkCmdBeginRenderPass(cb, &renPassBeg, VK_SUBPASS_CONTENTS_INLINE);
            //bind descriptor sets
            vkCmdBindDescriptorSets(cb, VK_PIPELINE_BIND_POINT_GRAPHICS, reinterpret_cast<VkPipelineLayout>(material->getPipelineLayout()), 0, sets.size(), sets.data(), 0, nullptr);
            //bind the vertex and index buffer
            auto* meshPool = static_cast<GLGE::Graphic::Backend::Graphic::Vulkan::MeshPool*>(material->getVertexLayout()->getPool().get());
            VkBuffer vbo = reinterpret_cast<VkBuffer>(meshPool->getVbo());
            VkDeviceSize offs = 0;
            vkCmdBindVertexBuffers(cb, 0, 1, &vbo, &offs);
            VkBuffer ibo = reinterpret_cast<VkBuffer>(meshPool->getIbo());
            vkCmdBindIndexBuffer(cb, ibo, 0, VK_INDEX_TYPE_UINT32);
            //start the pipeline
            vkCmdBindPipeline(cb, VK_PIPELINE_BIND_POINT_GRAPHICS, reinterpret_cast<VkPipeline>(material->getPipeline()));
            //set the dynamic states
            VkViewport viewport {};
            viewport.x = 0;
            viewport.y = 0;
            viewport.minDepth = 0.f;
            viewport.maxDepth = 1.f;
            viewport.width = size.x;
            viewport.height = size.y;
            vkCmdSetViewport(cb, 0, 1, &viewport);
            VkRect2D scissor {};
            scissor.offset = {0,0};
            scissor.extent = {size.x, size.y};
            vkCmdSetScissor(cb, 0, 1, &scissor);
            //draw all the meshes
            vkCmdDrawIndexedIndirect(cb, indirectBuffer, 0, meshes.size(), sizeof(VkDrawIndexedIndirectCommand));

            //finish the render pass
            vkCmdEndRenderPass(cb);
        }
    }

    //gather up all the light sources
    m_pointLights.clear();
    m_spotLights.clear();
    m_directionalLights.clear();
    auto pointLightAddr       = [this](const Tiny::ECS::Entity& entity, const Component::PointLight&)      {this->m_pointLights.emplace_back(entity);};
    auto spotLightAddr        = [this](const Tiny::ECS::Entity& entity, const Component::SpotLight&)       {this->m_spotLights.emplace_back(entity);};
    auto directionalLightAddr = [this](const Tiny::ECS::Entity& entity, const Component::DirectionalLight&){this->m_directionalLights.emplace_back(entity);};
    m_world->each<Component::PointLight>      (pointLightAddr);
    m_world->each<Component::SpotLight>       (spotLightAddr);
    m_world->each<Component::DirectionalLight>(directionalLightAddr);
    //create the light buffers
    m_pointLightBuffer->resize(sizeof(PointLightData)*((m_pointLights.size() == 0) ? 1 : m_pointLights.size()), false);
    m_spotLightBuffer->resize(sizeof(SpotLightData)*((m_spotLights.size() == 0) ? 1 : m_spotLights.size()), false);
    m_dirLightBuffer->resize(sizeof(DirectionalLightData)*((m_directionalLights.size() == 0) ? 1 : m_directionalLights.size()), false);
}

void GLGE::Graphic::Backend::Graphic::Vulkan::Renderer::update() {
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
                auto size = reinterpret_cast<GLGE::Graphic::Window*>(m_target.getTarget())->getResolution();
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
        data.proj_near = cam->clip_near;
        data.proj_far  = cam->clip_far;
        //compute the projection matrix
        data.projection = glm::perspectiveRH_ZO(data.fov, aspect, data.proj_near, data.proj_far);
        //vulkan y is inverted
        data.projection[1][1] *= -1.f;

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

    //convert a color to a packed color
    auto col = [](const vec3& color) -> u32 {return ((static_cast<u32>(glm::min(color.r, 1.f)*0xff) & 0xff)<<24) | 
                                                    ((static_cast<u32>(glm::min(color.g, 1.f)*0xff) & 0xff)<<16) | 
                                                    ((static_cast<u32>(glm::min(color.b, 1.f)*0xff) & 0xff)<< 8);};

    //start at maximum to instantly wrap around to the minimum
    size_t lightId = SIZE_MAX;
    //the minimum intensity before culling
    const constexpr float e = 1E-4;
    //iterate over all point lights
    for (const auto& obj : m_pointLights) {
        //next light
        ++lightId;

        //store the GPU data
        PointLightData data;

        //extract the point light data
        Component::PointLight* light = m_world->get<Component::PointLight>(obj);

        //only continue of point light data exists
        if (!light) {continue;}

        //store the light data
        data.color = col(light->color);
        data.intensity = light->intensity;
        data.radius = light->radius;
        data.fallof_linear = light->fallof_linear;
        data.fallof_quadratic = light->fallof_quadratic;
        //compute the culling distance
        /*
        Intensity computation: Intensity(Distance) = Intensity / (1 + fallof_linear * Distance + fallof_quadratic * Distance*Distance)
        Final intensity should be less than some epsilon (it will never be zero, but after some distance it is too little to matter)
        Let's assume epsilon = e
        Then we should compute the minimum safe culling distance:
        e = Intensity / (fallof_linear * Distance + fallof_quadratic * Distance*Distance)
        Let's solve for Distance (and use the positive evaluation since the distance is never negative):
        Distance = (sqrt(4 * Intensity * e * fallof_quadratic + e^2 * fallof_linear^2) - e * fallof_linear) / (2 * e * fallof_quadratic)
        */
        float cullDistance = (glm::sqrt(4.f * light->intensity * e * light->fallof_quadratic + e*e * light->fallof_linear*light->fallof_linear) - e * light->fallof_linear) / (2 * e * light->fallof_quadratic);
        //store the cull distance
        data.cullDistance = cullDistance;

        //fill in the position
        Transform* transf = m_world->get<Transform>(obj);
        if (transf) {
            //copy the position
            data.pos = transf->pos;
        } else {
            //try to use a transform 2D
            Transform2D* transf2d = m_world->get<Transform2D>(obj);
            if (transf2d) {
                //copy and upcast
                data.pos = vec3(transf2d->pos, 0);
            } else {
                //zero the position
                data.pos = vec3{0,0,0};
            }
        }

        //upload the data
        m_pointLightBuffer->write(&data, sizeof(data), sizeof(data)*lightId);
    }
    //iterate over all spot lights
    lightId = SIZE_MAX;
    for (const auto& obj : m_spotLights) {
        //next light
        ++lightId;

        //store the GPU data
        SpotLightData data;

        //extract the spot light data
        Component::SpotLight* light = m_world->get<Component::SpotLight>(obj);

        //only continue of spot light data exists
        if (!light) {continue;}

        //store the light data
        data.color = col(light->color);
        data.intensity = light->intensity;
        data.fallof_linear = light->fallof_linear;
        data.fallof_quadratic = light->fallof_quadratic;
        //store the cosines of the angles
        data.cos_cone_inner = glm::cos(light->cone_inner);
        data.cos_cone_outer = glm::cos(light->cone_outer);

        //compute the culling distance
        /*
        Intensity computation: Intensity(Distance) = Intensity / (1 + fallof_linear * Distance + fallof_quadratic * Distance*Distance)
        Final intensity should be less than some epsilon (it will never be zero, but after some distance it is too little to matter)
        Let's assume epsilon = e
        Then we should compute the minimum safe culling distance:
        e = Intensity / (fallof_linear * Distance + fallof_quadratic * Distance*Distance)
        Let's solve for Distance (and use the positive evaluation since the distance is never negative):
        Distance = (sqrt(4 * Intensity * e * fallof_quadratic + e^2 * fallof_linear^2) - e * fallof_linear) / (2 * e * fallof_quadratic)
        */
        float cullDistance = (glm::sqrt(4.f * light->intensity * e * light->fallof_quadratic + e*e * light->fallof_linear*light->fallof_linear) - e * light->fallof_linear) / (2 * e * light->fallof_quadratic);
        //store the cull distance
        data.cullDistance = cullDistance;

        //fill in the position
        Transform* transf = m_world->get<Transform>(obj);
        if (transf) {
            //copy the position
            data.pos = transf->pos;
            //compute the direction vector
            data.dir = vec3(0,0,-1) * transf->rot;
        } else {
            //try to use a transform 2D
            Transform2D* transf2d = m_world->get<Transform2D>(obj);
            if (transf2d) {
                //copy and upcast
                data.pos = vec3(transf2d->pos, 0);
                //store the direction vector
                data.dir = vec3(1,0,0) * Quaternion(vec3(0,0,transf2d->angle));
            } else {
                //zero the position
                data.pos = vec3{0,0,0};
                //just use the forward vector
                data.dir = vec3(0,0,-1);
            }
        }

        //upload the data
        m_spotLightBuffer->write(&data, sizeof(data), sizeof(data)*lightId);
    }
    //iterate over all directional lights
    lightId = SIZE_MAX;
    for (const auto& obj : m_directionalLights) {
        //next light
        ++lightId;

        //store the GPU data
        DirectionalLightData data;

        //extract the directional light data
        Component::DirectionalLight* light = m_world->get<Component::DirectionalLight>(obj);

        //only continue of directional light data exists
        if (!light) {continue;}

        //store the light data
        data.color = col(light->color);
        data.intensity = light->intensity;

        //fill in the position
        Transform* transf = m_world->get<Transform>(obj);
        if (transf) {
            //compute the direction vector
            data.dir = vec3(0,1,0) * transf->rot;
        } else {
            //try to use a transform 2D
            Transform2D* transf2d = m_world->get<Transform2D>(obj);
            if (transf2d) {
                //store the direction vector
                data.dir = vec3(0,1,0) * Quaternion(vec3(0,0,transf2d->angle));
            } else {
                //just use the down vector
                data.dir = vec3(0,1,0);
            }
        }

        //upload the light data
        m_dirLightBuffer->write(&data, sizeof(data), sizeof(data) * lightId);
    }
}
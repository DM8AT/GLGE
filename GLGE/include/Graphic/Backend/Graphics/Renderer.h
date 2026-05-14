/**
 * @file Renderer.h
 * @author DM8AT
 * @brief define the backend renderer interface
 * @version 0.1
 * @date 2026-04-05
 * 
 * @copyright Copyright (c) 2026
 * 
 */
//header guard
#ifndef _GLGE_GRAPHIC_BACKEND_RENDERER_
#define _GLGE_GRAPHIC_BACKEND_RENDERER_

//add common stuff
#include "Core/Common.h"
//add references
#include "Core/Reference.h"
//add render targets
#include "Graphic/RenderTarget.h"
//add the ECS system
#include "Core/Object.h"

//add command buffers
#include "CommandBuffer.h"
//add buffers
#include "Buffer.h"

//graphic frontend classes can only exist as forward declarations
namespace GLGE::Graphic {class Buffer; class Mesh; class Material;}

//use the backend namespace
namespace GLGE::Graphic::Backend::Graphic {

    /**
     * @brief the abstract backend renderer interface
     */
    class Renderer : public Referable {
    public:

        /**
         * @brief Construct a new Renderer
         * 
         * @param world a reference to the world to render (reference -> cannot be null)
         * @param camera store a pointer to the object to use as a camera, this can be null
         * @param target the target to adapt to
         */
        Renderer(World& world, Object* camera, RenderTarget target)
         : m_world(&world), m_camera(camera), m_target(target)
        {}

        /**
         * @brief Destroy the Renderer
         */
        virtual ~Renderer() = default;

        /**
         * @brief a function to build the renderer command buffer
         * 
         * @param cmdBuff the buffer to record the drawing to
         */
        virtual void record(CommandBuffer& cmdBuff) = 0;

        /**
         * @brief update the internal data buffers
         */
        virtual void update() = 0;

        /**
         * @brief Get the Camera Buffer
         * 
         * @return `GLGE::Graphic::Buffer*` a pointer to the camera buffer
         */
        inline GLGE::Graphic::Buffer* getCameraBuffer() const noexcept
        {return m_cameraBuffer;}

        /**
         * @brief Get the Transform Buffer
         * 
         * @return `GLGE::Graphic::Buffer*` a pointer to the transform buffer
         */
        inline GLGE::Graphic::Buffer* getTransformBuffer() const noexcept
        {return m_transformBuffer;}

        /**
         * @brief Get the buffer that stores all the point lights
         * 
         * @return `GLGE::Graphic::Buffer*` a pointer to the buffer of all point lights
         */
        inline GLGE::Graphic::Buffer* getPointLightBuffer() const noexcept
        {return m_pointLightBuffer;}

        /**
         * @brief Get the buffer that stores all the spot lights
         * 
         * @return `GLGE::Graphic::Buffer*` a pointer to the buffer of all spot lights
         */
        inline GLGE::Graphic::Buffer* getSpotLightBuffer() const noexcept
        {return m_spotLightBuffer;}

        /**
         * @brief Get the buffer that stores all the directional lights
         * 
         * @return `GLGE::Graphic::Buffer*` a pointer to the buffer of all directional lights
         */
        inline GLGE::Graphic::Buffer* getDirectionalLightBuffer() const noexcept
        {return m_dirLightBuffer;}

        /**
         * @brief Get the buffer that stores all the indirect draw data
         * 
         * @return `GLGE::Graphic::Buffer*` a pointer to the buffer of the indirect draw data
         */
        inline GLGE::Graphic::Buffer* getIndirectDrawBuffer() const noexcept
        {return m_drawBuffer;}

        /**
         * @brief Get the amount of objects that are drawn
         * 
         * @return `u64` the amount of objects that are drawn each tick
         */
        inline u64 getObjectCount() const noexcept
        {return m_entities.size();}

        /**
         * @brief store the GPU point light data layout
         * 
         * Compatable with std430 when accessing pos as float x,y,z;
         */
        struct PointLightData {
            /**
             * @brief store the 3D position of the light
             */
            vec3 pos;
            /**
             * @brief store the spherical radius of the point light source
             */
            float radius;
            /**
             * @brief store the 11R11G10B-255 UNorm color of the light source
             */
            u32 color;
            /**
             * @brief store the intensity of the color
             */
            float intensity;
            /**
             * @brief store the linear fallof coefficient
             */
            float fallof_linear;
            /**
             * @brief store the quadratic fallof coefficient
             */
            float fallof_quadratic;
            /**
             * @brief store the cull distance of the light source
             */
            float cullDistance;
        };

        /**
         * @brief store the GPU spot light data layout
         * 
         * Compatable with std430 when accessing vec3s as float x,y,z;
         */
        struct SpotLightData {
            /**
             * @brief store the 3D position of the light
             */
            vec3 pos;
            /**
             * @brief store the 3D direction vector of the light
             */
            vec3 dir;
            /**
             * @brief store the 11R11G10B-255 UNorm color of the light source
             */
            u32 color;
            /**
             * @brief store the intensity of the light source
             */
            float intensity;
            /**
             * @brief store the linear fallof coefficient
             */
            float fallof_linear;
            /**
             * @brief store the quadratic fallof coefficient
             */
            float fallof_quadratic;
            /**
             * @brief store the cosine of the inner cone angle
             */
            float cos_cone_inner;
            /**
             * @brief store the cosine of the outer cone angle
             */
            float cos_cone_outer;
            /**
             * @brief store the culling distance
             */
            float cullDistance;
        };

        /**
         * @brief store the GPU directional light data layout
         * 
         * Compatable with std430 when accessing vec3s as float x,y,z;
         */
        struct DirectionalLightData {
            /**
             * @brief store the direction vector of the directional light
             */
            vec3 dir;
            /**
             * @brief store the 11R11G10B-255 UNorm color of the light source
             */
            u32 color;
            /**
             * @brief store the intensity of the light source
             */
            float intensity;
        };

    protected:

        /**
         * @brief define the layout of the camera data buffer
         */
        struct CameraData {
            /**
             * @brief store the position of the camera
             * 
             * Due to alignment this must be accessed as `float x,y,z;` in GLSL. 
             */
            vec3 position;
            /**
             * @brief store the normalized quaternion in a 16 bit per element format
             * 
             * i and j components
             */
            u32 compressedQuaternion_i_j;
            /**
             * @brief store the normalized quaternion in a 16 bit per element format
             * 
             * k and w components
             */
            u32 compressedQuaternion_k_w;
            /**
             * @brief store the field of view in radians
             */
            f32 fov;
            /**
             * @brief store the distance of the near clipping plane in world units
             */
            f32 proj_near;
            /**
             * @brief store the distance of the far clipping plane in world units
             */
            f32 proj_far;
            /**
             * @brief store the camera projection matrix
             */
            glm::mat4 projection;
        };

        /**
         * @brief define the layout of the per-mesh transform data
         */
        struct TransformData {
            /**
             * @brief store the position of the camera
             * 
             * Due to alignment this must be accessed as `float x,y,z;` in GLSL. 
             */
            vec3 position;
            /**
             * @brief store the normalized quaternion in a 16 bit per element format
             * 
             * i and j components
             */
            u32 compressedQuaternion_i_j;
            /**
             * @brief store the normalized quaternion in a 16 bit per element format
             * 
             * k and w components
             */
            u32 compressedQuaternion_k_w;
            /**
             * @brief store the per-axis scaling of the object
             * 
             * Due to alignment this must be accessed as `float x,y,z;` in GLSL. 
             */
            vec3 scale;
        };

        /**
         * @brief store a buffer for the camera
         */
        GLGE::Graphic::Buffer* m_cameraBuffer = nullptr;
        /**
         * @brief store the transform buffer
         * 
         * The transform index corresponds to the index ot the mesh in the mesh list
         * 
         * Some backends may not require this buffer, but it is provided anyways
         */
        GLGE::Graphic::Buffer* m_transformBuffer = nullptr;

        /**
         * @brief store the buffer for all the point lights
         */
        GLGE::Graphic::Buffer* m_pointLightBuffer = nullptr;
        /**
         * @brief store the buffer for all the spot lights
         */
        GLGE::Graphic::Buffer* m_spotLightBuffer  = nullptr;
        /**
         * @brief store the buffer for all the directional lights
         */
        GLGE::Graphic::Buffer* m_dirLightBuffer   = nullptr;

        /**
         * @brief store a pointer to the draw buffer
         */
        GLGE::Graphic::Buffer* m_drawBuffer = nullptr;

        /**
         * @brief store all known objects to render
         */
        std::vector<Object> m_entities;

        /**
         * @brief store all the point lights
         */
        std::vector<Object> m_pointLights;
        /**
         * @brief store all the spot lights
         */
        std::vector<Object> m_spotLights;
        /**
         * @brief store all the directional lights
         */
        std::vector<Object> m_directionalLights;
        
        /**
         * @brief store a pointer to the world to use for rendering
         */
        World* m_world = nullptr;
        /**
         * @brief store a pointer to the object to use as a camera
         */
        Object* m_camera = nullptr;
        /**
         * @brief store the target to adapt to
         */
        RenderTarget m_target;

    };

}

#endif
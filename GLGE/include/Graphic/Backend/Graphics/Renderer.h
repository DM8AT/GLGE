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
         * @param target a target to render to
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
            f32 near;
            /**
             * @brief store the distance of the far clipping plane in world units
             */
            f32 far;
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
         * @brief store all known objects to render
         */
        std::vector<Object> m_entities;
        
        /**
         * @brief store a pointer to the world to use for rendering
         */
        World* m_world = nullptr;
        /**
         * @brief store a pointer to the object to use as a camera
         */
        Object* m_camera = nullptr;
        /**
         * @brief store the render target to render to
         */
        RenderTarget m_target;

    };

}

#endif
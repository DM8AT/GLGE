/**
 * @file Material.h
 * @author DM8AT
 * @brief define a material backend interface class
 * @version 0.1
 * @date 2026-03-27
 * 
 * @copyright Copyright (c) 2026
 * 
 */
//header guard
#ifndef _GLGE_GRAPHIC_BACKEND_MATERIAL_
#define _GLGE_GRAPHIC_BACKEND_MATERIAL_

//add references
#include "Core/Reference.h"
//add shader
#include "Shader.h"
//add vertex layouts
#include "VertexLayout.h"
//add command buffer
#include "CommandBuffer.h"
//add backend framebuffers
#include "Framebuffer.h"

//use the backend namespace
namespace GLGE::Graphic::Backend::Graphic {

    /**
     * @brief a material combines a shader and vertex structures
     */
    class Material : public Referable {
    public:

        /**
         * @brief define the depth comparison modes for the shader
         */
        enum DepthMode : u8 {
            /**
             * @brief the depth test always suchseeddes
             */
            DEPTH_COMPARE_ALWAYS = 0,
            /**
             * @brief the depth test never suchseeddes
             */
            DEPTH_COMPARE_NEVER,
            /**
             * @brief the depth test only suchseeddes if the fragment's depth is less than the buffered depth
             */
            DEPTH_COMPARE_LESS,
            /**
             * @brief the depth test only suchseeddes if the fragment's depth is less than or equal to the buffered depth
             */
            DEPTH_COMPARE_LESS_EQUALS,
            /**
             * @brief the depth test only suchseeddes if the fragment's depth is greater than the buffered depth
             */
            DEPTH_COMPARE_GREATER,
            /**
             * @brief the depth test only suchseeddes if the fragment's depth is greater than or equal to the buffered depth
             */
            DEPTH_COMPARE_GREATER_EQUALS,
            /**
             * @brief the depth test only suchseeddes if the fragment's depth is equal to the buffered depth
             */
            DEPTH_COMPARE_EQUALS,
            /**
             * @brief the depth test only suchseeddes if the fragment's depth is not equal to the buffered depth
             */
            DEPTH_COMPARE_NOT_EQUAL
        };

        /**
         * @brief define how faces are culled
         */
        enum CullMode : u8 {
            /**
             * @brief this disables all culling
             */
            OFF = 0,
            /**
             * @brief this will cull the back faces with the winding order being counter-clockwise
             */
            BACK,
            /**
             * @brief this will cull the front faces with the winding order being counter-clockwise
             */
            FRONT
        };

        /**
         * @brief Construct a new Material
         * 
         * @param shader a reference to the used shader backend
         * @param layout a reference to the used vertex layout
         * @param fbuff a reference to the framebuffer to adapt to
         * @param cullMode the mode for the backface culling
         * @param depthMode the mode to use for depth compares
         * @param depthWrite `true` to enable depth writing, `false` to disable depth writing
         */
        Material(Reference<GLGE::Graphic::Backend::Graphic::Shader> shader, Reference<GLGE::Graphic::Backend::Graphic::VertexLayout> layout, Reference<GLGE::Graphic::Backend::Graphic::Framebuffer> fbuff, 
                 CullMode cullMode, DepthMode depthMode, bool depthWrite)
         : m_shader(shader), m_layout(layout), m_fbuff(fbuff), m_cullMode(cullMode), m_depthMode(depthMode), m_depthWrite(depthWrite)
        {}

        /**
         * @brief Destroy the Material
         */
        virtual ~Material() = default;

        /**
         * @brief bind the material
         * 
         * @param buffer the buffer to bind to
         * @param VBOOffset the VBO offset from 0 in bytes
         */
        virtual void bind(GLGE::Graphic::Backend::Graphic::CommandBuffer* buffer, size_t VBOOffset) = 0;

        /**
         * @brief a function to re-bind the material to update the VBO offset
         * 
         * @param buffer the buffer to bind to
         * @param VBOOffset the VBO offset from 0 in bytes
         */
        virtual void rebindMesh(GLGE::Graphic::Backend::Graphic::CommandBuffer* buffer, size_t VBOOffset) = 0;

        /**
         * @brief get the framebuffer the material uses
         */
        inline Reference<GLGE::Graphic::Backend::Graphic::Framebuffer> getFbuff() const noexcept
        {return m_fbuff;}

        /**
         * @brief get the shader the material uses
         */
        inline Reference<GLGE::Graphic::Backend::Graphic::Shader> getShader() const noexcept
        {return m_shader;}

        /**
         * @brief get the vertex layout the material uses
         */
        inline Reference<GLGE::Graphic::Backend::Graphic::VertexLayout> getVertexLayout() const noexcept
        {return m_layout;}

    protected:

        /**
         * @brief store a reference to the shader backend the material is using
         */
        Reference<GLGE::Graphic::Backend::Graphic::Shader> m_shader;
        /**
         * @brief store a reference to the vertex layout the material is using
         */
        Reference<GLGE::Graphic::Backend::Graphic::VertexLayout> m_layout;
        /**
         * @brief store the framebuffer the material uses
         */
        Reference<GLGE::Graphic::Backend::Graphic::Framebuffer> m_fbuff;
        /**
         * @brief define the cull mode of the material
         */
        CullMode m_cullMode = CullMode::OFF;
        /**
         * @brief store the depth compare mode
         */
        DepthMode m_depthMode = DEPTH_COMPARE_ALWAYS;
        /**
         * @brief store if depth writing is enabled
         * 
         * `true` if the depth writing is enabled, `false` if depth writing is disabled
         */
        bool m_depthWrite = false;

    };

}

#endif
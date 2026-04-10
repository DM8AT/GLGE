/**
 * @file Shader.h
 * @author DM8AT
 * @brief define the API for the graphic shader
 * @version 0.1
 * @date 2026-03-19
 * 
 * @copyright Copyright (c) 2026
 * 
 */
//header guard
#ifndef _GLGE_GRAPHIC_BACKEND_GRAPHICS_SHADER_
#define _GLGE_GRAPHIC_BACKEND_GRAPHICS_SHADER_

//add common stuff
#include "Core/Common.h"
//add references
#include "Core/Reference.h"

//shader are defined later
namespace GLGE::Graphic
{class Shader;}

//use the backend namespace
namespace GLGE::Graphic::Backend::Graphic {

    /**
     * @brief define the shader backend
     */
    class Shader : public GLGE::Referable {
    public:

        /**
         * @brief Construct a new Shader
         * 
         * @param frontend a pointer to the frontend
         */
        Shader(GLGE::Graphic::Shader* frontend)
         : m_shader(frontend), m_finalized(false)
        {}

        /**
         * @brief Destroy the Shader
         */
        virtual ~Shader() = default;

        /**
         * @brief add the SPIR-V source code for a shader element
         * 
         * @param data the binary SPIR-V binary data
         * @param size the size of the binary blob in bytes
         * @param element store information about the element
         */
        virtual void addShaderElement(void* data, u32 size, const void* element) = 0;

        /**
         * @brief Get the frontend shader
         * 
         * @return `GLGE::Graphic::Shader*` a pointer to the shader frontend
         */
        inline GLGE::Graphic::Shader* getFrontend() const noexcept
        {return m_shader;}

        /**
         * @brief finalize the shader
         */
        virtual void finalize() = 0;

        /**
         * @brief check if the shader is finalized
         * 
         * @return `true` if it is finalized, `false` if it is not
         */
        inline bool finalized() const noexcept
        {return m_finalized;}

    protected:

        /**
         * @brief store a pointer to the shader frontend
         */
        GLGE::Graphic::Shader* m_shader = nullptr;
        /**
         * @brief store if the shader is finalized
         */
        bool m_finalized = false;

    };

}

#endif
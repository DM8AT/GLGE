/**
 * @file Shader.h
 * @author DM8AT
 * @brief define the shader backend overload for OpenGL
 * @version 0.1
 * @date 2026-03-19
 * 
 * @copyright Copyright (c) 2026
 * 
 */
//header guard
#ifndef _GLGE_GRAPHIC_BACKEND_BUILTIN_GRAPHICS_OGL_SHADER_
#define _GLGE_GRAPHIC_BACKEND_BUILTIN_GRAPHICS_OGL_SHADER_

//add shader
#include "Graphic/Backend/Graphics/Shader.h"

//use the namespace
namespace GLGE::Graphic::Backend::Graphic::OpenGL {

    /**
     * @brief define the shader overload for OpenGL
     */
    class Shader : public GLGE::Graphic::Backend::Graphic::Shader {
    public:

        /**
         * @brief Construct a new Shader
         * 
         * @param frontend a pointer to the shader frontend
         */
        Shader(GLGE::Graphic::Shader* frontend);

        /**
         * @brief Destroy the Shader
         */
        virtual ~Shader();

        /**
         * @brief add the SPIR-V source code for a shader element
         * 
         * @param data the binary SPIR-V binary data
         * @param size the size of the binary blob in bytes
         * @param element store information about the element
         */
        virtual void addShaderElement(void* data, u32 size, const void* element) override;

        /**
         * @brief finalize the shader
         */
        virtual void finalize() override;

        /**
         * @brief Get the OpenGL program
         * 
         * @return `u32` the OpenGL program
         */
        inline u32 getProgram() const noexcept
        {return m_program;}

    protected:

        /**
         * @brief store a list of all currently loaded shaders
         */
        std::vector<u32> m_shader;
        /**
         * @brief store the finalized shader program
         */
        u32 m_program = 0;

        /**
         * @brief store a counter for name generation
         */
        inline static u32 ms_nameCounter = 0;
        /**
         * @brief store the name enumerator
         */
        u32 m_name = ++ms_nameCounter;
        
    };

}

#endif
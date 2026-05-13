/**
 * @file Shader.h
 * @author DM8AT
 * @brief define the vulkan shader
 * @version 0.1
 * @date 2026-05-12
 * 
 * @copyright Copyright (c) 2026
 * 
 */
//header guard
#ifndef _GLGE_GRAPHIC_BACKEND_BUILTIN_GRAPHICS_VK_SHADER_
#define _GLGE_GRAPHIC_BACKEND_BUILTIN_GRAPHICS_VK_SHADER_

//add shader
#include "Graphic/Backend/Graphics/Shader.h"

//use the namespace
namespace GLGE::Graphic::Backend::Graphic::Vulkan {

    /**
     * @brief define the shader overload for vulkan
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
         * @brief Get the Compute Pipeline
         * 
         * @return `void*` the compute pipeline or `nullptr` if no compute pipeline exists
         */
        void* getComputePipeline() const noexcept
        {return m_computePipe;}

    protected:

        /**
         * @brief store the shader modules
         */
        struct Module {
            /**
             * @brief store the identification tag of the module
             * 
             * This is the `SpvExecutionModel_` value of the reflected type
             */
            i32 tag;
            /**
             * @brief store the created shader module
             */
            void* shaderModule;
        };

        /**
         * @brief store the shader modules
         */
        std::vector<Module> m_modules;

        /**
         * @brief store the compute pipeline
         */
        void* m_computePipe = nullptr;

    };

}

#endif
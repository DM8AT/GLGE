/**
 * @file SampledTexture.h
 * @author DM8AT
 * @brief overload the sampled texture backend for OpenGL
 * @version 0.1
 * @date 2026-03-22
 * 
 * @copyright Copyright (c) 2026
 * 
 */
//header guard
#ifndef _GLGE_GRAPHIC_BACKEND_BUILTIN_GRAPHICS_OGL_SAMPLED_TEXTURE_
#define _GLGE_GRAPHIC_BACKEND_BUILTIN_GRAPHICS_OGL_SAMPLED_TEXTURE_

//add the sampled texture backend
#include "Graphic/Backend/Graphics/SampledTexture.h"

//use the namespace
namespace GLGE::Graphic::Backend::Graphic::OpenGL {

    /**
     * @brief overload for the sampled texture
     */
    class SampledTexture : public GLGE::Graphic::Backend::Graphic::SampledTexture {
    public:

        /**
         * @brief Construct a new Sampled Texture
         * 
         * @param texture a reference to the texture to sample
         * @param sampler a reference to the sampler to use
         */
        SampledTexture(const Reference<GLGE::Graphic::Backend::Graphic::Texture>& texture, const Reference<GLGE::Graphic::Backend::Graphic::Sampler>& sampler)
         : GLGE::Graphic::Backend::Graphic::SampledTexture(texture, sampler)
        {}

        /**
         * @brief Construct a new Sampled Texture
         * 
         * @param image a reference to the image to sample
         * @param sampler a reference to the sampler to use
         */
        SampledTexture(const Reference<GLGE::Graphic::Backend::Graphic::Image>& image, const Reference<GLGE::Graphic::Backend::Graphic::Sampler>& sampler)
         : GLGE::Graphic::Backend::Graphic::SampledTexture(image, sampler)
        {}

        /**
         * @brief a function that is called to build a binding of the resource to a unit in the resource set
         * 
         * @param set a pointer to the set to bind to
         * @param unit the unit to bind to
         */
        virtual void onBuildBinding(GLGE::Graphic::ResourceSet* set, u32 unit) override;

    };

}

#endif
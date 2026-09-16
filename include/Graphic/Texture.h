/**
 * @file Texture.h
 * @author DM8AT
 * @brief define the frontend API for textures
 * @version 0.1
 * @date 2026-01-03
 * 
 * @copyright Copyright (c) 2026
 * 
 */
//header guard
#ifndef _GLGE_GRAPHIC_TEXTURE_
#define _GLGE_GRAPHIC_TEXTURE_

//include common stuff
#include "Core/Common.h"
//add the base class
#include "Core/BaseClass.h"
//add instances
#include "Core/Instance.h"
//add CPU textures
#include "TextureCPU.h"
//add graphic instances
#include "Instance.h"

//add the texture backend
#include "Backend/Graphics/Texture.h"
//add commands
#include "Command.h"

//use the library namespace
namespace GLGE::Graphic {

    //forward declaration
    class SampledTexture;

    /**
     * @brief a class to wrap a texture in a safe container
     */
    class Texture : public BaseClass, CommandInvalidator {
    public:

        /**
         * @brief Construct a new Texture
         * 
         * @param size the size of the texture in pixels
         * @param mipCount the amount of mip images the texture has (0 = automatic)
         * @param format the format of the texture
         */
        Texture(const uvec2& size, u16 mipCount, PixelFormat format)
         : BaseClass(), m_texture(getInstance()->getExtension<GLGE::Graphic::Instance>()->getGraphicDescription()->createTexture(size, mipCount, format, getInstance()->getExtension<GLGE::Graphic::Instance>()->getGraphicBackendInstance().get()))
        {}

        /**
         * @brief Construct a new Texture
         * 
         * @param texture the texture to copy to the GPU
         */
        Texture(const TextureCPU& texture)
         : BaseClass(), m_texture(getInstance()->getExtension<GLGE::Graphic::Instance>()->getGraphicDescription()->createTexture(texture[0].getSize(), 
                                                                                                                                 texture.getMipCount(), 
                                                                                                                                 texture[0].getFormat(),
                                                                                                                                 getInstance()->getExtension<GLGE::Graphic::Instance>()->getGraphicBackendInstance().get()))
        {m_texture->update(texture);}

        /**
         * @brief get the backend texture of the wrapper
         * 
         * @return `GLGE::Graphic::Backend::Graphic::Texture>` a reference to the texture backend
         */
        inline Reference<GLGE::Graphic::Backend::Graphic::Texture> getBackend() const noexcept
        {return m_texture;}

        /**
         * @brief register a sampled texture for this texture
         * 
         * @param st a pointer to the sampler texture pair
         */
        void registerSampler(SampledTexture* st);

        /**
         * @brief remove a sampler texture pair
         * 
         * @param st a pointer to the pair to remove
         */
        void removeSampler(SampledTexture* st);

    protected:

        /**
         * @brief store the texture
         */
        Reference<GLGE::Graphic::Backend::Graphic::Texture> m_texture;

    };

}

#endif
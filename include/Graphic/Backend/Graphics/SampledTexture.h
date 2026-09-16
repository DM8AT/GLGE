/**
 * @file SampledTexture.h
 * @author DM8AT
 * @brief define a backend binding for sampled textures
 * @version 0.1
 * @date 2026-03-22
 * 
 * @copyright Copyright (c) 2026
 * 
 */
//header guard
#ifndef _GLGE_GRAPHIC_BACKEND_GRAPHICS_SAMPLED_TEXTURE_
#define _GLGE_GRAPHIC_BACKEND_GRAPHICS_SAMPLED_TEXTURE_

//add texture, image and sampler backends
#include "Texture.h"
#include "Image.h"
#include "Sampler.h"

//add references
#include "Core/Reference.h"

//use the backend namespace
namespace GLGE::Graphic::Backend::Graphic {

    /**
     * @brief define a backend for a sampled texture
     */
    class SampledTexture : public Referable {
    public:

        /**
         * @brief store the image or texture
         */
        union Data {
            /**
             * @brief store a reference to the texture to sample
             */
            Reference<GLGE::Graphic::Backend::Graphic::Texture> texture;
            /**
             * @brief store a reference to the image to sample
             */
            Reference<GLGE::Graphic::Backend::Graphic::Image> image;

            /**
             * @brief Construct a new Data
             * 
             * @param _texture a constant reference to the texture to store
             */
            Data(const Reference<GLGE::Graphic::Backend::Graphic::Texture>& _texture) 
             : texture(_texture)
            {}

            /**
             * @brief Construct a new Data
             * 
             * @param _image a constant reference to the image to store
             */
            Data(const Reference<GLGE::Graphic::Backend::Graphic::Image>& _image)
             : image(_image)
            {}

            /**
             * @brief Destroy the Data
             */
            ~Data() {}
        };

        /**
         * @brief Construct a new Sampled Texture
         * 
         * @param texture a reference to the texture to sample
         * @param sampler a reference to the sampler to use
         */
        SampledTexture(const Reference<GLGE::Graphic::Backend::Graphic::Texture>& texture, const Reference<GLGE::Graphic::Backend::Graphic::Sampler>& sampler)
         : m_isTexture(true), m_data(texture), m_sampler(sampler)
        {}

        /**
         * @brief Construct a new Sampled Texture
         * 
         * @param image a reference to the image to sample
         * @param sampler a reference to the sampler to use
         */
        SampledTexture(const Reference<GLGE::Graphic::Backend::Graphic::Image>& image, const Reference<GLGE::Graphic::Backend::Graphic::Sampler>& sampler)
         : m_isTexture(false), m_data(image), m_sampler(sampler)
        {}

        /**
         * @brief Destroy the Sampled Texture
         */
        virtual ~SampledTexture() {
            //clean up
            if (m_isTexture)
            {m_data.texture.~Reference();}
            else
            {m_data.image.~Reference();}
        }

        /**
         * @brief a function that is called to build a binding of the resource to a unit in the resource set
         * 
         * @param set a pointer to the set to bind to
         * @param unit the unit to bind to
         */
        virtual void onBuildBinding(GLGE::Graphic::ResourceSet* set, u32 unit) = 0;

        /**
         * @brief a function that is called to build a binding of the resource to a unit in the resource set
         * 
         * @param set a pointer to the set to bind to
         */
        virtual void onDropBinding(GLGE::Graphic::ResourceSet* set) = 0;

        /**
         * @brief Get the Data
         * 
         * @return `const Data&` a constant reference to the stored data
         */
        inline const Data& getData() const noexcept
        {return m_data;}

        /**
         * @brief get if the sampled texture stores a texture or an image
         * 
         * @return `true` if a texture is stored, `false` if it is an image
         */
        inline bool isTexture() const noexcept
        {return m_isTexture;}

        /**
         * @brief Get the Sampler
         * 
         * @return `Reference<GLGE::Graphic::Backend::Graphic::Sampler>` a reference to the stored sampler
         */
        inline Reference<GLGE::Graphic::Backend::Graphic::Sampler> getSampler() const noexcept
        {return m_sampler;}

    protected:

        /**
         * @brief store if this is a texture or an image
         */
        bool m_isTexture = false;
        /**
         * @brief the data storage for the sampler
         */
        Data m_data;
        /**
         * @brief store a reference to the sampler
         */
        Reference<GLGE::Graphic::Backend::Graphic::Sampler> m_sampler;

    };

}

#endif
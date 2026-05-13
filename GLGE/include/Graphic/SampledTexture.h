/**
 * @file SampledTexture.h
 * @author DM8AT
 * @brief define a match of a sampler and texture
 * @version 0.1
 * @date 2026-03-22
 * 
 * @copyright Copyright (c) 2026
 * 
 */
//header guard
#ifndef _GLGE_GRAPHIC_SAMPLED_TEXTURE_
#define _GLGE_GRAPHIC_SAMPLED_TEXTURE_

//include common stuff
#include "Core/Common.h"
//add the base class
#include "Core/BaseClass.h"
//add instances
#include "Core/Instance.h"
//add graphic instances
#include "Instance.h"

//add textures, images and samplers
#include "Texture.h"
#include "Image.h"
#include "Sampler.h"

//use the library namespace
namespace GLGE::Graphic {

    /**
     * @brief a mapping between a sampler and a texture
     */
    class SampledTexture : public BaseClass, public Resource {
    public:

        /**
         * @brief Construct a new Sampled Texture
         * 
         * @param texture a reference to the texture to sample
         * @param sampler a reference to the sampler to use
         */
        SampledTexture(const Texture& texture, const Sampler& sampler)
         : BaseClass(), Resource(), m_sampledTexture(getInstance()->getExtension<GLGE::Graphic::Instance>()->getGraphicDescription()->createSampledTexture(texture.getBackend(), sampler.getBackend()))
        {}

        /**
         * @brief Construct a new Sampled Texture
         * 
         * @param image a reference to the image to sample
         * @param sampler a reference to the sampler to use
         */
        SampledTexture(const Image& image, const Sampler& sampler)
         : BaseClass(), Resource(), m_sampledTexture(getInstance()->getExtension<GLGE::Graphic::Instance>()->getGraphicDescription()->createSampledTexture(image.getBackend(), sampler.getBackend()))
        {}

        /**
         * @brief Destroy the Sampled Texture
         */
        virtual ~SampledTexture() = default;

        /**
         * @brief a function that is called to build a binding of the resource to a unit in the resource set
         * 
         * @param set a pointer to the set to bind to
         * @param unit the unit to bind to
         */
        virtual void onBuildBinding(GLGE::Graphic::ResourceSet* set, u32 unit) override
        {m_sampledTexture->onBuildBinding(set, unit);}
        
        /**
         * @brief a function that is called when a resource set holding a reference to this resource is destroyed
         * 
         * @param set a pointer to the resource set that was destroyed
         */
        virtual void onRemoveBinding(ResourceSet* set) override
        {m_sampledTexture->onDropBinding(set);}

        /**
         * @brief Get the type of resource this is
         * 
         * @return `ResourceType` the type to identify the resource
         */
        virtual ResourceType getType() const noexcept override
        {return ResourceType::SAMPLED_TEXTURE;}

        /**
         * @brief Get the hash of the actual resource type
         * 
         * @return `u64` the hash of the type
         */
        virtual u64 getTypeHash() const noexcept override
        {return getTypeHash64<SampledTexture>();}

    protected:

        /**
         * @brief store the sampled texture
         */
        Reference<GLGE::Graphic::Backend::Graphic::SampledTexture> m_sampledTexture;

    };

}

#endif
/**
 * @file Sampler.h
 * @author DM8AT
 * @brief define a GPU side sampler class
 * @version 0.1
 * @date 2026-03-11
 * 
 * @copyright Copyright (c) 2026
 * 
 */
//header guard
#ifndef _GLGE_GRAPHIC_BACKEND_GRAPHICS_SAMPLER_
#define _GLGE_GRAPHIC_BACKEND_GRAPHICS_SAMPLER_

//add references
#include "Core/Reference.h"
//add graphic instances
#include "Instance.h"
//add the CPU sampler
#include "Graphic/SamplerCPU.h"

//add the resource base
#include "Graphic/Resource.h"

//use the backend namespace
namespace GLGE::Graphic::Backend::Graphic {

    /**
     * @brief store the API for the sampler backend
     */
    class Sampler : public Referable {
    public:

        /**
         * @brief Construct a new Sampler
         * 
         * @param state the state to store
         * @param instance a pointer to the owning instance
         */
        Sampler(const SamplerCPU& state, GLGE::Graphic::Backend::Graphic::Instance* instance)
         : m_instance(instance), m_state(state)
        {}

        /**
         * @brief Destroy the Sampler
         */
        virtual ~Sampler() = default;

        /**
         * @brief update the internal sampler state
         */
        virtual void update() = 0;

        /**
         * @brief access the state of the sampler
         * 
         * @return `const SamplerCPU&` access the state of the sampler
         */
        inline const SamplerCPU& state() const noexcept
        {return m_state;}

        /**
         * @brief access the state of the sampler
         * 
         * @warning modifying the state won't update the sampler. Call update to update the internal sampler. 
         * 
         * @return `SamplerCPU&` access the state of the sampler
         */
        inline SamplerCPU& state() noexcept
        {return m_state;}

        /**
         * @brief a function that is called to build a binding of the resource to a unit in the resource set
         * 
         * @param set a pointer to the set to bind to
         * @param unit the unit to bind to
         */
        virtual void onBuildBinding(GLGE::Graphic::ResourceSet* set, u32 unit) = 0;

        /**
         * @brief a function that is called to remove a binding of the resource to a unit in the resource set
         * 
         * @param set a pointer to the set to remove from
         */
        virtual void onRemoveBinding(GLGE::Graphic::ResourceSet* set) = 0;

    protected:

        /**
         * @brief store a pointer to the owning instance
         */
        GLGE::Graphic::Backend::Graphic::Instance* m_instance = nullptr;
        /**
         * @brief store the internal sampler state
         */
        SamplerCPU m_state;

    };

}

#endif
/**
 * @file Sampler.h
 * @author DM8AT
 * @brief define the vulkan sampler overload
 * @version 0.1
 * @date 2026-05-17
 * 
 * @copyright Copyright (c) 2026
 * 
 */
//header guard
#ifndef _GLGE_GRAPHIC_BACKEND_BUILTIN_GRAPHICS_OGL_SAMPLER_
#define _GLGE_GRAPHIC_BACKEND_BUILTIN_GRAPHICS_OGL_SAMPLER_

//add samplers
#include "Graphic/Backend/Graphics/Sampler.h"

//use the namespace
namespace GLGE::Graphic::Backend::Graphic::Vulkan {

    /**
     * @brief an overload for the sampler for the OpenGL API
     */
    class Sampler : public GLGE::Graphic::Backend::Graphic::Sampler {
    public:

        /**
         * @brief Construct a new Sampler
         * 
         * @param state the initial state of the sampler
         * @param instance the instance the sampler will belong to
         */
        Sampler(const SamplerCPU& state, GLGE::Graphic::Backend::Graphic::Instance* instance);

        /**
         * @brief Destroy the Sampler
         */
        virtual ~Sampler();

        /**
         * @brief update the internal sampler state
         */
        virtual void update() override;

        /**
         * @brief a function that is called to build a binding of the resource to a unit in the resource set
         * 
         * @param set a pointer to the set to bind to
         * @param unit the unit to bind to
         */
        virtual void onBuildBinding(GLGE::Graphic::ResourceSet* set, u32 unit) override;

        /**
         * @brief a function that is called to remove a binding of the resource to a unit in the resource set
         * 
         * @param set a pointer to the set to remove from
         */
        virtual void onRemoveBinding(GLGE::Graphic::ResourceSet* set) override;

        /**
         * @brief Get the Sampler
         * 
         * @return `void*` the vulkan sampler object
         */
        void* getSampler() const noexcept
        {return m_sampler;}

    protected:

        /**
         * @brief store all bindings the sampler has
         */
        std::vector<std::pair<GLGE::Graphic::ResourceSet*, u32>> m_bindings;
        /**
         * @brief store the sampler
         */
        void* m_sampler = nullptr;
        /**
         * @brief store the old sampler
         */
        void* m_oldSampler = nullptr;

    };

}

#endif
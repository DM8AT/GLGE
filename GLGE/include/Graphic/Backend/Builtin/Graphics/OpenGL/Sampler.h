/**
 * @file Sampler.h
 * @author DM8AT
 * @brief define the overload for an OpenGL sampler
 * @version 0.1
 * @date 2026-03-11
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
namespace GLGE::Graphic::Backend::Graphic::OpenGL {

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
         * @brief Get the Sampler
         * 
         * @return `u32` the OpenGL sampler object
         */
        inline u32 getSampler() const noexcept
        {return m_sampler;}

        /**
         * @brief a function that is called to build a binding of the resource to a unit in the resource set
         * 
         * @param set a pointer to the set to bind to
         * @param unit the unit to bind to
         */
        virtual void onBuildBinding(GLGE::Graphic::ResourceSet* set, u32 unit) override;

    protected:

        /**
         * @brief store a counter for name generation
         */
        inline static u32 ms_nameCounter = 0;

        /**
         * @brief store the sampler object
         */
        u32 m_sampler = 0;
        /**
         * @brief store the name enumerator
         */
        u32 m_name = ++ms_nameCounter;

    };

}

#endif
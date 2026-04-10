/**
 * @file Sampler.h
 * @author DM8AT
 * @brief define the frontend sampler class
 * @version 0.1
 * @date 2026-03-12
 * 
 * @copyright Copyright (c) 2026
 * 
 */
//header guard
#ifndef _GLGE_GRAPHIC_SAMPLER_
#define _GLGE_GRAPHIC_SAMPLER_

//add base classes
#include "Core/BaseClass.h"
//add instances
#include "Core/Instance.h"
//add the CPU sampler
#include "SamplerCPU.h"
//add graphic instances
#include "Instance.h"

//add the sampler backend
#include "Backend/Graphics/Sampler.h"

//use the library namespace
namespace GLGE::Graphic {

    /**
     * @brief store the frontend class for a GPU sampler object
     */
    class Sampler : public BaseClass {
    public:

        /**
         * @brief Construct a new Sampler
         */
        Sampler()
         : BaseClass(), m_sampler(getInstance()->getExtension<GLGE::Graphic::Instance>()->getGraphicDescription()->createSampler(SamplerCPU(), getInstance()->getExtension<GLGE::Graphic::Instance>()->getGraphicBackendInstance().get()))
        {}

        /**
         * @brief Construct a new Sampler
         * 
         * @param state the initial state of the sampler
         */
        Sampler(const SamplerCPU& state)
         : BaseClass(), m_sampler(getInstance()->getExtension<GLGE::Graphic::Instance>()->getGraphicDescription()->createSampler(state, getInstance()->getExtension<GLGE::Graphic::Instance>()->getGraphicBackendInstance().get()))
        {}

        /**
         * @brief update the internal sampler state
         */
        inline void update() 
        {m_sampler->update();}

        /**
         * @brief access the state of the internal sampler
         * 
         * @warning if the state is modified here, this does NOT update the GPU state directly. To update the GPU state the `update` function must be called. 
         * 
         * @return `SamplerCPU&` a reference to the sampler state
         */
        inline SamplerCPU& state() noexcept
        {return m_sampler->state();}

        /**
         * @brief access the state of the internal sampler
         * 
         * @return `const SamplerCPU&` a constant reference to the sampler state
         */
        inline const SamplerCPU& state() const noexcept
        {return m_sampler->state();}

        /**
         * @brief get the backend sampler of the wrapper
         * 
         * @return `GLGE::Graphic::Backend::Graphic::Sampler>` a reference to the sampler backend
         */
        inline Reference<GLGE::Graphic::Backend::Graphic::Sampler> getBackend() const noexcept
        {return m_sampler;}

    protected:

        /**
         * @brief store the abstract sampler
         */
        Reference<GLGE::Graphic::Backend::Graphic::Sampler> m_sampler;

    };

}

#endif
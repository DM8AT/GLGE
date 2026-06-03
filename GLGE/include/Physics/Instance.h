/**
 * @file Instance.h
 * @author DM8AT
 * @brief define the physics instance
 * @version 0.1
 * @date 2026-06-01
 * 
 * @copyright Copyright (c) 2026
 * 
 */
//header guard
#ifndef _GLGE_PHYSICS_INSTANCE_
#define _GLGE_PHYSICS_INSTANCE_

//add instances
#include "Core/Instance.h"
//add physic backend descriptions
#include "Backend/Description.h"
//add physic instances
#include "Backend/Instance.h"

//use the library namespace
namespace GLGE {

    /**
     * @brief a namespace that contain all physics classes and functions
     */
    namespace Physic {

        /**
         * @brief store a physics instance
         * 
         * If this is attached to a `GLGE::Instance`, physics capability is enabled. 
         */
        class Instance : public InstanceExtension {
        public:

            /**
             * @brief Construct a new Instance
             * 
             * @warning no ownership of the description is taken. The description must outlive the instance. 
             * 
             * @param description a pointer to the description to wrap
             */
            Instance(Backend::Description* description);

            /**
             * @brief Destroy the Instance
             */
            virtual ~Instance() override;

            /**
             * @brief a function that is called when the `start` function of the parent is called. 
             * 
             * The purpose of the function is to prepare for running. Here, stuff like asset loading should be finalizing. 
             * The threads used for async asset loading should rejoin here. 
             */
            virtual void onStart() override;

            /**
             * @brief a function that is called every tick. This happens when the `update` function of the instance is called. 
             */
            virtual void onUpdate() override;

            /**
             * @brief a function that is called when the `shutdown` function of the instance is called or when the instance is being deleted - 
             *        whatever happens first. 
             * 
             * The purpose of this is to return to a stable start state. 
             * 
             * @warning this function is not forced to be the end of lifetime of the component. The instance may get re-started. 
             */
            virtual void onShutdown() override;

            /**
             * @brief a function that is called when the instance is set
             */
            virtual void onInstanceSetting() override;

            /**
             * @brief a function that is called when the instance is bound
             */
            virtual void onBind() override;

            /**
             * @brief a function that is called when the instance is unbound
             */
            virtual void onUnbind() override;

            /**
             * @brief Get the Description
             * 
             * @return `Backend::Description*` a pointer to the backend description
             */
            inline Backend::Description* getDescription() const noexcept
            {return m_descr;}

            /**
             * @brief get the backend instance
             * 
             * @return `Reference<Backend::Instance>` a reference to the backend instance
             */
            inline Reference<Backend::Instance> getBackend() const noexcept
            {return m_inst;}

        protected:

            /**
             * @brief store the description of the backend
             */
            Backend::Description* m_descr = nullptr;

            /**
             * @brief store the backend instance
             */
            Reference<Backend::Instance> m_inst;

        };

    }

}

#endif
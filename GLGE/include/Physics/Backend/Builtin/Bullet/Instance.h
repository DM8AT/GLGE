/**
 * @file Instance.h
 * @author DM8AT
 * @brief define the overload for the bullet instance
 * @version 0.1
 * @date 2026-06-04
 * 
 * @copyright Copyright (c) 2026
 * 
 */
//header guard
#ifndef _GLGE_PHYSICS_BACKEND_BULLET_INSTANCE_
#define _GLGE_PHYSICS_BACKEND_BULLET_INSTANCE_

//add the instance glue layer
#include "Physics/Backend/Instance.h"
//add bullet
#include <btBulletDynamicsCommon.h>

//use the backend namespace
namespace GLGE::Physic::Backend {

    /**
     * @brief a namespace for the bullet physics implementation
     */
    namespace Bullet {

        /**
         * @brief overload the backend instance for bullet
         */
        class Instance : public Backend::Instance {
        public:

            /**
             * @brief Construct a new Instance
             * 
             * @param instance a pointer to the frontend instance
             */
            Instance(GLGE::Physic::Instance* instance);

            /**
             * @brief Destroy the Instance
             */
            virtual ~Instance() override;

            /**
             * @brief this call is called once every tick
             */
            virtual void onUpdate() override;

        };

    }

}

#endif
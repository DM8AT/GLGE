/**
 * @file Instance.h
 * @author DM8AT
 * @brief define a backend instance class
 * @version 0.1
 * @date 2026-06-01
 * 
 * @copyright Copyright (c) 2026
 * 
 */
//header guard
#ifndef _GLGE_PHYSICS_BACKEND_JOLT_INSTANCE_
#define _GLGE_PHYSICS_BACKEND_JOLT_INSTANCE_

//add backend instances
#include "Physics/Backend/Instance.h"

//use the backend namespace
namespace GLGE::Physic::Backend::Jolt {

    /**
     * @brief store a backend instance
     */
    class Instance : public Backend::Instance {
    public:

        /**
         * @brief Construct a new Instance
         * 
         * @param instance a pointer to the frontend instance
         */
        Instance(Physic::Instance* instance);

        /**
         * @brief Destroy the Instance
         */
        virtual ~Instance() override;

        /**
         * @brief this call is called once every tick
         */
        virtual void onUpdate() override;
    
    protected:

        /**
         * @brief store the amount of instances in existence
         */
        inline static u64 ms_refCount = 0;
    
    };

}

#endif
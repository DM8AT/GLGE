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
#ifndef _GLGE_PHYSICS_BACKEND_INSTANCE_
#define _GLGE_PHYSICS_BACKEND_INSTANCE_

//add references
#include "Core/Reference.h"
//add limiter
#include "Core/RateLimiter.h"
//add threading
#include <thread>
#include <atomic>

//instances are defined somewhere else
namespace GLGE::Physic
{class Instance;}

//use the backend namespace
namespace GLGE::Physic::Backend {

    //worlds are defined somewhere else
    class World;

    /**
     * @brief store a backend instance
     */
    class Instance : public GLGE::Referable {
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
        virtual ~Instance();

        /**
         * @brief this call is called once every tick
         */
        virtual void onUpdate() = 0;
    
        /**
         * @brief register a new world
         * 
         * @param world a pointer to the new world
         */
        virtual void registerWorld(World* world) final;

        /**
         * @brief remove an existing world
         * 
         * @param world a pointer to the world to remove
         */
        virtual void removeWorld(World* world) final;

        /**
         * @brief start the instance
         */
        void start();

        /**
         * @brief end the simulation instance
         */
        void end();

        /**
         * @brief Get the Instance
         * 
         * @return `Physic::Instance*` a pointer to the frontend instance
         */
        inline Physic::Instance* getInstance() const noexcept
        {return m_instance;}

        /**
         * @brief access the physic rate limit
         * 
         * @return `RateLimit&` the structure responsible for limiting the physics iteration rate
         */
        inline RateLimit& getLimiter() noexcept
        {return m_limiter;}

    protected:

        /**
         * @brief the update function running on the physics thread
         */
        void async();

        /**
         * @brief a function that is called when a new world is created
         * 
         * @param world a pointer to the new world
         */
        virtual void onNewWorld(World* world) {}

        /**
         * @brief a function that is called when a world is deleted
         * 
         * @param world a pointer to the world that WILL BE deleted
         */
        virtual void onDeleteWorld(World* world) {}

        /**
         * @brief store all worlds known to the instance
         */
        std::vector<World*> m_worlds;
        /**
         * @brief store a pointer to the frontend instance
         */
        Physic::Instance* m_instance = nullptr;

        /**
         * @brief store the thread that runs physics
         */
        std::thread m_physicsThread;
        /**
         * @brief store if the physics thread should be alive
         */
        std::atomic_bool m_alive {true};
        /**
         * @brief store the rate limiter
         */
        GLGE::RateLimit m_limiter;

    };

}

#endif
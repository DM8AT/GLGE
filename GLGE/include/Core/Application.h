/**
 * @file Application.h
 * @author DM8AT
 * @brief define a simple app wrapper to allow application-based usage
 * @version 0.1
 * @date 2026-05-27
 * 
 * @copyright Copyright (c) 2026
 * 
 */
//header guard
#ifndef _GLGE_CORE_APPLICATION_
#define _GLGE_CORE_APPLICATION_

//add instances
#include "Instance.h"
//add reference counting
#include "Reference.h"
//add the layer system
#include "LayerManager.h"

//use the library namespace
namespace GLGE {

    /**
     * @brief define a simple application class
     * 
     * An application class is a form of design where code is injected into the project flow via hooks like `onUpdate` which are called if pre-defined events are met. 
     * The application handles the runtime and main loop, the overload manages it. 
     * 
     * This class is purely virtual and cannot be created without being overloaded. `onUpdate` must be overriden. 
     */
    class Application : public Referable {
    public:

        /**
         * @brief Construct a new Application
         * 
         * @tparam Args the types of the extension parsed to it
         * @param name the name of the application
         * @param version the version of the application
         * @param extensions the named extensions the app uses
         */
        template <typename... Args>
        Application(const std::string_view& name, GLGE::Version version, std::pair<const char*, Args*> ...extensions)
         : m_instance(name, version, std::forward<std::pair<const char*, Args*>>(extensions)...)
        {
            //main thread check
            if (!GLGE::Instance::isMainThread()) {throw GLGE::Exception("An application can only be created on the main thread. If this is the main thread, please call \'GLGE::Instance::staticInit()\' before creating an application.", "GLGE::Application");}
            //init the instance
            m_instance.init();
        }

        /**
         * @brief Destroy the Application
         */
        virtual ~Application() = default;

        //applications are immovable
        Application(Application&&) = delete;
        Application(const Application&) = delete;
        Application& operator=(Application&&) = delete;
        Application& operator=(const Application&) = delete;

        /**
         * @brief run the application
         * 
         * This function does NOT return until the application is finished
         */
        virtual void run() final;

        /**
         * @brief Get the Delta Time
         * 
         * @return `GLGE::f64` the delta time in seconds
         */
        inline f64 getDeltaTime() const noexcept
        {return m_instance.mainLimiter().getCurrentDeltaTime();}

        /**
         * @brief Access the instance of the application
         * 
         * @return `GLGE::Instance&` the embedded instance
         */
        inline GLGE::Instance& getInstance() noexcept
        {return m_instance;}

        /**
         * @brief check if the application is active
         * 
         * @return `true` if the application is active, `false` if not
         */
        inline bool isActive() const noexcept
        {return m_active;}

        /**
         * @brief stop the application
         */
        inline void stop() noexcept
        {m_active = false;}

        /**
         * @brief access the layer manager of the application
         * 
         * @return `LayerManager&` a reference to the layer manager
         */
        inline LayerManager& layerManager() noexcept
        {return m_manager;}

    protected:

        /**
         * @brief a function that is called every tick
         */
        virtual void onUpdate() = 0;

    private:

        /**
         * @brief store if the application is active
         */
        bool m_active = true;

        /**
         * @brief store the main instance
         */
        GLGE::Instance m_instance;
        /**
         * @brief store the layer manager
         */
        GLGE::LayerManager m_manager;

    };

}

#endif
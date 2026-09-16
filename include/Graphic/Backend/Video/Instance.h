/**
 * @file Instance.h
 * @author DM8AT
 * @brief define the API for a video instance
 * @version 0.1
 * @date 2025-12-29
 * 
 * @copyright Copyright (c) 2025
 * 
 */
//header guard
#ifndef _GLGE_GRAPHIC_BACKEND_VIDEO_INSTANCE_
#define _GLGE_GRAPHIC_BACKEND_VIDEO_INSTANCE_

//include the display setup
#include "Graphic/DisplaysSetup.h"
//include the instances
#include "Graphic/Instance.h"

//add contracts
#include "APIContracts/Contracts.h"

//use the backend namespace
namespace GLGE::Graphic {
/**
 * @brief a namespace that stores the backend related classes for graphics
 */
namespace Backend {
/**
 * @brief a namespace that stores the video backend related classes for graphics
 */
namespace Video {

    /**
     * @brief define an instance that is responsible for managing the video backend
     */
    class Instance {
    public:

        /**
         * @brief Construct a new Instance
         * 
         * @param instance a pointer to the frontend instance 
         * @param api the graphic API to specialize the instance for
         * @param version the version of the graphic API to use
         */
        Instance(GLGE::Graphic::Instance* instance, GLGE::Graphic::GraphicAPI api, GLGE::Version version)
         : m_instance(instance), m_api(api), m_apiVersion(version)
        {}

        /**
         * @brief Destroy the Instance
         */
        virtual ~Instance() {}

        /**
         * @brief Get the Instance the video instance belongs to
         * 
         * @return `GLGE::Graphic::Instance*` a pointer to the frontend instance the backend instance belongs to
         */
        inline GLGE::Graphic::Instance* getInstance() const noexcept
        {return m_instance;}

        /**
         * @brief get the graphic API the instance is specialized for
         * 
         * @return `const GLGE::Graphic::GraphicAPI&` a constant reference to the API specialization
         */
        inline const GLGE::Graphic::GraphicAPI& getAPI() const noexcept
        {return m_api;}

        /**
         * @brief get the version of the graphic API the instance is specialized for
         * 
         * @return `const GLGE::Version&` a constant reference to the version specialization
         */
        inline const GLGE::Version& getAPIVersion() const noexcept
        {return m_apiVersion;}

        /**
         * @brief Get the Display Setup known to the video system
         * 
         * @return `const GLGE::Graphic::DisplaySetup&` a constant reference to the video system
         */
        virtual const GLGE::Graphic::DisplaySetup& getDisplaySetup() const noexcept = 0;

        /**
         * @brief a function that is called every time an update occurred
         * 
         * This should quarry all events and update what requires updating
         */
        virtual void onUpdate() = 0;

        /**
         * @brief a function that is used to inform the instance of the existence of a new window
         * 
         * @param window a pointer to the window to register
         */
        virtual void onWindowRegister(GLGE::Graphic::Backend::Video::Window* window) = 0;

        /**
         * @brief a function that is used to inform the instance of the destruction of a window
         * 
         * @param window a pointer to the window that no longer exists
         */
        virtual void onWindowRemove(GLGE::Graphic::Backend::Video::Window* window) = 0;

        /**
         * @brief Get the Contract
         * 
         * @tparam T the type of contract to quarry
         * @return `T*` a pointer to the contract
         */
        template <typename T>
        requires std::is_base_of_v<Contracts::BaseContract, T>
        T* getContract() const noexcept
        {return reinterpret_cast<T*>(m_contract);}

    protected:

        /**
         * @brief store a pointer to the contract the instance owns
         */
        Contracts::BaseContract* m_contract = nullptr;

    private:

        /**
         * @brief store the API the instance is specialized for
         */
        GLGE::Graphic::GraphicAPI m_api = GLGE::Graphic::GraphicAPI::UNDEFINED;
        /**
         * @brief store the version of the graphic API the instance is specialized for
         */
        GLGE::Version m_apiVersion = GLGE::Version(0, 0, 0);

        /**
         * @brief store a pointer to the instance the video instance belongs to
         */
        GLGE::Graphic::Instance* m_instance = nullptr;

    };

}
}
}

#endif
/**
 * @file Instance.h
 * @author DM8AT
 * @brief define an instance for the graphic core
 * @version 0.1
 * @date 2025-12-21
 * 
 * @copyright Copyright (c) 2025
 * 
 */
//header guard
#ifndef _GLGE_GRAPHIC_INSTANCE_
#define _GLGE_GRAPHIC_INSTANCE_

//include the core
#include "Core/Core.h"

//include backend descriptions
#include "Backend/Graphics/BackendDescription.h"
#include "Backend/Video/VideoDescription.h"
//include backend instances
#include "Backend/Graphics/Instance.h"
#include "Backend/Video/Instance.h"
//add mesh pools
#include "Backend/Graphics/MeshPool.h"

//use the libraries namespace
namespace GLGE {
/**
 * @brief a namespace for all classes that have directly to do with graphics
 */
namespace Graphic {

    /**
     * @brief windows are defined later
     */
    class Window;

    /**
     * @brief a graphic instance extends the GLGE instance
     */
    class Instance : public GLGE::InstanceExtension {
    public:

        /**
         * @brief Construct a new Instance
         * 
         * @param graphicDescription a pointer to an object that stores factories to the graphic backend classes
         * @param videoDescription a pointer to an object that stores factories to the video backend classes
         */
        Instance(GLGE::Graphic::Backend::Graphic::Description* graphicDescription, 
                 GLGE::Graphic::Backend::Video::Description*   videoDescription);

        /**
         * @brief Destroy the Instance
         * 
         * This cleans up the graphics backend
         */
        virtual ~Instance();

        /**
         * @brief if the parent instance was bound, also bind this
         */
        virtual void onBind() override
        {msl_instance = this;}

        /**
         * @brief if the parent instance was unbound, also unbind this
         */
        virtual void onUnbind() override
        {msl_instance = nullptr;}

        /**
         * @brief Get the currently bound graphic instance
         * 
         * @return `GLGE::Graphic::Instance*` a pointer to the currently bound graphics instance
         */
        inline static GLGE::Graphic::Instance* getBound() noexcept
        {return msl_instance;}

        /**
         * @brief Get the name of the graphic backend
         * 
         * @return `const char*` the name of the graphic backend as a human readable string
         */
        inline const char* getGraphicBackendName() const noexcept
        {return m_gDesc->getName();}

        /**
         * @brief Get the Graphic Backend version
         * 
         * @return `GLGE::Version` the version of the graphic backend
         */
        inline GLGE::Version getGraphicBackendVersion() const noexcept
        {return m_gDesc->getVersion();}

        /**
         * @brief Get the name of the video backend
         * 
         * @return `const char*` the name of the video backend as a human readable string
         */
        inline const char* getVideoBackendName() const noexcept
        {return m_vDesc->getName();}

        /**
         * @brief a function that is called when the GLGE instance is started
         */
        virtual void onStart() override
        {}

        /**
         * @brief a function that is called every time the GLGE instance is updated
         */
        virtual void onUpdate() override {}

        /**
         * @brief a function that is called every tick from the main thread
         */
        virtual void onMainUpdate() override;

        /**
         * @brief a function that is called when the GLGE instance shuts down
         */
        virtual void onShutdown() override
        {}

        /**
         * @brief actually setup the graphic instance
         * 
         * The instance extension is only fully valid once the instance is set, so this is the point in time where the graphics backend is set up
         */
        virtual void onInstanceSetting() override;

        /**
         * @brief a function that is used by the backend to initialize backend critical structures
         */
        void onGraphicBackendInit();

        /**
         * @brief a function that is used by the backend to deinitialize the backend critical structures
         */
        void onGraphicBackendDestroy();

        /**
         * @brief Get the Graphic Backend Instance of the instance
         * 
         * @return `Reference<GLGE::Graphic::Backend::Graphic::Instance>` a pointer to the graphic backend instance
         */
        inline Reference<GLGE::Graphic::Backend::Graphic::Instance> getGraphicBackendInstance() const noexcept
        {return m_gInst;}

        /**
         * @brief Get the Video Backend Instance of the instance
         * 
         * @return `GLGE::Graphic::Backend::Video::Instance*` a pointer to the video backend instance
         */
        inline GLGE::Graphic::Backend::Video::Instance* getVideoBackendInstance() const noexcept
        {return m_vInst;}

        /**
         * @brief Get the Graphic Description used by the graphic instance
         * 
         * @return `GLGE::Graphic::Backend::Graphic::Description*` a pointer to the graphic description used by the graphic instance
         */
        inline GLGE::Graphic::Backend::Graphic::Description* getGraphicDescription() const noexcept
        {return m_gDesc;}

        /**
         * @brief Get the Video Description used by the graphic instance
         * 
         * @return `GLGE::Graphic::Backend::Video::Description*` a pointer to the video description used by the graphic instance
         */
        inline GLGE::Graphic::Backend::Video::Description* getVideoDescription() const noexcept
        {return m_vDesc;}

        /**
         * @brief check if the used backend supports HDR
         * 
         * @return `true` if the backend supports HDR, `false` otherwise 
         */
        inline bool supportsHDR() const noexcept
        {return m_gInst->supportsHDR();}

        /**
         * @brief Get the Display Setup known to the graphic instance
         * 
         * @return `const GLGE::Graphic::DisplaySetup&` a constant reference to the display setup
         */
        const GLGE::Graphic::DisplaySetup& getDisplaySetup() const noexcept;

        /**
         * @brief Get the Mesh Pool
         * 
         * @return `Reference<GLGE::Graphic::Backend::MeshPool>` a reference to the backend mesh pool
         */
        inline Reference<GLGE::Graphic::Backend::Graphic::MeshPool> getMeshPool() const noexcept
        {return m_meshPool;}

        /**
         * @brief get the name of the GPU
         * 
         * @return `const std::string&` the name of the GPU
         */
        inline const std::string& getGPUName() const noexcept
        {return m_gDesc->getGPUName();}

        /**
         * @brief get the name of the GPU vendor
         * 
         * @return `const std::string&` the name of the GPU vendor
         */
        inline const std::string& getGPUVendorName() const noexcept
        {return m_gDesc->getGPUVendorName();}

        /**
         * @brief get the name of the GPU driver version
         * 
         * @return `const std::string&` the name of the GPU driver version
         */
        inline const std::string& getGPUDriverVersion() const noexcept
        {return m_gDesc->getDriverVersion();}

    protected:

        //add the window as a friend class
        friend class GLGE::Graphic::Window;
        //add the render pipeline as a friend class
        friend class GLGE::Graphic::RenderPipeline;

        /**
         * @brief store the backend description for the graphic instance
         */
        GLGE::Graphic::Backend::Graphic::Description* m_gDesc;
        /**
         * @brief store the backend description for the video system
         */
        GLGE::Graphic::Backend::Video::Description* m_vDesc;

        /**
         * @brief store the internal graphic instance
         */
        Reference<Backend::Graphic::Instance> m_gInst = nullptr;
        /**
         * @brief store the internal video instance
         */
        Backend::Video::Instance* m_vInst = nullptr;

        /**
         * @brief store a reference to the mesh pool
         */
        Reference<Backend::Graphic::MeshPool> m_meshPool;

        /**
         * @brief store all the windows
         */
        std::vector<Window*> m_windows;

        /**
         * @brief store the currently bound graphic instance
         */
        inline static thread_local GLGE::Graphic::Instance* msl_instance = nullptr;

        //windows are friends
        friend class Window;

    };

}
}

#endif
/**
 * @file Instance.h
 * @author DM8AT
 * @brief define an overload of the backend instance for Vulkan
 * @version 0.1
 * @date 2026-04-14
 * 
 * @copyright Copyright (c) 2026
 * 
 */
//header guard
#ifndef _GLGE_GRAPHIC_BACKEND_BUILTIN_GRAPHICS_VK_INSTANCE_
#define _GLGE_GRAPHIC_BACKEND_BUILTIN_GRAPHICS_VK_INSTANCE_

//add backend instances
#include "Graphic/Backend/Graphics/Instance.h"

//include common stuff
#include "Core/Common.h"
//add atomics
#include <atomic>
//add a mutex
#include <mutex>

//backend windows are defined elsewhere
namespace GLGE::Graphic::Backend::Graphic {class Window;}

//use the graphic backend OpenGL namespace
namespace GLGE::Graphic::Backend::Graphic {
/**
 * @brief a namespace that stores the OpenGL overloads for all graphic backend related classes
 */
namespace Vulkan {

    /**
     * @brief extend the graphic instance for vulkan functionality
     */
    class Instance : public GLGE::Graphic::Backend::Graphic::Instance {
    public:

        /**
         * @brief store a reference to a queue
         */
        struct QueueReference {
            /**
             * @brief store a lock for a mutex
             */
            std::unique_lock<std::mutex> lock;
            /**
             * @brief store the actual vulkan queue
             */
            void* queue;

            /**
             * @brief Construct a new Queue Reference
             * 
             * @param mtx a reference to the mutex
             * @param _queue the actual vulkan queue
             */
            QueueReference(std::mutex& mtx, void* _queue)
             : lock(mtx), queue(_queue)
            {}

            /**
             * @brief Construct a new Queue Reference
             * 
             * Move constructor
             */
            QueueReference(QueueReference&&) = default;

            /**
             * @brief Destroy the Queue Reference
             */
            ~QueueReference() = default;
        };

        /**
         * @brief store a pool of multiple queues that can be submitted to from multiple sources
         */
        struct QueuePool {
            /**
             * @brief store the index of the queue family
             */
            u32 familyIdx = 0;
            /**
             * @brief store the index of the queue to pull
             */
            std::atomic_uint32_t idx{0};
            /**
             * @brief store the single use command pool
             */
            void* singleUsePool = nullptr;
            /**
             * @brief store the amount of existing queues
             */
            u32 queueCount = 0;
            /**
             * @brief store all the actual queues
             */
            std::pair<void*, std::mutex>* queues = nullptr;

            /**
             * @brief get a queue that is safe to operate on
             * 
             * @return `QueueReference` a reference to the queue
             */
            QueueReference acquire() {
                u32 i = idx.fetch_add(1, std::memory_order_acq_rel) % queueCount;
                return std::move(QueueReference(queues[i].second, queues[i].first));
            }

            /**
             * @brief Destroy the Queue Pool
             */
            ~QueuePool()
            {if (queueCount) {delete[] queues;}}
        };

        /**
         * @brief Construct a new Instance for OpenGL
         * 
         * @param instance a pointer to the frontend instance to attach to
         */
        Instance(GLGE::Graphic::Instance* instance);

        /**
         * @brief Destroy the Instance
         */
        virtual ~Instance();

        /**
         * @brief a hook that is called when the instance is bound
         */
        virtual void onBind() override;

        /**
         * @brief a hook that is called when the instance is unbound
         */
        virtual void onUnbind() override;

        /**
         * @brief a function that is called when a new window using the graphic instance was created
         * 
         * @param window a pointer to the new window
         */
        virtual void onRegisterWindow(GLGE::Graphic::Backend::Graphic::Window* window) override;

        /**
         * @brief a function that is called when a window that belonged to the instance was closed
         * 
         * @param window a pointer to the closed window
         */
        virtual void onRemoveWindow(GLGE::Graphic::Backend::Graphic::Window* window) override;

        /**
         * @brief Get the vulkan instance
         * 
         * @return `void*` the vulkan instance
         */
        inline void* getInstance() const noexcept
        {return m_vkInstance;}

        /**
         * @brief Get the vulkan device
         * 
         * @return `void*` the vulkan device
         */
        inline void* getDevice() const noexcept
        {return m_device;}

        /**
         * @brief Get the Physical Device
         * 
         * @return `void*` the physical device
         */
        inline void* getPhysicalDevice() const noexcept
        {return m_physicalDevice;}

        /**
         * @brief get the graphics queue
         */
        QueuePool& getGraphicsQueue() noexcept
        {return *m_graphicsQueue;}

        /**
         * @brief get the transfer queues
         */
        QueuePool& getTransferQueue() noexcept
        {return *m_transferQueue;}

        /**
         * @brief get the compute queues
         */
        QueuePool& getComputeQueue() noexcept
        {return *m_computeQueue;}

        /**
         * @brief Get the VMA allocator
         * 
         * @return `void*` the VMA allocator instance
         */
        void* getAllocator() const noexcept
        {return m_allocator;}

        /**
         * @brief Get the max sample count enum value
         * 
         * @return `i32` the max sample count enum value
         */
        i32 getMaxSampleCount() const noexcept
        {return m_maxSampleCount;}

        /**
         * @brief Get the vkCreateRenderPass2KHR function
         * 
         * @return `void*` a pointer to the function
         */
        void* get_vkCreateRenderPass2KHR() const noexcept
        {return m_vkCreateRenderPass2KHR;}

        /**
         * @brief Get the Depth Averaging Modes
         * 
         * @return `i32` the depth averaging modes
         */
        i32 getDepthAveragingModes() const noexcept
        {return m_validDepthAveraging;}

    protected:

        /**
         * @brief store the amount of known windows
         */
        std::atomic_uint64_t m_windowCount = 0;

        /**
         * @brief store the vulkan instance
         */
        void* m_vkInstance = nullptr;
        /**
         * @brief store the selected physical device
         */
        void* m_physicalDevice = nullptr;
        /**
         * @brief store the vulkan device
         */
        void* m_device = nullptr;

        /**
         * @brief store the VMA allocator
         */
        void* m_allocator = nullptr;

        /**
         * @brief store the vkCreateRenderPass2KHR function
         */
        void* m_vkCreateRenderPass2KHR = nullptr;

        /**
         * @brief store the graphics queue
         */
        QueuePool* m_graphicsQueue = nullptr;
        /**
         * @brief store the transfer queues
         */
        QueuePool* m_transferQueue = nullptr;
        /**
         * @brief store the compute queues
         */
        QueuePool* m_computeQueue = nullptr;

        /**
         * @brief store the maximal samples for framebuffer attachments
         * 
         * This is the minimum of the depth and color sample count
         */
        i32 m_maxSampleCount = 0;
        /**
         * @brief store the depth averaging modes
         */
        i32 m_validDepthAveraging = 0;

    };

}
}

#endif
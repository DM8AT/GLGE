/**
 * @file CommandBuffer.h
 * @author DM8AT
 * @brief Wrap Vulkan secondary command buffers
 * @version 0.1
 * @date 2026-05-08
 *
 * @copyright Copyright (c) 2026
 *
 */
#ifndef _GLGE_GRAPHIC_BACKEND_BUILTIN_GRAPHICS_VK_COMMAND_BUFFER_
#define _GLGE_GRAPHIC_BACKEND_BUILTIN_GRAPHICS_VK_COMMAND_BUFFER_
//add the backend
#include "Graphic/Backend/Graphics/CommandBuffer.h"
//for size_t
#include <cstddef>

//use the library namespace
namespace GLGE::Graphic::Backend::Graphic::Vulkan {

    /**
     * @brief the default command buffer for vulkan
     */
    class CommandBuffer : public GLGE::Graphic::Backend::Graphic::CommandBuffer {
    public:
        /**
         * @brief Define the maximum number of allowed command buffers.
         */
        static constexpr size_t MAX_CMD_BUFFS = 4;

        /**
         * @brief Construct a new Command Buffer.
         *
         * @param instance a pointer to the instance the command buffer belongs to
         */
        CommandBuffer(GLGE::Graphic::Instance* instance);

        /**
         * @brief Destroy the Command Buffer
         */
        virtual ~CommandBuffer();

        /**
         * @brief adjusts the number of allocated secondary command buffers.
         *
         * @param count the number of buffers to allocate (clamped to MAX_CMD_BUFFS).
         */
        void setCommandBufferCount(size_t count);

        /**
         * @brief Get a specific Vulkan secondary command buffer
         *
         * @param index The index of the buffer to fetch (defaults to 0)
         * @return `void*` the Vulkan command buffer, or nullptr if out of bounds
         */
        inline void* getBuffer(size_t index = 0) const noexcept {
            if (index < m_cmdBufferCount) return m_cmdBuffers[index];
            return nullptr; 
        }

        /**
         * @brief Get the Buffer Count
         * 
         * @return `size_t` the amount of used buffers
         */
        inline size_t getBufferCount() const noexcept
        {return m_cmdBufferCount;}

    protected:

        /**
         * @brief a function that is called when the command buffer is started
         */
        virtual void onBegin() override;
        /**
         * @brief a function that is called when the command buffer is finalized
         */
        virtual void onFinalize() override;
        /**
         * @brief a function that is called when the buffer is executed
         */
        virtual void onPlay() override;

        /**
         * @brief store the command pool
         */
        void* m_cmdPool = nullptr;
        /**
         * @brief store the command buffers
         */
        void* m_cmdBuffers[MAX_CMD_BUFFS] = {nullptr};
        /**
         * @brief store the amount of used command buffers
         */
        size_t m_cmdBufferCount = 1;
    };
}
#endif
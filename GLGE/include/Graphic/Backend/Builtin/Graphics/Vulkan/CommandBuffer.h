 /**
  * @file CommandBuffer.h
  * @author DM8AT
  * @brief Wrap a single Vulkan secondary command buffer
  * @version 0.1
  * @date 2026-05-08
  *
  * @copyright Copyright (c) 2026
  *
  */

 //header guard
 #ifndef _GLGE_GRAPHIC_BACKEND_BUILTIN_GRAPHICS_VK_COMMAND_BUFFER_
 #define _GLGE_GRAPHIC_BACKEND_BUILTIN_GRAPHICS_VK_COMMAND_BUFFER_

 //add the default command buffer
 #include "Graphic/Backend/Graphics/CommandBuffer.h"

 //use the namespace
 namespace GLGE::Graphic::Backend::Graphic::Vulkan {

     /**
      * @brief Store a Vulkan secondary command buffer
      */
     class CommandBuffer : public GLGE::Graphic::Backend::Graphic::CommandBuffer {
     public:

         /**
          * @brief Construct a new Command Buffer.
          *
          * @param instance a pointer to the instance the command buffer belongs to
          *
          * The command buffer is initialized in an empty state
          */
         CommandBuffer(GLGE::Graphic::Instance* instance);

         /**
          * @brief Destroy the Command Buffer
          */
         virtual ~CommandBuffer();

         /**
          * @brief Get the Vulkan secondary command buffer
          *
          * @return `void*` the Vulkan command buffer
          */
         inline void* getBuffer() const noexcept
         { return m_cmdBuffer; }

     protected:

         /**
          * @brief Begin recording the secondary command buffer
          *
          * @param window an optional pointer to the window to operate on
          */
         virtual void onBegin(GLGE::Graphic::Window* window) override;

         /**
          * @brief End the recording phase.
          *
          * @param window an optional pointer to the window to operate on
          */
         virtual void onEnd(GLGE::Graphic::Window* window) override;

         /**
          * @brief Finalize the secondary command buffer.
          */
         virtual void onFinalize() override;

         /**
          * @brief Play back the recorded command buffer.
          *
          * This is intentionally empty. Secondary command buffers cannot
          * be submitted directly and this class does not own submission.
          */
         virtual void onPlay() override;

         /**
          * @brief Store the command pool.
          */
         void* m_cmdPool = nullptr;

         /**
          * @brief Store the single Vulkan secondary command buffer.
          */
         void* m_cmdBuffer = nullptr;

     };

 }

 #endif
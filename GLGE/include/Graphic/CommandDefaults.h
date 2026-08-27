/**
 * @file CommandDefaults.h
 * @author DM8AT
 * @brief define the default commands
 * @version 0.1
 * @date 2026-08-27
 * 
 * @copyright Copyright (c) 2026
 * 
 */
//header guard
#ifndef _GLGE_GRAPHIC_DEFAULT_COMMANDS_
#define _GLGE_GRAPHIC_DEFAULT_COMMANDS_

//add commands
#include "Command.h"

//add types that are used in commands
#include "Window.h"
#include "ColorInfo.h"
#include "Framebuffer.h"
#include "RenderTarget.h"

//use the library namespace
namespace GLGE::Graphic {

    /**
     * @brief a namespace that contains the default render command frontends
     */
    namespace Cmd {

        /**
         * @brief define a command that is used to clear a target
         */
        class Clear : public GLGE::Graphic::Command {
        public:

            /**
             * @brief define what the storage actually contains
             */
            enum StorageType {
                /**
                 * @brief the storage holds a window
                 */
                WINDOW,
                /**
                 * @brief the storage holds a framebuffer
                 */
                FRAMEBUFFER
            };

            /**
             * @brief define the type used for storage
             * 
             * @note In contrast to the other frontends, window is not just a thin wrapper
             */
            using Storage = std::variant<Window*, Framebuffer>;

            /**
             * @brief Construct a new Clear Command
             * 
             * @param win the window to clear
             * @param col the color to clear it to
             * @param depth the depth to clear it with, default is 1
             * @param stencil the stencil value to clear to, default is 0
             */
            Clear(Window* win, const vec4& col, float depth = 1.f, u32 stencil = 0)
             : Command(*win, col, depth, stencil), m_storage(win), m_sType(StorageType::WINDOW), m_targetIdx(0), m_color(col), m_depth(depth), m_stencil(stencil)
            {markDirty();}

            /**
             * @brief Construct a new Clear Command
             * 
             * @param fbuff the framebuffer to clear
             * @param attachmentIdx the color attachment of the framebuffer to clear
             * @param col the color to clear it to
             * @param depth the depth to clear it with, default is 1
             * @param stencil the stencil to clear to, default is 0
             */
            Clear(const Framebuffer& fbuff, u8 attachmentIdx, const vec4& col, float depth = 1.f, u32 stencil = 0)
             : Command(fbuff, attachmentIdx, col, depth, stencil), m_storage(fbuff), m_sType(StorageType::FRAMEBUFFER), m_targetIdx(attachmentIdx), m_color(col), m_depth(depth), m_stencil(stencil)
            {markDirty();}

            /**
             * @brief Destroy the Clear Command
             */
            virtual ~Clear() {}

            /**
             * @brief set the target to clear
             * 
             * @param win the window to clear
             */
            inline void set(Window* win) noexcept
            {m_storage = win; m_targetIdx = 0; m_sType = StorageType::WINDOW; markDirty();}

            /**
             * @brief set the target to clear
             * 
             * @param fbuff the framebuffer to clear
             * @param attachmentIdx the index of the color attachment to clear
             */
            inline void set(const Framebuffer& fbuff, u8 attachmentIdx) noexcept
            {m_storage = fbuff; m_targetIdx = attachmentIdx; m_sType = StorageType::FRAMEBUFFER; markDirty();}

            /**
             * @brief Get the Storage
             * 
             * @return `const Storage&` a constant reference to the storage
             */
            inline const Storage& getStorage() const noexcept
            {return m_storage;}

            /**
             * @brief Get the Storage Type
             * 
             * @return `StorageType` the type of the stored target
             */
            inline StorageType getStorageType() const noexcept
            {return m_sType;}

            /**
             * @brief Set the Clear Color
             * 
             * @param color the color to clear to
             */
            inline void setClearColor(const vec4& color) noexcept
            {m_color = color; markDirty();}

            /**
             * @brief Get the Clear Color
             * 
             * @return `const vec4&` the current clear color
             */
            inline const vec4& getClearColor() const noexcept
            {return m_color;}

            /**
             * @brief Set the Clear Depth
             * 
             * @param depth the depth to clear to
             */
            inline void setClearDepth(float depth) noexcept
            {m_depth = depth; markDirty();}

            /**
             * @brief Get the Clear Depth
             * 
             * @return `float` the current clear depth
             */
            inline float getClearDepth() const noexcept
            {return m_depth;}

            /**
             * @brief Set the Clear Stencil
             * 
             * @param stencil the stencil to clear to
             */
            inline void setClearStencil(u32 stencil) noexcept
            {m_stencil = stencil; markDirty();}

            /**
             * @brief Get the Clear Stencil
             * 
             * @return `u32` the current clear stencil
             */
            inline u32 getClearStencil() const noexcept
            {return m_stencil;}

            /**
             * @brief Set the Attachment Index
             * 
             * The attachment index defines which attachment to clear. 
             * If the storage type does not use attachment indices, it is ignored. 
             * 
             * @param idx the new attachment index
             */
            inline void setAttachmentIndex(u8 idx) noexcept
            {m_targetIdx = idx; markDirty();}

            /**
             * @brief Get the Attachment Index
             * 
             * @return `u8` the current attachment index
             */
            inline u8 getAttachmentIndex() const noexcept
            {return m_targetIdx;}

            /**
             * @brief Get the command type of the command
             * 
             * The type determines the implementation function that is called
             * 
             * @return `Backend::Graphic::CommandType` the type of the command
             */
            [[nodiscard]] virtual Backend::Graphic::CommandType getType() const noexcept override
            {return Backend::Graphic::COMMAND_CLEAR; /*identify this command as a clear command*/}

            /**
             * @brief Get the a handle that contains the copied command arguments
             * 
             * Handles are used to parse data between the command frontend and the implementation functions
             * in a type-save way
             * 
             * @return `Backend::Graphic::CommandHandle` a handle that contains the command arguments
             */
            [[nodiscard]] virtual Backend::Graphic::CommandHandle getHandle() noexcept {
                //Note: Handles are designed to be owning. This ensures that the data stays stable during recording
                return Backend::Graphic::CommandHandle::create<
                    GLGE::Graphic::RenderTarget, u8, vec4, f32, u32
                >((m_sType == StorageType::WINDOW) ? RenderTarget(std::get<Window*>(m_storage)) : RenderTarget(&std::get<Framebuffer>(m_storage)), 
                  m_targetIdx, m_color, m_depth, m_stencil
                );
            }

        protected:

            /**
             * @brief actual storage for the objects
             * 
             * Remember: The frontend wrappers are only thin wrappers that reference backends
             *           As long as the frontend storage exists, the backends remain valid (ref-counted)
             */
            Storage m_storage;
            /**
             * @brief define what the storage contains
             */
            StorageType m_sType;
            /**
             * @brief define the color target index to clear if a target with multiple indices is used
             */
            u8 m_targetIdx = 0;
            /**
             * @brief store the color to clear to
             */
            vec4 m_color = {0,0,0,1};
            /**
             * @brief store the depth to clear to
             */
            float m_depth = 1.f;
            /**
             * @brief store the clear value for the stencil
             */
            u32 m_stencil = 0;

        };

    }

}

#endif
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
#include "Shader.h"

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
            Clear(Window& win, const vec4& col, float depth = 1.f, u32 stencil = 0)
             : Command(win, col, depth, stencil), m_storage(&win), m_sType(StorageType::WINDOW), m_targetIdx(0), m_color(col), m_depth(depth), m_stencil(stencil)
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
            Clear(Framebuffer& fbuff, u8 attachmentIdx, const vec4& col, float depth = 1.f, u32 stencil = 0)
             : Command(fbuff, attachmentIdx, col, depth, stencil), m_storage(fbuff), m_sType(StorageType::FRAMEBUFFER), m_targetIdx(attachmentIdx), m_color(col), m_depth(depth), m_stencil(stencil)
            {markDirty();}

            /**
             * @brief Destroy the Clear Command
             */
            virtual ~Clear() {}

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
        /**
         * @brief define a command for a copy command
         *
         * Copies the contents of one render target to another.
         */
        class Copy : public GLGE::Graphic::Command {
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
             * @brief Construct a new Copy Command
             *
             * Copies from a window to a window
             *
             * @param from The source window
             * @param to The destination window
             */
            Copy(Window& from, Window& to)
             : Command(from, to), 
               m_from(&from), m_fromType(StorageType::WINDOW), m_to(&to), m_toType(StorageType::WINDOW), m_fromIdx(0), m_toIdx(0)
            {markDirty();}

            /**
             * @brief Construct a new Copy Command
             *
             * Copies from a framebuffer to a window
             *
             * @param from The source framebuffer
             * @param to The destination window
             * @param fromIdx The source color attachment
             */
            Copy(Framebuffer& from, Window& to, u8 fromIdx = 0)
             : Command(from, to), 
               m_from(from), m_fromType(StorageType::FRAMEBUFFER), m_to(&to), m_toType(StorageType::WINDOW), m_fromIdx(fromIdx), m_toIdx(0)
            {markDirty();}

            /**
             * @brief Construct a new Copy Command
             *
             * Copies from a window to a framebuffer
             *
             * @param from The source window
             * @param to The destination framebuffer
             * @param toIdx The destination color attachment
             */
            Copy(Window& from, Framebuffer& to, u8 toIdx = 0)
             : Command(from, to), 
               m_from(&from), m_fromType(StorageType::WINDOW), m_to(to), m_toType(StorageType::FRAMEBUFFER), m_fromIdx(0), m_toIdx(toIdx)
            {markDirty();}

            /**
             * @brief Construct a new Copy Command
             *
             * Copies from one framebuffer to another
             *
             * @param from The source framebuffer
             * @param to The destination framebuffer
             * @param fromIdx The source color attachment
             * @param toIdx The destination color attachment
             */
            Copy(Framebuffer& from, u8 fromIdx, Framebuffer& to, u8 toIdx)
             : Command(from, to), 
               m_from(from), m_fromType(StorageType::FRAMEBUFFER), m_to(to), m_toType(StorageType::FRAMEBUFFER), m_fromIdx(fromIdx), m_toIdx(toIdx)
            {markDirty();}

            /**
             * @brief Destroy the Copy Command
             */
            virtual ~Copy() {}

            /**
             * @brief Get the source storage
             *
             * @return `const Storage&` The source storage
             */
            [[nodiscard]] inline const Storage& getFrom() const noexcept
            {return m_from;}

            /**
             * @brief Get the source storage type
             *
             * @return `StorageType` The source storage type
             */
            [[nodiscard]] inline StorageType getFromType() const noexcept
            {return m_fromType;}

            /**
             * @brief Get the destination storage
             *
             * @return `const Storage&` The destination storage
             */
            [[nodiscard]] inline const Storage& getTo() const noexcept
            {return m_to;}

            /**
             * @brief Get the destination storage type
             *
             * @return `StorageType` The destination storage type
             */
            [[nodiscard]] inline StorageType getToType() const noexcept
            {return m_toType;}

            /**
             * @brief Set the source attachment index
             *
             * Ignored when the source is a window
             *
             * @param idx The source attachment index
             */
            inline void setFromAttachmentIndex(u8 idx) noexcept {
                m_fromIdx = idx;
                markDirty();
            }

            /**
             * @brief Get the source attachment index
             *
             * @return `u8` The source attachment index
             */
            [[nodiscard]] inline u8 getFromAttachmentIndex() const noexcept
            {return m_fromIdx;}

            /**
             * @brief Set the destination attachment index
             *
             * Ignored when the destination is a window
             *
             * @param idx The destination attachment index
             */
            inline void setToAttachmentIndex(u8 idx) noexcept {
                m_toIdx = idx;
                markDirty();
            }

            /**
             * @brief Get the destination attachment index
             *
             * @return `u8` The destination attachment index
             */
            [[nodiscard]] inline u8 getToAttachmentIndex() const noexcept
            {return m_toIdx;}

            /**
             * @brief Get the command type
             *
             * @return `Backend::Graphic::CommandType`
             */
            [[nodiscard]]virtual Backend::Graphic::CommandType getType() const noexcept override
            {return Backend::Graphic::COMMAND_COPY;}

            /**
             * @brief Get a handle containing the copied command arguments
             *
             * Handles are owning, ensuring that all data remains valid while the command is being recorded
             *
             * @return `Backend::Graphic::CommandHandle`
             */
            [[nodiscard]] virtual Backend::Graphic::CommandHandle getHandle() noexcept {
                //Create owning render targets from the stored frontend objects
                RenderTarget fromTarget = (m_fromType == StorageType::WINDOW) ? RenderTarget(std::get<Window*>(m_from)) : RenderTarget(&std::get<Framebuffer>(m_from));
                RenderTarget toTarget = (m_toType == StorageType::WINDOW) ? RenderTarget(std::get<Window*>(m_to)) : RenderTarget(&std::get<Framebuffer>(m_to));
                return Backend::Graphic::CommandHandle::create<RenderTarget, u8, RenderTarget, u8>(std::move(fromTarget), m_fromIdx, std::move(toTarget), m_toIdx);
            }

        protected:

            /**
             * @brief Store the object to copy from
             */
            Storage m_from;
            /**
             * @brief Define the storage type for the from element
             */
            StorageType m_fromType;
            /**
             * @brief Store the object to copy to
             */
            Storage m_to;
            /**
             * @brief Define the storage type for the to element
             */
            StorageType m_toType;

            /**
             * @brief Define the index to copy from
             */
            u8 m_fromIdx = 0;
            /**
             * @brief Define the index to copy to
             */
            u8 m_toIdx = 0;

        };

        /**
         * @brief define a command to dispatch a compute shader
         */
        class DispatchCompute : public GLGE::Graphic::Command {
        public:

            /**
             * @brief Construct a new Dispatch Compute
             * 
             * @warning The shader must remain valid for the lifetime of the command
             * 
             * @param shader a reference to the shader to use
             * @param extent the extent of the compute shader to use
             */
            DispatchCompute(Shader& shader, const uvec3& extent)
             : Command(shader), m_extent(extent), m_shader(&shader)
            {
                #if !GLGE_DEBUG
                if (!m_shader->isValidComputeShader())
                {throw GLGE::Exception("Cannot use non-compute shader for dispatch compute", "GLGE::Graphic::Cmd::DispatchCompute::DispatchCompute");}
                #endif
            }

            /**
             * @brief Destroy the Dispatch Compute
             */
            virtual ~DispatchCompute() {}

            /**
             * @brief Get the Shader that the command uses
             * 
             * @return `Shader*` a pointer to the shader
             */
            inline Shader* getShader() const noexcept
            {return m_shader;}

            /**
             * @brief Set the Extent
             * 
             * @param extent the new extent
             */
            inline void setExtent(const uvec3& extent) noexcept {
                //update only if not matching
                if ((extent.x != m_extent.x) || (extent.y != m_extent.y) || (extent.z != m_extent.z)) {
                    m_extent = extent;
                    onInvalidate();
                }
            }

            /**
             * @brief Get the Extent
             * 
             * @return `const uvec3&` the extent of the dispatch command
             */
            inline const uvec3& getExtent() const noexcept
            {return m_extent;}

            /**
             * @brief Get the command type
             *
             * @return `Backend::Graphic::CommandType`
             */
            [[nodiscard]]virtual Backend::Graphic::CommandType getType() const noexcept override
            {return Backend::Graphic::COMMAND_DISPATCH_COMPUTE;}

            /**
             * @brief Get a handle containing the copied command arguments
             *
             * Handles are owning, ensuring that all data remains valid while the command is being recorded
             *
             * @return `Backend::Graphic::CommandHandle`
             */
            [[nodiscard]] virtual Backend::Graphic::CommandHandle getHandle() noexcept {
                //Create owning render targets from the stored frontend objects
                return Backend::Graphic::CommandHandle::create<GLGE::Graphic::Shader*, GLGE::uvec3>(m_shader, m_extent);
            }

        protected:

            /**
             * @brief store a pointer to the shader to dispatch
             */
            Shader* m_shader = nullptr;

            /**
             * @brief define the extent of the shader to run
             */
            uvec3 m_extent;

        };

    }

}

#endif
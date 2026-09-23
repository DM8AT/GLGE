/**
 * @file DebugContext.h
 * @author DM8AT
 * @brief define a debug context that is used to maintain state while recording data for the GPU
 * @version 0.1
 * @date 2026-09-19
 * 
 * @copyright Copyright (c) 2026
 * 
 */
//header guard
#ifndef _GLGE_GRAPHIC_DEBUG_VISUALIZER_
#define _GLGE_GRAPHIC_DEBUG_VISUALIZER_

//add the command system
#include "Command.h"
//add components
#include "Components.h"
//add targets
#include "RenderTarget.h"

//add the base class
#include "GLGE/Core/BaseClass.h"

//add the debug draw data provider
#include "DebugDrawDataProvider.h"

//add the backend
#include "Backend/Graphics/DebugDrawer.h"

//add variants
#include <variant>

//use the library namespace
namespace GLGE::Graphic {

    /**
     * @brief Define the context for debug visualization
     * 
     * This is used to actually render debug visualization data
     * 
     * @note this does GPU-Operations. 
     */
    class DebugContext : public CommandInvalidator, public BaseClass {
    public:

        /**
         * @brief define the current state of the context
         */
        enum class State : u8 {
            /**
             * @brief no recording was started nor finished
             */
            UNINITIALIZED = 0,
            /**
             * @brief a recording was started but not yet finished
             */
            RECORDING = 1,
            /**
             * @brief a recording was started and finished
             */
            RECORDED = 2
        };

        /**
         * @brief define a structure to store recorded commands
         */
        struct Command {
            /**
             * @brief Command sub-type to set the camera
             */
            struct SetCamera {
                /**
                 * @brief the camera to set to
                 */
                Component::Camera camera;
                /**
                 * @brief the 3D position of the camera
                 */
                vec3 pos;
            };
            /**
             * @brief Command sub-type to set the target
             */
            struct SetTarget {
                /**
                 * @brief the new target to render to
                 */
                RenderTarget target;
            };
            /**
             * @brief Command sub-type to draw something
             */
            struct Draw {
                /**
                 * @brief store the amount of entries to draw
                 */
                u32 drawCount;
                /**
                 * @brief store the start index for the commands
                 */
                u32 cmdStartIdx;
            };

            /**
             * @brief store the actual command record
             */
            std::variant<
                SetCamera, 
                SetTarget,
                Draw
            > command;
        };

        /**
         * @brief Construct a new Debug Context
         */
        DebugContext() : BaseClass() {}

        /**
         * @brief Destroy the Debug Context
         */
        ~DebugContext() = default;

        //immovable, cannot be copied
        DebugContext(DebugContext&&) = delete;
        DebugContext(const DebugContext&) = delete;
        DebugContext& operator=(DebugContext&&) = delete;
        DebugContext& operator=(const DebugContext&) = delete;

        /**
         * @brief start the recording of the debug context
         * 
         * This readies the context for debug recording. This sets the state to `RECORDING` and prepares the backend. 
         * 
         * @warning this call is only valid if the context is NOT in a recording state
         * @note this does not invalidate old recordings. The backend should create a new recording target while leaving a potential old one valid. 
         */
        void beginRecording();

        /**
         * @brief Set the Camera
         * 
         * @warning this call is only valid if the context is in a recording state
         * @note this needs to be submitted before calling any submit renderer commands
         * 
         * @param camera the camera component to use
         * @param pos the world position of the camera
         */
        void setCamera(const Component::Camera& camera, const vec3& pos);

        /**
         * @brief Set the Target
         * 
         * @warning this call is only valid if the context is in a recording state
         * @note this needs to be submitted before calling any submit renderer commands
         * 
         * @param target the target to render to
         */
        void setTarget(const RenderTarget& target);

        /**
         * @brief draw something
         * 
         * @note this will copy the data from the provider. This will not change the provider in any way. 
         * 
         * @param drawData a pointer to a debug draw data provider that provides the draw data
         */
        void draw(const DebugDrawDataProvider* drawData);
        
        /**
         * @brief finish the recording
         * 
         * This sets the state to `RECORDED` and finalizes the backend. 
         * 
         * @warning this call is only valid if the context is in a recording state
         * @note this call invalidates the old recording. 
         */
        void endRecording();

        /**
         * @brief Get the State of the context
         * 
         * @return `State` the current state
         */
        inline State getState() const noexcept
        {return m_state;}

        /**
         * @brief Get all currently recorded commands
         * 
         * @return `const std::vector<Command>&` the currently recorded commands
         */
        inline const std::vector<Command>& getCommands() const noexcept
        {return m_cmds;}

        /**
         * @brief Get the Draw Sub Commands
         * 
         * @return `const std::vector<DebugDrawDataProvider::CommandRecord>` the recorded draw commands
         */
        inline const std::vector<DebugDrawDataProvider::CommandRecord> getDrawSubCommands() const noexcept
        {return m_drawCmdRecords;}

        /**
         * @brief Get the Vertex Buffer
         * 
         * @return `const std::vector<vec3>&` the full recorded vertex buffer
         */
        inline const std::vector<vec3>& getVertexBuffer() const noexcept
        {return m_vertices;}

        /**
         * @brief Get the Index Buffer
         * 
         * @return `const std::vector<u32>&` the full recorded index buffer
         */
        inline const std::vector<u32>& getIndexBuffer() const noexcept
        {return m_indices;}

    protected:

        /**
         * @brief store the current state of the context
         */
        State m_state = State::UNINITIALIZED;

        /**
         * @brief store the backend
         */
        GLGE::Reference<GLGE::Graphic::Backend::Graphic::DebugDrawer> m_backend;

        /**
         * @brief store all currently recorded commands
         */
        std::vector<Command> m_cmds;
        /**
         * @brief store recordings for all draw command sub-commands
         */
        std::vector<DebugDrawDataProvider::CommandRecord> m_drawCmdRecords;
        /**
         * @brief store a unified vertex buffer
         */
        std::vector<vec3> m_vertices;
        /**
         * @brief store a unified index buffer
         */
        std::vector<u32> m_indices;

    };

}

#endif
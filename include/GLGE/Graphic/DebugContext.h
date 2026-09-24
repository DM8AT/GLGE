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

//add GPU buffers
#include "Buffer.h"
//add shader
#include "Shader.h"

//add the debug draw data provider
#include "DebugDrawDataProvider.h"

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
             * @brief Command sub-type to set the currently used shader
             */
            struct SetShader {
                /**
                 * @brief store a pointer to the new shader to use
                 */
                GLGE::Graphic::Shader* shader;
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
                SetShader,
                Draw
            > command;
        };

        /**
         * @brief define how the per-draw data for the geometry looks
         */
        struct PerDraw {
            /**
             * @brief only used when rendering in vertex mode, set to 0 if not
             */
            float pointSize;
            /**
             * @brief the index of the camera matrix in the camera matrix buffer
             */
            i32 cameraIdx;
            /**
             * @brief store the index of the target to render to
             */
            i32 targetIdx;

            /**
             * @brief unused padding, aligns for std430 layout
             */
            u32 padding;

            /**
             * @brief color in RGBA format
             */
            GLGE::vec4 color;
        };

        /**
         * @brief Construct a new Debug Context
         * 
         * @param defaultShader a pointer to the default shader to use. Nullptr is valid. 
         */
        explicit DebugContext(GLGE::Graphic::Shader* defaultShader = nullptr)
         : BaseClass(), 
           m_defaultShader(defaultShader),
           m_vbo(Buffer::Type::STORAGE, nullptr, 64, Buffer::Usage::STREAMING_UPLOAD), 
           m_ibo(Buffer::Type::STORAGE, nullptr, 64, Buffer::Usage::STREAMING_UPLOAD),
           m_camBuff(Buffer::Type::STORAGE, nullptr, 64, Buffer::Usage::STREAMING_UPLOAD), 
           m_targetInfoBuff(Buffer::Type::STORAGE, nullptr, 64, Buffer::Usage::STREAMING_UPLOAD),
           m_perDrawBuff(Buffer::Type::STORAGE, nullptr, 64, Buffer::Usage::STREAMING_UPLOAD)
        {}

        /**
         * @brief Destroy the Debug Context
         */
        ~DebugContext();

        //immovable, cannot be copied
        DebugContext(DebugContext&&) = delete;
        DebugContext(const DebugContext&) = delete;
        DebugContext& operator=(DebugContext&&) = delete;
        DebugContext& operator=(const DebugContext&) = delete;

        /**
         * @brief Set the Default Shader
         * 
         * The default shader is the shader that will be initially set on begin recording. Nullptr is valid and skips this setting. 
         * 
         * @warning the shader pointer must remain valid for the lifetime of the debug context or until it is updated again
         * @note this does not invalidate the resource
         * 
         * @param shader a pointer to the new default shader
         */
        inline void setDefaultShader(GLGE::Graphic::Shader* shader)
        {m_defaultShader = shader;}

        /**
         * @brief Get the Default Shader
         * 
         * @return `GLGE::Graphic::Shader*` a pointer to the default shader
         */
        inline GLGE::Graphic::Shader* getDefaultShader() const noexcept
        {return m_defaultShader;}

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
         * @brief Set the Shader to use
         * 
         * @param shader a pointer to the shader to use
         */
        void setShader(GLGE::Graphic::Shader* shader);

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

        /**
         * @brief get the vertex buffer
         * 
         * @return `Buffer*` the vertex buffer
         */
        inline Buffer* getVBO() noexcept
        {return &m_vbo;}

        /**
         * @brief get the index buffer
         * 
         * @return `Buffer*` the index buffer
         */
        inline Buffer* getIBO() noexcept
        {return &m_ibo;}

        /**
         * @brief get the camera buffer
         * 
         * @return `Buffer*` the camera buffer
         */
        inline Buffer* getCameraBuffer() noexcept
        {return &m_camBuff;}

        /**
         * @brief get the per draw buffer
         * 
         * @return `Buffer*` the per draw buffer
         */
        inline Buffer* getPerDrawbuffer() noexcept
        {return &m_perDrawBuff;}

        /**
         * @brief get the target info buffer
         * 
         * @return `Buffer*` the target info buffer
         */
        inline Buffer* getTargetInfoBuffer() noexcept
        {return &m_targetInfoBuff;}

        /**
         * @brief Get the Backend Data
         * 
         * @warning The layout and contents are fully defined by the backend
         * 
         * @return `void*` a pointer to the backend data
         */
        inline void* getBackendData() const noexcept
        {return m_backendData;}

        /**
         * @brief Set the Backend Data
         * 
         * @param newData a pointer to the new backend data
         */
        inline void setBackendData(void* newData) noexcept
        {m_backendData = newData;}

    protected:

        /**
         * @brief store the current state of the context
         */
        State m_state = State::UNINITIALIZED;

        /**
         * @brief store the default shader
         */
        GLGE::Graphic::Shader* m_defaultShader = nullptr;

        /**
         * @brief store all currently recorded commands
         */
        std::vector<Command> m_cmds;
        /**
         * @brief store recordings for all draw command sub-commands
         */
        std::vector<DebugDrawDataProvider::CommandRecord> m_drawCmdRecords;
        /**
         * @brief store all recorded camera matrices
         */
        std::vector<glm::mat4> m_camMatrices;
        /**
         * @brief store how many targets where set
         */
        u32 m_targetCount = 0;
        /**
         * @brief store the current shader
         */
        GLGE::Graphic::Shader* m_currentShader = nullptr;
        /**
         * @brief store a unified vertex buffer
         */
        std::vector<vec3> m_vertices;
        /**
         * @brief store a unified index buffer
         */
        std::vector<u32> m_indices;
        /**
         * @brief keep track of all currently referenced shaders
         */
        std::vector<GLGE::Graphic::Shader*> m_currentlyReferencedShader;
        /**
         * @brief a list to keep track of which shader were referenced in this recording cycle
         */
        std::vector<GLGE::Graphic::Shader*> m_newReferencedShader;
        /**
         * @brief keep track of all currently referenced targets
         */
        std::vector<RenderTarget> m_currentlyReferencedTargets;
        /**
         * @brief a list to keep track of which target was referenced in this recording cycle
         */
        std::vector<RenderTarget> m_newReferencedTargets;
        /**
         * @brief store the amount of draw calls recorded
         * 
         * This does not mean the amount that a draw command was added, but the sum of all sub-draw commands. 
         */
        u32 m_drawCallCount = 0;

        /**
         * @brief store the VBO for the debug context
         */
        Buffer m_vbo;
        /**
         * @brief store the IBO for the debug context
         */
        Buffer m_ibo;
        /**
         * @brief store a buffer that contains an UBO for the camera data
         */
        Buffer m_camBuff;
        /**
         * @brief store information about the render targets
         */
        Buffer m_targetInfoBuff;
        /**
         * @brief store a buffer that contains per-draw data for the geometry
         */
        Buffer m_perDrawBuff;

        /**
         * @brief store data for the backend
         */
        void* m_backendData = nullptr;

    };

}

#endif
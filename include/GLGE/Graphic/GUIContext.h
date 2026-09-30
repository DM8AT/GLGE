/**
 * @file GUIContext.h
 * @author DM8AT
 * @brief define a container for a single GUI context
 * @version 0.1
 * @date 2026-09-26
 * 
 * @copyright Copyright (c) 2026
 * 
 */
//header guard
#ifndef _GLGE_GRAPHIC_GUI_CONTEXT_
#define _GLGE_GRAPHIC_GUI_CONTEXT_

//add render targets
#include "RenderTarget.h"
//add the GUI provider
#include "GUIProvider.h"

//add the command system
#include "Command.h"

//add buffers
#include "Buffer.h"

//use the library namespace
namespace GLGE::Graphic {

    /**
     * @brief a class used to render a graphical user interface
     */
    class GUIContext : public CommandInvalidator {
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
         * @brief define a command
         */
        struct Command {
            /**
             * @brief use sub-commands
             */
            using DrawSubcmd = GUIProvider::Command;

            /**
             * @brief a command to set the start of the index and vertex buffer to draw from
             */
            struct SetDrawSubsection {
                /**
                 * @brief the new base vertex offset
                 */
                u32 baseVertexOffset;
                /**
                 * @brief the new base index offset
                 */
                u32 baseIndexOffset;
            };

            /**
             * @brief the actual recorded command
             */
            std::variant<
                DrawSubcmd,
                SetDrawSubsection
            > command;
        };

        /**
         * @brief define the function layout for the backend cleanup function
         */
        using CleanupPfn = void (*)(GUIContext*);

        /**
         * @brief Construct a new GUI Context
         */
        GUIContext();

        /**
         * @brief prepare the context for a new recording
         * 
         * @warning only valid if the state is NOT recording
         * 
         * @note this does not invalidate the old command buffers directly
         * @note this sets the state to recording
         */
        void beginRecording();

        /**
         * @brief render the data from a gui provider
         * 
         * @warning only valid when the state is recording
         * 
         * @param provider a constant pointer to the gui provider to draw from
         */
        void draw(const GUIProvider* provider);

        /**
         * @brief 
         * 
         * @warning only valid when the state is recording
         * 
         * @note this sets the state to recorded
         * @note this does actual GPU work. 
         */
        void endRecording();

        /**
         * @brief Get the Command buffer
         * 
         * @return `const std::vector<Command>` a ordered list of all recorded commands
         */
        inline const std::vector<Command> getCommands() const noexcept
        {return m_cmds;}

        /**
         * @brief Get the Vertex Buffer
         * 
         * @return `const std::vector<GUIProvider::Vertex>&` the full recorded vertex buffer
         */
        inline const std::vector<GUIProvider::Vertex>& getVertexBuffer() const noexcept
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
         * @brief get the projection buffer
         * 
         * @return `Buffer*` the projection matrix buffer
         */
        inline Buffer* getProjMatBuffer() noexcept
        {return &m_projMatBuff;}

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

        /**
         * @brief Set the Cleanup Fn
         * 
         * @warning NEVER USE THIS IN THE FRONTEND! IT MAY BREAK THE RENDERER!
         * 
         * @param fn the function used for clean up
         */
        inline void setCleanupFn(CleanupPfn fn) noexcept
        {m_cleanupFn = fn;}

    protected:

        /**
         * @brief store the state of the context
         */
        State m_state = State::UNINITIALIZED;
    
        /**
         * @brief store the index buffer of the provider
         */
        std::vector<u32> m_indices;
        /**
         * @brief store the vertex buffer of the provider
         */
        std::vector<GUIProvider::Vertex> m_vertices;

        /**
         * @brief keep track of all currently referenced images
         */
        std::vector<GLGE::Graphic::Image*> m_currentlyReferencedImages;
        /**
         * @brief a list to keep track of which images were referenced in this recording cycle
         */
        std::vector<GLGE::Graphic::Image*> m_newReferencedImages;
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
         * @brief store a buffer that contains all projection matrices
         */
        Buffer m_projMatBuff;

        /**
         * @brief store the commands
         * 
         * @note order is important
         */
        std::vector<GUIContext::Command> m_cmds;

        /**
         * @brief store data for the backend
         */
        void* m_backendData = nullptr;
        /**
         * @brief store the function used to clean up
         */
        CleanupPfn m_cleanupFn = nullptr;

    };

}

#endif
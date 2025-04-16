/**
 * @file GLGEGraphicAPI_CommandBuffer.h
 * @author DM8AT
 * @brief define an abstracted command buffer that can be used to fill out a command list on seperate threads and then executes them in a single main thread
 * @version 0.1
 * @date 2025-04-16
 * 
 * @copyright Copyright (c) 2025
 * 
 */

//header guard
#ifndef _GLGE_GRAPHIC_API_COMMAND_BUFFER_
#define _GLGE_GRAPHIC_API_COMMAND_BUFFER_

//include the settings
#include "../../GLGECore/GLGESettings.h"

//include instances
#include "GLGEGraphicAPI_Instance.h"

//check if this is C++
#if GLGE_CPP

/**
 * @brief store a command list that should be executed on the main render thread
 */
class GraphicCommandBuffer : public GraphicInstAttatchable
{
public:

    /**
     * @brief store a command for the command buffer
     */
    struct Command
    {
        /**
         * @brief store the identifyer for the command
         */
        uint64_t commandIdentifyer = 0;
        /**
         * @brief store the data for the command execution
         */
        void* commandData = 0;
        /**
         * @brief store the size of the data in bytes
         */
        uint64_t commandDataSize = 0;
    };

    /**
     * @brief Construct a new Graphic Command Buffer
     */
    GraphicCommandBuffer() = default;

    /**
     * @brief Construct a new Graphic Command Buffer
     * 
     * @param instance a pointer to the instance the command buffer belongs to
     */
    GraphicCommandBuffer(GraphicInstance* instance) {create(instance);}

    /**
     * @brief Destroy the Graphic Command Buffer
     */
    ~GraphicCommandBuffer() {destroy();}

    /**
     * @brief create a new command buffer
     * 
     * @param instance a pointer to the instance the command buffer belongs to
     */
    void create(GraphicInstance* instance);

    /**
     * @brief destroy the command buffer
     */
    void destroy();

    /**
     * @brief play back the command buffer and execute all stored commands
     */
    void play();

protected:

    /**
     * @brief a overloadable hook to create the command buffer
     */
    virtual void onCreate() {}
    /**
     * @brief a overloadable hook to destroy the command buffer
     */
    virtual void onDestroy() {}
    /**
     * @brief a overloadable hook to execute all stored commands
     */
    virtual void onPlay() {}

    /**
     * @brief store all the commands for the command buffer
     */
    std::vector<Command> m_commands;
    /**
     * @brief store if the command buffer is currently used
     */
    std::mutex m_inUse;
};

#endif //C++ section

#endif
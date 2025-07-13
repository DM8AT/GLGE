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

//say that windows will be defined
class Window;

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
         * @brief Construct a new Command
         */
        Command() = default;

        /**
         * @brief Construct a new Command
         * 
         * @param cI the identifyer of the command
         * @param cP a pointer to a potential function or 0
         * @param cD the data for the command
         * @param cDS the size of the data in bytes for the command
         */
        Command(uint64_t cI, void* cP, void* cD, uint64_t cDS) : commandIdentifyer(cI), commandPtr(cP), commandData(cD), commandDataSize(cDS) {}

        /**
         * @brief store the identifyer for the command
         */
        uint64_t commandIdentifyer = 0;
        /**
         * @brief store a pointer to a custom function that executes the commands
         */
        void* commandPtr = 0;
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
    virtual ~GraphicCommandBuffer() {destroy();}

    /**
     * @brief create a new command buffer
     * 
     * @param instance a pointer to the instance the command buffer belongs to
     */
    virtual void create(GraphicInstance* instance);

    /**
     * @brief destroy the command buffer
     */
    virtual void destroy();

    /**
     * @brief play back the command buffer and execute all stored commands
     */
    virtual void play();

    /**
     * @brief record a command
     * 
     * @param command the index of the command to record
     * @param func a pointer to a function that could be executed
     * @param data the data for the command
     * @param dataSize the size of the data in bytes
     */
    inline void add(uint64_t command, void* func, void* data, uint64_t dataSize) {m_commands.emplace_back(command, func, data, dataSize);}

    /**
     * @brief begin the usage of the command buffer
     */
    inline void begin() {m_inUse.lock();}

    /**
     * @brief end the usage of the command buffer
     */
    inline void end() {m_inUse.unlock();m_filled = true;}

    /**
     * @brief empty the command buffer and discard all commands
     */
    inline void clear() {m_commands.clear();}

    /**
     * @brief get if the command buffer is filled
     * 
     * @return true : the command buffer is filled
     * @return false : the command buffer is not filled
     */
    inline bool isFilled() const noexcept {return m_filled;}

    /**
     * @brief Get the currently active window
     * 
     * @return Window* a pointer to the current active window
     */
    inline Window* getCurrentWindow() noexcept {return m_current;}

    /**
     * @brief Set the Currently active window of the command buffer
     * 
     * @warning ONLY USE IF YOU KNOW WHAT YOU ARE DOING
     * 
     * @param window a pointer to the new currently active window
     */
    inline void setCurrentWindow(Window* window) noexcept {m_current = window;}

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
    /**
     * @brief store if the command buffer is filled
     */
    bool m_filled = false;

    /**
     * @brief store a pointer to the currently active window or 0 if none is active
     */
    Window* m_current = 0;
};

#endif //C++ section

#endif
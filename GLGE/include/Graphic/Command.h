/**
 * @file Command.h
 * @author DM8AT
 * @brief define a structure that holds information about rendering commands
 * @version 0.1
 * @date 2025-12-30
 * 
 * @copyright Copyright (c) 2025
 * 
 */
//header guard
#ifndef _GLGE_GRAPHIC_COMMAND_
#define _GLGE_GRAPHIC_COMMAND_

//include common stuff
#include "Core/Common.h"

//use the library namespace
namespace GLGE::Graphic {

    /**
     * @brief store a table of command names to simplify readability
     */
    enum CommandType : u32 {
        /**
         * @brief a default / error value for the type
         * 
         * During execution this acts as a hard-coded NOOP value. 
         */
        COMMAND_UNKNOWN = 0,

        /**
         * @brief swap the front and back buffer of a window
         * 
         * @param window a pointer to the window to swap
         */
        COMMAND_SWAP_WINDOW = 1,

        /**
         * @brief clear a render target with a solid color, a potential depth and a potential stencil value
         * 
         * @param target the render target to clear
         * @param index `u8` the potential ID of the color attachment to clear, ignored if unused by the target type
         * @param color a vec<f32, 4> that stores the clear color. x is the red channel, y the green channel, z the blue channel and w the alpha channel. 
         * @param depth a float that defines the clear depth
         * @param stencil a `u32` that defines the clear value for the stencil buffer
         */
        COMMAND_CLEAR = 2,

        /**
         * @brief copy the contents of one render target to another render target
         * 
         * @warning only one at maximum of the targets may be a window
         * 
         * @param from the render target to copy from
         * @param from_idx `u8` the index of the image to copy from (only used if viable for render target)
         * @param to the render target to copy to
         * @param to_idx `u8` the index of the image to copy to (only used if viable for render target)
         */
        COMMAND_COPY = 3,

        /**
         * @brief dispatch a compute shader
         * 
         * @param shader a pointer to the shader to dispatch
         * @param invocations a 3D vector with the amount of instances to create per axis
         */
        COMMAND_DISPATCH_COMPUTE = 4,

        /**
         * @brief draw a simple mesh, potentially instancing it multiple times
         * 
         * @param mesh a pointer to the mesh to draw
         * @param material a pointer to the material to use
         * @param target the render target to draw the mesh to
         * @param LOD the level of detail of the mesh to draw
         * @param instanceCount the amount of instances of the mesh to draw
         */
        COMMAND_DRAW_MESH = 5,

        /**
         * @brief draw all objects that are enabled at the time of recording the buffer
         * 
         * @param renderer a pointer to the renderer to use to render the world
         */
        COMMAND_DRAW_WORLD = 6,

        /**
         * @brief a base value for custom commands. All custom commands must 
         */
        COMMAND_CUSTOM = 0xfff
    };

    /**
     * @brief store a full command entry. 
     * 
     * A command consists of an identifier (a value from the `CommandType` enum) and 
     * a payload. The payload acts as a list of parameters. 
     */
    template <typename... Args>
    class Command {
    public:

        /**
         * @brief store the size of the payload in bytes
         */
        static constexpr size_t PAYLOAD_SIZE = (sizeof(Args) + ... + 0);

        /**
         * @brief Construct a new Command
         * 
         * @param _type the type of the command
         * @param _payload the payload of the command
         */
        Command(u32 _type = (u32)CommandType::COMMAND_UNKNOWN, Args... _payload)
         : type((CommandType)_type) {
            //copy all arguments in order
            size_t pos = 0;
            (packArgument(_payload, pos), ...);
        }

        /**
         * @brief store what type of command is stored
         */
        CommandType type = COMMAND_UNKNOWN;
        /**
         * @brief store the raw data for the payload
         */
        u8 payload[PAYLOAD_SIZE];

    protected:

        /**
         * @brief a helper function to pack a single argument
         * 
         * @tparam T the type for the argument to pack
         * @param arg the value to pack
         * @param offset the offset to pack to
         */
        template <typename T> 
        void packArgument(const T& arg, size_t& offset) {
            //copy the argument into the payload
            memcpy(payload + offset, &arg, sizeof(T));
            //go to the next argument
            offset += sizeof(T);
        }

    };

    /**
     * @brief a struct that is invoked if a type is NOT a `Command` type
     * 
     * evaluates to false
     * 
     * @tparam T the type that is NOT of the type `Command`
     */
    template <typename T>
    struct is_command : std::false_type {};

    /**
     * @brief a struct that is invoked if a type is a `Command` type
     * 
     * evaluates to true
     * 
     * @tparam Args the arguments of the command type
     */
    template <typename... Args>
    struct is_command<Command<Args...>> : std::true_type {};

    /**
     * @brief a value that is used to check if something is a command type
     * 
     * @tparam T the type to check
     * 
     * `true` if T is a command type, `false` if not
     */
    template <typename T>
    inline constexpr bool is_command_v = is_command<T>::value;

}

#endif
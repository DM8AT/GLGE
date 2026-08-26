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
     * @brief store a full command entry. 
     * 
     * A command consists of an identifier (a value from the `CommandType` enum) and 
     * a payload. The payload acts as a list of parameters. 
     */
    template <typename... Args>
    requires (
        (std::is_trivially_copyable_v<Args> && ...) && 
        (std::is_trivially_copy_assignable_v<Args> && ...) 
    )
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
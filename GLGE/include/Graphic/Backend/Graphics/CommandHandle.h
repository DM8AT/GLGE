/**
 * @file CommandHandle.h
 * @author DM8AT
 * @brief a command handle is a wrapper around a list of arguments that can be parsed to a function
 * 
 * A command handle has a type-erased interface where the arguments can be extracted as any list of types that 
 * exactly matches the stored size. All type handles do not store the argument data themself. They store a pointer
 * to an external data pool where the parameter data is read from. 
 * 
 * @version 0.1
 * @date 2025-12-30
 * 
 * @copyright Copyright (c) 2025
 * 
 */
//header guard
#ifndef _GLGE_GRAPHIC_BACKEND_GRAPHIC_COMMAND_HANDLE_
#define _GLGE_GRAPHIC_BACKEND_GRAPHIC_COMMAND_HANDLE_

//add common stuff
#include "Core/Common.h"
//add exceptions
#include "Core/Exception.h"

//use the backend namespace
namespace GLGE::Graphic::Backend::Graphic {

    /**
     * @brief a command handle is the structure used to parse arguments to graphic commands
     * 
     * A command handle is a type-erased interface to a block of RAM that can be re-interpreted
     * as any set of types at will. The command handle does only store the size and a pointer into
     * a data pool where the data is actually stored. It does not hold a copy of the parameters. 
     * 
     * @warning the size of the read types must exactly match the size of the command region. This 
     * safety guard is set up so the types are not read as different types accidentally. 
     */
    class CommandHandle {
    public:

        /**
         * @brief Construct a new Command Handle
         * 
         * @param size the size of the command handle
         * @param data the actual data storage
         */
        CommandHandle(size_t size = 0, u8* data = nullptr)
         : m_size(size), m_data(data)
        {}

        /**
         * @brief create a new command handle from a set of arguments
         * 
         * @tparam Args a list of types that the parameters have
         * @param size the size of the region in the buffer reserved for the commands
         * @param buffer a pointer to the actual buffer that will store the data
         * @param args a list of arguments to feed into the buffer
         * @return CommandHandle a command handle to the arguments in the buffer region
         */
        template <typename... Args>
        requires (std::is_trivially_copyable_v<Args>, ...)
        inline static CommandHandle create(size_t size, u8* buffer, Args&&... args) {
            //compute the actually required size
            constexpr size_t s = (sizeof(Args) + ... + 0);
            //sanity check when in debug
            #if GLGE_DEBUG
            if (s > size) {
                throw GLGE::Exception("Tried to create a command handle but the parsed arguments don't fit into the required size", "GLGE::Graphic::Backend::Graphic::CommandHandle");
            }
            #endif

            //copy all the elements
            u8* curr = buffer;
            (packOne(&curr, args), ...);

            //return a finalized handle
            return CommandHandle(s, buffer);
        }

        /**
         * @brief get the arguments stored in the command handle
         * 
         * @tparam Args a list of types for the arguments to return
         * @return `std::tuple<Args...>` a tuple containing the data for all the arguments
         */
        template <typename ...Args>
        requires (std::is_trivially_copyable_v<Args>, ...)
        inline std::tuple<Args...> getArguments() const {
            //compute the required size
            constexpr size_t s = (sizeof(Args) + ... + 0);

            //in debug do a quick sanity check to prevent wrong types
            #if GLGE_DEBUG
            if (s != m_size) {
                throw GLGE::Exception("Tried to read data from a command handle but tried to read too little", "GLGE::Graphic::Backend::Graphic::CommandHandle::getArguments");
            }
            #endif

            //copy all the elements into a tuple
            const u8* ptr = m_data;
            return std::tuple<Args...> { readOne<Args>(&ptr)... };
        }

        /**
         * @brief access a specific data element
         * 
         * @tparam T the type of element to access
         * @param offset the offset in bytes of the element
         * @return `T*` a pointer to the element
         */
        template <typename T>
        T* access(size_t offset) noexcept
        {return reinterpret_cast<T*>(m_data + offset);}

    protected: 

        /**
         * @brief a helper function to store a single argument into a tightly packed buffer
         * 
         * @tparam T the type of the element to store
         * @param ptr a pointer to a pointer to the buffer
         * @param arg the argument to store
         */
        template <typename T>
        inline static void packOne(u8** ptr, const T& arg) {
            //copy over the actual data
            memcpy(*ptr, &arg, sizeof(T));
            //advance the pointer
            *ptr += sizeof(T);
        }

        /**
         * @brief a function to read a single element from a buffer
         * 
         * @tparam T the type of the element to read
         * @param ptr a pointer to a pointer that stores the data
         * @return T 
         */
        template <typename T>
        inline static const T& readOne(const u8** ptr) {
            //store the value
            const T* value = reinterpret_cast<const T*>(*ptr);
            //move up the pointer value
            *ptr += sizeof(T);
            //return the read value
            return *value;
        }

        /**
         * @brief store the size of the region that holds the command parameters
         */
        size_t m_size = 0;
        /**
         * @brief store a pointer to the region that actually holds the command parameters
         */
        u8* m_data = nullptr;

    };

}

#endif
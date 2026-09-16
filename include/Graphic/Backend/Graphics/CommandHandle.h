/**
 * @file CommandHandle.h
 * @author DM8AT
 * @brief Type-erased storage for graphic command arguments.
 *
 * A command handle stores a heterogeneous collection of objects in a single
 * type-erased memory block
 *
 * Objects are constructed using placement new and destroyed explicitly when
 * the command handle is destroyed. The memory layout accounts for the
 * alignment requirements of every stored type.
 *
 * @version 0.2
 * @date 2026-08-26
 *
 * @copyright Copyright (c) 2025
 */
//header guard
#ifndef _GLGE_GRAPHIC_BACKEND_GRAPHIC_COMMAND_HANDLE_
#define _GLGE_GRAPHIC_BACKEND_GRAPHIC_COMMAND_HANDLE_

//add common stuff
#include "Core/Common.h"
//add exceptions
#include "Core/Exception.h"

//standard library
#include <cstddef>
#include <cstdint>
#include <memory>
#include <new>
#include <tuple>
#include <type_traits>
#include <utility>
#include <vector>

//use the backend namespace
namespace GLGE::Graphic::Backend::Graphic {

    /**
     * @brief Type-erased storage for the arguments of a graphic command
     * 
     * This structure allows for type-erased storage, movement and reconstruction of a type-erased payload
     */
    class CommandHandle {
    public:

        /**
         * @brief Construct an empty command handle
         */
        CommandHandle() noexcept = default;

        /**
         * @brief Destructor
         *
         * Explicitly destroys all objects stored in the command
         */
        ~CommandHandle() 
        {destroyObjects();}

        /**
         * @brief Command handles own their storage and therefore cannot be copied
         */
        CommandHandle(const CommandHandle&) = delete;

        /**
         * @brief Command handles can be moved
         *
         * @param other handle to move from
         */
        CommandHandle(CommandHandle&& other) noexcept
            : m_data(std::move(other.m_data)), m_size(other.m_size), m_alignment(other.m_alignment), m_objects(std::move(other.m_objects))
        {
            //invalidate other
            other.m_size = 0;
            other.m_alignment = 1;
        }

        /**
         * @brief Move assignment
         *
         * @param other handle to move from
         * @return reference to this handle
         */
        CommandHandle& operator=(CommandHandle&& other) noexcept {
            //prevent move to self
            if (this == &other) {return *this;}

            //clean up
            destroyObjects();

            //move data
            m_data = std::move(other.m_data);
            m_size = other.m_size;
            m_alignment = other.m_alignment;
            m_objects = std::move(other.m_objects);

            //invalidate other
            other.m_size = 0;
            other.m_alignment = 1;

            //return ref to self
            return *this;
        }

        /**
         * @brief Create a command handle from a set of arguments
         *
         * @tparam Args argument types
         * @param args arguments to store
         * @return a command handle containing the arguments
         */
        template <typename... Args>
        static CommandHandle create(const Args&... args) {
            //Store the resulting command handle
            CommandHandle result;

            //The types used for storage must not be references.
            //If you want to use indirection, use pointer
            static_assert((!std::is_reference_v<Args> && ...), "CommandHandle cannot store reference types, for indirection use pointers");

            //The stored type is the decayed argument type
            using Types = std::tuple<std::decay_t<Args>...>;
            //Compute size and alignment at compile time
            constexpr size_t size = calculateSize<std::decay_t<Args>...>();
            constexpr size_t alignment = calculateAlignment<std::decay_t<Args>...>();
            result.m_size = size;
            result.m_alignment = alignment;
            //allocate required storage
            if constexpr (size != 0)
            {result.m_data = allocateStorage(size, alignment);}

            size_t offset = 0;
            //Note: constructOne can throw. Use catch to clean up before re-throwing
            try {
                (
                constructOne<std::decay_t<Args>>(result.m_data.get(), offset, result.m_objects, args),
                ...
                );
            }
            catch (...) {
                result.destroyObjects();
                throw;
            }

            //Return the final command handle
            return result;
        }

        /**
         * @brief Get copies of all stored arguments
         *
         * The requested types must exactly match the types used when the
         * command was created.
         *
         * @tparam Args types of the arguments
         * @return tuple containing copies of the stored objects
         */
        template <typename... Args>
        std::tuple<Args...> getArguments() const {
            //compute expected size
            constexpr size_t expectedSize = calculateSize<Args...>();

            //sanity check
            #if GLGE_DEBUG
            if (expectedSize != m_size) {
                throw GLGE::Exception("Tried to read data from a command handle with an incompatible set of argument types", "GLGE::Graphic::Backend::Graphic::CommandHandle::getArguments");
            }
            #endif

            //read into tuple
            size_t offset = 0;
            return std::tuple<Args...>{readOne<Args>(offset)...};
        }

        /**
         * @brief Access an object stored in the command.
         *
         * The returned pointer refers directly to the object stored inside
         * the command handle. The object remains valid until the command
         * handle is destroyed or moved from.
         *
         * @tparam T type of the stored object
         * @param offset byte offset into the storage
         * @return pointer to the object
         */
        template <typename T>
        T* access(size_t offset) noexcept 
        {return std::launder(reinterpret_cast<T*>(m_data.get() + offset));}

        /**
         * @brief Const version of access().
         *
         * @tparam T type of the stored object
         * @param offset byte offset into the storage
         * @return pointer to the object
         */
        template <typename T>
        const T* access(size_t offset) const noexcept 
        {return std::launder(reinterpret_cast<const T*>(m_data.get() + offset));}

        /**
         * @brief Get the amount of storage occupied by the objects.
         *
         * @return storage size in bytes
         */
        size_t size() const noexcept 
        {return m_size;}

    private:

        /**
         * @brief Metadata required to destroy one type-erased object
         */
        struct ObjectInfo {
            /**
             * @brief Byte offset of the object inside the storage
             */
            size_t offset;

            /**
             * @brief Function used to destroy the object
             */
            void (*destroy)(void*) noexcept;
        };

        /**
         * @brief Destroy an object of type T
         *
         * @tparam T type of the stored object
         * @param ptr pointer to the object
         */
        template <typename T>
        static void destroyOne(void* ptr) noexcept 
        {static_cast<T*>(ptr)->~T();}

        /**
         * @brief Calculate the offset of an object after applying alignment.
         *
         * @param offset current offset
         * @param alignment required alignment
         * @return aligned offset
         */
        static constexpr size_t alignOffset(size_t offset, size_t alignment) noexcept {
            //compute how bad the current offset is in regards to the alignment
            const size_t remainder = offset % alignment;

            //special case: perfect match
            if (remainder == 0)
            {return offset;}

            //remainder is how bad it is, invert to correct it
            return offset + (alignment - remainder);
        }

        /**
         * @brief Calculate the total storage size required for a set of types
         *
         * This includes padding required to satisfy the alignment of each
         * individual object
         *
         * @tparam Args types to store
         * @return required storage size in bytes
         */
        template <typename... Args>
        static consteval size_t calculateSize() noexcept {
            //sum up using compile-time lambda folding to sum-up offset
            size_t offset = 0;
            (
                [&] {
                    offset = alignOffset(offset, alignof(Args));
                    offset += sizeof(Args);
                }(),
                ...
            );

            return offset;
        }

        /**
         * @brief Calculate the maximum alignment required by a set of types
         *
         * @tparam Args types to inspect
         * @return maximum alignment
         */
        template <typename... Args>
        static consteval size_t calculateAlignment() noexcept {
            //compute max alignment using compile-time lambda folding
            size_t alignment = 1;
            (
                [&] {
                    if (alignof(Args) > alignment)
                    {alignment = alignof(Args);}
                }(),
                ...
            );

            return alignment;
        }

        /**
         * @brief Construct one object in the storage
         *
         * @tparam T type of the object
         * @tparam Arg constructor argument type
         * @param ptr pointer to the storage
         * @param offset current storage offset
         * @param infos object metadata list
         * @param arg constructor argument
         */
        template <typename T, typename Arg>
        static void constructOne(std::byte* ptr, size_t& offset, std::vector<ObjectInfo>& infos, const Arg& arg) {
            //align the location correctly
            offset = alignOffset(offset, alignof(T));
            void* location = ptr + offset;
            //construct helper
            std::construct_at(static_cast<T*>(location), arg);
            //add type info
            infos.push_back({offset, &destroyOne<T>});
            //bookkeeping
            offset += sizeof(T);
        }

        /**
         * @brief Destroy all objects stored in the command
         */
        void destroyObjects() noexcept {
            //Objects must be destroyed in reverse construction order.
            for (auto it = m_objects.rbegin(); it != m_objects.rend(); ++it) 
            {it->destroy(m_data.get() + it->offset);}

            m_objects.clear();
        }

        /**
         * @brief Deleter for the command's aligned allocation
         */
        struct StorageDeleter {
            //store the alignment
            size_t alignment = alignof(std::max_align_t);

            /**
             * @brief functor to delete the storage
             * 
             * @param ptr a pointer to the data
             */
            void operator()(std::byte* ptr) const noexcept {
                //prevent nullptr
                if (ptr == nullptr)
                {return;}

                //delete operator invoker
                ::operator delete(ptr, std::align_val_t(alignment));
            }
        };

        /**
         * @brief Pointer type for the command storage
         */
        using StoragePtr = std::unique_ptr<std::byte[], StorageDeleter>;

        /**
         * @brief Allocate an aligned memory block
         *
         * @param size number of bytes
         * @param alignment required alignment
         * @return allocated memory
         */
        static StoragePtr allocateStorage(size_t size, size_t alignment) {
            //explicit memory alignment with untyped new
            void* ptr = ::operator new(size, std::align_val_t(alignment));

            //create storage ptr from it
            return StoragePtr(static_cast<std::byte*>(ptr), StorageDeleter{alignment});
        }

        /**
         * @brief Read one object from the storage
         *
         * @tparam T object type
         * @param offset current offset
         * @return const reference to the stored object
         */
        template <typename T>
        const T& readOne(size_t& offset) const {
            //compute offset
            offset = alignOffset(offset, alignof(T));
            //launder memory
            const T* value = std::launder(reinterpret_cast<const T*>(m_data.get() + offset));

            //step and return
            offset += sizeof(T);
            return *value;
        }

        /**
         * @brief Storage containing the actual objects
         */
        StoragePtr m_data{nullptr, StorageDeleter{}};

        /**
         * @brief Total size of the object storage
         */
        size_t m_size = 0;

        /**
         * @brief Alignment used for the storage allocation
         */
        size_t m_alignment = 1;

        /**
         * @brief Destruction metadata for all stored objects
         */
        std::vector<ObjectInfo> m_objects;
    };

}

#endif
/**
 * @file Event.h
 * @author DM8AT
 * @brief define a class for generic, type-erased events
 * @version 0.1
 * @date 2025-12-27
 * 
 * @copyright Copyright (c) 2025
 * 
 */
//header guard
#ifndef _GLGE_CORE_EVENT_
#define _GLGE_CORE_EVENT_

//add common stuff
#include "Common.h"
//also add type stuff
#include "TypeInfo.h"
//add exceptions
#include "Exception.h"

//add the profiler
#include "Core/Profiler.h"

//use the library namespace
namespace GLGE {

    /**
     * @brief store a type-erased event that contains an arbitrary payload and a u64 as identifier
     */
    class Event {
    public:

        /**
         * @brief Construct a new Event
         * 
         * Protected default constructor
         */
        Event() = default;

        /**
         * @brief Construct a new Event
         * 
         * @param id the id for the event
         * @param data the data for the event
         * @param dataSize the size of the data
         */
        constexpr Event(u64 id, u8* data, u8 dataSize)
         : m_id(id)
        {
            //in debug sanity check the size
            #if GLGE_DEBUG
            if (dataSize > sizeof(m_storage))
            {throw GLGE::Exception("Tried to create an event with data that is larger than 24 bytes", "Event::Event");}
            #endif
            //copy over the size that fits
            memcpy(m_storage, data, (dataSize > 24) ? 24 : dataSize);
        }

        /**
         * @brief Construct a new Event
         * 
         * @param name the name for the event (the FNV 1a 64 bit hash is used as the ID)
         * @param data a pointer to the data
         * @param dataSize the size of the data
         */
        constexpr Event(const std::string_view& name, u8* data, u8 dataSize)
         : Event(__FNV_1A_64_HASH(name), data, dataSize)
        {}

        /**
         * @brief check if the event has a specific ID
         * 
         * @param id the ID to check against
         * @return `true` if the IDs match, `false` otherwise
         */
        inline constexpr bool operator==(u64 id)
        {return m_id == id;}

        /**
         * @brief check if the event has a specific ID
         * 
         * @param name the name to check against. The ID is computed as the FNV 1A 64 bit hash of the string. 
         * @return `true` when the ID matches the FNV 1A 64 bit hash of the string
         */
        inline constexpr bool operator==(const std::string_view& name)
        {return m_id == __FNV_1A_64_HASH(name);}

        /**
         * @brief get the ID of the event
         * 
         * @return u64 the ID of the event
         */
        inline constexpr u64 getID() const noexcept
        {return m_id;}

        /**
         * @brief create a new event
         * 
         * @tparam Args the types of arguments to parse to the event
         * @param id the ID of the event
         * @param args the arguments of data to parse to the event
         * @return Event the constructed event
         */
        template <typename ...Args>
        static constexpr Event create(u64 id, Args&&... args) {
            //check that all arguments are trivially copyable
            static_assert((std::is_trivially_copyable_v<std::decay_t<Args>> && ...),
                        "All Event payload types must be trivially copyable");

            GLGE_PROFILER_SCOPE();
            
            //create the event to return
            Event e;
            e.m_id = id;

            //compute total size of all arguments
            constexpr size_t total_size = (sizeof(std::decay_t<Args>) + ... + 0);
            static_assert(total_size <= sizeof(e.m_storage), "Event payload is too large");

            //helper to copy arguments into storage sequentially
            u8* ptr = e.m_storage;
            //use a lambda to copy a single argument over
            auto copy_arg = [&ptr](auto&& arg) {
                //decay and memcpy the data, then increment the pointer
                //this makes sure that the next element doesn't override this one
                using T = std::decay_t<decltype(arg)>;
                memcpy(ptr, &arg, sizeof(T));
                ptr += sizeof(T);
            };

            //expand the fold expression and copy the arguments
            (copy_arg(std::forward<Args>(args)), ...);

            //return the finalized event
            return e;
        }

        /**
         * @brief create a new event
         * 
         * @tparam Args the types of arguments to parse to the event
         * @param name the name of the event. The ID is computed as the FNV 1A 64 bit hash of the string. 
         * @param args the arguments of data to parse to the event
         * @return Event the constructed event
         */
        template <typename ...Args>
        static constexpr Event create(const std::string_view& name, Args&&... args)
        {return create(__FNV_1A_64_HASH(name), std::forward<Args>(args)...);}

        /**
         * @brief get the internal data storage of the event
         * 
         * @tparam Args the types of the arguments to quarry
         * @return `auto` the stored arguments (type is evaluated dynamically)
         */
        template <typename ...Args>
        auto constexpr get() const {
            //check that all arguments are trivially copyable
            static_assert((std::is_trivially_copyable_v<std::decay_t<Args>> && ...),
                        "All Event payload types must be trivially copyable");

            GLGE_PROFILER_SCOPE();

            //compute total size of all arguments
            constexpr size_t total_size = (sizeof(std::decay_t<Args>) + ... + 0);
            static_assert(total_size <= sizeof(m_storage), "Event payload is too large");

            //helper to copy arguments into the tuple sequentially
            const u8* ptr = m_storage;

            //optimization for a single argument: Skip the tuple, use the type directly
            if constexpr (sizeof...(Args) == 1) {
                //get the first element
                using T = std::tuple_element_t<0, std::tuple<Args...>>;
                T value;
                //copy the data over
                memcpy(&value, ptr, sizeof(T));
                //return the value
                return value;
            } else {
                //create the tuple to return
                std::tuple<Args...> result;

                //use a lambda to copy a single argument over
                auto copy_arg_by_index = [&ptr, &result]<std::size_t... Is>(std::index_sequence<Is...>) mutable {
                    //copy to the correct element of the result and then increment the pointer to not override the previous element
                    //also expand the fold expression
                    ((memcpy(&std::get<Is>(result), ptr, sizeof(std::tuple_element_t<Is, std::tuple<Args...>>)),
                    ptr += sizeof(std::tuple_element_t<Is, std::tuple<Args...>>)), ...);
                };
                copy_arg_by_index(std::make_index_sequence<sizeof...(Args)>{});

                //return the finalized tuple
                return result;
            }
        }

        /**
         * @brief Get a pointer to the raw payload storage
         * 
         * @return u8* a pointer to the raw payload storage
         */
        inline constexpr u8* getStorage() const noexcept
        {return (u8*)m_storage;}

    protected:

        /**
         * @brief store the ID of the event
         */
        u64 m_id = 0;
        /**
         * @brief storage for the arbitrary payload
         */
        u8 m_storage[32 - sizeof(m_id)] = { 0 };

    };

}

#endif
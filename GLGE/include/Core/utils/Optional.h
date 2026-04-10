/**
 * @file Optional.h
 * @author DM8AT
 * @brief define a templated class that can hold a class instance that may not exist
 * @version 0.1
 * @date 2025-12-21
 * 
 * @copyright Copyright (c) 2025
 * 
 */
//header guard
#ifndef _GLGE_CORE_OPTIONAL_
#define _GLGE_CORE_OPTIONAL_

//add common stuff
#include "../Common.h"

//use the library namespace
namespace GLGE {

    /**
     * @brief a class that MAY hold an instance of T
     * 
     * @tparam T the type to potentially store an instance of
     */
    template <typename T> 
    class Optional {
    public:

        /**
         * @brief create a new optional instance that holds no data
         */
        constexpr Optional() = default;

        /**
         * @brief create an allready initialized instance
         * 
         * @tparam Args the argument types to parse to the instance
         * @param args a reference to all arguments to parse to the instance
         */
        template <typename ...Args>
        constexpr Optional(Args... args)
         : m_hasValue(true)
        {(void) new (m_storage) T(std::forward<Args>(args)...);}

        /**
         * @brief Destroy the Optional
         * 
         * This cleans up the stored element if it exists
         */
        constexpr ~Optional() 
        {clear();}

        /**
         * @brief create a new optional
         * 
         * @param other a constant reference to the other optional to create from
         */
        constexpr Optional(const Optional& other)
         : m_hasValue(other.m_hasValue)
        {
            //only copy if the data exists
            if (other.m_hasValue) {
                new (m_storage) T(*reinterpret_cast<const T*>(other.m_storage));
            }
        }

        /**
         * @brief move constructor
         * 
         * @param other a move reference to create from
         */
        constexpr Optional(Optional&& other) noexcept(std::is_nothrow_move_constructible_v<T>)
            : m_hasValue(other.m_hasValue)
        {
            //only move create if a value exists
            if (other.m_hasValue) {
                new (m_storage) T(std::move(*reinterpret_cast<T*>(other.m_storage)));
                //clean up the other's value
                other.clear();
            }
        }

        /**
         * @brief copy the data from another optional
         * 
         * @param other the other optional to copy from
         * @return `constexpr Optional&` the other optional to copy from
         */
        constexpr Optional& operator=(const Optional& other)
        {
            //don't allow a move to self. In that case, just return this
            if (this == &other)
            {return *this;}

            //copy the data over and correctly override the internal storage
            if (m_hasValue && other.m_hasValue) {
                **reinterpret_cast<T*>(m_storage) = *reinterpret_cast<const T*>(other.m_storage);
            }
            //else, just copy the data
            else if (other.m_hasValue) {
                new (m_storage) T(*reinterpret_cast<const T*>(other.m_storage));
                m_hasValue = true;
            }
            //else, no data exists. Clear the internal data. 
            else {
                clear();
            }

            //just return this
            return *this;
        }

        /**
         * @brief copy move assignment
         * 
         * @param other the other optional to copy from
         * @return `constexpr Optional&` a reference to this
         */
        constexpr Optional& operator=(Optional&& other) noexcept(
            //only make noexcept if noexcept is correct
            std::is_nothrow_move_assignable_v<T> &&
            std::is_nothrow_move_constructible_v<T>)
        {
            //if this is self, just return a reference to this
            if (this == &other)
            {return *this;}

            //if both have a value, use move assignment
            if (m_hasValue && other.m_hasValue) {
                *reinterpret_cast<T*>(m_storage) =
                    std::move(*reinterpret_cast<T*>(other.m_storage));
            }
            //else, if only the other has a value, store the other value
            else if (other.m_hasValue) {
                new (m_storage) T(std::move(*reinterpret_cast<T*>(other.m_storage)));
                m_hasValue = true;
            }
            //else, no value exists. So clean up. 
            else {
                clear();
            }

            //clean the other and return this
            other.clear();
            return *this;
        }

        /**
         * @brief clean up the stored contents
         */
        constexpr void clear() {
            //if a value exists, clear it
            if (m_hasValue) 
            {((T*)m_storage)->~T();}
            m_hasValue = false;
        }

        /**
         * @brief set the optional to a specific value
         * 
         * @tparam Args the arguments to parse to the constructor
         * @param args the arguments to parse to the constructor
         */
        template <typename ...Args>
        constexpr void set(Args... args) {
            //if no value exists, initialize the optional
            if (!m_hasValue) 
            {(void) new (m_storage) T(std::forward<Args>(args)...);}
            m_hasValue = true;
        }

        /**
         * @brief check if the optional has a value stored
         * 
         * @return true : a value is stored
         * @return false : no value is stored
         */
        inline constexpr bool hasValue() const noexcept {return m_hasValue;}

        /**
         * @brief get a reference to the stored element
         * 
         * @warning if the optional contains no value, then the returned reference is garbage
         * 
         * @return const T& a constant reference to the stored value
         */
        inline constexpr const T& get() const noexcept {return *((T*)m_storage);}

        /**
         * @brief get a reference to the stored element
         * 
         * @warning if the optional contains no value, then the returned reference is garbage
         * 
         * @return T& a reference to the stored value
         */
        inline constexpr T& get() noexcept {return *((T*)m_storage);}

        /**
         * @brief get a reference to the stored element
         * 
         * @warning if the optional contains no value, then the returned reference is garbage
         * 
         * @return T* a reference to the stored value
         */
        inline constexpr T* operator->() noexcept {return ((T*)m_storage);}

        /**
         * @brief access the internal data
         * 
         * @return constexpr T& a reference to the internal data
         */
        inline constexpr T& operator*() noexcept {return *((T*)m_storage);}

    protected:

        /**
         * @brief store a single instance of the T type
         */
        u8 m_storage[sizeof(T)] = { 0 };
        /**
         * @brief store if a value is stored
         */
        bool m_hasValue = false;

    };

}

#endif
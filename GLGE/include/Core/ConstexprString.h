/**
 * @file ConstexprString.h
 * @author DM8AT
 * @brief define a constexpr string container
 * @version 0.1
 * @date 2026-06-10
 * 
 * @copyright Copyright (c) 2026
 * 
 * @details
 * Provides a stack-allocated, fixed-capacity string type that is usable in
 * constexpr contexts (subject to compiler support).
 * 
 * The class owns its storage and guarantees null-termination.
 */
//header guard
#ifndef _GLGE_CORE_CONSTEXPR_STRING_
#define _GLGE_CORE_CONSTEXPR_STRING_

//add string views and arrays
#include <string_view>
#include <array>
//to overload output stream operator
#include <ostream>

/**
 * @brief use the libraries namespace
 */
namespace GLGE {

    /**
     * @brief define a constexpr string
     * 
     * @details
     * A statically-sized string type designed for constexpr-friendly usage.
     * The string always maintains a null-terminated buffer internally and
     * behaves similarly to std::string_view/std::string in read operations,
     * while owning its storage.
     * 
     * @tparam Capacity the maximum capacity of the constexpr string without the null terminator
     * @tparam _CharT Character type (default: char).
     * @tparam _Traits Character traits type (default: std::char_traits<_CharT>).
     */
    template <std::size_t Capacity = 127, typename _CharT = char, typename _Traits = std::char_traits<_CharT>>
    class BasicConstexprString {
    public:

        //this MUST be C++ 23
        static_assert(__cplusplus >= 202302L);

        /**
         * @brief the type that identifies a single value
         */
        using value_type                = _CharT;
        /**
         * @brief a type that holds the traits of the value type
         */
        using traits_type               = _Traits;
        /**
         * @brief a pointer to the value type
         */
        using pointer                   = value_type*;
        /**
         * @brief a constant pointer to a value type
         */
        using const_pointer             = const value_type*;
        /**
         * @brief a reference to a value type
         */
        using reference                 = value_type&;
        /**
         * @brief a constant reference to a value type
         */
        using const_reference           = const value_type&;
        /**
         * @brief a constant iterator to the value type
         */
        using const_iterator            = const value_type*;
        /**
         * @brief an iterator the value type
         * 
         * Same as a constant iterator in this case
         */
        using iterator                  = const_iterator;
        /**
         * @brief a reverse constant iterator
         */
        using const_reverse_iterator    = std::reverse_iterator<const_iterator>;
        /**
         * @brief a reverse iterator
         * 
         * same as a constant reverse iterator
         */
        using reverse_iterator          = const_reverse_iterator;
        /**
         * @brief store the type used for sizes
         */
        using size_type                 = std::size_t;
        /**
         * @brief store the type used for differences
         */
        using difference_type           = ptrdiff_t;

        /**
         * @brief use the same npos as the string view
         * 
         * This allows for easy use of the string view npos internally
         */
        static constexpr size_type npos = std::basic_string_view<value_type, traits_type>::npos;

        /**
         * @brief say that this string is always null terminated
         */
        static constexpr bool null_terminated = true;

        /**
         * @brief store the capacity of the string
         * 
         * @warning this does NOT include the null terminator. 
         */
        inline static constexpr size_type CAPACITY = Capacity;

        /**
         * @brief create a new empty constexpr string
         */
        constexpr BasicConstexprString() noexcept = default;

        /**
         * @brief create a new constexpr string
         * 
         * @warning if `Cap` is smaller than `size`, truncation may occur. 
         * 
         * @tparam s the size of the string to copy
         * @param other a constant reference to another constexpr string
         */
        template <size_type Cap>
        constexpr BasicConstexprString(const BasicConstexprString<Cap, value_type, traits_type>& other) noexcept {
            //compute the length
            size_type len = (other.size() > CAPACITY) ? CAPACITY : other.size();
            //copy the data over
            for (size_type i = 0; i < len; ++i)
            {m_storage[i] = other.data()[i];}
            //null-terminate
            terminate();
            //store the size
            m_size = len;
        }

        /**
         * @brief create a new constexpr string
         * 
         * @warning if `Cap` is smaller than `size`, truncation may occur. 
         * 
         * @tparam s the size of the string to move from
         * @param other a constant reference to another constexpr string
         */
        template <size_type Cap>
        constexpr BasicConstexprString(BasicConstexprString<Cap, value_type, traits_type>&& other) noexcept {
            //compute the length
            size_type len = (other.size() > CAPACITY) ? CAPACITY : other.size();
            //copy the data over
            for (size_type i = 0; i < len; ++i)
            {m_storage[i] = other.data()[i];}
            //null-terminate
            terminate();
            //store the size
            m_size = len;

            //clean up the other
            other.m_size = 0;
            other.terminate();
        }
        
        /**
         * @brief create a new constexpr string
         * 
         * @tparam N the size of the inputted array
         * @param str the string to store
         */
        template <size_type N>
        requires (N < (Capacity+1))
        constexpr BasicConstexprString(const value_type (&str)[N]) noexcept {
            //store the length
            constexpr size_type len = (N-1 > CAPACITY) ? CAPACITY : (N-1);
            //copy the data
            for (size_type i = 0; i < len; ++i)
            {m_storage[i] = str[i];}
            //store the size
            m_size = len;
            //null-terminate
            terminate();
        }

        /**
         * @brief store an inputted string view
         * 
         * @warning if the size of the string view is larger than `CAPACITY` the string will be truncated. 
         * 
         * @param str the string to store
         */
        constexpr BasicConstexprString(std::basic_string_view<value_type, traits_type> str) noexcept {
            //get the length of the section to copy
            size_type len = (str.size() > CAPACITY) ? CAPACITY : str.size();
            //copy the data over
            for (size_type i = 0; i < len; ++i) 
            {m_storage[i] = str[i];}
            //store the size
            m_size = len;
            //write null terminator
            terminate();
        }

        /**
         * @brief copy the data from another constexpr string
         * 
         * @tparam Cap the capacity of the string to copy
         * @param other the string to copy from
         * @return `constexpr ConstexprString<Capacity>&` a constant reference to this after copy
         */
        template <size_type Cap>
        constexpr BasicConstexprString<Capacity>& operator=(const BasicConstexprString<Cap, value_type, traits_type>& other) noexcept {
            //prevent copy to self
            //first check if the capacities are equal, they can only be the same if they have the same type
            if constexpr (Cap == CAPACITY)
            {if (this == &other) {return *this;}}

            //compute the length
            size_type len = (other.size() > CAPACITY) ? CAPACITY : other.size();
            //copy the data over
            for (size_type i = 0; i < len; ++i)
            {m_storage[i] = other.data()[i];}
            //store the size
            m_size = len;
            //null-terminate
            terminate();

            //return a reference to this
            return *this;
        }

        /**
         * @brief move the data from another constexpr string
         * 
         * @tparam Cap the capacity of the string to move
         * @param other the string to move from
         * @return `constexpr ConstexprString<Capacity>&` a constant reference to this after move
         */
        template <size_type Cap>
        constexpr BasicConstexprString<Capacity>& operator=(BasicConstexprString<Cap, value_type, traits_type>&& other) noexcept {
            //prevent move to self
            //first check if the capacities are equal, they can only be the same if they have the same type
            if constexpr (Cap == CAPACITY)
            {if (this == &other) {return *this;}}

            //compute the length
            size_type len = (other.size() > CAPACITY) ? CAPACITY : other.size();
            //copy the data over
            for (size_type i = 0; i < len; ++i)
            {m_storage[i] = other.data()[i];}
            //store the size
            m_size = len;
            //null-terminate
            terminate();

            //clean up the other
            other.m_size = 0;
            other.terminate();

            //return a reference to this
            return *this;
        }

        /**
         * @brief get a view into the string
         * 
         * @return `constexpr std::basic_string_view<value_type, traits_type>` a view into the constexpr string
         */
        inline constexpr std::basic_string_view<value_type, traits_type> view() const noexcept
        {return std::basic_string_view<value_type, traits_type>(m_storage.data(), m_size);}

        /**
         * @brief convert the constexpr string to a string view
         * 
         * @return `constexpr std::basic_string_view<value_type, traits_type>` a view into the constexpr string
         */
        inline constexpr operator std::basic_string_view<value_type, traits_type>() const noexcept
        {return view();}

        /**
         * @brief Get the Size of the constexpr string
         * 
         * @warning this does NOT include the null termination character
         * 
         * @return `constexpr size_type` the size of the string
         */
        inline constexpr size_type size() const noexcept
        {return m_size;}

        /**
         * @brief Get the Size of the constexpr string
         * 
         * @warning this does NOT include the null termination character
         * 
         * @return `constexpr size_type` the size of the string
         */
        inline constexpr size_type length() const noexcept
        {return m_size;}

        /**
         * @brief get the data
         * 
         * @return `constexpr const char*` a pointer to the continues data storage
         */
        inline constexpr const char* data() const noexcept
        {return m_storage.data();}

        /**
         * @brief get the data as a C string
         * 
         * It is allready prepared as a C string, so just return it. 
         * 
         * @warning if the string contains intermediate `\0` characters, the output will look truncated. 
         * 
         * @return `const char*` the C string
         */
        inline constexpr const char* c_str() const noexcept
        {return m_storage.data();}

        /**
         * @brief Get the capacity
         * 
         * @return `size_type` the capacity of the string
         */
        inline static constexpr size_type capacity() noexcept
        {return CAPACITY;}

        /**
         * @brief check if the constexpr string is empty
         * 
         * @return `true` if it is empty, `false` if not
         */
        inline constexpr bool empty() const noexcept
        {return m_size == 0;}

        /**
         * @brief access a specific element of the string
         * 
         * @warning the index is not bounds-checked
         * 
         * @param idx the index to query the data from
         * @return `constexpr value_type` the value at the index
         */
        inline constexpr value_type operator[](size_type idx) const noexcept
        {return m_storage[idx];}

        /**
         * @brief access a specific element of the string
         * 
         * @warning the index is not bounds-checked
         * 
         * @param idx the index to query the data from
         * @return `constexpr reference` a reference to the value at the index
         */
        inline constexpr reference operator[](size_type idx) noexcept
        {return m_storage[idx];}

        /**
         * @brief access a specific element of the string
         * 
         * @throws `std::out_of_range` if the index is out of range of the data
         * 
         * @param idx the index of the string to query
         * @return `constexpr value_type` the value at the index
         */
        inline constexpr value_type at(size_type idx) const {
            //only do safety check at runtime
            //safety check
            if (idx >= m_size) {
                if consteval {
                    //fallback for consteval
                    throw std::out_of_range("BasicConstexprString::at: idx >= m_size");
                } else {
                    //store the maximum buffer size
                    static constexpr std::size_t buffSize = 128;
                    //store the buffer
                    char buffer[buffSize];
                    //format
                    std::snprintf(buffer, buffSize, "BasicConstexprString::at: idx (which is %zu) >= m_size (which is %zu)", idx, m_size);
                    //throw using the buffer
                    throw std::out_of_range(buffer);
                }
            }
            //safe, so return
            return m_storage[idx];
        }

        /**
         * @brief get the first entry
         * 
         * @warning if the string is empty, this returns a null terminator
         * 
         * @return `constexpr value_type` the first element of the string
         */
        inline constexpr value_type front() const noexcept
        {return m_storage.front();}

        /**
         * @brief get the last entry
         * 
         * @warning if the string is empty, this returns a null terminator
         * 
         * @return `constexpr value_type` the last element of the string
         */
        inline constexpr value_type back() const noexcept
        {return m_storage[(m_size == 0) ? 0 : (m_size-1)];}

        /**
         * @brief get a constant iterator pointing to the beginning of the data
         * 
         * @return `constexpr const_iterator` a const iterator pointing to the beginning of the data
         */
        inline constexpr const_iterator begin() const noexcept
        {return m_storage.data();}

        /**
         * @brief get a constant iterator pointing to the end of the data
         * 
         * @return `constexpr const_iterator` a constant iterator pointing to the end of the data
         */
        inline constexpr const_iterator end() const noexcept
        {return m_storage.data() + m_size;}

        /**
         * @brief get a constant iterator pointing to the beginning of the data
         * 
         * @return `constexpr const_iterator` a const iterator pointing to the beginning of the data
         */
        inline constexpr const_iterator cbegin() const noexcept
        {return m_storage.data();}

        /**
         * @brief get a constant iterator pointing to the end of the data
         * 
         * @return `constexpr const_iterator` a constant iterator pointing to the end of the data
         */
        inline constexpr const_iterator cend() const noexcept
        {return m_storage.data() + m_size;}

        /**
         * @brief get a constant reversed iterator to the beginning of the reverse (the end)
         * 
         * @return `constexpr const_reverse_iterator` a constant reversed iterator to the beginning of the reverse (the end)
         */
        constexpr const_reverse_iterator rbegin() const noexcept
        {return const_reverse_iterator(end());}

        /**
         * @brief get a constant reversed iterator to the ending of the reverse (the beginning)
         * 
         * @return `constexpr const_reverse_iterator` a constant reversed iterator to the ending of the reverse (the beginning)
         */
        constexpr const_reverse_iterator rend() const noexcept
        {return const_reverse_iterator(begin());}

        /**
         * @brief get a constant reversed iterator to the beginning of the reverse (the end)
         * 
         * @return `constexpr const_reverse_iterator` a constant reversed iterator to the beginning of the reverse (the end)
         */
        constexpr const_reverse_iterator crbegin() const noexcept
        {return const_reverse_iterator(this->end());}

        /**
         * @brief get a constant reversed iterator to the ending of the reverse (the beginning)
         * 
         * @return `constexpr const_reverse_iterator` a constant reversed iterator to the ending of the reverse (the beginning)
         */
        constexpr const_reverse_iterator crend() const noexcept 
        {return const_reverse_iterator(this->begin());}

        /**
         * @brief remove data from the front of the constexpr string
         * 
         * @param n the number of elements to remove from the front
         */
        constexpr void remove_prefix(size_type n) noexcept {
            //clamp n
            n = (n > m_size) ? m_size : n;
            //update the size
            m_size -= n;
            //move the data to the front
            std::copy(m_storage.begin() + n, m_storage.begin() + m_size, m_storage.begin());
            //set the null terminator
            terminate();
        }

        /**
         * @brief remove data from the end of the constexpr string
         * 
         * @param n the number of elements to remove from the end
         */
        constexpr void remove_suffix(size_type n) noexcept {
            //update the size
            if (n > m_size) {m_size = 0;}
            else {m_size -= n;}
            //null-terminate
            terminate();
        }

        /**
         * @brief swap two constexpr strings
         * 
         * @warning if the sizes do not match, truncation may occur
         * 
         * @tparam s the capacity of the other string
         * @param other 
         */
        template <size_type Cap>
        constexpr void swap(BasicConstexprString<Cap, value_type, traits_type>& other) noexcept {
            //get the minimum of the sizes
            const size_type min_size = (m_size > other.m_size) ? other.m_size : m_size;

            //go over the storage and swap the elements
            for (size_type i = 0; i < min_size; ++i)
            {std::swap(m_storage[i], other.m_storage[i]);}

            //copy the rest of the larger storage
            if (m_size > other.m_size) {
                for (size_type i = min_size; i < m_size; ++i)
                {other.m_storage[i] = m_storage[i];}
            } else {
                for (size_type i = min_size; i < other.m_size; ++i)
                {m_storage[i] = other.m_storage[i];}
            }

            //swap the sizes
            std::swap(m_size, other.m_size);

            //terminate both
            terminate();
            other.terminate();
        }

        /**
         * @brief a function to copy `n` characters starting from `pos` into the string `str`
         * 
         * @param str the string to copy to
         * @param n the amount of characters to copy
         * @param pos the position of the internal buffer to start the copy at
         * @return `constexpr size_type` the amount of copied characters
         */
        constexpr size_type copy(pointer str, size_type n, size_type pos = 0) const {
            //sanity check pos
            if (pos > m_size) {throw std::out_of_range("BasicConstexprString::copy: pos out of range");}

            //get the length to copy
            const size_type rlen = std::min(n, m_size - pos);

            //copy
            for (size_type i = 0; i < rlen; ++i)
            {str[i] = m_storage[pos + i];}

            //return the len
            return rlen;
        }

        /**
         * @brief get a substring from the string
         * 
         * @param pos the position to start from
         * @param n the amount of elements to copy
         * @return `constexpr BasicConstexprString<CAPACITY, value_type, traits_type>` the substring
         */
        constexpr BasicConstexprString<CAPACITY, value_type, traits_type> substr(size_type pos = 0, size_type n = npos) const noexcept {
            //store the result (initially empty)
            BasicConstexprString<CAPACITY, value_type, traits_type> result;
            //bounds check, on overflow just return empty
            if (pos >= m_size) {return result;}

            //compute the length
            const size_type rlen = (n > m_size-pos)  ? (m_size-pos) : n;
            const size_type len  = (rlen > CAPACITY) ? CAPACITY : rlen;

            //copy data over
            for (size_type i = 0; i < len; ++i) 
            {result.m_storage[i] = m_storage[pos + i];}

            //set the result
            result.m_size = len;
            result.terminate();

            //return the result
            return result;
        }

        /**
         * @brief compare two strings
         * 
         * @tparam Cap the capacity of the string to compare with
         * @param str a constant reference to the string to compare with
         * @return `constexpr int` Negative value if this string is less than the other character sequence, zero if the both character sequences are equal, positive value if this string is greater than the other character sequence. 
         */
        template <std::size_t Cap>
        constexpr int compare(const BasicConstexprString<Cap, value_type, traits_type>& str) const noexcept 
        {return view().compare(str.view());}

        /**
         * @brief compare a substring with another string
         * 
         * @tparam Cap the capacity of the string to compare with
         * @param pos1 the position to start the comparison at
         * @param n1 the number of elements to compare
         * @param str a constant reference to the string to compare with
         * @return `constexpr int` Negative value if this string is less than the other character sequence, zero if the both character sequences are equal, positive value if this string is greater than the other character sequence. 
         */
        template <std::size_t Cap>
        constexpr int compare(size_type pos1, size_type n1, const BasicConstexprString<Cap, value_type, traits_type>& str) const 
        {return view().compare(pos1, n1, str.view());}

        /**
         * @brief compare a substring of this string with a substring of another string
         * 
         * @tparam Cap the capacity of the other string
         * @param pos1 the position to start the comparison at
         * @param n1 the number of elements to compare
         * @param str a constant reference to the string to compare with
         * @param pos2 the position to start the comparison at in the other string
         * @param n2 the number of elements to compare from the other string
         * @return `constexpr int` Negative value if this string is less than the other character sequence, zero if the both character sequences are equal, positive value if this string is greater than the other character sequence. 
         */
        template <std::size_t Cap>
        constexpr int compare(size_type pos1, size_type n1, const BasicConstexprString<Cap, value_type, traits_type>& str, size_type pos2, size_type n2) const 
        {return view().compare(pos1, n1, str.view(), pos2, n2);}

        /**
         * @brief compare this string with a C-String
         * 
         * @param str the C-String to compare with
         * @return `constexpr int` Negative value if this string is less than the other character sequence, zero if the both character sequences are equal, positive value if this string is greater than the other character sequence. 
         */
        constexpr int compare(const_pointer str) const noexcept 
        {return view().compare(str);}

        /**
         * @brief compare a substring from this string with a C-String
         * 
         * @param pos1 the position to start the comparison from
         * @param n1 the number of elements to compare
         * @param str the C-String to compare with
         * @return `constexpr int` Negative value if this string is less than the other character sequence, zero if the both character sequences are equal, positive value if this string is greater than the other character sequence. 
         */
        constexpr int compare(size_type pos1, size_type n1, const_pointer str) const
        {return view().compare(pos1, n1, str);}

        /**
         * @brief compare a substring from this string with a C-String with a size
         * 
         * @param pos1 the position to start the comparison from
         * @param n1 the number of elements to compare
         * @param str the C-String to compare with
         * @param n2 the number of elements in the C-String to compare with
         * @return `constexpr int` Negative value if this string is less than the other character sequence, zero if the both character sequences are equal, positive value if this string is greater than the other character sequence. 
         */
        constexpr int compare(size_type pos1, size_type n1, const_pointer str, size_type n2) const noexcept 
        {return view().compare(pos1, n1, str, n2);}

        /**
         * @brief Checks whether string starts with given prefix
         * 
         * @tparam Cap the capacity of the string to check for
         * @param x the string to check for
         */
        template <std::size_t Cap>
        constexpr bool starts_with(const BasicConstexprString<Cap, value_type, traits_type>& x) const noexcept
        {return view().starts_with(x.view());}

        /**
         * @brief Checks whether string starts with given prefix
         * 
         * @param x the starting element to check for
         */
        constexpr bool starts_with(value_type x) const noexcept 
        {return view().starts_with(x);}

        /**
         * @brief Checks whether string starts with given prefix
         * 
         * @param x the C-String to check for
         */
        constexpr bool starts_with(const_pointer x) const noexcept 
        {return view().starts_with(x);}

        /**
         * @brief Checks whether string ends with given suffix
         * 
         * @tparam Cap the capacity of the string to compare with
         * @param x the string to check for
         */
        template <std::size_t Cap>
        constexpr bool ends_with(const BasicConstexprString<Cap, value_type, traits_type>& x) const noexcept 
        {return view().ends_with(x.view());}

        /**
         * @brief Checks whether string ends with given suffix
         * 
         * @param x the starting element to check for
         */
        constexpr bool ends_with(value_type x) const noexcept 
        {return view().ends_with(x);}

        /**
         * @brief Checks whether string ends with given suffix
         * 
         * @param x the C-String to check for
         */
        constexpr bool ends_with(const_pointer x) const noexcept 
        {return view().ends_with(x);}

        /**
         * @brief Checks whether string contains substring or character
         * 
         * @tparam Cap the capacity of the substring to check for
         * @param x the substring to check for
         */
        template <std::size_t Cap>
        constexpr bool contains(const BasicConstexprString<Cap, value_type, traits_type>& x) const noexcept 
        {return view().contains(x);}

        /**
         * @brief Checks whether string contains substring or character
         * 
         * @param x the string element to check for
         */
        constexpr bool contains(value_type x) const noexcept 
        {return view().contains(x);}

        /**
         * @brief Checks whether string contains substring or character
         * 
         * @param x the C-String to check for
         */
        constexpr bool contains(const_pointer x) const noexcept 
        {return view().contains(x);}

        /**
         * @brief Finds first occurrence of a substring/character
         * 
         * This starts searching from the front
         * 
         * @tparam Cap the capacity of the substring to find
         * @param str the string to find
         * @param pos the position to start the search at
         */
        template <std::size_t Cap>
        constexpr size_type find(const BasicConstexprString<Cap, value_type, traits_type>& str, size_type pos = 0) const noexcept 
        {return view().find(str.view(), pos);}

        /**
         * @brief Finds first occurrence of a substring/character
         * 
         * This starts searching from the front
         * 
         * @param c the character to find
         * @param pos the position to start the search at
         */
        constexpr size_type find(value_type c, size_type pos = 0) const noexcept 
        {return view().find(c, pos);}

        /**
         * @brief Finds first occurrence of a substring/character
         * 
         * This starts searching from the front
         * 
         * @param str the character sequence to search for
         * @param pos the position to start the search at
         * @param n the length of the substring to search for
         */
        constexpr size_type find(const_pointer str, size_type pos, size_type n) const noexcept 
        {return view().find(str, pos, n);}

        /**
         * @brief Finds first occurrence of a substring/character
         * 
         * This starts searching from the front
         * 
         * @param str the C-String to search for
         * @param pos the position to start the search at
         */
        constexpr size_type find(const_pointer str, size_type pos = 0) const noexcept 
        {return view().find(str, pos);}

        /**
         * @brief Finds first occurrence of a substring/character
         * 
         * This starts searching from the end
         * 
         * @tparam Cap the capacity of the substring to find
         * @param str the string to find
         * @param pos the position to start the search at
         */
        template <std::size_t Cap>
        constexpr size_type rfind(const BasicConstexprString<Cap, value_type, traits_type>& str, size_type pos = npos) const noexcept 
        {return view().rfind(str.view(), pos);}

        /**
         * @brief Finds first occurrence of a substring/character
         * 
         * This starts searching from the end
         * 
         * @param c the character to find
         * @param pos the position to start the search at
         */
        constexpr size_type rfind(value_type c, size_type pos = npos) const noexcept 
        {return view().rfind(c, pos);}

        /**
         * @brief Finds first occurrence of a substring/character
         * 
         * This starts searching from the end
         * 
         * @param str the character sequence to search for
         * @param pos the position to start the search at
         * @param n the length of the substring to search for
         */
        constexpr size_type rfind(const_pointer str, size_type pos, size_type n) const noexcept 
        {return view().rfind(str, pos, n);}

        /**
         * @brief Finds first occurrence of a substring/character
         * 
         * This starts searching from the end
         * 
         * @param str the C-String to search for
         * @param pos the position to start the search at
         */
        constexpr size_type rfind(const_pointer str, size_type pos = npos) const noexcept 
        {return view().rfind(str, pos);}

        /**
         * @brief Finds first occurrence of a character from a character set
         * 
         * This starts searching from the front
         * 
         * @tparam Cap the capacity of the substring to find
         * @param str the string containing an element set to search for
         * @param pos the position to start the search at
         */
        template <std::size_t Cap>
        constexpr size_type find_first_of(const BasicConstexprString<Cap, value_type, traits_type>& str, size_type pos = 0) const noexcept 
        {return view().find_first_of(str.view(), pos);}

        /**
         * @brief Finds first occurrence of a character from a character set
         * 
         * This starts searching from the front
         * 
         * @param c the character to find
         * @param pos the position to start the search at
         */
        constexpr size_type find_first_of(value_type c, size_type pos = 0) const noexcept 
        {return view().find_first_of(c, pos);}

        /**
         * @brief Finds first occurrence of a character from a character set
         * 
         * This starts searching from the front
         * 
         * @param str the character sequence that contains the elements to search for
         * @param pos the position to start the search at
         * @param n the length of the character sequence
         */
        constexpr size_type find_first_of(const_pointer str, size_type pos, size_type n) const noexcept 
        {return view().find_first_of(str, pos, n);}

        /**
         * @brief Finds first occurrence of a character from a character set
         * 
         * This starts searching from the front
         * 
         * @param str the C-String containing the set to find one of
         * @param pos the position to start the search at
         */
        constexpr size_type find_first_of(const_pointer str, size_type pos = 0) const noexcept 
        {return view().find_first_of(str, pos);}

        /**
         * @brief Finds first occurrence of a character from a character set
         * 
         * This starts searching from the back
         * 
         * @tparam Cap the capacity of the substring to find
         * @param str the string containing an element set to search for
         * @param pos the position to start the search at
         */
        template <std::size_t Cap>
        constexpr size_type find_last_of(const BasicConstexprString<Cap, value_type, traits_type>& str, size_type pos = npos) const noexcept 
        {return view().find_last_of(str.view(), pos);}

        /**
         * @brief Finds first occurrence of a character from a character set
         * 
         * This starts searching from the back
         * 
         * @param c the character to find
         * @param pos the position to start the search at
         */
        constexpr size_type find_last_of(value_type c, size_type pos = npos) const noexcept 
        {return view().find_last_of(c, pos);}

        /**
         * @brief Finds first occurrence of a character from a character set
         * 
         * This starts searching from the back
         * 
         * @param str the character sequence that contains the elements to search for
         * @param pos the position to start the search at
         * @param n the length of the character sequence
         */
        constexpr size_type find_last_of(const_pointer str, size_type pos, size_type n) const noexcept 
        {return view().find_last_of(str, pos, n);}

        /**
         * @brief Finds first occurrence of a character from a character set
         * 
         * This starts searching from the back
         * 
         * @param str the C-String containing the set to find one of
         * @param pos the position to start the search at
         */
        constexpr size_type find_last_of(const_pointer str, size_type pos = npos) const noexcept 
        {return view().find_last_of(str, pos);}

        /**
         * @brief Finds first occurrence of a character NOT from a character set
         * 
         * This starts searching from the front
         * 
         * @tparam Cap the capacity of the substring to find
         * @param str the string containing an element set to ignore
         * @param pos the position to start the search at
         */
        template <std::size_t Cap>
        constexpr size_type find_first_not_of(const BasicConstexprString<Cap, value_type, traits_type>& str, size_type pos = 0) const noexcept 
        {return view().find_first_not_of(str.view(), pos);}

        /**
         * @brief Finds first occurrence of a character NOT from a character set
         * 
         * This starts searching from the front
         * 
         * @param c the character to ignore
         * @param pos the position to start the search at
         */
        constexpr size_type find_first_not_of(value_type c, size_type pos = 0) const noexcept 
        {return view().find_first_not_of(c, pos);}

        /**
         * @brief Finds first occurrence of a character NOT from a character set
         * 
         * This starts searching from the front
         * 
         * @param str the character sequence that contains the elements to ignore
         * @param pos the position to start the search at
         * @param n the length of the character sequence
         */
        constexpr size_type find_first_not_of(const_pointer str, size_type pos, size_type n) const noexcept 
        {return view().find_first_not_of(str, pos, n);}

        /**
         * @brief Finds first occurrence of a character NOT from a character set
         * 
         * This starts searching from the front
         * 
         * @param str the C-String containing the set to ignore
         * @param pos the position to start the search at
         */
        constexpr size_type find_first_not_of(const_pointer str, size_type pos = 0) const noexcept 
        {return view().find_first_not_of(str, pos);}

        /**
         * @brief Finds first occurrence of a character NOT from a character set
         * 
         * This starts searching from the back
         * 
         * @tparam Cap the capacity of the substring to find
         * @param str the string containing an element set to ignore
         * @param pos the position to start the search at
         */
        template <std::size_t Cap>
        constexpr size_type find_last_not_of(const BasicConstexprString<Cap, value_type, traits_type>& str, size_type pos = npos) const noexcept 
        {return view().find_last_not_of(str.view(), pos);}

        /**
         * @brief Finds first occurrence of a character NOT from a character set
         * 
         * This starts searching from the back
         * 
         * @param c the character to ignore
         * @param pos the position to start the search at
         */
        constexpr size_type find_last_not_of(value_type c, size_type pos = npos) const noexcept 
        {return view().find_last_not_of(c, pos);}

        /**
         * @brief Finds first occurrence of a character NOT from a character set
         * 
         * This starts searching from the back
         * 
         * @param str the character sequence that contains the elements to ignore
         * @param pos the position to start the search at
         * @param n the length of the character sequence
         */
        constexpr size_type find_last_not_of(const_pointer str, size_type pos, size_type n) const noexcept 
        {return view().find_last_not_of(str, pos, n);}

        /**
         * @brief Finds first occurrence of a character NOT from a character set
         * 
         * This starts searching from the back
         * 
         * @param str the C-String containing the set to ignore
         * @param pos the position to start the search at
         */
        constexpr size_type find_last_not_of(const_pointer str, size_type pos = npos) const noexcept 
        {return view().find_last_not_of(str, pos);}

        /**
         * @brief print the constexpr string to an output stream
         * 
         * @param os the output stream to print to
         * @param str the compile time string to print
         * @return `std::ostream&` a reference to the output stream after printing
         */
        friend std::ostream& operator<<(std::ostream& os, const BasicConstexprString<CAPACITY, value_type, traits_type>& str)
        {return (os << str.view());}

    protected:

        //make them all friends
        template<std::size_t, typename, typename>
        friend class BasicConstexprString;

        /**
         * @brief a helper function to terminate the string
         */
        constexpr void terminate() noexcept {
            //clamp the size
            if (m_size > CAPACITY) {m_size = CAPACITY;}
            //null terminate
            m_storage[m_size] = '\0';
        }

        /**
         * @brief store the data
         */
        std::array<value_type, CAPACITY+1> m_storage{};

        /**
         * @brief store the size of the stored string
         * 
         * This does NOT include the null termination character
         */
        size_type m_size = 0;

    };

}

#endif
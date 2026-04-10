/**
 * @file TypeInfo.h
 * @author DM8AT
 * @brief this file is contains functions to get information about types at compile time. 
 *        Everything in this file is consteval so it is evaluated fully at compile time. 
 * @version 0.1
 * @date 2025-12-14
 * 
 * @copyright Copyright (c) 2025
 * 
 */
//header guard
#ifndef _GLGE_CORE_TYPE_NAME_
#define _GLGE_CORE_TYPE_NAME_

//add common stuff
#include "Common.h"

/**
 * @brief use the libraries namespace
 */
namespace GLGE {

    /**
     * @brief Helper: constexpr find substring (returns npos if not found)
     * 
     * @param s the string view to search
     * @param needle the needle to find
     * @param start the starting position to search at
     * @return `constexpr size_t` the position of the found element or std::string_view::npos if the element was not found
     */
    inline consteval size_t __cexpr_find(std::string_view s, std::string_view needle, std::size_t start = 0) noexcept {
        //sanity check if the needle would even fit into the string
        if (needle.empty() || s.size() < needle.size()) return std::string_view::npos;
        //iterate over all elements the element could possibly be
        for (size_t i = start; i + needle.size() <= s.size(); ++i) {
            //store if the string is ok so far (start by assuming it is found)
            bool ok = true;
            //check if the strings match
            for (size_t j = 0; j < needle.size(); ++j) {
                if (s[i + j] != needle[j]) { ok = false; break; }
            }
            //if the string is fully ok, return it
            if (ok) 
            {return i;}
        }
        //if the element was not found, return npos
        return std::string_view::npos;
    }

    /**
     * @brief Helper: constexpr find one of characters
     * 
     * @param s the string to search in
     * @param chars a collection of string elements to find ONE from
     * @param start the element to start the search at
     * @return `constexpr size_t` the position of the found element or std::string_view::npos if it was not found
     */
    inline consteval std::size_t __cexpr_find_one_of(std::string_view s, const char* chars, std::size_t start = 0) noexcept {
        //iterate over all elements to find it
        for (std::size_t i = start; i < s.size(); ++i) {
            //check for the symbols
            for (const char* p = chars; *p; ++p) {
                //if it is correct, return the position
                if (s[i] == *p) return i;
            }
        }
        //if the element is not found, return npos as error symbol
        return std::string_view::npos;
    }

    /**
     * @brief Helper: constexpr find last of characters
     * 
     * @param s the string to search in
     * @param chars a collection of string elements to find ONE from
     * @param start the element to start the search at
     * @return `consteval size_t` the position of the found element or std::string_view::npos
     */
    inline consteval size_t __cexpr_find_last_of(std::string_view s, const char* chars, std::size_t start = std::string_view::npos) noexcept {
        //sanity check if the string is filled
        if (s.empty()) return std::string_view::npos;

        //start from end if no start position given and the loop till the loop returns or till the start is hit
        size_t i = (start == std::string_view::npos ? s.size() : start);
        while (i-- > 0)
        {
            for (const char* p = chars; *p; ++p) {
                if (s[i] == *p)
                    return i;
            }
        }

        //not found : return npos
        return std::string_view::npos;
    }

    /**
     * @brief get the raw name of a templated function
     * 
     * @tparam T the type to get the function for
     * @return `constexpr std::string_view` the name of the templated function
     */
    template <typename T>
    inline consteval std::string_view __raw_func_name() noexcept {
        //return the correct string depending on the compiler
        //throw an error if the compiler is not supported
        #if defined(_MSC_VER)
            return __FUNCSIG__;
        #elif defined(__clang__) || defined(__GNUC__)
            return __PRETTY_FUNCTION__;
        #else
            //error - unsupported compiler
            #error "Unsupported compiler. Please inform the maintainer of GLGE to add the required compiler"
        #endif
    }

    /**
     * @brief get the name of a type
     * 
     * @tparam T the type to get the name from
     * @return `consteval std::string_view` the name of the type
     */
    template<typename T>
    inline consteval std::string_view getTypeName() noexcept {
        //use compile time caching//use compile time caching
        static constexpr std::string_view result = [] {
            //first, get the raw name of the function with the template name
            constexpr std::string_view pf = __raw_func_name<T>();

            //check for different compiler semantics that are commonly used

            #if defined(__clang__) || defined(__GNUC__)
                //1) GCC/Clang usually include "T = <typename>" in the pretty function
                //so search for something that resembles this structure and extract the type name that is stored in between
                constexpr std::string_view marker1 = "T = ";
                if (const std::size_t pos = __cexpr_find(pf, marker1); pos != std::string_view::npos) {
                    const std::size_t start = pos + marker1.size();
                    //usually ends with ']' or ';' depending on compiler context, check common terminators
                    //make sure not to check for > or , as they may appear in templated type names
                    constexpr const char terminators[] = "];";
                    const std::size_t end = __cexpr_find_one_of(pf, terminators, start);
                    //sanity check
                    if (end != std::string_view::npos && end > start)
                        return pf.substr(start, end - start);
                    //else return to end
                    return pf.substr(start);
                }
            #elif defined(_MSC_VER)
                //2) MSVC layout is "... func_name<type>(...)" so extract the type between '<' and the matching '>'
                //first, find the last < symbol before the initializer list (known to be void)
                const std::size_t paren = __cexpr_find(pf, "(void)");
                const std::size_t lt = __cexpr_find_last_of(pf.substr(0, paren), "<");
                //then extract the string from that symbol to the corresponding > symbol
                if (lt != std::string_view::npos) {
                    //store the current nesting depth
                    std::size_t depth = 0;
                    //store the position of the matching '>'
                    std::size_t gt = std::string_view::npos;
                    //iterate until the correct closing bracket is found, tracking nested '<'
                    for (std::size_t i = lt + 1; i < pf.size(); ++i) {
                        const char c = pf[i];
                        if (c == '<') ++depth;
                        else if (c == '>') {
                            if (depth == 0) { gt = i; break; }
                            else --depth;
                        }
                    }
                    //sanity check before returning the name
                    if (gt != std::string_view::npos && gt > lt + 1)
                        return pf.substr(lt + 1, gt - (lt + 1));
                }
            #else
                //3) Unsupported compiler -> error
                #error "The compiler you are compiling on is not supported. Please contact the GLGE maintainers."
            #endif

            // final fallback: return the whole pretty/function string if parsing failed
            return pf;
        }();
        return result;
    }

    /**
     * @brief compute the 64 bit hash for a specific string
     * 
     * This function uses the FNV-1A hash on any inputted string and runs fully at compile time. 
     * 
     * @return `constexpr u64` the hash of the string
     */
    inline constexpr u64 __FNV_1A_64_HASH(const std::string_view& string) noexcept {
        //run the FNV-1a 64-bit hash
        u64 hash = 1469598103934665603ull;
        for (unsigned char c : string)
            hash = (hash ^ c) * 1099511628211ull;
        return hash;
    }

    /**
     * @brief compute the 64 bit hash for a specific type
     * 
     * This function uses the templated function name to use the type name indirectly to compute unique hashes for all types
     * 
     * @tparam T the type to compute the hash for
     * @return `consteval u64` the hash of the type
     */
    template <typename T>
    inline consteval u64 getTypeHash64() noexcept {
        //get the type name
        constexpr std::string_view name = getTypeName<T>();
        //return the hashed name
        return __FNV_1A_64_HASH(name);
    }

    /**
     * @brief fold a 64 bit hash down to a 32 bit value
     * 
     * @param hash the hash to fold down
     * @return `constexpr u32` the 32 bit result
     */
    inline constexpr u32 __hash_fold64to32(uint64_t hash) noexcept {
        //fold down to reduce the likelihood of a collision
        hash ^= hash >> 33;
        hash *= 0xff51afd7ed558ccdULL;
        hash ^= hash >> 33;
        hash *= 0xc4ceb9fe1a85ec53ULL;
        hash ^= hash >> 33;
        return static_cast<u32>(hash & std::numeric_limits<u32>::max());
    }

    /**
     * @brief compute the 32 bit hash for a specific type
     * 
     * This function uses the templated function name to use the type name indirectly to compute unique hashes for all types
     * 
     * @tparam T the type to compute the hash for
     * @return `consteval u32` the 32 bit hash of the type
     */
    template <typename T>
    inline consteval u32 getTypeHash32() noexcept {
        //get the type name
        constexpr std::string_view name = getTypeName<T>();
        //FNV-1a 64-bit hash
        u64 hash = 1469598103934665603ull;
        for (unsigned char c : name)
            hash = (hash ^ c) * 1099511628211ull;
        return __hash_fold64to32(hash);
    }

}

//sanity check the hash function
static_assert(GLGE::getTypeHash64<int>() != GLGE::getTypeHash64<float>(), "Invalid hashing. Please check your compiler settings.");
static_assert(GLGE::getTypeHash32<int>() != GLGE::getTypeHash64<float>(), "Invalid hashing that results in problems during folding. Please check your compiler settings.");
//check that the handle is correctly cross-platform
static_assert(GLGE::getTypeHash32<int>() == 4043213090, "Invalid type ID registered for type \"int\"");
static_assert(GLGE::getTypeHash32<float>() == 572149578, "Invalid type ID registered for type \"float\"");

#endif
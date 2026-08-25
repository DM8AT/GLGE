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
//add constexpr strings
#include "ConstexprString.h"

/**
 * @brief use the libraries namespace
 */
namespace GLGE {

    /**
     * @brief get the raw name of a templated function
     * 
     * @tparam T the type to get the function for
     * @return `constexpr std::string_view` the name of the templated function
     */
    template <typename T>
    inline consteval std::string_view __glge__raw_func_name() noexcept {
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
     * @tparam T the type to get the name for
     * @return `consteval auto` the name of the type
     */
    template <typename T>
    inline consteval auto getTypeName() noexcept {
        //first, get the raw name of the function with the template name
        constexpr std::string_view pf = __glge__raw_func_name<T>();

        //convert to an owning string
        BasicConstexprString<pf.size()> pfStr(pf);

        //remove all spaces
        size_t removedSpaceCount = 0;
        for (size_t i = 0; i < pfStr.size(); ++i) {
            if (pfStr[i] == ' ') 
            {++removedSpaceCount;}
            else 
            {pfStr[i - removedSpaceCount] = pfStr[i];}
        }
        //convert to a substring of one self
        pfStr = pfStr.substr(0, pfStr.size() - removedSpaceCount);

        #if defined(__clang__) || defined(__GNUC__)
            //1) GCC/Clang usually include "T = <typename>" in the pretty function
            //so search for something that resembles this structure and extract the type name that is stored in between
            constexpr std::string_view marker1 = "T=";
            if (const std::size_t pos = pfStr.find(marker1.data()); pos != pfStr.npos) {
                const std::size_t start = pos + marker1.size();
                //usually ends with ']' or ';' depending on compiler context, check common terminators
                //make sure not to check for > or , as they may appear in templated type names
                constexpr const char terminators[] = "];";
                const std::size_t end = pfStr.find_first_of(terminators, start);
                //sanity check
                if (end != std::string_view::npos && end > start) {
                    return pfStr.substr(start, end - start);
                } else {
                    //else return to end
                    return pfStr.substr(start);
                }
            }
        #elif defined(_MSC_VER)
            //2) MSVC layout is "... func_name<type>(...)" so extract the type between '<' and the matching '>'
            //first, find the last < symbol before the initializer list (known to be void)
            const std::size_t paren = pfStr.find("(void)");
            const std::size_t lt = pfStr.substr(0, paren).find_last_of("<");
            //then extract the string from that symbol to the corresponding > symbol
            if (lt != pfStr.npos) {
                //store the current nesting depth
                std::size_t depth = 0;
                //store the position of the matching '>'
                std::size_t gt = pfStr.npos;
                //iterate until the correct closing bracket is found, tracking nested '<'
                for (std::size_t i = lt + 1; i < pfStr.size(); ++i) {
                    const char c = pfStr[i];
                    if (c == '<') 
                    {++depth;}
                    else if (c == '>') {
                        if (depth == 0) {gt = i; break;}
                        else --depth;
                    }
                }
                //sanity check before returning the name
                if (gt != std::string_view::npos && gt > lt + 1)
                {return pfStr.substr(lt + 1, gt - (lt + 1));}
            }
        #else
            //Unsupported compiler -> error
            #error "The compiler you are compiling on is not supported. Please contact the GLGE maintainers."
        #endif

        //something went terribly wrong
        return BasicConstexprString<pf.size()>("This should not happen");
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
        {hash = (hash ^ c) * 1099511628211ull;}
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
        constexpr auto name = getTypeName<T>();
        //return the hashed name
        return __FNV_1A_64_HASH(name.view());
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
        constexpr auto name = getTypeName<T>();
        //FNV-1a 64-bit hash
        u64 hash = 1469598103934665603ull;
        for (unsigned char c : name)
            hash = (hash ^ c) * 1099511628211ull;
        return __hash_fold64to32(hash);
    }

    /**
     * @brief store the tag for a type
     */
    struct TypeTag {
        /**
         * @brief store the hash of the type
         */
        const u64 hash = 0;
        /**
         * @brief the full name length
         */
        const u16 fullNameLen = 0;

        /**
         * @brief create a new type tag
         * 
         * @warning for large N values the name may be truncated
         * 
         * @param _name the name of the type to tag
         */
        template <std::size_t N>
        constexpr TypeTag(const BasicConstexprString<N>& _name)
         : hash(__FNV_1A_64_HASH(_name.view())), fullNameLen(static_cast<u16>(_name.size()))
        {}

        /**
         * @brief check if two type tags are identical
         * 
         * @warning This is technically not fully identical. It is possible for two unrelated types to collide, but this is considered to be very, very, very unlikely. 
         * 
         * @details
         * Equality is determined by the hash of the complete type name and the
         * length of the complete type name. This is not a mathematically perfect
         * identity check, but collisions are considered sufficiently improbable
         * for runtime type identification purposes.
         * 
         * @param other the type tag to compare against
         * @return `true` if the type tags are identical, `false` if not
         */
        inline constexpr bool operator==(const TypeTag& other) const noexcept {
            //check the hashes and name sizes
            bool res = (hash == other.hash) && (fullNameLen == other.fullNameLen);
            //return the result
            return res;
        }
    };

    /**
     * @brief Get the type tag of a specific type
     * 
     * @tparam T the type to get the tag for
     * @return `TypeTag` the type tag of the type
     */
    template <typename T>
    inline consteval TypeTag getTypeTag() noexcept {
        //get the name
        static constexpr auto name = getTypeName<T>();
        //create the tag
        static constexpr TypeTag tag = TypeTag(name);
        //return the tag
        return tag;
    };

}

/**
 * @brief specialize the hash for type tags
 * 
 * @tparam specialized to GLGE::TypeTag
 */
template <>
struct std::hash<GLGE::TypeTag> {
    /**
     * @brief hash a type tag
     * 
     * @param tag the type tag to hash
     * @return `std::size_t` the hash of the type
     */
    std::size_t operator()(const GLGE::TypeTag& tag) const noexcept {
        //the hash is cached, just re-use it (that's fast)
        return tag.hash;
    }
};

//sanity check name getting
static_assert(GLGE::getTypeName<int>().view() == std::string_view("int"), "Invalid name query. Make sure you are using one of the following compilers: MSVC, GCC or Clang");
static_assert(GLGE::getTypeName<float>().view() == std::string_view("float"), "Invalid name query. Make sure you are using one of the following compilers: MSVC, GCC or Clang");
static_assert(GLGE::getTypeName<std::vector<int>>().view() == std::string_view("std::vector<int>"), "Invalid name query. Make sure you are using one of the following compilers: MSVC, GCC or Clang");
static_assert(GLGE::getTypeName<std::vector<std::vector<std::vector<float>>>>().view() == std::string_view("std::vector<std::vector<std::vector<float>>>"), "Invalid name query. Make sure you are using one of the following compilers: MSVC, GCC or Clang");
static_assert(GLGE::getTypeName<GLGE::TypeTag>().view() == std::string_view("TypeTag"), "Invalid name query. Make sure you are using one of the following compilers: MSVC, GCC or Clang");
//sanity check the hash function
static_assert(GLGE::getTypeHash64<int>() != GLGE::getTypeHash64<float>(), "Invalid hashing. Please check your compiler settings.");
static_assert(GLGE::getTypeHash32<int>() != GLGE::getTypeHash64<float>(), "Invalid hashing that results in problems during folding. Please check your compiler settings.");
//check that the handle is correctly cross-platform
static_assert(GLGE::getTypeHash32<int>() == 4043213090, "Invalid type ID registered for type \"int\"");
static_assert(GLGE::getTypeHash32<float>() == 572149578, "Invalid type ID registered for type \"float\"");

#endif
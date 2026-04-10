/**
 * @file TinyECS.h
 * @author DM8AT
 * @brief a fast, low-overhead single file small entity component system for C++ 23+
 * @version 0.1
 * @date 2026-02-15
 * 
 * @copyright Copyright (c) 2026
 * 
 */
//header guard
#ifndef _DM8AT_TINY_ECS_
#define _DM8AT_TINY_ECS_

//for dynamic sized lists
#include <vector>
//for hashmaps
#include <unordered_map>
//for integers
#include <cstdint>
//for type info
#include <type_traits>
//for easy manipulatable bitsets
#include <bitset>
//for memory operations
#include <cstring>
//for thread-safe read/write
#include <atomic>
//for simple in-between thread communication
#include <condition_variable>
//for blocking
#include <mutex>
//for thread stuff
#include <thread>

//GLGE namespace wrapper (added because this is part of GLGE)
namespace GLGE {

//check for debug mode
#if defined(DEBUG) || defined(_DEBUG) || defined(__DEBUG) || defined(ECS_DEBUG) || defined(_ECS_DEBUG) || defined(__ECS_DEBUG) || defined(_TINY_ECS_DEBUG) || defined(__TINY_ECS_DEBUG)
#define TINY_ECS_DEBUG 1
#endif

//check if TinyJobs is allready included
#ifndef _DM8AT_TINY_JOBS_
    //try local include dir
    #if __has_include("TinyJobs.h")
        #include "TinyJobs.h"
        /**
         * @brief added the multithreading features
         */
        #define TINY_ECS_FEATURE_MULTITHREADED 1
    #else //else, try the global include dir
        //try to add the job system (unlocks some features)
        #if __has_include(<TinyJobs.h>)
            #include <TinyJobs.h>
            /**
             * @brief added the multithreading features
             */
            #define TINY_ECS_FEATURE_MULTITHREADED 1
        #else
            /**
             * @brief disable multithreading features
             */
            #define TINY_ECS_FEATURE_MULTITHREADED 0
        #endif
    #endif
#else
    /**
     * @brief added the multithreading features
     */
    #define TINY_ECS_FEATURE_MULTITHREADED 1
#endif

/**
 * @brief use the main namespace of all tiny libraries
 */
namespace Tiny {

/**
 * @brief store everything related to the ECS
 */
namespace ECS {

/**
 * @brief a namespace that holds all utility structures / functions to not pollute the `ECS` namespace
 */
namespace util {

/**
 * @brief a structure to check if a type is an entity type
 * 
 * default : not an entity
 * 
 * @tparam T the type to check
 */
template <typename T>
struct is_entity_type : public std::false_type
{};

/**
 * @brief a simple function to check if a value is a power of 2
 * 
 * @param x the integer to check
 * @return `true` if it is a power of 2, `false` otherwise
 */
constexpr bool is_power_of_2(size_t x) {
    return x != 0 && (x & (x - 1)) == 0;
}

/**
 * @brief a function to compute the log2 of a value
 * 
 * This function runs fully at compile-time
 * 
 * @tparam x the value to compute the logarithm for
 * @return `consteval size_t` the logarithm of the value
 */
template <size_t x>
consteval std::size_t log2_size_t() {
    //sanity check for 0
    static_assert(x != 0, "log2 of 0 is undefined");
    //store the result
    size_t result = 0;
    //a copy of x that can be modified
    size_t v = x;
    //repeatedly shift down and check if 0 is reached
    while (v > 1) {
        v >>= 1;
        ++result;
    }
    //return the final logarithm
    return result;
}

/**
 * @brief a fully compile time string
 * 
 * @tparam N the length of the string in characters
 */
template<uint64_t N>
struct fixed_string {
    /**
     * @brief store all the elements
     */
    char data[N+1]{0};
    /**
     * @brief store the size 
     */
    static constexpr uint64_t size = N;
};

/**
 * @brief get the name of a type at compile time
 * 
 * @tparam T the type of the element to get the name for
 * @return `consteval fixed_string` a compile-time string containing the type name
 */
template<typename T>
consteval auto raw_type_name() {
    //get the name according to the compiler
#if defined(__clang__)
    //use clang and __PRETTY_FUNCTION__
    constexpr char const* sig = __PRETTY_FUNCTION__;
    constexpr uint64_t prefix = sizeof("consteval auto raw_type_name() [T = ") - 1;
    constexpr uint64_t suffix = 1; // ']'
#elif defined(__GNUC__)
    //use gcc and also __PRETTY_FUNCTION__
    constexpr char const* sig = __PRETTY_FUNCTION__;
    constexpr uint64_t prefix = sizeof("consteval auto raw_type_name() [with T = ") - 1;
    constexpr uint64_t suffix = 1;
#elif defined(_MSC_VER)
    //use MSC and __FUNCSIG__
    constexpr char const* sig = __FUNCSIG__;
    constexpr uint64_t prefix = sizeof("consteval auto __cdecl raw_type_name<") - 1;
    constexpr uint64_t suffix = sizeof(">(void)") - 1;
#else
    //this compiler is not supported
    #error "Unsupported compiler. Please use GCC, Clang or Microsoft Visual Compiler"
#endif

    //compute the size of the type substring
    constexpr uint64_t total = __builtin_strlen(sig);
    constexpr uint64_t size  = total - prefix - suffix;

    //copy the data into the compile time string
    fixed_string<size> out{};
    for (uint64_t i = 0; i < size; ++i)
    {out.data[i] = sig[prefix + i];}

    //return the string
    return out;
}

/**
 * @brief hash a compile time string
 * 
 * @param s the string to hash
 * @return `consteval uint64_t` the computed hash
 */
consteval uint64_t hash_string(auto const& s) {
    //starting seed
    uint64_t h = 1469598103934665603ull;
    //FNV-1a hash combination
    for (uint64_t i = 0; i < s.size; ++i)
    {h = (h ^ static_cast<uint64_t>(s.data[i])) * 1099511628211ull;}
    //return the final hash
    return h;
}

/**
 * @brief a wrapper function to hash a type
 * 
 * @tparam T the type to hash
 * @return `consteval uint64_t` the computed type hash
 */
template<typename T>
consteval uint64_t type_hash() {
    //get the type name and return the hash
    return hash_string(raw_type_name<T>());
}

/**
 * @brief always false
 * 
 * @tparam T a dummy type
 */
template <typename T>
constexpr bool always_false = false;

/**
 * @brief a list of types
 * 
 * @tparam Ts the types the list contains
 */
template<typename... Ts>
struct type_list {
    /**
     * @brief add a type to the front of the list
     * 
     * @tparam X the element to add to the front
     */
    template<typename X>
    using prepend = type_list<X, Ts...>;
};

/**
 * @brief a structure to get the index of a specific type in a type list
 * 
 * @tparam T the type to quarry the index for
 * @tparam List the list to look in
 */
template<typename T, typename List>
struct index_of;

/**
 * @brief found case
 * 
 * @tparam T the type to check for
 * @tparam Ts the types to look in
 */
template<typename T, typename... Ts>
struct index_of<T, type_list<T, Ts...>>
 : public std::integral_constant<std::size_t, 0> 
{};

/**
 * @brief not found case
 * 
 * @tparam T the type to check for
 * @tparam X the first element in the type list
 * @tparam Ts the tail of the type list
 */
template<typename T, typename X, typename... Ts>
struct index_of<T, type_list<X, Ts...>>
 : std::integral_constant<
       std::size_t,
       1 + index_of<T, type_list<Ts...>>::value
   > 
{};

/**
 * @brief a template to insert a type element into a list
 * 
 * @tparam T the type to insert
 * @tparam List the type list to insert to
 */
template<typename T, typename List>
struct insert;

/**
 * @brief if the type list is empty, make this the initial item
 * 
 * @tparam T the type to make the list from
 */
template<typename T>
struct insert<T, type_list<>> {
    /**
     * @brief the new type list
     */
    using type = type_list<T>;
};

/**
 * @brief insert a type in a type list based on its hash
 * 
 * @tparam T the type to insert
 * @tparam Head the head of the type list
 * @tparam Tail the tail of the type list
 */
template<typename T, typename Head, typename... Tail>
struct insert<T, type_list<Head, Tail...>> {
private:
    /**
     * @brief true if the element should move left in the list
     */
    static constexpr bool go_left =
        (type_hash<T>() < type_hash<Head>());

public:
    /**
     * @brief conditional assignment on two possible lists based on if the element should insert left in the list
     */
    using type = std::conditional_t<
        go_left,
        type_list<T, Head, Tail...>,
        typename insert<T, type_list<Tail...>>::type::template prepend<Head>
    >;
};

/**
 * @brief a helper to sort a type list structure
 * 
 * @tparam List the type list to sort
 */
template<typename List>
struct sort_impl;

/**
 * @brief the implementation for if the type list is empty
 * 
 * @tparam None
 */
template<>
struct sort_impl<type_list<>> {
    /**
     * @brief just store an empty type list
     */
    using type = type_list<>;
};

/**
 * @brief the implementation if the type list is not empty
 * 
 * @tparam Head the current head of the type list
 * @tparam Tail the current tail of the type list
 */
template<typename Head, typename... Tail>
struct sort_impl<type_list<Head, Tail...>> {
private:
    /**
     * @brief sort the tail
     */
    using sorted_tail = typename sort_impl<type_list<Tail...>>::type;
public:
    /**
     * @brief store the full sorted type list
     */
    using type = typename insert<Head, sorted_tail>::type;
};

/**
 * @brief a type to sort a parameter pack
 * 
 * @tparam Ts the types to sort
 */
template<typename... Ts>
using sort_types = typename sort_impl<type_list<Ts...>>::type;

/**
 * @brief a structure to get the type at a specific index
 * 
 * @tparam I the index to get the type at
 * @tparam List the type list to get the type from
 */
template<std::size_t I, typename List>
struct type_at;

/**
 * @brief case if the count down hit 0
 * 
 * @tparam Head the head of the list
 * @tparam Tail the tailing elements
 */
template<typename Head, typename... Tail>
struct type_at<0, type_list<Head, Tail...>> {
    /**
     * @brief store the final type
     */
    using type = Head;
};

/**
 * @brief helper to recurse the list and decrease the counter
 * 
 * @tparam I the index of the element
 * @tparam Head the head of the list
 * @tparam Tail the tail of the list
 */
template<std::size_t I, typename Head, typename... Tail>
struct type_at<I, type_list<Head, Tail...>> {
    //sanity check and recurse
    static_assert(I < sizeof...(Tail) + 1);
    /**
     * @brief iteratively search the type list
     */
    using type = typename type_at<I-1, type_list<Tail...>>::type;
};

/**
 * @brief a utility to check if a type appears in a parameter pack
 * 
 * @tparam T the type to search
 * @tparam Ts the parameter pack to look in
 */
template <typename T, typename... Ts>
inline constexpr bool contains_type_v = (std::same_as<T, Ts> || ...);

/**
 * @brief get traits of a function type
 * 
 * @tparam T the function type to get the traits for
 */
template <typename T>
struct function_traits;

/**
 * @brief implementation for a function pointer
 * 
 * @tparam R the return type of the function
 * @tparam Args the arguments of the function
 */
template <typename R, typename... Args>
struct function_traits<R(*)(Args...)> {
    /**
     * @brief store the return type
     */
    using return_type = R;
    /**
     * @brief store the type list of the argument types
     */
    using param_types = type_list<Args...>;
    /**
     * @brief store the amount of arguments
     */
    static constexpr uint64_t arity = sizeof...(Args);
};

/**
 * @brief implementation for a function reference
 * 
 * @tparam R the return type of the function
 * @tparam Args the arguments of the function
 */
template <typename R, typename... Args>
struct function_traits<R(&)(Args...)> {
    /**
     * @brief store the return type
     */
    using return_type = R;
    /**
     * @brief store the type list of the argument types
     */
    using param_types = type_list<Args...>;
    /**
     * @brief store the amount of arguments
     */
    static constexpr uint64_t arity = sizeof...(Args);
};

/**
 * @brief implementation for a function reference
 * 
 * @tparam R the return type of the function
 * @tparam Args the arguments of the function
 */
template <typename R, typename... Args>
struct function_traits<R(Args...)> {
    /**
     * @brief store the return type
     */
    using return_type = R;
    /**
     * @brief store the type list of the argument types
     */
    using param_types = type_list<Args...>;
    /**
     * @brief store the amount of arguments
     */
    static constexpr uint64_t arity = sizeof...(Args);
};

/**
 * @brief implementation for a member function
 * 
 * @tparam R the return type of the function
 * @tparam C the class the function belongs to
 * @tparam Args the arguments of the function
 */
template <typename R, typename C, typename... Args>
struct function_traits<R(C::*)(Args...)> {
    /**
     * @brief store the return type
     */
    using return_type = R;
    /**
     * @brief store the class that owns the function
     */
    using class_type = C;
    /**
     * @brief store the type list of the argument types
     */
    using param_types = type_list<Args...>;
    /**
     * @brief store the amount of arguments
     */
    static constexpr uint64_t arity = sizeof...(Args);
};

/**
 * @brief implementation for a constant member function
 * 
 * @tparam R the return type of the function
 * @tparam C the class the function belongs to
 * @tparam Args the arguments of the function
 */
template <typename R, typename C, typename... Args>
struct function_traits<R(C::*)(Args...) const> {
    /**
     * @brief store the return type
     */
    using return_type = R;
    /**
     * @brief store the class that owns the function
     */
    using class_type = C;
    /**
     * @brief store the type list of the argument types
     */
    using param_types = type_list<Args...>;
    /**
     * @brief store the amount of arguments
     */
    static constexpr uint64_t arity = sizeof...(Args);
};

/**
 * @brief implementation for a ()-operator, lambda or functor
 * 
 * @tparam T the type of the ()-operator, lambda or functor
 */
template <typename T>
struct function_traits {
private:
    /**
     * @brief use the member function implementation
     */
    using call_type = function_traits<decltype(&T::operator())>;
public:
    /**
     * @brief store the return type
     */
    using return_type = typename call_type::return_type;
    /**
     * @brief store the type list of the argument types
     */
    using param_types = typename call_type::param_types;
    /**
     * @brief store the amount of arguments
     */
    static constexpr uint64_t arity = call_type::arity;
};

/**
 * @brief a helper to check if something can be written to
 * 
 * @tparam T the type to check
 * @tparam Param a parameter to check
 */
template <typename T, typename Param>
static constexpr bool wants_write_v = std::is_lvalue_reference_v<Param> && !std::is_const_v<std::remove_reference_t<Param>>;

}

/**
 * @brief a wrapper class for std exceptions
 */
class Exception : public std::exception {
public:

    /**
     * @brief Construct a new Exception
     * 
     * @param msg the message to store
     */
    Exception(const std::string& msg)
     : m_msg("[Tiny ECS Error] " + msg)
    {}

    /**
     * @brief the overload to get the message of the exception
     * 
     * @return `const char*` the message of the exception
     */
    inline const char* what() const noexcept(true) override
    {return m_msg.c_str();}

protected:
    /**
     * @brief store the entered message
     */
    std::string m_msg;
};

/**
 * @brief define what an entity is
 * 
 * @tparam T the type of the index type to store
 * @tparam INDEX_SIZE_V the size of the index section
 * @tparam INDEX_OFFSET_V the offset of the index section
 */
template <typename T = uint32_t, T INDEX_SIZE_V = 28, T INDEX_OFFSET_V = 0>
requires std::is_integral_v<T>
struct Entity_t {

    //no default constructor
    Entity_t() = delete;

    /**
     * @brief 
     * 
     * @tparam _T the type of the index type to store
     * @tparam _INDEX_SIZE_V the size of the index section
     * @tparam _INDEX_OFFSET_V the offset of the index section
     * @param other the entity to compare with
     * @return `true` if the entities match, `false` if they do not match
     */
    template <typename _T, _T _INDEX_SIZE_V, _T _INDEX_OFFSET_V>
    bool operator==(const Entity_t<_T, _INDEX_SIZE_V, _INDEX_OFFSET_V>& other) const
    {return (blob == static_cast<T>(other.blob));}

protected:

    /**
     * @brief store the type of the storage
     */
    using base_type = T;

    /**
     * @brief worlds are friends
     */
    template <size_t ChunkSize, size_t MaxThreads, typename EntityT, typename ArchtypeIntegral, typename ColumnIntegral, typename ChunkIntegral, template <typename> typename Allocator>
    requires (util::is_entity_type<EntityT>::value && std::is_base_of_v<std::allocator<EntityT>, Allocator<EntityT>> && (ChunkSize > 0) && util::is_power_of_2(ChunkSize) && std::is_integral_v<ArchtypeIntegral> && std::is_integral_v<ColumnIntegral> && std::is_integral_v<ChunkIntegral>)
    friend class World_t;

    /**
     * @brief store the data
     * 
     * Packed as `VERSION | INDEX`
     */
    T blob = 0;

    /**
     * @brief the size of the index section
     */
    inline static constexpr T INDEX_SIZE = INDEX_SIZE_V;
    /**
     * @brief the offset of the index section
     */
    inline static constexpr T INDEX_OFFSET = INDEX_OFFSET_V;
    /**
     * @brief store a mask to get the index data
     */
    inline static constexpr T INDEX_MASK = ((1ULL << INDEX_SIZE)-1ULL) << INDEX_OFFSET;

    /**
     * @brief store the amount of bits for the version number
     */
    inline static constexpr T VERSION_SIZE = (sizeof(blob)*8)-(INDEX_SIZE + INDEX_OFFSET);
    /**
     * @brief store the offset of the version
     */
    inline static constexpr T VERSION_OFFSET = INDEX_SIZE+INDEX_OFFSET;
    /**
     * @brief store the mask to get the version
     */
    inline static constexpr T VERSION_MASK = ((1ULL << VERSION_SIZE)-1ULL) << VERSION_OFFSET;

    /**
     * @brief set the index of the entity
     * 
     * @param index the new value for the index
     */
    inline constexpr void setIndex(T index) noexcept
    {blob = (blob & ~INDEX_MASK) | ((index<<INDEX_OFFSET) & INDEX_MASK);}

    /**
     * @brief read the index from the blob
     */
    inline constexpr T getIndex() const noexcept
    {return T((blob & INDEX_MASK)>>INDEX_OFFSET);}

    /**
     * @brief set the version of the entity
     * 
     * @param version the new value for the version
     */
    inline constexpr void setVersion(T version) noexcept
    {blob = (blob & ~VERSION_MASK) | ((version<<VERSION_OFFSET) & VERSION_MASK);}

    /**
     * @brief read the version from the blob
     */
    inline constexpr T getVersion() const noexcept
    {return T((blob & VERSION_MASK)>>VERSION_OFFSET);}

    /**
     * @brief Construct a new Entity_t
     * 
     * @param index the index of the entity
     * @param version the version of the entity
     */
    constexpr Entity_t(base_type index, base_type version)
     : blob((((index<<INDEX_OFFSET) & INDEX_MASK)) | ((version<<VERSION_OFFSET) & VERSION_MASK))
    {}

public:

    /**
     * @brief store an invalid identifier
     */
    inline static constexpr Entity_t INVALID = Entity_t(INDEX_MASK, VERSION_MASK);

};

/**
 * @brief the entity check too lives in the utility namespace
 */
namespace util {

/**
 * @brief a structure to check if a type is an entity type
 * 
 * overload for the entity type
 * 
 * @tparam T the type of the entity type's storage
 * @tparam INDEX_SIZE the size of the index section
 * @tparam INDEX_OFFSET the offset of the index section
 */
template <typename T, T INDEX_SIZE, T INDEX_OFFSET>
struct is_entity_type<Entity_t<T, INDEX_SIZE, INDEX_OFFSET>> : std::true_type
{};

}

/**
 * @brief define a default value for the entity type
 */
using Entity = Entity_t<uint32_t, 28, 0>;

/**
 * @brief The main class of the ECS that is responsible for data storage
 * 
 * @tparam ChunkSize the amount of elements in each chunk
 * @tparam MaxThreads the maximum amount of reading / non-overlapping writing threads that can access a single column at any time
 * @tparam EntityT what type to use as an entity
 * @tparam ArchtypeIntegral the integral type to use for archtype counting
 * @tparam ColumnIntegral the integral type to use for column counting
 * @tparam ChunkIntegral the integral type to use for chunk related operations (increase if more data storage is required)
 * @tparam Allocator the allocator type to use
 */
template <size_t ChunkSize = 1024, size_t MaxThreads = 32, typename EntityT = Entity, typename ArchtypeIntegral = uint16_t, typename ColumnIntegral = uint16_t, typename ChunkIntegral = uint32_t, template <typename> typename Allocator = std::allocator>
requires (util::is_entity_type<EntityT>::value && std::is_base_of_v<std::allocator<EntityT>, Allocator<EntityT>> && (ChunkSize > 0) && util::is_power_of_2(ChunkSize) && std::is_integral_v<ArchtypeIntegral> && std::is_integral_v<ColumnIntegral> && std::is_integral_v<ChunkIntegral>)
class World_t {
public:

    /**
     * @brief Construct a new World
     */
    World_t() = default;

    /**
     * @brief create a new entity
     * 
     * @tparam Components the components the entity has
     * @tparam Args the arguments of the entity
     * @param args the arguments of the entity
     * @return `EntityT` the final entity ID
     */
    template <typename... Components, typename... Args>
    requires (util::contains_type_v<Args, Components...> && ... )
    EntityT create(Args&&... args) {
        //compute the archtype
        i_arch_t archt = archtypeId<Components...>();
        //check if the archtype exists
        if (archt >= m_arches.size()) {
            //sanity check the amount to not surpass the maximum amount
            #ifdef TINY_ECS_DEBUG
            if (m_arches.size() >= MAX_ARCHTYPE_COUNT) {
                throw Exception(std::string("Ran out of archtype IDs: Can only create ") + std::to_string(MAX_ARCHTYPE_COUNT) + " archtypes, but was tasked to create more");
                return;
            }
            #endif

            //if it is safe, add to it
            m_arches.resize(archt + 1);
        }
        if (m_arches[archt].m_world == nullptr) {
            m_arches[archt] = ArchtypeView<Components...>::create(this);
        }

        //store the new entity ID
        EntityT ent(0, 0);
        //create a global ID
        if (m_freeList.empty()) {
            //sanity check in debug mode
            #ifdef TINY_ECS_DEBUG
            if (m_entities.size() >= MAX_ENTITY_COUNT) {
                throw Exception(std::string("Ran out of type IDs: Can only create ") + std::to_string(MAX_ENTITY_COUNT) + " entities, but was tasked to create more");
                return;
            }
            #endif

            //add to the end
            ent.setIndex(m_entities.size());
            m_entities.push_back(EntityMeta{});
        } else {
            //read the last entry and pop it
            i_base_t id = m_freeList.back();
            m_freeList.pop_back();
            ent.setIndex(id);
            //read the allready updated version (version update upon destruction)
            ent.setVersion(m_entities[ent.getIndex()].version);
        }

        //store the new archtype and new local ID
        m_entities[ent.getIndex()].archtypeId = archt;
        m_entities[ent.getIndex()].localId = m_arches[archt].template createView<Components...>().template allocateEntity<Args...>(ent, std::forward<Args>(args)...);

        //return the final entity entry
        return ent;
    }

    /**
     * @brief a function to create a lot of entities at once
     * 
     * @tparam Components the components for the entities to create
     * @tparam Args the argument types to use for entity creation
     * @param count the amount of entities to create
     * @param entities a std::vector to fill with the entity names
     * @param args the arguments to use for construction
     */
    template <typename... Components, typename... Args>
    requires (util::contains_type_v<Args, Components...> && ... )
    void bulkCreate(size_t count, std::vector<EntityT>& entities, Args&&... args) {
        //compute the archtype
        i_arch_t archt = archtypeId<Components...>();
        //check if the archtype exists
        if (archt >= m_arches.size()) {
            //sanity check the amount to not surpass the maximum amount
            #ifdef TINY_ECS_DEBUG
            if (m_arches.size() >= MAX_ARCHTYPE_COUNT) {
                throw Exception(std::string("Ran out of archtype IDs: Can only create ") + std::to_string(MAX_ARCHTYPE_COUNT) + " archtypes, but was tasked to create more");
                return;
            }
            #endif

            //if it is safe, add to it
            m_arches.resize(archt + 1);
        }
        if (m_arches[archt].m_world == nullptr) {
            m_arches[archt] = ArchtypeView<Components...>::create(this);
        }

        //fill in the entity IDs
        entities.reserve(count);
        for (size_t i = 0; i < count; ++i) {
            //store the new entity ID
            EntityT ent(0, 0);

            //check if an ID can be re-used
            if (m_freeList.empty()) {
                //sanity check in debug mode
                #ifdef TINY_ECS_DEBUG
                if (m_entities.size() >= MAX_ENTITY_COUNT) {
                    throw Exception(std::string("Ran out of type IDs: Can only create ") + std::to_string(MAX_ENTITY_COUNT) + " entities, but was tasked to create more");
                    return;
                }
                #endif

                //add to the end
                ent.setIndex(m_entities.size());
                m_entities.push_back(EntityMeta{});
            } else {
                //read the last entry and pop it
                i_base_t id = m_freeList.back();
                m_freeList.pop_back();
                ent.setIndex(id);
                //read the allready updated version (version update upon destruction)
                ent.setVersion(m_entities[ent.getIndex()].version);
            }

            //add to the collection
            entities.push_back(ent);
        }

        //bulk-create the entitys
        std::vector<i_base_t> locals;
        m_arches[archt].template createView<Components...>().template bulkAllocateEntity<Args...>(entities, locals, std::forward<Args>(args)...);

        //update all entity mappings
        for (size_t i = 0; i < count; ++i) {
            m_entities[entities[i].getIndex()].archtypeId = archt;
            m_entities[entities[i].getIndex()].localId = locals[i];
        }
    }

    /**
     * @brief remove an entity
     * 
     * @param entity the entity to remove
     */
    void remove(EntityT entity) {
        //reject invalid entity IDs
        if (entity.getIndex() >= m_entities.size())
        {return;}

        //reject a stale reference
        EntityMeta& entry = m_entities[entity.getIndex()];
        if (entity.getVersion() != entry.version)
        {return;}

        //invalidate all other handles
        ++entry.version;
        //remove from archtypes
        m_arches[entry.archtypeId].removeElement(entry.localId);
    }

    /**
     * @brief add a new component to an entity
     * 
     * @tparam Component the component to add
     * @tparam Args the arguments to use for component construction
     * @param entity the entity to add the component at
     * @param args the arguments to use for component construction
     */
    template <typename Component, typename... Args>
    void add(EntityT entity, Args&&... args) {
        //reject invalid entity IDs
        if (entity.getIndex() >= m_entities.size())
        {return;}

        //reject a stale reference
        EntityMeta& entry = m_entities[entity.getIndex()];
        if (entity.getVersion() != entry.version)
        {return;}

        //get the component type IDs
        std::vector<i_col_t, Allocator<i_col_t>> tids = m_archtypeResolution[entry.archtypeId];
        i_col_t newCol = typeId<Component>();
        tids.push_back(newCol);

        //check if the column of the added component exists
        if (m_columns.size() <= newCol) {
            //sanity check in debug if the column id count is saturated
            #ifdef TINY_ECS_DEBUG
            if (m_columns.size() >= MAX_COLUMN_COUNT) {
                throw Exception(std::string("Ran out of column IDs: Can only create ") + std::to_string(MAX_COLUMN_COUNT) + " columns, but was tasked to create more");
                return;
            }
            #endif

            m_columns.resize(newCol + 1);
        }
        if (m_columns[newCol].m_world == nullptr) {
            m_columns[newCol] = ColumnView<Component>::createColumn(this);
        }

        //get the new archtype ID
        i_arch_t archt = archtypeId(tids);
        //check if the archtype exists
        if (archt >= m_arches.size()) {
           //sanity check the amount to not surpass the maximum amount
           #ifdef TINY_ECS_DEBUG
           if (m_arches.size() >= MAX_ARCHTYPE_COUNT) {
               throw Exception(std::string("Ran out of archtype IDs: Can only create ") + std::to_string(MAX_ARCHTYPE_COUNT) + " archtypes, but was tasked to create more");
               return;
           }
           #endif

           //if it is safe, add to it
           m_arches.resize(archt + 1);
        }
        ArchtypeBase& arch = m_arches[archt];
        if (arch.m_world == nullptr) 
        {arch = ArchtypeBase(archt, tids.begin(), tids.end(), this);}

        //create the new entity in the archtype
        i_base_t newLocal = arch.m_localToGlobal.size();
        arch.m_localToGlobal.push_back(entity);

        //swap-erase from the old archtype
        ArchtypeBase& oldArch = m_arches[entry.archtypeId];
        i_base_t oldLocal = entry.localId;
        EntityT oldBack = oldArch.m_localToGlobal.back();
        oldArch.m_localToGlobal[entry.localId] = oldBack;
        oldArch.m_localToGlobal.pop_back();
        m_entities[oldBack.getIndex()].localId = oldLocal;

        //migrate over the old columns
        for (size_t i = 0; i < oldArch.m_columns.size(); ++i)
        {m_columns[oldArch.m_columns[i]].move(oldArch.m_id, oldLocal, archt);}

        //register new archtype
        entry.archtypeId = archt;
        entry.localId = newLocal;

        //add to the new column
        m_columns[newCol].template createView<Component>().addElement(archt, std::forward<Args>(args)...);
    }

    /**
     * @brief remove a component from an entity
     * 
     * @tparam Component the component type to remove
     * @param entity the entity to remove the component from
     */
    template <typename Component>
    void remove(EntityT entity) {
        //reject invalid entity IDs
        if (entity.getIndex() >= m_entities.size())
        {return;}

        //reject a stale reference
        EntityMeta& entry = m_entities[entity.getIndex()];
        if (entity.getVersion() != entry.version)
        {return;}

        //get the component id
        i_col_t tid = typeId<Component>();
        //get the component type IDs
        std::vector<i_col_t, Allocator<i_col_t>> tids = m_archtypeResolution[entry.archtypeId];
        for (size_t i = 0; i < tids.size(); ++i) {
            if (tids[i] == tid) {
                tids.erase(tids.begin() + i); 
                break;
            }
        }

        //get the new archtype ID
        i_arch_t archt = archtypeId(tids);
        //check if the archtype exists
        if (archt >= m_arches.size()) {
           //sanity check the amount to not surpass the maximum amount
           #ifdef TINY_ECS_DEBUG
           if (m_arches.size() >= MAX_ARCHTYPE_COUNT) {
               throw Exception(std::string("Ran out of archtype IDs: Can only create ") + std::to_string(MAX_ARCHTYPE_COUNT) + " archtypes, but was tasked to create more");
               return;
           }
           #endif

           //if it is safe, add to it
           m_arches.resize(archt + 1);
        }
        ArchtypeBase& arch = m_arches[archt];
        if (arch.m_world == nullptr) 
        {arch = ArchtypeBase(archt, tids.begin(), tids.end(), this);}

        //create the new entity in the archtype
        i_base_t newLocal = arch.m_localToGlobal.size();
        arch.m_localToGlobal.push_back(entity);

        //swap-erase from the old archtype
        ArchtypeBase& oldArch = m_arches[entry.archtypeId];
        i_base_t oldLocal = entry.localId;
        EntityT oldBack = oldArch.m_localToGlobal.back();
        oldArch.m_localToGlobal[entry.localId] = oldBack;
        oldArch.m_localToGlobal.pop_back();
        m_entities[oldBack.getIndex()].localId = oldLocal;

        //migrate over the old columns
        for (size_t i = 0; i < arch.m_columns.size(); ++i)
        {m_columns[arch.m_columns[i]].move(oldArch.m_id, oldLocal, archt);}

        //register new archtype
        entry.archtypeId = archt;
        entry.localId = newLocal;
    }

    /**
     * @brief a function to get a specific element from an entity
     * 
     * @tparam Component the type of the component to get
     * @param entity the entity to get the ID for
     * @return `Component*` a pointer to the component or `nullptr` if it was not found
     */
    template <typename Component>
    Component* get(EntityT entity) {
        //reject invalid entity IDs
        if (entity.getIndex() >= m_entities.size())
        {return nullptr;}

        //reject a stale reference
        if (entity.getVersion() != m_entities[entity.getIndex()].version)
        {return nullptr;}

        //read the component
        return m_arches[m_entities[entity.getIndex()].archtypeId].template get<Component>(m_entities[entity.getIndex()].localId);
    }

    /**
     * @brief a function that runs for all entities that exactly match the specified components
     * 
     * @tparam Components the exact component mask the entities must match
     * @tparam Func the function to call for all of them
     * @param fn the function to call for all of them
     */
    template <typename... Components, typename Func>
    void each(Func&& fn) {
        //get the archtype ID
        i_arch_t archt = archtypeId<Components...>();

        //if the archtype is unknown, stop (no entities to iterate)
        if (archt >= m_arches.size()) {return;}
        ArchtypeBase& arch = m_arches[archt];
        if (arch.m_world == nullptr) {return;}

        //get the amount of elements to iterate over and early out on 0
        const size_t count = arch.m_localToGlobal.size();
        if (count == 0) {return;}

        //get pointers to all first elements
        std::tuple<Components*...> columns = {reinterpret_cast<Components*>(m_columns[typeId<Components>()].get(arch.m_id, 0))...};

        //lock all the requested components locks
        std::array<ColumnId, sizeof...(Components)> tids = {(ColumnId{typeId<Components>(), util::index_of<Components, util::type_list<Components...>>::value}) ...};
        std::sort(tids.begin(), tids.end());
        bool writers[sizeof...(Components)] = {(util::wants_write_v<Components, typename util::type_at<util::index_of<Components, util::type_list<Components...>>::value, typename util::function_traits<Func>::param_types>::type>)...};
        typename ColumnLock::LockKey keys[sizeof...(Components)] = {m_columns[tids[util::index_of<Components, util::type_list<Components...>>::value].column].m_lock.lock(archt, writers[tids[util::index_of<Components, util::type_list<Components...>>::value].local]) ...};

        //iterate over the entities
        for (size_t local = 0; local < count; ++local) {
            //invoke the function
            invokeEach<Func, Components...>(fn, arch, local, columns);
            //advance the pointers
            ((++std::get<Components*>(columns)), ...);
        }

        //unlock in reversed order
        (keys[(sizeof...(Components)-1) - util::index_of<Components, util::type_list<Components...>>::value].unlock(), ...);
    }

    /**
     * @brief a function to run a function on all instances of a specific component
     * 
     * @tparam Component the component to run on
     * @tparam Func the function to invoke
     * @param fn the function to invoke
     */
    template <typename Component, typename Func>
    void each_with(Func&& fn) {
        //get the column (type) id
        i_col_t tid = typeId<Component>();
        
        //check if the column exists
        if (tid >= m_columns.size()) {return;}
        ColumnBase& base = m_columns[tid];
        if (base.m_world == nullptr) {return;}

        //pre-fetch all archtypes that use this column
        std::vector<i_arch_t> archtypes;
        archtypes.reserve(m_archtypeResolution.size());
        for (size_t i = 0; i < m_archtypeResolution.size(); ++i) {
            //check if this satisfies the combination
            for (size_t j = 0; j < m_archtypeResolution[i].size(); ++j) {
                //check if this matches the type
                if (m_archtypeResolution[i][j] == tid) {
                    archtypes.push_back(i);
                    break;
                }
            }
        }

        //now, play back all archtypes and iterate over them
        for (i_arch_t archt : archtypes) {
            //obtain a lock on the archtype
            typename ColumnLock::LockKey key = m_columns[tid].m_lock.lock(archt, util::wants_write_v<Component, typename util::function_traits<Func>::param_types>);

            i_base_t size = m_arches[archt].m_localToGlobal.size();
            Component* start = reinterpret_cast<Component*>(base.get(archt, 0));
            //play back for all entries
            for (size_t i = 0; i < size; ++i) {
                //invoke the function
                invokeEach<Func, Component>(fn, m_arches[archt], i, std::tuple<Component*>(start));
                //step the pointer
                ++start;
            }
        }
    }

    /**
     * @brief a function to run on all entities that have AT LEAST the specified components
     * 
     * @tparam Components the components that are AT LEAST required
     * @tparam Func the function to call
     * @param fn the function to call
     */
    template <typename ...Components, typename Func>
    void each_match(Func&& fn) {
        //lock all the requested components locks
        std::array<ColumnId, sizeof...(Components)> tids = {(ColumnId{typeId<Components>(), util::index_of<Components, util::type_list<Components...>>::value}) ...};
        std::sort(tids.begin(), tids.end());
        bool writers[sizeof...(Components)] = {(util::wants_write_v<Components, typename util::type_at<util::index_of<Components, util::type_list<Components...>>::value, typename util::function_traits<std::remove_cvref_t<Func>>::param_types>::type>)...};

        //store all column bases
        ColumnBase* columns[sizeof...(Components)] = { nullptr };
        //check if the columns exists
        for (size_t i = 0; i < sizeof...(Components); ++i) {
            if (tids[i].column >= m_columns.size()) {return;}
            columns[i] = &m_columns[tids[i].column];
            if (columns[i]->m_world == nullptr) {return;}
        }

        //pre-fetch all archtypes that use this column
        std::vector<i_arch_t> archtypes;
        archtypes.reserve(m_archtypeResolution.size());
        for (size_t i = 0; i < m_archtypeResolution.size(); ++i) {
            //check if this satisfies the combination
            size_t found = 0;
            for (size_t j = 0; j < m_archtypeResolution[i].size(); ++j) {
                //check if this matches ANY type
                for (size_t k = 0; k < sizeof...(Components); ++k) {
                    if (m_archtypeResolution[i][j] == tids[k].column) {
                        ++found;
                        break;
                    }
                }
            }

            //if all were found, add to the combination count
            if (found == sizeof...(Components)) {
                archtypes.push_back(i);
            }
        }

        //now, play back all archtypes and iterate over them
        for (i_arch_t archt : archtypes) {

            //lock the columns
            typename ColumnLock::LockKey keys[sizeof...(Components)] = {m_columns[tids[util::index_of<Components, util::type_list<Components...>>::value].column].m_lock.lock(archt, writers[tids[util::index_of<Components, util::type_list<Components...>>::value].local]) ...};

            //quarry size and starting positions
            i_base_t size = m_arches[archt].m_localToGlobal.size();
            std::tuple<Components*...> columns = {reinterpret_cast<Components*>(m_columns[typeId<Components>()].get(archt, 0))...};
            //play back for all entries
            for (size_t i = 0; i < size; ++i) {
                //invoke the function
                invokeEach<Func, Components...>(fn, m_arches[archt], i, columns);
                //step the pointer
                (++std::get<Components*>(columns), ...);
            }
        }
    }

    /**
     * @brief a function to run a function on all instances of a specific component
     * 
     * @tparam Component the component to run on
     * @tparam Func the function to invoke
     * @param fn the function to invoke, only accepted parameter is the entity
     */
    template <typename Component, typename... Args>
    requires ((sizeof...(Args) == 0) || ((sizeof...(Args) == 1) && util::contains_type_v<EntityT, Args...>))
    void invoke(void (Component::*fn)(Args...)) {
        //get the column (type) id
        i_col_t tid = typeId<Component>();
        
        //check if the column exists
        if (tid >= m_columns.size()) {return;}
        ColumnBase& base = m_columns[tid];
        if (base.m_world == nullptr) {return;}

        //pre-fetch all archtypes that use this column
        std::vector<i_arch_t> archtypes;
        archtypes.reserve(m_archtypeResolution.size());
        for (size_t i = 0; i < m_archtypeResolution.size(); ++i) {
            //check if this satisfies the combination
            for (size_t j = 0; j < m_archtypeResolution[i].size(); ++j) {
                //check if this matches the type
                if (m_archtypeResolution[i][j] == tid) {
                    archtypes.push_back(i);
                    break;
                }
            }
        }

        //now, play back all archtypes and iterate over them
        for (i_arch_t archt : archtypes) {
            //obtain a lock on the archtype
            typename ColumnLock::LockKey key = m_columns[tid].m_lock.lock(archt, util::wants_write_v<Component, util::type_list<Args...>>);

            i_base_t size = m_arches[archt].m_localToGlobal.size();
            Component* start = reinterpret_cast<Component*>(base.get(archt, 0));
            //play back for all entries
            for (size_t i = 0; i < size; ++i) {
                //invoke the function
                if constexpr (util::contains_type_v<EntityT, Args...>) {
                    (start->*fn)(m_arches[archt].m_localToGlobal[i]);
                } else {
                    (start->*fn)();
                }
                //step the pointer
                ++start;
            }
        }
    }

    //enable multithreaded features only if multi threaded feature flag is set
    #if TINY_ECS_FEATURE_MULTITHREADED

    /**
     * @brief a function that runs for all entities that exactly match the specified components parallelized using the job system
     * 
     * @tparam Components the exact component mask the entities must match
     * @tparam Func the function to call for all of them
     * @tparam The amount of fibers the employer has per thread
     * @param fn the function to call for all of them
     * @param employer the employer to utilize for threading
     */
    template <typename ...Components, typename Func, size_t FiberCount>
    void parallel_each(Func&& fn, Jobs::Employer_t<FiberCount>& employer) {
        //get the archtype ID
        i_arch_t archt = archtypeId<Components...>();

        //if the archtype is unknown, stop (no entities to iterate)
        if (archt >= m_arches.size()) {return;}
        ArchtypeBase& arch = m_arches[archt];
        if (arch.m_world == nullptr) {return;}

        //get the amount of elements to iterate over and early out on 0
        const size_t count = arch.m_localToGlobal.size();
        if (count == 0) {return;}

        //get pointers to all first elements
        std::tuple<Components*...> columns = {reinterpret_cast<Components*>(m_columns[typeId<Components>()].get(arch.m_id, 0))...};

        //lock all the requested components locks
        std::array<ColumnId, sizeof...(Components)> tids = {(ColumnId{typeId<Components>(), util::index_of<Components, util::type_list<Components...>>::value}) ...};
        std::sort(tids.begin(), tids.end());
        bool writers[sizeof...(Components)] = {(util::wants_write_v<Components, typename util::type_at<util::index_of<Components, util::type_list<Components...>>::value, typename util::function_traits<Func>::param_types>::type>)...};

        //compute the amount of tasks required (complicated because underflow prevention is required)
        size_t freeChunks = (m_columns[tids[0].column].getArchtypeMeta(archt).freeCount / ChunkSize);
        size_t taskCount = ((count + (ChunkSize - 1)) / ChunkSize);
        if (taskCount > freeChunks) {taskCount -= ((freeChunks > 0) ? 0 : (freeChunks - 1));}
        std::atomic_size_t leftJobs{taskCount};
        auto workerTask = [&](size_t chunkId, std::tuple<Components*...> localColumns) {
            //compute start and end index
            size_t startIdx = chunkId * ChunkSize;
            size_t endIdx = ((chunkId + 1) * ChunkSize);
            if (endIdx > count) {endIdx = count;}
            //set all the starting positions in the tuple
            ((std::get<Components*>(localColumns) += startIdx), ...);
            //iterate over the chunk
            for (size_t i = startIdx; i < endIdx; ++i) {
                //invoke the function
                invokeEach<Func, Components...>(fn, arch, i, localColumns);
                //advance the pointers
                ((++std::get<Components*>(localColumns)), ...);
            }
            //ask done
            leftJobs.fetch_sub(1, std::memory_order_acq_rel);
        };
        //create the tasks
        Jobs::BulkTask tasks(taskCount, workerTask, columns);

        //get all the locks
        typename ColumnLock::LockKey keys[sizeof...(Components)] = {m_columns[tids[util::index_of<Components, util::type_list<Components...>>::value].column].m_lock.lock(archt, writers[tids[util::index_of<Components, util::type_list<Components...>>::value].local]) ...};

        //run the jobs
        employer.add_bulk(tasks);
        //wait for the job to be done
        while (leftJobs.load(std::memory_order_acquire) > 0)
        {std::this_thread::sleep_for(std::chrono::nanoseconds(1));}
    }

    /**
     * @brief a function to run a function on all instances of a specific component parallelized using the job system
     * 
     * @tparam Component the component to run on
     * @tparam Func the function to invoke
     * @tparam The amount of fibers the employer has per thread
     * @param fn the function to invoke
     * @param employer the employer to utilize for threading
     */
    template <typename Component, typename Func, size_t FiberCount>
    void parallel_each_with(Func&& fn, Jobs::Employer_t<FiberCount>& employer) {
        //get the column (type) id
        i_col_t tid = typeId<Component>();
        
        //check if the column exists
        if (tid >= m_columns.size()) {return;}
        ColumnBase& base = m_columns[tid];
        if (base.m_world == nullptr) {return;}

        //pre-fetch all archtypes that use this column
        std::vector<i_arch_t> archtypes;
        archtypes.reserve(m_archtypeResolution.size());
        for (size_t i = 0; i < m_archtypeResolution.size(); ++i) {
            //check if this satisfies the combination
            for (size_t j = 0; j < m_archtypeResolution[i].size(); ++j) {
                //check if this matches the type
                if (m_archtypeResolution[i][j] == tid) {
                    archtypes.push_back(i);
                    break;
                }
            }
        }

        //now, play back all archtypes and iterate over them
        for (i_arch_t archt : archtypes) {
            //compute the amount of tasks required (complicated because underflow prevention is required)
            size_t count = m_arches[archt].m_localToGlobal.size();
            size_t freeChunks = (m_columns[tid].getArchtypeMeta(archt).freeCount / ChunkSize);
            size_t taskCount = ((count + (ChunkSize - 1)) / ChunkSize);
            if (taskCount > freeChunks) {taskCount -= ((freeChunks > 0) ? 0 : (freeChunks - 1));}
            std::atomic_size_t leftJobs{taskCount};
            auto workerTask = [&](size_t chunkId, Component* localColumn) {
                //compute start and end index
                size_t startIdx = chunkId * ChunkSize;
                size_t endIdx = ((chunkId + 1) * ChunkSize);
                if (endIdx > count) {endIdx = count;}
                //set all the starting positions in the tuple
                localColumn += startIdx;
                //iterate over the chunk
                for (size_t i = startIdx; i < endIdx; ++i) {
                    //invoke the function
                    invokeEach<Func, Component>(fn, m_arches[archt], i, std::tuple<Component*>(localColumn));
                    //advance the pointers
                    ++localColumn;;
                }
                //ask done
                leftJobs.fetch_sub(1, std::memory_order_acq_rel);
            };
            //create the tasks
            Jobs::BulkTask tasks(taskCount, workerTask, reinterpret_cast<Component*>(base.get(archt, 0)));

            //obtain a lock on the archtype
            typename ColumnLock::LockKey key = m_columns[tid].m_lock.lock(archt, util::wants_write_v<Component, typename util::function_traits<Func>::param_types>);

            //run the jobs
            employer.add_bulk(tasks);
            //wait for the job to be done
            while (leftJobs.load(std::memory_order_acquire) > 0)
            {std::this_thread::sleep_for(std::chrono::nanoseconds(1));}
        }
    }

    /**
     * @brief a function to run on all entities that have AT LEAST the specified components
     * 
     * @tparam Components the components that are AT LEAST required
     * @tparam Func the function to call
     * @tparam The amount of fibers the employer has per thread
     * @param fn the function to call
     * @param employer the employer to utilize for threading
     */
    template <typename ...Components, typename Func, size_t FiberCount>
    void parallel_each_match(Func&& fn, Jobs::Employer_t<FiberCount>& employer) {
        //lock all the requested components locks
        std::array<ColumnId, sizeof...(Components)> tids = {(ColumnId{typeId<Components>(), util::index_of<Components, util::type_list<Components...>>::value}) ...};
        std::sort(tids.begin(), tids.end());
        bool writers[sizeof...(Components)] = {(util::wants_write_v<Components, typename util::type_at<util::index_of<Components, util::type_list<Components...>>::value, typename util::function_traits<Func>::param_types>::type>)...};

        //store all column bases
        ColumnBase* columns[sizeof...(Components)] = { nullptr };
        //check if the columns exists
        for (size_t i = 0; i < sizeof...(Components); ++i) {
            if (tids[i].column >= m_columns.size()) {return;}
            columns[i] = &m_columns[tids[i].column];
            if (columns[i]->m_world == nullptr) {return;}
        }

        //pre-fetch all archtypes that use this column
        std::vector<i_arch_t> archtypes;
        archtypes.reserve(m_archtypeResolution.size());
        for (size_t i = 0; i < m_archtypeResolution.size(); ++i) {
            //check if this satisfies the combination
            size_t found = 0;
            for (size_t j = 0; j < m_archtypeResolution[i].size(); ++j) {
                //check if this matches ANY type
                for (size_t k = 0; k < sizeof...(Components); ++k) {
                    if (m_archtypeResolution[i][j] == tids[k].column) {
                        ++found;
                        break;
                    }
                }
            }

            //if all were found, add to the combination count
            if (found == sizeof...(Components)) {
                archtypes.push_back(i);
            }
        }

        //now, play back all archtypes and iterate over them
        for (i_arch_t archt : archtypes) {

            //compute the amount of tasks required (complicated because underflow prevention is required)
            i_base_t count = m_arches[archt].m_localToGlobal.size();
            size_t freeChunks = (m_columns[tids[0].column].getArchtypeMeta(archt).freeCount / ChunkSize);
            size_t taskCount = ((count + (ChunkSize - 1)) / ChunkSize);
            if (taskCount > freeChunks) {taskCount -= ((freeChunks > 0) ? 0 : (freeChunks - 1));}
            std::atomic_size_t leftJobs{taskCount};
            auto workerTask = [&](size_t chunkId, std::tuple<Components*...> localColumns) {
                //compute start and end index
                size_t startIdx = chunkId * ChunkSize;
                size_t endIdx = ((chunkId + 1) * ChunkSize);
                if (endIdx > count) {endIdx = count;}
                //set all the starting positions in the tuple
                ((std::get<Components*>(localColumns) += startIdx), ...);
                //iterate over the chunk
                for (size_t i = startIdx; i < endIdx; ++i) {
                    //invoke the function
                    invokeEach<Func, Components...>(fn, m_arches[archt], i, localColumns);
                    //advance the pointers
                    ((++std::get<Components*>(localColumns)), ...);
                }
                //ask done
                leftJobs.fetch_sub(1, std::memory_order_acq_rel);
            };

            //lock the columns
            typename ColumnLock::LockKey keys[sizeof...(Components)] = {m_columns[tids[util::index_of<Components, util::type_list<Components...>>::value].column].m_lock.lock(archt, writers[tids[util::index_of<Components, util::type_list<Components...>>::value].local]) ...};

            //quarry size and starting positions
            std::tuple<Components*...> columns = {reinterpret_cast<Components*>(m_columns[typeId<Components>()].get(archt, 0))...};
            //create the tasks
            Jobs::BulkTask tasks(taskCount, workerTask, columns);

            //run the jobs
            employer.add_bulk(tasks);
            //wait for the job to be done
            while (leftJobs.load(std::memory_order_acquire) > 0)
            {std::this_thread::sleep_for(std::chrono::nanoseconds(1));}
        }
    }

    /**
     * @brief a function to run a function on all instances of a specific component
     * 
     * @tparam Component the component to run on
     * @tparam Func the function to invoke
     * @tparam The amount of fibers the employer has per thread
     * @param fn the function to invoke, only accepted parameter is the entity
     * @param employer the employer to utilize for threading
     */
    template <typename Component, typename... Args, size_t FiberCount>
    requires ((sizeof...(Args) == 0) || ((sizeof...(Args) == 1) && util::contains_type_v<EntityT, Args...>))
    void parallel_invoke(void (Component::*fn)(Args...), Jobs::Employer_t<FiberCount>& employer) {
        //get the column (type) id
        i_col_t tid = typeId<Component>();
        
        //check if the column exists
        if (tid >= m_columns.size()) {return;}
        ColumnBase& base = m_columns[tid];
        if (base.m_world == nullptr) {return;}

        //pre-fetch all archtypes that use this column
        std::vector<i_arch_t> archtypes;
        archtypes.reserve(m_archtypeResolution.size());
        for (size_t i = 0; i < m_archtypeResolution.size(); ++i) {
            //check if this satisfies the combination
            for (size_t j = 0; j < m_archtypeResolution[i].size(); ++j) {
                //check if this matches the type
                if (m_archtypeResolution[i][j] == tid) {
                    archtypes.push_back(i);
                    break;
                }
            }
        }

        //now, play back all archtypes and iterate over them
        for (i_arch_t archt : archtypes) {
            //compute the amount of tasks required (complicated because underflow prevention is required)
            size_t count = m_arches[archt].m_localToGlobal.size();
            size_t freeChunks = (m_columns[tid].getArchtypeMeta(archt).freeCount / ChunkSize);
            size_t taskCount = ((count + (ChunkSize - 1)) / ChunkSize);
            if (taskCount > freeChunks) {taskCount -= ((freeChunks > 0) ? 0 : (freeChunks - 1));}
            std::atomic_size_t leftJobs{taskCount};
            auto workerTask = [&](size_t chunkId, Component* localColumn) {
                //compute start and end index
                size_t startIdx = chunkId * ChunkSize;
                size_t endIdx = ((chunkId + 1) * ChunkSize);
                if (endIdx > count) {endIdx = count;}
                //set all the starting positions in the tuple
                localColumn += startIdx;
                //iterate over the chunk
                for (size_t i = startIdx; i < endIdx; ++i) {
                    //invoke the function
                    if constexpr (util::contains_type_v<EntityT, Args...>) {
                        (localColumn->*fn)(m_arches[archt].m_localToGlobal[i]);
                    } else {
                        (localColumn->*fn)();
                    }
                    //step the pointer
                    ++localColumn;
                }
                //ask done
                leftJobs.fetch_sub(1, std::memory_order_acq_rel);
            };
            //create the tasks
            Jobs::BulkTask tasks(taskCount, workerTask, reinterpret_cast<Component*>(base.get(archt, 0)));

            //obtain a lock on the archtype
            typename ColumnLock::LockKey key = m_columns[tid].m_lock.lock(archt, util::wants_write_v<Component, util::type_list<Args...>>);

            //run the jobs
            employer.add_bulk(tasks);
            //wait for the job to be done
            while (leftJobs.load(std::memory_order_acquire) > 0)
            {std::this_thread::sleep_for(std::chrono::nanoseconds(1));}
        }
    }

    #endif

    /**
     * @brief get the maximum amount of entities allowed by the world
     * 
     * @return `size_t` the maximum allowed entity count
     */
    inline static constexpr size_t maxEntityCount() noexcept(true)
    {return MAX_ENTITY_COUNT;}

    /**
     * @brief get the maximum amount of archtypes allowed by the world
     * 
     * @return `size_t` the maximum allowed archtype count
     */
    inline static constexpr size_t maxArchtypeCount() noexcept(true)
    {return MAX_ARCHTYPE_COUNT;}

protected:

    /**
     * @brief get the base type from the entity type
     */
    using i_base_t = EntityT::base_type;
    /**
     * @brief store the integer type for all chunk-related integers
     */
    using i_chunk_t = ChunkIntegral;
    /**
     * @brief store the integer type for an column / type id
     */
    using i_col_t  = ColumnIntegral;
    /**
     * @brief store the integer type for an archtype (simple alias)
     */
    using i_arch_t = ArchtypeIntegral;

    /**
     * @brief store the maximum entity ID
     */
    static constexpr i_base_t MAX_ENTITY_COUNT = (1ULL << EntityT::INDEX_SIZE)-1;

    /**
     * @brief store the maximum amount of columns
     */
    static constexpr i_col_t  MAX_COLUMN_COUNT = std::numeric_limits<i_col_t>::max();

    /**
     * @brief store the maximum amount of archtypes allowed
     */
    static constexpr i_arch_t MAX_ARCHTYPE_COUNT = std::numeric_limits<i_arch_t>::max();

    /**
     * @brief compute a shift down value to correct the archtype local index to a chunk index
     */
    static constexpr i_chunk_t CHUNK_CORRECTION_SHIFT = util::log2_size_t<ChunkSize>();
    /**
     * @brief store a bitmask that is used to extract the chunk-local index from an archtype local index
     */
    static constexpr i_chunk_t CHUNK_INDEX_MASK = ChunkSize-1;

    /**
     * @brief a utility structure for type id mapping
     */
    struct ColumnId {
        /**
         * @brief store the global column ID
         */
        i_col_t column = 0;
        /**
         * @brief store the local column ID
         */
        i_col_t local = 0;

        /**
         * @brief operations for sorting
         * 
         * @param other the other to check if this is less than
         * @return `true` if it is, `false` otherwise
         */
        inline bool operator<(const ColumnId& other) const noexcept(true)
        {return column < other.column;}
    };

    /**
     * @brief store metadata for all entities
     */
    struct EntityMeta {
        /**
         * @brief the ID of the archtype the entity belongs to
         */
        i_arch_t archtypeId;
        /**
         * @brief the current version of the Entity
         */
        uint16_t version;
        /**
         * @brief the archtype local entity ID
         */
        i_base_t localId;
    };

    /**
     * @brief store a single chunk of data
     * 
     * This is done to minimize false-sharing
     * 
     * @tparam T the element type to store in the chunk
     */
    template <typename T>
    requires (std::is_move_constructible_v<T> && std::is_default_constructible_v<T> && (alignof(T) <= alignof(std::max_align_t)))
    struct alignas(alignof(std::max_align_t)) Chunk {

        /**
         * @brief Construct a new Chunk
         */
        Chunk() = default;

        /**
         * @brief Construct a new Chunk
         * 
         * @param other the chunk to move from
         */
        Chunk(Chunk&& other)
        noexcept (std::is_nothrow_move_assignable_v<T>)
        {
            //just move all elements
            for (size_t i = 0; i < ChunkSize; ++i) 
            {elements[i] = std::move(other.elements[i]);}
        }

        /**
         * @brief Destroy the Chunk
         */
        ~Chunk() = default;

        /**
         * @brief move-assign operator
         * 
         * @param other the chunk to move from
         * @return `Chunk&` a reference to this chunk
         */
        Chunk& operator=(Chunk&& other) 
        noexcept (std::is_nothrow_move_assignable_v<T>)
        {
            //if moving to self, just stop
            if (this == &other) {return *this;}

            //move all the elements
            for (size_t i = 0; i < ChunkSize; ++i)
            {elements[i] = std::move(other.elements[i]);}

            //return self
            return *this;
        }

        /**
         * @brief store the actual data of the chunk
         */
        T elements[ChunkSize] {};

    };

    /**
     * @brief store metadata for a single archtype element
     */
    struct ArchtypeMeta {
        /**
         * @brief the offset in chunks from the start of the data array
         */
        i_chunk_t offset = 0;
        /**
         * @brief the amount of chunks this archtype owns
         */
        i_chunk_t size = 0;
        /**
         * @brief store the amount of elements left free
         * 
         * This can be MORE than a chunk. But the slots are guaranteed to be packed at the end. 
         */
        i_chunk_t freeCount = 0;
    };

    /**
     * @brief a structure that manages mutex-less semi-locking of a column
     */
    class ColumnLock {
    public:

        /**
         * @brief a RAII based handle to automatically unlock if the lock goes out of scope
         */
        class LockKey {
        public:

            /**
             * @brief Construct a new Lock Key
             */
            LockKey() = default;

            //Moveable but not copyable
            LockKey(const LockKey&) = delete;
            LockKey& operator=(const LockKey&) = delete;

            /**
             * @brief Construct a new Lock Key
             * 
             * Move constructor
             * 
             * @param other the lock key to move from
             */
            LockKey(LockKey&& other) noexcept
             : m_column(other.m_column), m_entryId(other.m_entryId), m_write(other.m_write) 
            {other.m_column = nullptr;}

            /**
             * @brief move assignment operator
             * 
             * @param other the lock key to move from
             * @return `LockKey&` a reference to this key after moving
             */
            LockKey& operator=(LockKey&& other) noexcept {
                //don't move to self
                if (this != &other) {
                    //unlock the current lock
                    unlock();
                    //copy over the data
                    m_column = other.m_column;
                    m_entryId = other.m_entryId;
                    m_write = other.m_write;
                    //invalidate other
                    other.m_column = nullptr;
                }
                //return reference to self
                return *this;
            }

            /**
             * @brief Destroy the Lock Key
             * 
             * If this lock contains a lock, unlock it
             */
            ~LockKey() 
            {unlock();}

            /**
             * @brief a function to unlock the lock that the key holds
             */
            void unlock() {
                //if it holds no lock, stop
                if (!m_column) return;

                //access the correct entry of the entry table
                auto& entry = m_column->m_entries[m_entryId];
                //if writing is enabled, just set the writing value to false
                if (m_write) 
                {entry.writer.store(false, std::memory_order_release);} 
                //else, decrement the reader count
                else 
                {entry.readers.fetch_sub(1, std::memory_order_release);}
                //drop the lock
                m_column = nullptr;
            }

        protected:

            //column lock is friend
            friend class ColumnLock;

            /**
             * @brief Construct a new Lock Key
             * 
             * @param lock the column lock the key belongs to
             * @param id the ID of the entry the lock lies in
             * @param w `true` for writing, `false` for reading
             */
            LockKey(ColumnLock* lock, size_t id, bool w)
             : m_column(lock), m_entryId(id), m_write(w) 
            {}

            /**
             * @brief a pointer to the 
             */
            ColumnLock* m_column = nullptr;
            /**
             * @brief store the index of the entry
             */
            size_t m_entryId = 0;
            /**
             * @brief `true` if writing was enabled, `false` if not
             */
            bool m_write = false;

        };

        /**
         * @brief a function to obtain a lock for the column
         * 
         * @param archtype the archtype to claim
         * @param write `true` for write access, `false` for read-only
         * @return `LockKey` a RAII wrapper for the obtained lock
         */
        LockKey lock(i_arch_t archtype, bool write) {
            //try until a lock is obtained
            while (true) {
                //find a free entry or matching archetype
                for (size_t i = 0; i < MaxThreads; ++i) {
                    //get the current entry and load the archtype
                    Entry& entry = m_entries[i];
                    i_arch_t currentArch = entry.archtype.load(std::memory_order_acquire);

                    //check if this is an abandoned slot
                    if (currentArch != archtype && entry.readers.load() == 0 && !entry.writer.load()) {
                        //try to claim this entry for this archtype
                        if (entry.archtype.compare_exchange_strong(currentArch, archtype, std::memory_order_acq_rel)) 
                        {currentArch = archtype;}
                    }

                    //check for an archtype match
                    if (currentArch == archtype) {
                        //depending on if writing should be permitted different lock ways are required
                        if (write) {
                            //try to acquire writer lock
                            bool expected = false;
                            if (entry.writer.compare_exchange_strong(expected, true, std::memory_order_acq_rel)) {
                                //wait for all readers to finish
                                while (entry.readers.load(std::memory_order_acquire) > 0) 
                                {std::this_thread::yield();}
                                //finally, return a key
                                return LockKey(this, i, true);
                            }
                        } else {
                            //acquire read lock if no writer
                            if (!entry.writer.load(std::memory_order_acquire)) {
                                entry.readers.fetch_add(1, std::memory_order_acq_rel);
                                return LockKey(this, i, false);
                            }
                        }
                    }
                }
                //no slot available yet, so yield thread and wait a bit
                std::this_thread::yield();
            }
        }

    protected:

        /**
         * @brief store a single lock table entry
         */
        struct Entry {
            /**
             * @brief number of current readers
             */
            std::atomic_uint16_t readers{0};
            /**
             * @brief `true` if a writer is active, `false` otherwise
             */
            std::atomic_bool writer{false};
            /**
             * @brief store the claimed archtype
             */
            std::atomic<i_arch_t> archtype{0};
        };

        /**
         * @brief store all the lock slots
         */
        std::array<Entry, MaxThreads> m_entries;

    };

    //Column Views are implemented later
    template <typename T>
    class ColumnView;
    
    /**
     * @brief a simple dummy type that is aligned as much as possible
     */
    struct alignas(std::max_align_t) StorageByte {
        /**
         * @brief the value of the byte
         */
        uint8_t value;
    };

    /**
     * @brief a storage of a single component type
     */
    class ColumnBase {
    public:

        /**
         * @brief Construct a new Column Base
         */
        ColumnBase() = default;

        /**
         * @brief Destroy the Column Base
         * 
         * If a destruction function exists, call it
         */
        ~ColumnBase()
        {if (m_destructor){(*m_destructor)(this);}}

        /**
         * @brief Construct a new Column Base
         * 
         * @param other the column base to move from
         */
        ColumnBase(ColumnBase&& other) noexcept(true) {
            //move over all elements
            m_knownTypes = std::move(other.m_knownTypes);
            m_storage = other.m_storage;
            m_totalChunks = other.m_totalChunks;
            m_allocator = std::move(other.m_allocator);
            m_destructor = other.m_destructor;
            m_removeElement = other.m_removeElement;
            m_getter = other.m_getter;
            m_mover = other.m_mover;
            m_world = other.m_world;
            m_version = other.m_version;

            //invalidate the other
            other.m_storage = nullptr;
            other.m_totalChunks = 0;
        }

        /**
         * @brief move-assign operator for the column base class
         * 
         * @param other the column to move from
         * @return `ColumnBase&` a reference to this column after moving
         */
        ColumnBase& operator=(ColumnBase&& other) noexcept {
            //only move on mismatch
            if (this != &other) {
                //clean up existing storage
                if (m_storage) {
                    if (m_destructor)
                    {(*m_destructor)(this);}
                }

                //move metadata (like in constructor)
                m_knownTypes   = std::move(other.m_knownTypes);
                m_storage      = other.m_storage;
                m_totalChunks  = other.m_totalChunks;
                m_allocator    = std::move(other.m_allocator);
                m_destructor   = other.m_destructor;
                m_removeElement= other.m_removeElement;
                m_getter       = other.m_getter;
                m_mover        = other.m_mover;
                m_world        = other.m_world;
                m_version      = other.m_version;

                //invalidate other
                other.m_storage = nullptr;
                other.m_totalChunks = 0;
            }
            return *this;
        }

        /**
         * @brief create a new view on this column
         */
        template <typename T>
        ColumnView<T> createView()
        {return ColumnView<T>(this);}

        /**
         * @brief a function to remove an element from the column
         * 
         * @param archtypeId the ID of the archtype to remove the element from
         * @param archtypeLocalId the archtype local ID of the element
         */
        void removeElement(i_arch_t archtypeId, i_base_t archtypeLocalId)
        {(*m_removeElement)(archtypeId, archtypeLocalId, this);}

        /**
         * @brief Get the Version of the column
         * 
         * @return `uint16_t` the version number
         */
        inline uint16_t getVersion() const noexcept(true) 
        {return m_version;}

        /**
         * @brief Get the metadata for a specific archtype
         * 
         * @param archtypeId the ID of the archtype to quarry the metadata for
         * @return `const ArchtypeMeta&` a constant reference to the meta data
         */
        inline const ArchtypeMeta& getArchtypeMeta(i_arch_t archtypeId) const noexcept(true)
        {return m_knownTypes[archtypeId];}

        /**
         * @brief a function to access a specific element of the underlying column in a type-erased way
         * 
         * @param archtype the archtype to quarry for
         * @param local the local index of the element to quarry
         * @return `void*` a pointer to the element or `nullptr` if the element was not found
         */
        inline void* get(i_arch_t archtype, i_base_t local) noexcept(true)
        {return m_getter(archtype, local, this);}

        /**
         * @brief a function to move an element between archtypes
         * 
         * This adds to the new archtype using the move constructor and then removes from the old one
         * 
         * @param old the archtype to move away from
         * @param oldLocal the local entity ID of the archtype to move
         * @param target the archtype to move to
         */
        inline void move(i_arch_t old, i_base_t oldLocal, i_arch_t target) noexcept(true)
        {m_mover(old, oldLocal, target, this);}

    protected:
        
        //views are friends
        template <typename T>
        friend class ColumnView;
        //worlds are friends
        friend class World_t;

        /**
         * @brief Construct a new Column Base
         * 
         * @param destructor the function to use as a destructor
         * @param removeElement the function to use for remove an element
         * @param getter the function to get a specific element
         * @param mover the function to move a specific element from one archtype to another
         * @param world a pointer to the world that owns the column
         */
        ColumnBase(void (*destructor)(ColumnBase*), void (*removeElement)(i_arch_t, i_base_t, ColumnBase*), void* (*getter)(i_arch_t, i_base_t, ColumnBase*), void (*mover)(i_arch_t, i_base_t, i_arch_t, ColumnBase*), World_t* world)
         : m_destructor(destructor), m_removeElement(removeElement), m_getter(getter), m_mover(mover), m_world(world)
        {}
    
        /**
         * @brief store the archtype meta data
         * 
         * Indexed directly via Archtype ID (holes are ok, continues RAM is better here since the meta data is small)
         */
        std::vector<ArchtypeMeta, Allocator<ArchtypeMeta>> m_knownTypes;
        /**
         * @brief store the raw storage for the data
         */
        uint8_t* m_storage = nullptr;
        /**
         * @brief store the allocator of the column
         */
        Allocator<StorageByte> m_allocator;
        /**
         * @brief the total amount of chunks in the column
         */
        i_chunk_t m_totalChunks = 0;
        /**
         * @brief store the function to call for destruction
         */
        void (*m_destructor)(ColumnBase*) = nullptr;

        /**
         * @brief a function used to remove an element from the column
         */
        void (*m_removeElement)(i_arch_t, i_base_t, ColumnBase*) = nullptr;

        /**
         * @brief read a specific element in a type-erased way
         */
        void* (*m_getter)(i_arch_t, i_base_t, ColumnBase*) = nullptr;
        /**
         * @brief a function to move an element between archtype storages
         */
        void (*m_mover)(i_arch_t, i_base_t, i_arch_t, ColumnBase*) = nullptr;

        /**
         * @brief store the world that owns the column
         */
        World_t* m_world = nullptr;
        /**
         * @brief store the current incremental version
         */
        uint16_t m_version = 0;

        /**
         * @brief a mutex-free lock for thread safety
         */
        ColumnLock m_lock;

    };

    /**
     * @brief a typed wrapper around a base column. This does not add data members, it just provides a usable API
     * 
     * @tparam T the type of the component to store
     */
    template <typename T>
    class ColumnView {
    public:

        /**
         * @brief Create a Column
         * 
         * @param world a pointer to the world the column will live in
         * @return ColumnBase the base column that now exists
         */
        inline static ColumnBase createColumn(World_t* world) {
            //create a new column base in place
            return ColumnBase([](ColumnBase* column){
                //clean up all the elements
                Chunk<T>* chunks = reinterpret_cast<Chunk<T>*>(column->m_storage);
                for (size_t i = 0; i < column->m_totalChunks; ++i) {
                    chunks[i].~Chunk<T>();
                }

                //free
                //rebind the allocator
                using Alloc = typename std::allocator_traits<Allocator<StorageByte>>::template rebind_alloc<Chunk<T>>;
                Alloc alloc(column->m_allocator);
                //free storage
                alloc.deallocate(reinterpret_cast<Chunk<T>*>(column->m_storage), column->m_totalChunks);

                //increase the version (the storage changed)
                ++column->m_version;
            }, [](i_arch_t archtypeId, i_base_t archtypeLocalId, ColumnBase* column){
                //check if the archtype is safe
                if (archtypeId >= column->m_knownTypes.size()) 
                {return;}

                //get the archtype entry
                ArchtypeMeta& meta = column->m_knownTypes[archtypeId];
                //compute the chunk and local index
                i_chunk_t chunkIdx = archtypeLocalId >> CHUNK_CORRECTION_SHIFT;
                i_chunk_t localIdx = archtypeLocalId & CHUNK_INDEX_MASK;
                //compute the full, actual element size
                size_t fullSize = (meta.size * ChunkSize) - meta.freeCount;
                //check if the chunk is valid
                if (archtypeLocalId >= fullSize)
                {return;}
                
                //swap-remove the element
                Chunk<T>& chunk = reinterpret_cast<Chunk<T>*>(column->m_storage)[meta.offset + chunkIdx];
                Chunk<T>& end = reinterpret_cast<Chunk<T>*>(column->m_storage)[meta.offset + meta.size - (((meta.freeCount + (ChunkSize - 1))/ChunkSize))];
                i_chunk_t lastLocal = (ChunkSize - (meta.freeCount & CHUNK_INDEX_MASK))-1;
                chunk.elements[localIdx] = end.elements[lastLocal];
                //increase the amount of free elements
                ++meta.freeCount;
                //don't destroy the element - that would cause double free later
            }, [](i_arch_t archtypeId, i_base_t archtypeLocalId, ColumnBase* column) -> void* {
                //check if the archtype is safe
                if (archtypeId >= column->m_knownTypes.size()) 
                {return nullptr;}

                //get the archtype entry
                ArchtypeMeta& meta = column->m_knownTypes[archtypeId];
                //compute the chunk and local index
                i_chunk_t chunkIdx = archtypeLocalId >> CHUNK_CORRECTION_SHIFT;
                i_chunk_t localIdx = archtypeLocalId & CHUNK_INDEX_MASK;
                //compute the full, actual element size
                size_t fullSize = (meta.size * ChunkSize) - meta.freeCount;
                //check if the chunk is valid
                if (archtypeLocalId >= fullSize)
                {return nullptr;}
                
                //return a pointer to the element
                Chunk<T>& chunk = reinterpret_cast<Chunk<T>*>(column->m_storage)[meta.offset + chunkIdx];
                return reinterpret_cast<void*>(&chunk.elements[localIdx]);
            }, [](i_arch_t old, i_base_t oldLocal, i_arch_t target, ColumnBase* column) {
                //create a typed view
                ColumnView<T> view = column->template createView<T>();
                //add to the new archtype, make sure to store temporary copy
                T temp = *view.accessElement(old, oldLocal);
                view.addElement(target, std::move(temp));
                //remove the old element
                column->removeElement(old, oldLocal);
            }, world);
        }

        /**
         * @brief check if the column contains a specific archtype
         * 
         * @param archtId the archtype ID to check for
         * @return `true` if the archtype is known, `false` otherwise
         */
        inline bool contains(i_arch_t archtId)
        {return (archtId < m_column->m_knownTypes.size()) ? (m_column->m_knownTypes[archtId].size > 0) : false;}

        /**
         * @brief a function to add a type for an argument type
         * 
         * @tparam Args the types of the parameters to parse to the constructor
         * @param archtId the ID of the archtype to create the new element for
         * @param args the arguments to parse to the elements constructor
         */
        template <typename... Args>
        void addElement(i_arch_t archtId, Args&&... args) {
            //check if the archtype is safe
            if (archtId >= m_column->m_knownTypes.size()) {
                //resize so the archtype fits
                m_column->m_knownTypes.resize(archtId + 1);
            }

            //get the archtype entry
            ArchtypeMeta& meta = m_column->m_knownTypes[archtId];
            //check if a new chunk is required
            if (meta.freeCount == 0) {
                //check if the archtype is known
                if (meta.size == 0) {
                    //unknown, set the offset as the end
                    meta.offset = m_column->m_totalChunks;
                }
                //compte the insertion index
                i_chunk_t insertion = meta.offset + meta.size;
                
                //rebind the allocator
                using Alloc = typename std::allocator_traits<Allocator<StorageByte>>::template rebind_alloc<Chunk<T>>;
                Alloc alloc = m_column->m_allocator;
                //allocate a new buffer
                Chunk<T>* newBuff = alloc.allocate(m_column->m_totalChunks+1);
                Chunk<T>* oldBuff = reinterpret_cast<Chunk<T>*>(m_column->m_storage);
                if (oldBuff) {
                    if constexpr (std::is_trivially_copyable_v<T>) {
                        //all data can be copied in 2 memcpy calls
                        //everything below the insertion point
                        if (insertion != 0)
                        {memcpy(newBuff, oldBuff, insertion*sizeof(Chunk<T>));}
                        if (insertion != m_column->m_totalChunks-1)
                        {memcpy(newBuff + insertion + 1, oldBuff + insertion, (m_column->m_totalChunks-insertion)*sizeof(Chunk<T>));}

                        //chunk creation is required
                        new (newBuff + insertion) Chunk<T>();
                    } else {
                        //full pod-movement is required
                        //move the old data over, shifting if after insertion point
                        for (size_t i = 0; i < m_column->m_totalChunks+1; ++i) {
                            if (i == insertion)
                            {new (newBuff + i) Chunk<T>();}
                            else 
                            {new (newBuff + i) Chunk<T>(std::move(oldBuff[i - size_t(i >= insertion)]));}
                        }
                    }
                    //free the old buffer
                    for (size_t i = 0; i < m_column->m_totalChunks; ++i)
                    {oldBuff[i].~Chunk<T>();}
                    alloc.deallocate(oldBuff, m_column->m_totalChunks);
                } else {
                    //chunk creation is required
                    new (newBuff) Chunk<T>();
                }
                m_column->m_storage = reinterpret_cast<uint8_t*>(newBuff);

                //increase the version (the storage changed)
                ++m_column->m_version;
                //update the trailing offsets
                for (ArchtypeMeta& entry :  m_column->m_knownTypes) {
                    if (entry.offset > insertion)
                    {++entry.offset;}
                }

                //create the element in the first slot of the chunk
                newBuff[insertion].elements[0] = T(std::forward<Args>(args)...);

                //increase the chunk size of this archtype and the total chunk count
                ++meta.size;
                ++m_column->m_totalChunks;
                //increase the amount of free elements (remember: 1 was used up)
                meta.freeCount += (ChunkSize-1);
            } else {
                //compute the free chunk index
                i_chunk_t cIdx = (meta.offset + meta.size - (meta.freeCount>>CHUNK_CORRECTION_SHIFT)) - 1;
                i_chunk_t lIdx = ChunkSize - (meta.freeCount & CHUNK_INDEX_MASK);

                //store the element using copy assignment
                Chunk<T>& c = *(reinterpret_cast<Chunk<T>*>(m_column->m_storage) + cIdx);
                c.elements[lIdx] = T(std::forward<Args>(args)...);
                //used up 1 element
                --meta.freeCount;
            }
        }

        /**
         * @brief add a lot of elements at once
         * 
         * @tparam Args arguments to use for construction of ALL elements
         * @param count the amount of elements to create
         * @param archtId the ID of the archtype to create for
         * @param args the arguments to use for construction of ALL elements
         */
        template <typename... Args>
        void multiAddElement(i_base_t count, i_arch_t archtId, Args&&... args) {
            //check if the archtype is safe
            if (archtId >= m_column->m_knownTypes.size()) {
                //resize so the archtype fits
                m_column->m_knownTypes.resize(archtId + 1);
            }

            //get the archtype entry
            ArchtypeMeta& meta = m_column->m_knownTypes[archtId];

            //compute the amount of entries to create using the free list
            i_base_t freeInsert = std::min<i_base_t>(meta.freeCount, count);
            for (i_base_t i = 0; i < freeInsert; ++i) {
                //compute the free chunk index
                i_chunk_t cIdx = (meta.offset + meta.size - (meta.freeCount>>CHUNK_CORRECTION_SHIFT)) - 1;
                i_chunk_t lIdx = ChunkSize - (meta.freeCount & CHUNK_INDEX_MASK);

                //store the element using forward creation
                Chunk<T>& c = *(reinterpret_cast<Chunk<T>*>(m_column->m_storage) + cIdx);
                c.elements[lIdx] = T(std::forward<Args>(args)...);
                //used up 1 element
                --meta.freeCount;

                //created 1 element
                --count;
            }

            //if all elements where created, stop
            if (count == 0) {return;}

            //check if the archtype is known
            if (meta.size == 0) {
                //unknown, set the offset as the end
                meta.offset = m_column->m_totalChunks;
            }
            //compte the insertion index
            i_chunk_t insertion = meta.offset + meta.size;

            //compute the required chunk count
            i_chunk_t reqChunks = (count + (ChunkSize - 1)) / ChunkSize;
            
            //rebind the allocator
            using Alloc = typename std::allocator_traits<Allocator<StorageByte>>::template rebind_alloc<Chunk<T>>;
            Alloc alloc = m_column->m_allocator;
            //allocate a new buffer
            Chunk<T>* newBuff = alloc.allocate(m_column->m_totalChunks+reqChunks);
            Chunk<T>* oldBuff = reinterpret_cast<Chunk<T>*>(m_column->m_storage);
            if (oldBuff) {
                if constexpr (std::is_trivially_copyable_v<T>) {
                    //all data can be copied in 2 memcpy calls
                    //everything below the insertion point
                    if (insertion != 0)
                    {memcpy(newBuff, oldBuff, (insertion-1)*sizeof(Chunk<T>));}
                    if (insertion != m_column->m_totalChunks-reqChunks)
                    {memcpy(newBuff + insertion + reqChunks, oldBuff + insertion, (m_column->m_totalChunks-insertion)*sizeof(Chunk<T>));}

                    //chunk creation is required
                    for (size_t i = 0; i < count; ++i)
                    {new (newBuff + insertion + i) Chunk<T>();}
                } else {
                    //full pod-movement is required
                    //move the old data over, shifting if after insertion point
                    for (size_t i = 0; i < m_column->m_totalChunks+reqChunks; ++i) {
                        if ((i >= insertion) && (i < (insertion + reqChunks)))
                        {new (newBuff + i) Chunk<T>();}
                        else 
                        {new (newBuff + i) Chunk<T>(std::move(oldBuff[i - reqChunks*size_t(i >= insertion)]));}
                    }
                }
            } else {
                //chunk creation is required
                for (size_t i = 0; i < reqChunks; ++i)
                {new (newBuff + i) Chunk<T>();}
            }
            //free the old buffer
            for (size_t i = 0; i < m_column->m_totalChunks; ++i)
            {oldBuff[i].~Chunk<T>();}
            alloc.deallocate(oldBuff, m_column->m_totalChunks);
            m_column->m_storage = reinterpret_cast<uint8_t*>(newBuff);

            //increase the version (the storage changed)
            ++m_column->m_version;
            //update the trailing offsets
            for (ArchtypeMeta& entry :  m_column->m_knownTypes) {
                if (entry.offset > insertion)
                {entry.offset += reqChunks;}
            }

            //create the element in the first slot of the chunk
            i_chunk_t local = 0;
            for (size_t i = 0; i < count; ++i) {
                newBuff[insertion].elements[local] = T(std::forward<Args>(args)...);
                //step forward
                ++local;
                if (local >= ChunkSize) {
                    local = 0;
                    ++insertion;
                }
            }

            //increase the chunk size of this archtype and the total chunk count
            meta.size += reqChunks;
            m_column->m_totalChunks += reqChunks;
            //increase the amount of free elements
            meta.freeCount += (ChunkSize - (local+1));
        }

        /**
         * @brief access a specific element of an archtype
         * 
         * @param archtypeId the ID of the archtype to access the element for
         * @param archtypeLocalId the archtype local ID of the element to access
         * @return `T*` a pointer to the element or a `nullptr` if the element was not found
         */
        T* accessElement(i_arch_t archtypeId, i_base_t archtypeLocalId) {
            //check if the archtype is safe
            if (archtypeId >= m_column->m_knownTypes.size()) 
            {return nullptr;}

            //get the archtype entry
            ArchtypeMeta& meta = m_column->m_knownTypes[archtypeId];
            //compute the chunk and local index
            i_chunk_t chunkIdx = archtypeLocalId >> CHUNK_CORRECTION_SHIFT;
            i_chunk_t localIdx = archtypeLocalId & CHUNK_INDEX_MASK;
            //compute the full, actual element size
            size_t fullSize = (meta.size * ChunkSize) - meta.freeCount;
            //check if the chunk is valid
            if (archtypeLocalId >= fullSize)
            {return nullptr;}
            //the element is valid, return it
            return &reinterpret_cast<Chunk<T>*>(m_column->m_storage)[meta.offset + chunkIdx].elements[localIdx];
        }

    private:
    
        /**
         * @brief Construct a new Column View
         * 
         * @param col a pointer to the column to view on
         */
        ColumnView(ColumnBase* col)
         : m_column(col)
        {}
    
        //Base is friend
        friend class ColumnBase;
        //worlds are friends
        friend class World_t;

        /**
         * @brief the column to operate on
         */
        ColumnBase* m_column = nullptr;

    };

    //archtype views are defined later
    template <typename... Components>
    class ArchtypeView;

    /**
     * @brief the base structure that holds all data for an archtype
     */
    class ArchtypeBase {
    public:

        /**
         * @brief Construct a new Archtype Base
         */
        ArchtypeBase() = default;

        /**
         * @brief create a new view on this archtype
         */
        template <typename... Components>
        ArchtypeView<Components...> createView()
        {return ArchtypeView<Components...>(this);}
    
        /**
         * @brief a function to remove a 
         * 
         * @param localId the local ID of the element to remove
         */
        void removeElement(i_base_t localId) {
            //if the ID is invalid, just stop
            if (localId >= m_localToGlobal.size()) 
            {return;}

            //remove from all columns
            for (i_base_t entry = 0; entry < m_columns.size(); ++entry) {
                //quarry the column and remove the element
                m_world->m_columns[entry].removeElement(m_id, localId);
            }
            //swap with the back entry and then pop
            EntityT updated = m_localToGlobal.back();
            m_localToGlobal[localId] = updated;
            m_localToGlobal.pop_back();
            //update the world entity map
            m_world->m_entities[updated.getIndex()].localId = localId;
        }

        /**
         * @brief a type-erased function to read a component
         * 
         * @tparam Component the type of the component to quarry
         * @param localId the local ID of the local entity to quarry
         * @return `Component*` a pointer to the data or a `nullptr` if the element was not found
         */
        template <typename Component>
        Component* get(i_base_t localId) {
            if (m_world->typeId<Component>() >= m_world->m_columns.size()) {return nullptr;}
            return reinterpret_cast<Component*>(m_world->m_columns[m_world->typeId<Component>()].get(m_id, localId));
        }
    
    protected:

        //views are friends
        template <typename... Components>
        friend class ArchtypeView;
        //world is friend
        friend class World_t;

        /**
         * @brief Construct a new Archtype Base
         * 
         * @param id the ID of the archtype
         * @param world a pointer to the world that owns the archtype
         */
        ArchtypeBase(i_arch_t id, World_t* world)
         : m_id(id), m_world(world)
        {}

        /**
         * @brief Construct a new Archtype Base
         * 
         * @param id the ID of the archtype
         * @param begin the begin iterator for the columns
         * @param end the end iterator for the columns
         * @param world a pointer to the world that owns the archtype
         */
        ArchtypeBase(i_arch_t id, std::vector<i_col_t, Allocator<i_col_t>>::iterator begin, std::vector<i_col_t, Allocator<i_col_t>>::iterator end, World_t* world)
         : m_columns(begin, end), m_id(id), m_world(world)
        {}

        /**
         * @brief store the columns used by the archtype
         */
        std::vector<i_col_t, Allocator<i_col_t>> m_columns;
        /**
         * @brief store a mapping from local entity IDs to global entity IDs
         */
        std::vector<EntityT, Allocator<EntityT>> m_localToGlobal;
        /**
         * @brief store the incremental archtype ID of this archtype
         */
        i_arch_t m_id = 0;
        /**
         * @brief store a pointer to the world that owns the archtype
         */
        World_t* m_world = nullptr;
    };

    /**
     * @brief a typed view for an archtype base
     * 
     * @tparam Components the components of the archtype
     */
    template <typename... Components>
    class ArchtypeView {
    public:

        /**
         * @brief create a new archtype
         * 
         * @param world the world that owns the new archtype
         * @return `ArchtypeBase` the new archtype
         */
        static ArchtypeBase create(World_t* world) {
            //create the actual archtype
            ArchtypeBase b(world->archtypeId<Components...>(), world);
            //fill in the columns
            size_t i = 0;
            b.m_columns.reserve(sizeof...(Components));
            (__initialize<Components>(b, world), ...);
            //return the new archtype
            return b;
        }

        /**
         * @brief allocate a new entity that uses this archtype
         * 
         * @tparam Args the argument types for the entity
         * @param ent the global entity name of the new archtype entry
         * @param args the arguments for the entity's elements (1 optional argument per component)
         * @return `i_base_t` the local entity ID
         */
        template <typename... Args>
        requires (sizeof...(Args) <= sizeof...(Components))
        i_base_t allocateEntity(const EntityT& ent, Args&&... args) {
            //create the local entity ID
            i_base_t local = m_arch->m_localToGlobal.size();
            m_arch->m_localToGlobal.push_back(ent);
            //create a tuple for the arguments
            auto tuple = std::make_tuple(std::forward<Args>(args) ...);
            //add the entity to the columns
            (__addElement<Components, Args...>(tuple), ...);
            //return the local ID
            return local;
        }

        /**
         * @brief a function to create a lot of entities
         * 
         * @tparam Args the types of the arguments to create
         * @param entities the world's entity names to create for
         * @param locals a list of local entity IDs to assign to
         * @param args the arguments to parse to the entity allocation
         */
        template <typename... Args>
        requires (sizeof...(Args) <= sizeof...(Components))
        void bulkAllocateEntity(const std::vector<EntityT>& entities, std::vector<i_base_t>& locals, Args&&... args) {
            //bulk-create new local to global mappings for all inputted entities
            locals.reserve(entities.size());
            m_arch->m_localToGlobal.reserve(m_arch->m_localToGlobal.size() + entities.size());
            for (const EntityT& ent : entities) {
                locals.push_back(m_arch->m_localToGlobal.size());
                m_arch->m_localToGlobal.push_back(ent);
            }
            //create a tuple for the arguments
            auto tuple = std::make_tuple(std::forward<Args>(args) ...);
            //add the entity to the columns
            (__multiAddElement<Components, Args...>(entities.size(), tuple), ...);
        }

        /**
         * @brief access a specific component of an entity
         * 
         * @tparam Component the component type to access
         * @param localId the local ID of the element to access
         * @return `Component*` a pointer to the stored data, or `nullptr` if the data was not found
         */
        template <typename Component>
        requires (util::contains_type_v<Component, Components...>)
        Component* accessComponent(i_base_t localId) {
            //if the ID is invalid, just stop
            if (localId > m_arch->m_localToGlobal.size())
            {return nullptr;}

            //access the correct column and fetch the data
            return ColumnView<Component>(&m_arch->m_world->m_columns[m_arch->m_world->template typeId<Component>()]).accessElement(m_arch->m_id, localId);
        }

        /**
         * @brief access multiple components at once
         * 
         * @tparam Comps the components to access
         * @param localId the local ID of the element to access
         * @return `std::tuple<Comps*...>` a tuple containing a pointer to all data members, or `nullptr` if they where not found
         */
        template <typename... Comps>
        requires ((util::contains_type_v<Comps, Components...> && ...) && (sizeof...(Comps) > 1))
        std::tuple<Comps*...> accessComponent(i_base_t localId) {
            //create the tuple to return
            std::tuple<Comps*...> ret;
            //iterate over all requested components and access that single component
            ((std::get<Comps*>(ret) = accessComponent<Comps>(localId)), ...);
            //return the tuple
            return ret;
        }

    protected:
    
        //base is friend
        friend class ArchtypeBase;
    
        /**
         * @brief a helper function to add a single component, either using the default constructor or the forwarding of a type
         * 
         * @tparam Component the component to initialize
         * @tparam Args the arguments that were passed
         * @param args a tuple containing the forwarded arguments
         */
        template <typename Component, typename... Args>
        void __addElement(auto& args) {
            //identify the type
            i_col_t tid = m_arch->m_world->template typeId<Component>();

            //check if the components exists in the arguments
            if constexpr (util::contains_type_v<Component, Args...>) {
                //add the element using the forward constructor
                ColumnView<Component>(&m_arch->m_world->m_columns[tid]).template addElement<Component>(m_arch->m_id, std::forward<Component>(std::get<Component>(args)));
            } else {
                //add the element using the default constructor
                ColumnView<Component>(&m_arch->m_world->m_columns[tid]).addElement(m_arch->m_id);
            }
        }

        /**
         * @brief a helper function to add a single component for multiple entities, either using the default constructor or the forwarding of a type
         * 
         * @tparam Component the component to initialize
         * @tparam Args the arguments that were passed
         * @param count the amount of entities to add
         * @param args a tuple containing the forwarded arguments
         */
        template <typename Component, typename... Args>
        void __multiAddElement(i_base_t count, auto& args) {
            //identify the type
            i_col_t tid = m_arch->m_world->template typeId<Component>();

            //check if the components exists in the arguments
            if constexpr (util::contains_type_v<Component, Args...>) {
                //add the element using the forward constructor
                ColumnView<Component>(&m_arch->m_world->m_columns[tid]).template multiAddElement<Component>(count, m_arch->m_id, std::forward<Component>(std::get<Component>(args)));
            } else {
                //add the element using the default constructor
                ColumnView<Component>(&m_arch->m_world->m_columns[tid]).multiAddElement(count, m_arch->m_id);
            }
        }

        /**
         * @brief initialize a single column entry for the archtype base
         * 
         * @tparam Component the component to initialize
         * @param base the base to initialize
         * @param world the world that owns everything
         */
        template <typename Component>
        static void __initialize(ArchtypeBase& base, World_t* world) {
            //check if the column exists
            i_col_t tid = world->typeId<Component>();
            if (world->m_columns.size() <= tid) {
                //sanity check in debug if the column id count is saturated
                #ifdef TINY_ECS_DEBUG
                if (world->m_columns.size() >= MAX_COLUMN_COUNT) {
                    throw Exception(std::string("Ran out of column IDs: Can only create ") + std::to_string(MAX_COLUMN_COUNT) + " columns, but was tasked to create more");
                    return;
                }
                #endif

                world->m_columns.resize(tid + 1);
            }
            if (world->m_columns[tid].m_world == nullptr) {
                world->m_columns[tid] = ColumnView<Component>::createColumn(world);
            }
            //add the new column
            base.m_columns.emplace_back(tid);
        }

        /**
         * @brief Create a new archtype view
         * 
         * @param base a pointer to the base type to view
         */
        ArchtypeView(ArchtypeBase* base)
         : m_arch(base)
        {}
        
        /**
         * @brief store a pointer to the archtype base to view
         */
        ArchtypeBase* m_arch = nullptr;

        /**
         * @brief a table used to resolve the local entity mapping
         */
        std::vector<EntityT> m_localToGlobal;
    
    };

    /**
     * @brief a helper functor to hash a vector
     */
    struct VectorHash {
        /**
         * @brief the functor caller
         * 
         * @tparam T the type of the vector elements
         * @param v the full vector
         * @return `std::size_t` the computed hash
         */
        template <typename T>
        std::size_t operator()(const std::vector<T>& v) const {
            //combine element-wise hashes of all elements
            std::size_t h = 0;
            for (const auto& elem : v) {
                h ^= std::hash<T>{}(elem) + 0x9e3779b9 + (h << 6) + (h >> 2);
            }
            //return the finalized hash
            return h;
        }
    };

    /**
     * @brief a function that stores the type counter
     * 
     * @return `i_col_t&` a reference to the counter
     */
    static i_col_t& typeCounter() noexcept {
        //initialize the counter to 0 and return it
        static i_col_t counter = 0;
        return counter;
    }

    /**
     * @brief 
     * 
     * @tparam T the type to get the type ID for 
     * @return `i_col_t` the type ID of the type
     */
    template<typename T>
    static i_col_t typeId() noexcept {
        //increment the type ID and store it in static storage
        static const i_col_t id = typeCounter()++;
        //return the now cached type ID
        return id;
    }

    /**
     * @brief compute a unique ID for an archtype
     * 
     * @param tids the type IDs of the columns for the archtype
     * @return `i_arch_t` the archtype ID 
     */
    i_arch_t archtypeId(std::vector<i_col_t>& tids) {
        //compute the sorted type ID list
        std::sort(tids.begin(), tids.end());

        //check if the type is known
        auto it = m_archtypeMap.find(tids);
        if (it != m_archtypeMap.end()) {
            return it->second;
        }

        //not found create new archetype
        i_arch_t newId = m_archtypeResolution.size();
        m_archtypeResolution.push_back(tids);
        m_archtypeMap[tids] = newId;
        return newId;
    }

    /**
     * @brief compute a unique ID for an archtype
     * 
     * @tparam Ts the unsorted type list
     * @return `i_arch_t` the fully unique ID of the archtype 
     */
    template <typename... Ts>
    i_arch_t archtypeId() noexcept {
        //compute the sorted type ID list
        std::vector<i_col_t> tids = {typeId<Ts>()...};
        std::sort(tids.begin(), tids.end());

        //check if the type is known
        auto it = m_archtypeMap.find(tids);
        if (it != m_archtypeMap.end()) {
            return it->second;
        }

        //not found, create new archetype
        i_arch_t newId = m_archtypeResolution.size();
        m_archtypeResolution.emplace_back(tids);
        m_archtypeMap[tids] = newId;
        return newId;
    }

    /**
     * @brief a helper function used in the each system that 
     * 
     * @tparam Fn the type of the function to call
     * @tparam Components the components to parse
     * @param fn the function to call
     * @param arch the archtype to call from
     * @param local the local ID to call
     * @param cols a tuple that contains the components to parse
     */
    template<typename Fn, typename... Components>
    static inline void invokeEach(Fn& fn, ArchtypeBase& arch, i_base_t local, const std::tuple<Components*...>& cols) {
        //check for entity and component reference layout
        if constexpr (std::invocable<Fn, EntityT, Components&...>) {
            //invoke with that layout
            fn(arch.m_localToGlobal[local], (*std::get<Components*>(cols))...);
        //check for just components
        } else if constexpr (std::invocable<Fn, Components&...>) {
            //just call with the components
            fn((*std::get<Components*>(cols))...);
        } else {
            //everything else is an error
            static_assert(util::always_false<Fn>, "Invalid iteration callback");
        }
    }

    /**
     * @brief store all the columns
     * 
     * All columns are indexed via the type id of the column
     */
    std::vector<ColumnBase, Allocator<ColumnBase>> m_columns;
    /**
     * @brief store all different archtypes
     * 
     * All Archtypes are indexed via Archtype IDs
     */
    std::vector<ArchtypeBase, Allocator<ArchtypeBase>> m_arches;
    /**
     * @brief store mappings from global entity IDs to internal IDs
     */
    std::vector<EntityMeta, Allocator<EntityMeta>> m_entities;
    /**
     * @brief store the entity IDs of free elements
     */
    std::vector<size_t> m_freeList;

    /**
     * @brief a mapping that resolves archtype IDs into `std::vector<i_col_t>` - a list of type IDs for all subtypes
     */
    std::vector<std::vector<i_col_t, Allocator<i_col_t>>, Allocator<std::vector<i_col_t, Allocator<i_col_t>>>> m_archtypeResolution = {};
    /**
     * @brief a map to lookup archtype IDs from type lists
     */
    std::unordered_map<
        std::vector<i_col_t, Allocator<i_col_t>>,
        i_arch_t,
        VectorHash,
        std::equal_to<std::vector<i_col_t, Allocator<i_col_t>>>, 
        Allocator<std::pair<const std::vector<i_col_t, Allocator<i_col_t>>, i_arch_t>>
    > m_archtypeMap;

};

/**
 * @brief a `World_t` but with default values
 */
using World = World_t<1024, 32, Entity, uint16_t, uint16_t, uint32_t, std::allocator>;

}

}

}

#endif
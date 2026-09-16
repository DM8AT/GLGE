/**
 * @file TinyJobs.h
 * @author DM8AT
 * @brief A library that allows for easy execution of functions on a different thread
 * 
 * This library is designed to be a simple to integrate, simple to use multi-threading utility. It allows for execution of simple tasks, as well as 
 * dependent task graphs (DAGs) in a multi-threaded manner. For more information, see [LINK TO GITHUB]
 * 
 * @version 1.0
 * @date 2026-02-21
 * 
 * @copyright Copyright (c) 2026
 * 
 */

/**
 * @defgroup Jobs Jobs
 * 
 * @brief Everything related to the job system. 
 * 
 * This group contains all structures directly related to the job system. The structures may also appear somewhere else too. 
 */
/**
 * @defgroup Tasks Tasks
 * 
 * @brief Everything related to the task system. 
 * 
 * This group contains all structures directly related to the task system. The structures may also appear somewhere else too. 
 */
/**
 * @defgroup Scheduler Scheduler
 * 
 * @brief Everything related to the scheduling system. 
 * 
 * This group contains all structures directly related to the scheduling system. The structures may also appear somewhere else too.
 */
/**
 * @defgroup Utilities Utilities
 * 
 * @brief Every structure that is an internal utility in the library
 * 
 * 
 * This group contains all structures that are mainly intended as internal utilities but may also be used outside of the library. 
 */

//header guard
#ifndef _DM8AT_TINY_JOBS_
#define _DM8AT_TINY_JOBS_

//for dynamically sized containers
#include <vector>
//for good integer types
#include <cstdint>
//for lists of types / arguments
#include <tuple>
//for type trait information
#include <type_traits>
//for default concepts
#include <concepts>

//for threads
#include <thread>
//add arrays
#include <array>
//for thread-safe stuff
#include <mutex>
//for notifying threads
#include <condition_variable>
//for a queue
#include <queue>
//for math
#include <cmath>
//for memory operations
#include <cstring>
//standard defines
#include <cstddef>

//includes for thread pinning on different Oses
#if defined(__linux__)
#include <pthread.h>
#include <sched.h>
#elif defined(_WIN32)
//exclude min / max defines
#ifndef NOMINMAX
    #define NOMINMAX
#endif
#include <windows.h>
#elif defined(__APPLE__)
#include <pthread.h>
#include <mach/mach.h>
#include <mach/thread_policy.h>
#endif

#ifndef TINY_JOBS_NO_FIBERS
//required for fibers
#include <boost/fiber/all.hpp>
//for faster hashmaps
#include <boost/unordered/unordered_flat_map.hpp>
#endif

#ifndef TINY_JOBS_NO_LOCKFREE
//required for fast multi-producers multi-consumer queues
#include <concurrentqueue.h>
#endif

//check for debug mode
#if defined(DEBUG) || defined(_DEBUG) || defined(__DEBUG) || defined(JOBS_DEBUG) || defined(_JOBS_DEBUG) || defined(__JOBS_DEBUG) || defined(_TINY_JOBS_DEBUG) || defined(__TINY_JOBS_DEBUG)
#define TINY_JOBS_DEBUG 1
#endif

//GLGE namespace wrapper (added because this is part of GLGE)
namespace GLGE {

/**
 * @brief use the main namespace of all tiny libraries
 */
namespace Tiny {

/**
 * @brief the namespace that contains the whole job system
 */
namespace Jobs {

/**
 * @brief store all utilities for the library
 * 
 * @ingroup Utilities
 */
namespace util {

/**
 * @brief a fully compile time string
 * 
 * @tparam N the length of the string in characters
 * 
 * @ingroup Utilities
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
 * 
 * @ingroup Utilities
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
 * 
 * @ingroup Utilities
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
 * 
 * @ingroup Utilities
 */
template<typename T>
consteval uint64_t type_hash() {
    //get the type name and return the hash
    return hash_string(raw_type_name<T>());
}

}

/**
 * @brief a custom exception class to make exception capturing possible
 * 
 * @ingroup Utilities
 */
class Exception : public std::exception {
public:

    /**
     * @brief Construct a new Exception
     */
    Exception(const std::string& msg)
     : m_msg(std::string("[TINY JOBS: ERROR] " + msg))
    {}

    /**
     * @brief get the string of the exception
     * 
     * @return `const char*` the message of the exception
     */
    inline const char* what() const noexcept(true) override
    {return m_msg.c_str();}

protected:

    /**
     * @brief store the message
     */
    std::string m_msg;

};

/**
 * @brief store a value that may be filled in the future
 * 
 * @note This class can be moved and copied. 
 * 
 * @ingroup Tasks
 */
class Future {
public:

    /**
     * @brief store the size of the small buffer optimization for a future
     * 
     * Choosen so that the size of a future is 64 on x86-64 machines
     */
    inline static constexpr size_t SMALL_BUFFER_SIZE = 24;

    /**
     * @brief Construct a new Future
     */
    Future() = default;

    /**
     * @brief move constructor
     * 
     * @tparam T the type of the element to move
     */
    template <typename T>
    requires (alignof(T) <= alignof(std::max_align_t))
    Future(T&& value) 
    {(void) assign<T>(std::forward<T>(value));}

    /**
     * @brief copy constructor
     * 
     * @tparam T the type of the element to copy
     */
    template <typename T>
    requires (alignof(T) <= alignof(std::max_align_t))
    Future(const T& value) 
    {(void) assign<T>(value);}

    /**
     * @brief Destroy the Future
     */
    ~Future() 
    {if (m_destroy) {m_destroy(this);}}

    /**
     * @brief move assignment
     * 
     * @tparam T the type of the element to move
     */
    template <typename T>
    requires (alignof(T) <= alignof(std::max_align_t))
    Future& operator=(T&& value) 
    {return assign<T>(std::forward<T>(value));}

    /**
     * @brief copy assignment
     * 
     * @tparam T the type of the element to copy
     */
    template <typename T>
    requires (alignof(T) <= alignof(std::max_align_t))
    Future& operator=(const T& value) 
    {return assign<T>(value);}

    /**
     * @brief a function to check if the future holds a valid value
     * 
     * @return `true` if it is valid, `false` otherwise
     */
    bool valid() const noexcept(true)
    {return m_ready.load(std::memory_order_acquire);}

    /**
     * @brief get the value from a future
     * 
     * @throws `Tiny::Jobs::Exception` An exception is thrown in debug mode if a type mismatch is detected or the future is invalid. 
     * 
     * @tparam T the type of the value to get
     * @return `T&` a reference to the value
     */
    template <typename T>
    requires (alignof(T) <= alignof(std::max_align_t))
    inline T& get() noexcept(false) {
        //in debug do some sanity checks
        #ifdef TINY_JOBS_DEBUG
        if (!valid()) {throw Exception("Tried to get a type from an invalid future");}
        if (m_type != util::type_hash<T>()) {throw Exception("Tried to get a type from a future, but reading with a different type");}
        #endif
        //return a reference to the storage
        if constexpr (sizeof(T) > SMALL_BUFFER_SIZE)
        {return *reinterpret_cast<T*>(m_data.large_ptr);}
        else
        {return *reinterpret_cast<T*>(m_data.small_buff);}
    }

    /**
     * @brief get the value from a future
     * 
     * @throws Tiny::Jobs::Exception An exception is thrown in debug mode if a type mismatch is detected or the future is invalid. 
     * 
     * @tparam T the type of the value to get
     * @return `const T&` constant a reference to the value
     */
    template <typename T>
    requires (alignof(T) <= alignof(std::max_align_t))
    inline const T& get() const noexcept(false) {
        //in debug do some sanity checks
        #ifdef TINY_JOBS_DEBUG
        if (!valid()) {throw Exception("Tried to get a type from an invalid future");}
        if (m_type != util::type_hash<T>()) {throw Exception("Tried to get a type from a future, but reading with a different type");}
        #endif
        //return a reference to the storage
        if constexpr (sizeof(T) > SMALL_BUFFER_SIZE)
        {return *reinterpret_cast<const T*>(m_data.large_ptr);}
        else
        {return *reinterpret_cast<const T*>(m_data.small_buff);}
    }

    /**
     * @brief a cast operator
     * 
     * @throws Tiny::Jobs::Exception An exception is thrown in debug mode if a type mismatch is detected or the future is invalid. 
     * 
     * @tparam T the type to cast to
     * @return T the value of the cast
     */
    template <typename T>
    requires (alignof(T) <= alignof(std::max_align_t))
    inline operator T() const
    {return get<T>();}

    /**
     * @brief a cast operator
     * 
     * @tparam T the type to cast to
     * @return `T&` a reference to the value
     */
    template <typename T>
    requires (alignof(T) <= alignof(std::max_align_t))
    inline operator T&()
    {return get<T>();}

    /**
     * @brief a cast operator
     * 
     * @throws Tiny::Jobs::Exception An exception is thrown in debug mode if a type mismatch is detected or the future is invalid. 
     * 
     * @tparam T the type to cast to
     * @return `const T&` a constant reference to the value
     */
    template <typename T>
    requires (alignof(T) <= alignof(std::max_align_t))
    inline operator T&() const
    {return get<T>();}

    /**
     * @brief move assignment from other type
     * 
     * @tparam T the type to move from
     */
    template <typename T>
    requires (alignof(T) <= alignof(std::max_align_t))
    Future& assign(T&& value) {
        //special operation for future type, preventing a future holding a future
        if constexpr (std::is_same_v<std::decay_t<T>, Future>) {
            //don't move to self
            if (this != &value) {
                //use the move constructor
                if (value.m_move) {value.m_move(this, &value);}
            }
        } else {
            //initialize the value storage
            if constexpr (sizeof(T) > SMALL_BUFFER_SIZE) {
                delete reinterpret_cast<T*>(m_data.large_ptr);
                m_data.large_ptr =  reinterpret_cast<void*>(new T(value));
            } 
            else {
                reinterpret_cast<T*>(m_data.small_buff.buff)->~T();
                new (m_data.small_buff.buff) T(std::forward<T>(value));
            }

            //setup the lambdas
            __createLambdas<T>(*this);

            //store the type
            constexpr uint64_t hash = util::type_hash<T>();
            m_type = hash;
            //finally ready
            m_ready.store(true, std::memory_order_release);
        }
        //return reference to self
        return *this;
    }

    /**
     * @brief copy assignment from other type
     * 
     * @tparam T the type to copy from
     */
    template <typename T>
    requires (alignof(T) <= alignof(std::max_align_t))
    Future& assign(const T& value) {
        //special operation for future type, preventing a future holding a future
        if constexpr (std::is_same_v<std::decay_t<T>, Future>) {
            //don't copy to self
            if (this != &value) {
                //use the copy constructor
                if (value.m_copy) {value.m_copy(this, &value);}
            }
        } else {
            //initialize the value storage
            if constexpr (sizeof(T) > SMALL_BUFFER_SIZE) {
                delete reinterpret_cast<T*>(m_data.large_ptr);
                m_data.large_ptr =  reinterpret_cast<void*>(new T(value));
            } 
            else {
                reinterpret_cast<T*>(m_data.small_buff)->~T();
                new (m_data.small_buff) T(value);
            }

            //setup the lambdas
            __createLambdas<T>(*this);

            //store the type
            m_type = util::type_hash<T>();
            //finally ready
            m_ready.store(true, std::memory_order_release);
        }
        //return reference to self
        return *this;
    }

    /**
     * @brief wait for the future to become valid
     * 
     * This just repeatedly gives up thread priority until the future becomes valid
     */
    inline void await() const noexcept(true)
    {while (!valid()) {std::this_thread::sleep_for(std::chrono::nanoseconds(1));}}

    /**
     * @brief reset the future to the default (invalid) state
     */
    void reset() noexcept(true)
    {if (m_destroy) {m_destroy(this);}}

protected:

    /**
     * @brief a function to initialize the lambdas for a future
     * 
     * @tparam T the type to initialize for
     * @param f the future to initialize
     */
    template <typename T>
    requires (alignof(T) <= alignof(std::max_align_t))
    static void __createLambdas(Future& f) {
        //store the destructor
        f.m_destroy = [](Future* f) {
            //no longer ready
            f->m_ready.store(false, std::memory_order_release);
            //switch between small and large buffer
            if constexpr (sizeof(T) > SMALL_BUFFER_SIZE) {
                //use large buffer
                //cast to T* and use delete function
                delete reinterpret_cast<T*>(f->m_data.large_ptr);
            } else {
                //use small buffer
                reinterpret_cast<T*>(f->m_data.small_buff.buff)->~T();
            }
            //reset to default state
            f->m_destroy = nullptr;
            f->m_move = nullptr;
            f->m_copy = nullptr;
            f->m_data.large_ptr = nullptr;
            f->m_type = 0;
        };
        //store the copy constructor
        f.m_copy = [](Future* to, const Future* from) {
            //clean up to
            if (to->m_destroy) {to->m_destroy(to);}

            //initialize the storage for the to
            if constexpr (sizeof(T) > SMALL_BUFFER_SIZE) 
            {to->m_data.large_ptr =  reinterpret_cast<void*>(new T(*reinterpret_cast<const T*>(from->m_data.large_ptr)));} 
            else 
            {new (to->m_data.small_buff.buff) T(*reinterpret_cast<const T*>(from->m_data.small_buff.buff));}

            //initialize the lambdas
            __createLambdas<T>(*to);

            //store the type
            to->m_type = util::type_hash<T>();
            //finally ready
            to->m_ready.store(true, std::memory_order_release);
        };
        //store the move constructor
        f.m_move = [](Future* to, Future* from) {
            //clean up to
            if (to->m_destroy) {to->m_destroy(to);}

            //initialize the storage for the to (for large objects just move the pointer)
            if constexpr (sizeof(T) > SMALL_BUFFER_SIZE) {
                to->m_data.large_ptr =  from->m_data.large_ptr;
                from->m_data.large_ptr = nullptr;
            }
            else 
            {new (to->m_data.small_buff.buff) T(std::move(*reinterpret_cast<T*>(from->m_data.small_buff.buff)));}

            //clean up from
            from->m_destroy(from);

            //initialize the lambdas
            __createLambdas<T>(*to);

            //store the type
            to->m_type = util::type_hash<T>();
            //finally ready
            to->m_ready.store(true, std::memory_order_release);
        };
    }

    /**
     * @brief required because MSVC complained about this in a union
     */
    struct SmallStorage {
        /**
         * @brief store the small buffer optimization data
         * 
         * Holds the invocable + captures + args
         */
        uint8_t buff[SMALL_BUFFER_SIZE];
    };

    /**
     * @brief store the data of the task
     */
    union Data {
        /**
         * @brief store the small buffer optimization data
         * 
         * Holds the invocable + captures + args
         */
        SmallStorage small_buff;
        /**
         * @brief fallback to heap buffer if the data is too large for the small buffer
         */
        void* large_ptr = nullptr;
    };

    /**
     * @brief store the data of the task
     */
    alignas (alignof(std::max_align_t)) Data m_data;

    /**
     * @brief store the function to clean up the future
     */
    void (*m_destroy)(Future*) = nullptr;
    /**
     * @brief store the function to move a future to another future
     */
    void (*m_move)(Future*, Future*) = nullptr;
    /**
     * @brief store the function to copy a future to another future
     */
    void (*m_copy)(Future*, const Future*) = nullptr;
    /**
     * @brief store a type tag to make the future type safe
     */
    uint64_t m_type = 0;
    /**
     * @brief store a single ready byte flag on a new cache line (to avoid false sharing)
     */
    alignas (8) std::atomic_bool m_ready{false};

};

/**
 * @brief a type-erased, immovable wrapper for a function that stores the result of the function in a type-erased future
 * 
 * @note This class can be neither copied nor moved. 
 * 
 * @ingroup Tasks
 */
class Task {
public:

    /**
     * @brief The size of the small buffer optimization of the task class
     * 
     * Choosen so that the size of a Task instance is 256 bytes (4 Cachelines) on x86-64 architecture
     */
    inline static constexpr size_t SMALL_BUFFER_SIZE = 176;

    /**
     * @brief Construct a new Task
     */
    Task() = default;

    /**
     * @brief Construct a new Task
     * 
     * @tparam Func the function that the task should execute
     * @tparam Args the arguments to parse to the functions
     * @param fn the function that the task should execute
     * @param args the arguments to parse to the functions
     */
    template <typename Func, typename... Args>
    Task(Func&& fn, Args&&... args) {
        //get the clean function type to store
        using CleanF = std::decay_t<Func>;
        //get the closure pack type
        using Pack = ClosurePack<CleanF, Args...>;
        //determine if the task is too large for the small buffer
        static constexpr bool large = sizeof(Pack) > SMALL_BUFFER_SIZE;

        //store the package
        if constexpr (large)
        {m_data.large_ptr = reinterpret_cast<void*>(new ClosurePack<CleanF, Args...>{std::forward<Func>(fn), std::tuple<Args...>(std::forward<Args>(args)...)});}
        else
        {new (m_data.small_buff.buff) ClosurePack<CleanF, Args...>(std::forward<Func>(fn), std::tuple<Args...>(std::forward<Args>(args)...));}

        //create the lambda
        __createLambdas<Func, Args...>(*this);
    }

    //immovable and cannot be copied
    Task(const Task&) = delete;
    Task(Task&&) = delete;
    Task& operator=(const Task&) = delete;
    Task& operator=(Task&&) = delete;

    /**
     * @brief Destroy the Task
     */
    ~Task()
    {if (m_destroy) {m_destroy(this);}}

    /**
     * @brief invoke the underlying function
     */
    void invoke() 
    {if (m_invoke) {m_invoke(this);}}

    /**
     * @brief access the result of the task
     * 
     * @return `Future&` a reference to the future that holds the result of the task
     */
    inline Future& result() noexcept(true)
    {return m_return;}

    /**
     * @brief give away execution priority for another task to run
     * 
     * @note if fibers are disabled, this will yield the OS thread. If fibers are enabled, this will yield the fiber. 
     * 
     * This should be done e.g. while waiting for file I/O or stalling during other operation
     */
    inline static void yield() {
        #ifndef TINY_JOBS_NO_FIBERS
        boost::this_fiber::yield();
        #else
        std::this_thread::yield();
        #endif
    }

    /**
     * @brief a function to sleep the task for some time
     * 
     * @note if fibers are disabled, this will sleep the OS thread. If fibers are enabled, this will sleep the fiber. 
     * 
     * @tparam Rep the arithmetic type to represent the duration
     * @tparam Period represents the tick period
     * @param duration the duration to sleep
     */
    template<typename Rep, typename Period>
    inline static void sleep(const std::chrono::duration<Rep, Period>& duration) {
        #ifndef TINY_JOBS_NO_FIBERS
        boost::this_fiber::sleep_for(duration);
        #else
        std::this_thread::sleep_for(duration);
        #endif
    }

protected:

    /**
     * @brief a function to create the custom function table for a task
     * 
     * @tparam Func the function that the task should execute
     * @tparam Args the arguments to parse to the functions
     * @param task the task to create the function table for
     */
    template <typename Func, typename... Args>
    static void __createLambdas(Task& task) {
        //create the destructor
        task.m_destroy = [](Task* task) {
            //get the closure pack type
            using CleanF = std::decay_t<Func>;
            using Pack = ClosurePack<CleanF, Args...>;
            //determine if the task is too large for the small buffer
            static constexpr bool large = sizeof(Pack) > SMALL_BUFFER_SIZE;

            //determine if the large or small slot is used
            if constexpr (large) 
            {delete reinterpret_cast<Pack*>(task->m_data.large_ptr);} 
            else 
            {reinterpret_cast<Pack*>(task->m_data.small_buff.buff)->~Pack();}

            //reset to default
            task->m_data.large_ptr = nullptr;
            task->m_destroy = nullptr;
            task->m_invoke = nullptr;
            task->m_return.reset();
        };
        //create the invoker
        task.m_invoke = [](Task* task) {
            //get the closure pack type
            using CleanF = std::decay_t<Func>;
            using Pack = ClosurePack<CleanF, Args...>;
            //determine if the task is too large for the small buffer
            static constexpr bool large = sizeof(Pack) > SMALL_BUFFER_SIZE;

            //obtain a pack pointer
            Pack* pack = nullptr;
            if constexpr (large) 
            {pack = reinterpret_cast<Pack*>(task->m_data.large_ptr);}
            else 
            {pack = reinterpret_cast<Pack*>(task->m_data.small_buff.buff);}

            //get the return type of the function
            using R = std::invoke_result_t<CleanF, Args...>;

            //if void is used, just store a dummy (notifies of valid), else store the result
            if constexpr (std::is_same_v<R, void>) {
                std::apply(pack->func, pack->args);
                task->m_return.assign<uint8_t>(1);
            } else 
            {task->m_return.assign<R>(std::apply(pack->func, pack->args));}
        };
    }

    /**
     * @brief the structure stored in the type erased storage
     * 
     * @tparam F the function type to store
     * @tparam Args the arguments to store
     */
    template <typename F, typename... Args> 
    struct ClosurePack {
        /**
         * @brief store the function
         */
        F func;
        /**
         * @brief store the arguments
         */
        std::tuple<Args...> args;
    };

    /**
     * @brief required because MSVC complained about this in a union
     */
    struct SmallStorage {
        /**
         * @brief store the small buffer optimization data
         * 
         * Holds the invocable + captures + args
         */
        uint8_t buff[SMALL_BUFFER_SIZE];
    };

    /**
     * @brief store the data of the task
     */
    union Data {
        /**
         * @brief store the small buffer optimization data
         * 
         * Holds the invocable + captures + args
         */
        SmallStorage small_buff;
        /**
         * @brief fallback to heap buffer if the data is too large for the small buffer
         */
        void* large_ptr = nullptr;
    };

    /**
     * @brief store the data of the task
     */
    alignas (alignof(std::max_align_t)) Data m_data;

    /**
     * @brief type-erased function to invoke the task
     */
    void (*m_invoke)(Task*) = nullptr;
    /**
     * @brief type-erased function to delete the task
     */
    void (*m_destroy)(Task*) = nullptr;
    /**
     * @brief hold the return value of the task
     */
    Future m_return;

};

/**
 * @brief a class that builds and owns a DAG-tree
 * 
 * @note This class can be neither copied nor moved. 
 * 
 * @ingroup Jobs
 */
class Job {
public:

    /**
     * @brief store the maximum amount of direct children a job can have
     */
    inline static constexpr size_t MAX_DIRECT_CHILD_COUNT = 16;

    /**
     * @brief an integral type that is used for all children counters
     */
    typedef uint16_t ChildIntegral_t;
    /**
     * @brief store the integer type for the priority
     */
    typedef uint16_t Priority_t;
    /**
     * @brief the type for child indices
     */
    typedef uint32_t TypeIndex_t;

    /**
     * @brief store the limit to start radix sort at
     */
    inline static constexpr size_t RADIX_SORT_START = 12;

    /**
     * @brief store a node of the Job's DAG tree
     * 
     * @ingroup Jobs
     * @ingroup Tasks
     */
    struct alignas(64) TaskNode {

        /**
         * @brief Construct a new Task Node
         */
        TaskNode() = default;

        /**
         * @brief Construct a new Task Node
         * 
         * @param _task a pointer to the task to wrap
         */
        TaskNode(Task* _task) 
         : task(_task)
        {}

        /**
         * @brief Construct a new Task Node
         * 
         * @param other the task node to move from
         */
        TaskNode(TaskNode&& other) noexcept
         : task(other.task),
           unmet(other.unmet.load(std::memory_order_relaxed)),
           childCount(other.childCount),
           owner(other.owner)
        {
            //copy array
            for (ChildIntegral_t i = 0; i < other.childCount; ++i) {
                children[i] = other.children[i];
            }

            //reset source
            other.task = nullptr;
            other.unmet.store(0, std::memory_order_relaxed);
            other.childCount = 0;
            other.owner = nullptr;
        }

        //no copy stuff nor move assignment
        TaskNode& operator=(TaskNode&&) = delete;
        TaskNode(const TaskNode&) = delete;
        TaskNode& operator=(const TaskNode&) = delete;

        /**
         * @brief store the task the node wraps
         */
        Task* task = nullptr;
        /**
         * @brief store the amount of unmet direct dependencies
         */
        std::atomic<ChildIntegral_t> unmet{0};
        /**
         * @brief store the direct children of the node
         */
        TypeIndex_t children[MAX_DIRECT_CHILD_COUNT];
        /**
         * @brief store the amount of direct children the node has
         */
        ChildIntegral_t childCount = 0;
        /**
         * @brief store a pointer to the owning job
         */
        Job* owner = nullptr;
    };

    /**
     * @brief a structure that unifies the task information required for DAG building
     * 
     * @ingroup Jobs
     */
    struct TaskEntry {
        /**
         * @brief store the actual task to run
         */
        Task* task;
        /**
         * @brief store the priority of the task (0 -> lowest priority, UINT16_MAX -> highest priority)
         */
        Priority_t priority;
        /**
         * @brief store indicies (given out as identifiers during registration) for all parents
         */
        std::vector<TypeIndex_t> parents;
    };

    /**
     * @brief store the lifecycle state of the Job
     */
    enum class State : uint8_t {
        /**
         * @brief the job is currently unordered (empty node array)
         */
        UNORDERED = 0,
        /**
         * @brief the job is currently building the DAG
         */
        SORTING = 1,
        /**
         * @brief the DAG tree is ready
         */
        READY = 2,
        /**
         * @brief all tasks of the job were run
         */
        FINISHED = 3
    };

    /**
     * @brief Construct a new Job
     */
    Job() = default;

    //cannot be moved nor copied
    Job(const Job&) = delete;
    Job(Job&&) = delete;
    Job& operator=(const Job&) = delete;
    Job& operator=(Job&&) = delete;

    /**
     * @brief add a new task to the job
     * 
     * @throws `Tiny::Jobs::Exception` If one of the dependency indices does not name a known task
     * 
     * @param task a reference to the task to add
     * @param priority the priority of the task
     * @param dependencies all dependencies of the task
     * @return `TypeIndex_t` the identification index of the task
     */
    TypeIndex_t add(Task& task, Priority_t priority, const std::vector<TypeIndex_t>& dependencies = {}) {
        //make sure that the dependencies are sane
        for (size_t i = 0; i < dependencies.size(); ++i)
        {if (dependencies[i] >= m_tasks.size()) {throw Exception("Found invalid dependency index");}}
        //store the new task entry
        m_tasks.emplace_back(&task, priority, dependencies);
        //return the index
        return m_tasks.size()-1;
    }

    /**
     * @brief the function to finalize the job
     * 
     * @throws `Tiny::Jobs::Exception` In debug when the maximum amount of direct children of a single node is surpassed. 
     */
    void finalize() {
        //the job is now building
        //only one may do that
        State expected = State::UNORDERED;
        if (!m_state.compare_exchange_strong(expected, State::SORTING, std::memory_order_acq_rel))
        {return;}
        m_tasksLeft.store(false, std::memory_order_release);

        //get the amount of tasks and then return if it is 0
        const TypeIndex_t taskCount = static_cast<TypeIndex_t>(m_tasks.size());
        if (taskCount == 0) {
            m_leftTotalTasks.store(0, std::memory_order_relaxed);
            m_state.store(State::READY, std::memory_order_release);
            return;
        }

        //allocate nodes
        m_nodes.resize(taskCount);
        for (TypeIndex_t i = 0; i < taskCount; ++i) {
            m_nodes[i].task = m_tasks[i].task;
            m_nodes[i].childCount = 0;
            m_nodes[i].unmet.store(static_cast<ChildIntegral_t>(m_tasks[i].parents.size()), std::memory_order_relaxed);
            m_nodes[i].owner = this;
        }

        //build children arrays
        //this step essentially flips the DAG
        for (TypeIndex_t i = 0; i < taskCount; ++i) {
            const auto& entry = m_tasks[i];
            for (TypeIndex_t parentIdx : entry.parents) {
                auto& parent = m_nodes[parentIdx];
                //add some sanity checks in debug
                #ifdef TINY_JOBS_DEBUG
                if (parent.childCount >= MAX_DIRECT_CHILD_COUNT)
                {throw Exception("Exceeded maximum direct children count. Split up the task tree or increase the maximum direct child count.");}
                #endif
                parent.children[parent.childCount++] = i;
            }
        }

        //collect ready node indices
        std::vector<TypeIndex_t> ready;
        ready.reserve(taskCount);
        for (TypeIndex_t i = 0; i < taskCount; ++i) {
            if (m_nodes[i].unmet.load(std::memory_order_relaxed) == 0) 
            {ready.push_back(i);}
        }

        //run sorting algorithm
        //for larger lists use radix, for smaller insertion
        if (ready.size() < RADIX_SORT_START) 
        {__insertion_sort(ready.data(), ready.size());} 
        else {
            std::vector<TypeIndex_t> scratch(ready.size(), 0);
            __radix_sort(ready.data(), ready.size(), scratch.data());
        }

        //enqueue all the ready nodes in the correct priority order
        m_initialReady.reserve(ready.size());
        {
        //for non-lockfree queue systems a lock is required
        #ifdef TINY_JOBS_NO_LOCKFREE
        std::unique_lock lock(m_queueMtx);
        #endif
        for (TypeIndex_t i : ready) {
            //switch between queue implementations
            #ifndef TINY_JOBS_NO_LOCKFREE
            m_readyNodes.enqueue(&m_nodes[i]);
            #else
            m_readyNodes.push(&m_nodes[i]);
            #endif
            m_initialReady.push_back(&m_nodes[i]);
        }
        }
        m_leftTotalTasks.store(ready.size(), std::memory_order_relaxed);
        //store that the job is ready
        m_tasksLeft.store(true, std::memory_order_release);
        m_state.store(State::READY, std::memory_order_release);
    }

    /**
     * @brief get the next task
     * 
     * @return `TaskNode*` a pointer to the next task node to run
     */
    TaskNode* next() {
        //store a pointer to the task to return
        TaskNode* node = nullptr;

        //switch implementation between lockfree / non-lockfree
        #ifndef TINY_JOBS_NO_LOCKFREE
        //try to get the node
        if (m_readyNodes.try_dequeue(node)) {
            //if this is the last task (count = 1) and it has no dependencies, then store that the queue is empty
            if ((m_leftTotalTasks.load(std::memory_order_acquire) == 1) && (node->childCount == 0))
            {m_tasksLeft.store(false, std::memory_order_release);}
        }
        #else
        //lock
        std::unique_lock lock(m_queueMtx);
        //if a node exists, get and pop it
        if (!m_readyNodes.empty()) {
            node = m_readyNodes.front();
            m_readyNodes.pop();
            //if this is the last task (count = 1) and it has no dependencies, then store that the queue is empty
            if ((m_leftTotalTasks.load(std::memory_order_acquire) == 1) && (node->childCount == 0))
            {m_tasksLeft.store(false, std::memory_order_release);}
        }
        #endif
        
        //return the node
        return node;
    }

    /**
     * @brief the task to notify if a node finished
     * 
     * @param finished a pointer to the node that was finished
     */
    void notify_done(TaskNode* finished) {
        //store the list of ready tasks
        //at max this is the amount of direct children
        TypeIndex_t ready[MAX_DIRECT_CHILD_COUNT];
        TypeIndex_t scratch[MAX_DIRECT_CHILD_COUNT];
        size_t ready_size = 0;

        //decrement childrens dependency counts
        for (ChildIntegral_t i = 0; i < finished->childCount; ++i) {
            TypeIndex_t childIdx = finished->children[i];
            TaskNode& child = m_nodes[childIdx];

            //if 0 was reached (was previously 1) store the task
            if (child.unmet.fetch_sub(1, std::memory_order_acq_rel) == 1)
            {ready[ready_size++] = childIdx;}
        }

        //if there are new elements, push them
        if (ready_size > 0) {
            //else, sort the data
            //for larger lists use radix, for smaller insertion
            if (ready_size < RADIX_SORT_START) 
            {__insertion_sort(ready, ready_size);} 
            else 
            {__radix_sort(ready, ready_size, scratch);}

            {
            //for the non-lockfree implementation a lock is required
            #ifdef TINY_JOBS_NO_LOCKFREE
            std::unique_lock lock(m_queueMtx);
            #endif
            //add all ready elements to the queue
            for (size_t i = 0; i < ready_size; ++i) {
                #ifndef TINY_JOBS_NO_LOCKFREE
                m_readyNodes.enqueue(&m_nodes[ready[i]]);
                #else
                m_readyNodes.push(&m_nodes[ready[i]]);
                #endif
            }
            }
            m_leftTotalTasks.fetch_add(ready_size, std::memory_order_acq_rel);
        }

        //job finally done
        m_leftTotalTasks.fetch_sub(1, std::memory_order_acq_rel);
        //check for finished
        if (m_leftTotalTasks.load(std::memory_order_acquire) == 0)
        {m_state.store(State::FINISHED, std::memory_order_release);}
    }

    /**
     * @brief check if the job is empty
     * 
     * @return `true` if the job is empty, `false` otherwise
     */
    inline bool empty() const noexcept(true)
    {return m_state.load(std::memory_order_acquire) == State::FINISHED;}

    /**
     * @brief a function to check if the job will be able to give more tasks
     * 
     * @return `true` if it can, `false` otherwise
     */
    inline bool tasksLeft() const noexcept(true)
    {return m_tasksLeft.load(std::memory_order_acquire);}

    /**
     * @brief reset the job
     * 
     * This allows a job to rebuild a job.
     * 
     * @warning This does not clear the job. It is a utility to reuse a single job. 
     * @warning This function may fail under multible conditions. If the job is currently running (Task queue is not empty) or the queue is not set up. 
     * @return `true` if the reset was done successfully, `false` if the reset failed
     */
    bool reset() noexcept(true) {
        //switch it to the unordered state to allow rebuilding
        State expected = State::FINISHED;
        return m_state.compare_exchange_strong(expected, State::UNORDERED);
    }

protected:

    /**
     * @brief a function to run radix sort a vector
     * 
     * @param data a pointer to the data array
     * @param size the amount of elements in the data array
     * @param scratch a scratch buffer to use
     */
    void __radix_sort(TypeIndex_t* data, size_t size, TypeIndex_t* scratch) {
        //Stable 2-pass LSD radix sort on indices
        //this is responsible for priority ordering
        if (size > 0) {
            //settings
            constexpr uint32_t RADIX = 256;
            constexpr uint32_t PASSES = 2;

            //store the ready and destination vectors
            //use pointers because they are swapped
            auto* src = data;
            auto* dst = scratch;

            //iterate over all requested passes
            for (uint8_t pass = 0; pass < PASSES; ++pass) {
                //store the pools
                TypeIndex_t count[RADIX] = {};
                TypeIndex_t offset[RADIX];
                //store how many bits to shift
                const TypeIndex_t shift = pass * 8;

                //count and insert into the buckets
                for (TypeIndex_t i = 0; i < size; ++i) {
                    Priority_t key = m_tasks[src[i]].priority;
                    TypeIndex_t bucket = (key >> shift) & 0xFF;
                    ++count[bucket];
                }

                //prefix sum in descending order
                //this prepares the offsets for all pools
                TypeIndex_t sum = 0;
                constexpr TypeIndex_t target = std::numeric_limits<TypeIndex_t>::max();
                for (TypeIndex_t i = RADIX-1; i != target; --i) {
                    offset[i] = sum;
                    sum += count[i];
                }

                //use a stable scattering algorithm to combine back the pools
                for (TypeIndex_t i = 0; i < size; ++i) {
                    Priority_t key = m_tasks[src[i]].priority;
                    TypeIndex_t bucket = (key >> shift) & 0xFF;
                    dst[offset[bucket]++] = src[i];
                }

                //swap destination and source buffers
                //this prepares for the next pass
                std::swap(src, dst);
            }

            //make the ready buffer the final buffer
            //this makes sure that the final buffer exists outside this scope
            if (src != data)
            {memcpy(data, scratch, sizeof(TypeIndex_t)*size);}
        }
    }

    /**
     * @brief a function to use insertion sort on a set of data
     * 
     * @param data the data to sort
     * @param size the size of the data
     */
    void __insertion_sort(TypeIndex_t* data, size_t size) {
        //linearly iterate over all elements
        //important: Skip the first element (list of length 1 always sorted)
        for (size_t i = 1; i < size; ++i) {
            //obtain the priority
            TypeIndex_t keyIndex = data[i];
            uint16_t keyPriority = m_tasks[keyIndex].priority;

            //start at the index
            size_t j = i;
            //descending order (highest priority first)
            while (j > 0) {
                //update the last priority
                TypeIndex_t prevIndex = data[j - 1];
                uint16_t prevPriority = m_tasks[prevIndex].priority;

                //if this is the correct element, stop here
                if (prevPriority >= keyPriority)
                {break;}

                //else, step down
                data[j] = prevIndex;
                --j;
            }

            //store the element at the requested position
            data[j] = keyIndex;
        }
    }

    #ifndef TINY_JOBS_NO_LOCKFREE
    /**
     * @brief store pointers to all top nodes of the DAG
     * 
     * Multi-Thread safe
     */
    moodycamel::ConcurrentQueue<TaskNode*> m_readyNodes;
    #else 
    /**
     * @brief store the queue
     */
    std::queue<TaskNode*> m_readyNodes;
    /**
     * @brief store a mutex for the queue
     */
    std::mutex m_queueMtx;
    #endif

    /**
     * @brief store all the initially ready nodes
     */
    std::vector<TaskNode*> m_initialReady;
    /**
     * @brief store all the nodes of the DAG
     */
    std::vector<TaskNode> m_nodes;
    /**
     * @brief store the unordered task list
     */
    std::vector<TaskEntry> m_tasks;
    /**
     * @brief store how many tasks are left
     */
    std::atomic<TypeIndex_t> m_leftTotalTasks{0};
    /**
     * @brief store the lifecycle state of the DAG
     */
    std::atomic<State> m_state{State::UNORDERED};
    /**
     * @brief store if tasks are left to be given out
     */
    std::atomic_bool m_tasksLeft{false};

};

/**
 * @brief a class that stores and prepares a lot of tasks at once
 * 
 * @note This class can be neither copied nor moved. 
 * 
 * @tparam COUNT the amount of tasks to prepare
 * 
 * @ingroup Tasks
 */
template <size_t COUNT>
class StaticBulkTask {
public:

    /**
     * @brief create a new bulk task
     * 
     * @tparam Func the function that the task should execute
     * @tparam Args the arguments to parse to the functions
     * @param fn the function that the task should execute
     * @param args the arguments to parse to the functions
     */
    template <typename Func, typename... Args>
    StaticBulkTask(Func&& fn, Args&&... args) {
        //create all the tasks, nodes and the indirection table
        for (size_t i = 0; i < COUNT; ++i) {
            if constexpr (std::is_invocable_v<Func, Args...>)
            {new (m_tasks + i) Task(std::forward<Func>(fn), std::forward<Args>(args)...);}
            else
            {new (m_tasks + i) Task(std::forward<Func>(fn), size_t(i), std::forward<Args>(args)...);}
            m_nodes[i].task = m_tasks + i;
            m_indirection[i] = m_nodes + i;
        }
    }

    /**
     * @brief Destroy the Static Bulk Task
     */
    ~StaticBulkTask() = default;

    //forbid copy and moving
    StaticBulkTask(const StaticBulkTask&) = delete;
    StaticBulkTask(StaticBulkTask&&) = delete;
    StaticBulkTask& operator=(const StaticBulkTask&) = delete;
    StaticBulkTask& operator=(StaticBulkTask&&) = delete;

    /**
     * @brief access the indirection table
     * 
     * @return `Job::TaskNode**` the indirection table
     */
    inline Job::TaskNode** table() noexcept(true)
    {return m_indirection;}

    /**
     * @brief get the amount of stored task nodes
     * 
     * @return `constexpr size_t` the amount of stored task nodes
     */
    inline static constexpr size_t count() noexcept(true)
    {return COUNT;}

protected:

    /**
     * @brief storeage for the actual tasks
     */
    Task m_tasks[COUNT];
    /**
     * @brief store nodes to give meta information for all tasks
     */
    Job::TaskNode m_nodes[COUNT]{};
    /**
     * @brief store an indirection table to the nodes
     * 
     * this is required for bulk task insertion
     */
    Job::TaskNode* m_indirection[COUNT]{};

};

/**
 * @brief a class that stores and prepares a lot of tasks at once
 * 
 * In contrast to static bulk tasks the amount of tasks that this bulk task prepares can be chosen at runtime
 * 
 * @note This class can be neither copied nor moved. 
 * 
 * @ingroup Tasks
 */
class BulkTask {
public:

    /**
     * @brief Construct a new Bulk Task
     * 
     * @tparam Func the function that the task should execute
     * @tparam Args the arguments to parse to the functions
     * @param count the amount of task instances to store
     * @param fn the function that the task should execute
     * @param args the arguments to parse to the functions
     */
    template <typename Func, typename... Args>
    BulkTask(size_t count, Func&& fn, Args&&... args) 
     : m_count(count)
    {
        //allocate the pools
        m_tasks = new Task[count];
        m_nodes = new Job::TaskNode[count];
        m_indirection = new Job::TaskNode*[count];
        //create all the tasks, nodes and the indirection table
        for (size_t i = 0; i < m_count; ++i) {
            if constexpr (std::is_invocable_v<Func, Args...>)
            {new (m_tasks + i) Task(std::forward<Func>(fn), std::forward<Args>(args)...);}
            else 
            {new (m_tasks + i) Task(std::forward<Func>(fn), size_t(i), std::forward<Args>(args)...);}
            new (m_nodes + i) Job::TaskNode(m_tasks + i);
            m_indirection[i] = m_nodes + i;
        }
    }

    /**
     * @brief Destroy the Bulk Task
     */
    ~BulkTask() {
        //clean up all the arrays
        delete[] m_tasks;
        m_tasks = nullptr;
        delete[] m_nodes;
        m_nodes = nullptr;
        delete[] m_indirection;
        m_indirection = nullptr;
    }

    //forbid copy and moving
    BulkTask(const BulkTask&) = delete;
    BulkTask(BulkTask&&) = delete;
    BulkTask& operator=(const BulkTask&) = delete;
    BulkTask& operator=(BulkTask&&) = delete;

    /**
     * @brief access the indirection table
     * 
     * @return `Job::TaskNode**` the indirection table
     */
    inline Job::TaskNode** table() noexcept(true)
    {return m_indirection;}

    /**
     * @brief get the amount of stored task nodes
     * 
     * @return `size_t` the amount of stored task nodes
     */
    inline size_t count() noexcept(true)
    {return m_count;}

protected:

    /**
     * @brief store the task array
     */
    Task* m_tasks = nullptr;
    /**
     * @brief store the array of task nodes
     */
    Job::TaskNode* m_nodes = nullptr;
    /**
     * @brief store an indirection table to map the task nodes
     */
    Job::TaskNode** m_indirection = nullptr;

    /**
     * @brief store the amount of prepared tasks
     */
    size_t m_count = 0;

};

/**
 * @brief a structure responsible for scheduling and running jobs in a multi-threaded way
 * 
 * @note This class can be neither copied nor moved. 
 * 
 * @tparam FIBER_COUNT the amount of fibers the employer offers per thread. It must be at least 1. To disabler fibers, use the define or option in the compiler. 
 * 
 * @ingroup Scheduler
 */
template <size_t FIBER_COUNT = 8>
requires (FIBER_COUNT > 0)
class Employer_t {
public:

    /**
     * @brief Construct a new Employer
     * 
     * @param threads the amount of threads to creat (0 = auto)
     * @param autostart `true` to start on creation, `false` otherwise
     * @param pinStart the offset of cores to pin to
     */
    Employer_t(uint32_t threads = 0, bool autostart = true, uint32_t pinStart = 0) {
        //if threads is 0, set to 1 (minimum)
        threads = (threads == 0) ? (std::thread::hardware_concurrency()-1) : threads;
        if (threads == 0) 
        {threads = 1;}

        //store the worker count
        m_workerCount = threads;

        //prepare the workers
        m_worker = new Worker[m_workerCount]();

        //if starting requested, start
        if (autostart)
        {start(pinStart);}
    }

    /**
     * @brief Destroy the Employer
     */
    ~Employer_t() {
        shutdown();

        //clean up
        delete[] m_worker;
        m_worker = nullptr;
    }

    //cannot be moved nor copied
    Employer_t(const Employer_t&) = delete;
    Employer_t(Employer_t&&) = delete;
    Employer_t& operator=(const Employer_t&) = delete;
    Employer_t& operator=(Employer_t&&) = delete;

    /**
     * @brief add a job to the system
     * 
     * @param job a pointer to the job to queue
     */
    void add(Job* job) {
        //can't add on shutdown
        if (m_locked.load(std::memory_order_acquire))
        {return;}
        //pull initial nodes
        while (Job::TaskNode* node = job->next()) {
            //queue the nodes and register the work
            m_todo.fetch_add(1, std::memory_order_acq_rel);
            m_queuedWork.fetch_add(1, std::memory_order_acq_rel);
            uint32_t idx = m_roundRobinNext.fetch_add(1, std::memory_order_relaxed) % m_workerCount;
            //switch implementation between lockfree / non-lockfree
            #ifndef TINY_JOBS_NO_LOCKFREE
            m_worker[idx].tasks.enqueue(node);
            #else
            //first, lock
            std::unique_lock lock(m_worker[idx].queueMtx);
            //then push to the queue
            m_worker[idx].tasks.push(node);
            #endif
            //wake up the thread if it was sleeping
            if (m_worker[idx].sleeping.load(std::memory_order_acquire))
            {m_worker[idx].cv.notify_one();}
        }
    }

    /**
     * @brief a function to add a single task node
     * 
     * This is used to add a single task
     * 
     * @param node a pointer to the node
     */
    void add(Job::TaskNode* node) {
        //can't add on shutdown
        if (m_locked.load(std::memory_order_acquire))
        {return;}
        //queue the nodes and register the work
        m_todo.fetch_add(1, std::memory_order_acq_rel);
        m_queuedWork.fetch_add(1, std::memory_order_acq_rel);
        uint32_t idx = m_roundRobinNext.fetch_add(1, std::memory_order_relaxed) % m_workerCount;
        //switch implementation between lockfree / non-lockfree
        #ifndef TINY_JOBS_NO_LOCKFREE
        m_worker[idx].tasks.enqueue(node);
        #else
        //first, lock
        std::unique_lock lock(m_worker[idx].queueMtx);
        //then push to the queue
        m_worker[idx].tasks.push(node);
        #endif
        //wake up the thread if it was sleeping
        if (m_worker[idx].sleeping.load(std::memory_order_acquire))
        {m_worker[idx].cv.notify_one();}
    }

    /**
     * @brief add a lot of new nodes
     * 
     * @param nodes the nodes to add
     * @param count the amount of nodes
     */
    void add_bulk(Job::TaskNode** nodes, uint64_t count) {
        //can't add on shutdown
        if (m_locked.load(std::memory_order_acquire))
        {return;}
        //register the work
        m_todo.fetch_add(count, std::memory_order_acq_rel);
        m_queuedWork.fetch_add(count, std::memory_order_acq_rel);
        //divide the load equally
        uint64_t step = (count + (m_workerCount-1)) / m_workerCount;
        uint64_t pos = 0;
        //add all elements
        for (size_t i = 0; i < m_workerCount; ++i) {
            //compute how much work to add
            uint64_t delta = step;
            if ((pos + delta) > count)
            {delta = count-pos;}
            //if delta is 0, just stop
            if (delta == 0) {break;}
            //switch implementation between lockfree / non-lockfree
            #ifndef TINY_JOBS_NO_LOCKFREE
            m_worker[i].tasks.enqueue_bulk(nodes + pos, delta);
            #else
            //first, lock
            std::unique_lock lock(m_worker[i].queueMtx);
            //then push to the queue
            for (size_t j = 0; j < delta; ++j)
            {m_worker[i].tasks.push(nodes[pos + j]);}
            #endif
            //wake up the thread if it was sleeping
            if (m_worker[i].sleeping.load(std::memory_order_acquire))
            {m_worker[i].cv.notify_one();}
            //step forward
            pos += delta;
        }
    }

    /**
     * @brief a function to add a bulk task to the employer
     * 
     * @tparam COUNT the amount of tasks stored in the bulk tas
     * @param task a reference to the bulk task to add
     */
    template <size_t COUNT>
    void add_bulk(StaticBulkTask<COUNT>& task)
    {add_bulk(task.table(), COUNT);}

    /**
     * @brief a function to add a bulk task to the employer
     * 
     * @param task a reference to the bulk task to add
     */
    void add_bulk(BulkTask& task) 
    {add_bulk(task.table(), task.count());}

    /**
     * @brief a function to check if the employer is idle
     * 
     * @return `true` if no work is queued, `false` otherwise
     */
    inline bool idle() const noexcept(true)
    {return m_todo.load(std::memory_order_relaxed) == 0;}

    /**
     * @brief start the scheduler system
     * 
     * automatically done on creation unless requested not to
     * 
     * @param pinStart the offset of cores to pin to
     */
    void start(uint32_t pinStart = 0) {
        //mark as running
        m_running.store(true, std::memory_order_release);
        //get the amount of hardware threads (1 on failure)
        uint32_t hardware = std::thread::hardware_concurrency();
        hardware = (hardware > 0) ? hardware : 1;
        //initialize all threads
        for (size_t i = 0; i < m_workerCount; ++i) {
            m_worker[i].thread = std::thread(&Employer_t::workerFn, this, i);
            pinThreadToCore(m_worker[i].thread, (i+pinStart) % hardware);
        }
        //unlock
        m_locked.store(false, std::memory_order_release);
    }

    /**
     * @brief shut down the employer scheduler
     */
    void shutdown() {
        //lock
        m_locked.store(true, std::memory_order_release);
        //drain the work
        waitIdle();
        //mark as non-running
        m_running.store(false, std::memory_order_release);
        //join the workers
        for (size_t i = 0; i < m_workerCount; ++i) {
            //wake up the thread
            m_worker[i].cv.notify_one();
            //join the thread
            m_worker[i].thread.join();
        }
    }

    /**
     * @brief wait for the employer to become idle
     */
    inline void waitIdle() const noexcept(true)
    {while (!idle()) {std::this_thread::sleep_for(std::chrono::nanoseconds(1));}}

protected:

    //check if fibers are allowed
    #ifndef TINY_JOBS_NO_FIBERS

    /**
     * @brief the main loop of a worker thread
     * 
     * @throws `Tiny::Jobs::Exception` If this function throws, please report it to the developers as this should not happen. This only throws if the scheduler messes up critically. 
     * 
     * @param idx the index of the worker in the 
     */
    void workerFn(uint32_t idx) {
        //set stack size
        boost::context::fixedsize_stack stack(256 * 1024);
        /**
         * @brief store all fibers (except main fiber, that is the thread)
         */
        std::array<boost::fibers::fiber, FIBER_COUNT> pool;
        /**
         * @brief store the tasks for all fibers
         * 
         * `nullptr` represents that the task is free
         */
        std::array<Job::TaskNode*, FIBER_COUNT> task{};
        /**
         * @brief store the amount of active fibers
         * 
         * Incremented by the main fiber, decremented by workers
         */
        uint32_t active = 0;
        /**
         * @brief store the queue to add to nex time
         */
        uint32_t roundRobinIdx = 0;

        //the main function for a fiber
        auto fiber_main = [&](size_t fiber_idx){
            //iterate while the worker is running
            while (m_running.load(std::memory_order_relaxed)) {
                //check if work is todo
                if (task[fiber_idx]) {
                    //run the task
                    task[fiber_idx]->task->invoke();
                    //notify the job that the task is done
                    Job* job = task[fiber_idx]->owner;
                    //if no owner exists (yes, that is legal) just skip this
                    if (job) {
                        job->notify_done(task[fiber_idx]);

                        //now there may be new tasks available. Poll them. 
                        while (Job::TaskNode* node = job->next()) {
                            m_todo.fetch_add(1, std::memory_order_acq_rel);
                            m_queuedWork.fetch_add(1, std::memory_order_acq_rel);
                            uint32_t i = (roundRobinIdx++) % m_workerCount;
                            //switch implementation between lockfree / non-lockfree
                            #ifndef TINY_JOBS_NO_LOCKFREE
                            m_worker[i].tasks.enqueue(node);
                            #else
                            //first, lock
                            std::unique_lock lock(m_worker[idx].queueMtx);
                            //then push to the queue
                            m_worker[i].tasks.push(node);
                            #endif
                            //wake up the thread if it was sleeping
                            if (m_worker[i].sleeping.load(std::memory_order_acquire))
                            {m_worker[i].cv.notify_one();}
                        }
                    }
                    //free the slot
                    m_todo.fetch_sub(1, std::memory_order_acq_rel);
                    task[fiber_idx] = nullptr;

                    //done
                    --active;
                }
                //finally, yield the fiber
                boost::this_fiber::yield();
            }
        };
        //create all fibers
        for (size_t i = 0; i < FIBER_COUNT; ++i) {
            //create the fiber using a lambda
            pool[i] = std::move(boost::fibers::fiber(fiber_main, i));
        }

        //main function for this fiber (thread is now a fiber too)
        while (m_running.load(std::memory_order_relaxed)) {
            //if all fibers are fill, skip
            if (active != FIBER_COUNT) {
                //try to poll from the queue
                Job::TaskNode* node = nullptr;
                //switch implementation between lockfree / non-lockfree implementation
                #ifndef TINY_JOBS_NO_LOCKFREE
                if (!m_worker[idx].tasks.try_dequeue(node)) {
                    //try to steal from somewhere
                    for (uint32_t i = 0; i < m_workerCount; ++i) {
                        //can't steal from self
                        if (i == idx) {continue;}
                        //steal work
                        m_worker[i].tasks.try_dequeue(node);
                        if (node) {break;}
                    }
                }
                #else
                //first, try to pull from the own queue
                {
                //lock the own mutex
                std::unique_lock lock(m_worker[idx].queueMtx);
                //then, try to pull from the queue
                if (!m_worker[idx].tasks.empty()) {
                    node = m_worker[idx].tasks.front();
                    m_worker[idx].tasks.pop();
                }
                }
                //if nothing was pulled, try to steal
                if (!node) {
                    //iterate over all other nodes
                    for (uint32_t i = 0; i < m_workerCount; ++i) {
                        //can't steal from self
                        if (i == idx) {continue;}
                        //first, obtain a lock
                        std::unique_lock lock(m_worker[i].queueMtx);
                        //then, try to dequeue
                        if (!m_worker[i].tasks.empty()) {
                            node = m_worker[i].tasks.front();
                            m_worker[i].tasks.pop();
                        }
                        if (node) {break;}
                    }
                }
                #endif
                if (node) {
                    //task was dequeued from somewhere
                    m_queuedWork.fetch_sub(1, std::memory_order_acq_rel);
                    //if a node was pulled, a worker must be free
                    //so give it to the first free worker found
                    bool given = false;
                    for (size_t i = 0; i < FIBER_COUNT; ++i) {
                        if (!task[i]) {
                            task[i] = node;
                            ++active;
                            given = true;
                            break;
                        }
                    }
                    //if given was not set to true, a scheduling error occurred
                    //how?
                    #ifdef TINY_JOBS_DEBUG
                    if (!given)
                    {throw Exception("Failed to schedule a task even thou a free slot was reported");}
                    #endif
                }
            }
            //if active is 0, that means the thread may sleep
            if ((active == 0) && (m_queuedWork.load(std::memory_order_acquire) == 0)) {
                //use the condition variable to sleep
                std::unique_lock lock(m_worker[idx].mtx);
                //re-check required
                if ((active == 0) && (m_queuedWork.load(std::memory_order_acquire) == 0)) {
                    m_worker[idx].sleeping.store(true, std::memory_order_release);
                    m_worker[idx].cv.wait(lock, [&]
                        {return !m_running.load(std::memory_order_relaxed) || (m_queuedWork.load(std::memory_order_acquire) > 0);}
                    );
                    //no longer sleeping
                    m_worker[idx].sleeping.store(false, std::memory_order_release);
                }
            } else {
                //finally, yield the fiber
                boost::this_fiber::yield();
            }
        }

        //lastly, make sure to join the fibers
        for (size_t i = 0; i < FIBER_COUNT; ++i)
        {if (pool[i].joinable()) {pool[i].join();}}
    }

    #else 
    //else, use a secondary, fiber-less implementation

    void workerFn(uint32_t idx) {
        /**
         * @brief store the queue to add to nex time
         */
        uint32_t roundRobinIdx = 0;

        //main function for this fiber (thread is now a fiber too)
        while (m_running.load(std::memory_order_relaxed)) {
            //try to poll from the queue
            Job::TaskNode* node = nullptr;
            //switch implementation between lockfree / non-lockfree implementation
            #ifndef TINY_JOBS_NO_LOCKFREE
            if (!m_worker[idx].tasks.try_dequeue(node)) {
                //try to steal from somewhere
                for (uint32_t i = 0; i < m_workerCount; ++i) {
                    //can't steal from self
                    if (i == idx) {continue;}
                    //steal work
                    m_worker[i].tasks.try_dequeue(node);
                    if (node) {break;}
                }
            }
            #else
            //first, try to pull from the own queue
            {
            //lock the own mutex
            std::unique_lock lock(m_worker[idx].queueMtx);
            //then, try to pull from the queue
            if (!m_worker[idx].tasks.empty()) {
                node = m_worker[idx].tasks.front();
                m_worker[idx].tasks.pop();
            }
            }
            //if nothing was pulled, try to steal
            if (!node) {
                //iterate over all other nodes
                for (uint32_t i = 0; i < m_workerCount; ++i) {
                    //can't steal from self
                    if (i == idx) {continue;}
                    //first, obtain a lock
                    std::unique_lock lock(m_worker[i].queueMtx);
                    //then, try to dequeue
                    if (!m_worker[i].tasks.empty()) {
                        node = m_worker[i].tasks.front();
                        m_worker[i].tasks.pop();
                    }
                    if (node) {break;}
                }
            }
            #endif
            if (node) {
                //task was dequeued from somewhere
                m_queuedWork.fetch_sub(1, std::memory_order_acq_rel);
                //run the task
                node->task->invoke();
                //notify the job that the task is done
                Job* job = node->owner;
                //if no owner exists (yes, that is legal) just skip this
                if (job) {
                    job->notify_done(node);

                    //now there may be new tasks available. Poll them. 
                    while (Job::TaskNode* node = job->next()) {
                        m_todo.fetch_add(1, std::memory_order_acq_rel);
                        m_queuedWork.fetch_add(1, std::memory_order_acq_rel);
                        uint32_t i = (roundRobinIdx++) % m_workerCount;
                        //switch implementation between lockfree / non-lockfree
                        #ifndef TINY_JOBS_NO_LOCKFREE
                        m_worker[i].tasks.enqueue(node);
                        #else
                        //first, lock
                        std::unique_lock lock(m_worker[idx].queueMtx);
                        //then push to the queue
                        m_worker[i].tasks.push(node);
                        #endif
                        //wake up the thread if it was sleeping
                        if (m_worker[i].sleeping.load(std::memory_order_acquire))
                        {m_worker[i].cv.notify_one();}
                    }
                }
                //free the slot
                m_todo.fetch_sub(1, std::memory_order_acq_rel);
            }
            //if active is 0, that means the thread may sleep
            if (m_queuedWork.load(std::memory_order_acquire) == 0) {
                //use the condition variable to sleep
                std::unique_lock lock(m_worker[idx].mtx);
                //re-check required
                if (m_queuedWork.load(std::memory_order_acquire) == 0) {
                    m_worker[idx].sleeping.store(true, std::memory_order_release);
                    m_worker[idx].cv.wait(lock, [&]
                        {return !m_running.load(std::memory_order_relaxed) || (m_queuedWork.load(std::memory_order_acquire) > 0);}
                    );
                    //no longer sleeping
                    m_worker[idx].sleeping.store(false, std::memory_order_release);
                }
            }
        }
    }

    #endif

    /**
     * @brief a function to pin an std::thread to a specific core
     * 
     * Because that is OS stuff defines are used to detect the operating system and switch implementation
     * 
     * @param t the thread to pin
     * @param coreIndex the core to pin to
     */
    void pinThreadToCore(std::thread& t, size_t coreIndex) {
        //use the correct thread pinning functionality per OS
    #if defined(__linux__)
        cpu_set_t cpuset;
        CPU_ZERO(&cpuset);
        CPU_SET(coreIndex, &cpuset);
        int rc = pthread_setaffinity_np(t.native_handle(), sizeof(cpu_set_t), &cpuset);
        if (rc != 0) perror("pthread_setaffinity_np");

    #elif defined(_WIN32) && defined(_MSC_VER)
        DWORD_PTR mask = 1ULL << coreIndex;
        HANDLE handle = (HANDLE)t.native_handle();
        DWORD_PTR result = SetThreadAffinityMask(handle, mask);
        if (result == 0) {
            DWORD err = GetLastError();
            fprintf(stderr, "SetThreadAffinityMask failed: %lu\n", err);
        }

    #elif defined(__APPLE__)
        //macOS does not support strict affinity; we can set QoS instead
        //Map cores in round-robin to threads
        thread_port_t mach_thread = pthread_mach_thread_np(t.native_handle());
        thread_affinity_policy_data_t policy = { static_cast<int>(coreIndex) };
        thread_policy_set(mach_thread, THREAD_AFFINITY_POLICY,
                        (thread_policy_t)&policy, THREAD_AFFINITY_POLICY_COUNT);
    #endif
    }
    
    /**
     * @brief store the per-worker data
     */
    struct Worker {
        /**
         * @brief store the thread of the worker
         */
        std::thread thread;
        
        //switch content depending on lockfree / non-lockfree setting
        #ifndef TINY_JOBS_NO_LOCKFREE
        /**
         * @brief store a per-worker task queue
         * 
         * This is a concurrent queue to allow for lock-free stealing
         */
        moodycamel::ConcurrentQueue<Job::TaskNode*> tasks;
        #else
        /**
         * @brief store the task queue
         */
        std::queue<Job::TaskNode*> tasks;
        /**
         * @brief store a mutex for the queue
         */
        std::mutex queueMtx;
        #endif

        /**
         * @brief store a mutex
         * 
         * Used for waiting with the condition variable
         */
        std::mutex mtx;
        /**
         * @brief a condition variable for using interrupt waiting
         */
        std::condition_variable cv;
        /**
         * @brief store if the worker is sleeping
         */
        std::atomic_bool sleeping{false};
    };

    /**
     * @brief store the worker
     */
    Worker* m_worker = nullptr;
    /**
     * @brief store the amount of alive worker
     * 
     * Typically this is the amount of available threads minus 1 (free for main thread), but can be customized
     */
    uint32_t m_workerCount = 0;
    /**
     * @brief next add round robin value
     */
    std::atomic_uint32_t m_roundRobinNext{0};

    /**
     * @brief store if the employer is running
     */
    std::atomic_bool m_running{false};
    /**
     * @brief store if the add functions are locked
     */
    std::atomic_bool m_locked{true};
    /**
     * @brief store how much work is to do
     */
    std::atomic_size_t m_todo{0};
    /**
     * @brief store how many tasks are queued
     */
    std::atomic_size_t m_queuedWork{0};

};

/**
 * @brief define an employer with the default options
 * 
 * @ingroup Scheduler
 */
typedef Employer_t<8> Employer;

}

}

}

#endif
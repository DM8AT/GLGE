/**
 * @file Task.h
 * @author DM8AT
 * @brief define a wrapper for a function that is fully type erased and works without heap allocation
 * @version 0.1
 * @date 2025-12-24
 * 
 * @copyright Copyright (c) 2025
 * 
 */
//header guard
#ifndef _GLGE_CORE_TASK_
#define _GLGE_CORE_TASK_

//add common stuff
#include "Core/Common.h"
//atomics are required for lock-free thread safety
#include <atomic>
//threads are required for thread operations
#include <thread>

//use the library namespace
namespace GLGE {

    /**
     * @brief hold a structure that captures the return value of a task and also notifys the caller if it was finished
     * 
     * @tparam Ret the return type of the task
     */
    template <typename Ret>
    class TaskHandle {
    public:

        /**
         * @brief Construct a new Task Handle
         */
        TaskHandle() = default;
        /**
         * @brief Construct a new Task Handle
         * 
         * Explicitly deleted
         */
        TaskHandle(const TaskHandle&) = delete;
        /**
         * @brief Construct a new Task Handle
         * 
         * Explicitly deleted
         */
        TaskHandle(TaskHandle&&) = delete;

        /**
         * @brief wait for the execution of the task to finish
         */
        void wait() {
            //do a non-busy wait until the task is done
            while (!m_done.load(std::memory_order_acquire))
            {std::this_thread::yield();}
        }

        /**
         * @brief get the return value of the function
         * 
         * @return Ret& a reference to the returned data
         */
        inline Ret& get() noexcept
        {return *((Ret*)m_storage);}

        /**
         * @brief get if the task is done
         * 
         * @return `true` if the task is done, `false` otherwise
         */
        inline bool isDone() const noexcept
        {return m_done.load(std::memory_order_acquire);}

    protected:

        /**
         * @brief add tasks as a friend structure
         * 
         * They need to access stuff like the return storage and the done flag
         */
        friend struct Task;

        /**
         * @brief store if the task corresponding to the 
         */
        std::atomic_bool m_done = true;

        /**
         * @brief hold enough storage for the return type
         * 
         * Do not store store by value directly as the return type may not be default-constructable
         */
        alignas(alignof(Ret)) u8 m_storage[sizeof(Ret)] = { 0 };

    };

    /**
     * @brief store a task handle that is specialized for a void return type
     * 
     * @tparam Ret the return type of the function, must be void
     */
    template<>
    class TaskHandle<void> {
    public:
        /**
         * @brief Construct a new Task Handle
         */
        TaskHandle() = default;
        /**
         * @brief Construct a new Task Handle
         * 
         * Explicitly deleted
         */
        TaskHandle(const TaskHandle&) = delete;
        /**
         * @brief Copy constructor
         * 
         * @return TaskHandle& explicitly deleted
         */
        TaskHandle& operator=(const TaskHandle&) = delete;
        /**
         * @brief Construct a new Task Handle
         * 
         * Explicitly deleted
         */
        TaskHandle(TaskHandle&&) = delete;
        /**
         * @brief Move constructor
         * 
         * @return TaskHandle& explicitly deleted
         */
        TaskHandle& operator=(TaskHandle&&) = delete;

        /**
         * @brief wait for the task to be done
         */
        void wait() {
            //do a non-busy wait until the task is done
            while (!m_done.load(std::memory_order_acquire)) 
            {std::this_thread::yield();}
        }

    private:
        /**
         * @brief the task that issued the handle
         */
        friend struct Task;

        /**
         * @brief store if the task is done
         */
        std::atomic<bool> m_done{false};
    };

    /**
     * @brief store a type-erased function together with arguments in an in-line storage
     */
    struct Task {
        /**
         * @brief store the type of the function to use to execute the task
         */
        using Func = void(*)(void*);
        /**
         * @brief store the type of the function to use to copy the task
         */
        using Copier = void(*)(void* dst, const void* src);
        /**
         * @brief store a function that is used to delete the task
         */
        using Destroyer = void(*)(void*);
        /**
         * @brief store the arguments to parse to the function
         * 
         * Make the storage exactly 128 byte large so it fits neatly in 8 cachelines (on most modern PCs)
         */
        alignas(max_align_t) u8 storage[128 - sizeof(Func) - sizeof(Copier) - sizeof(Destroyer)] = { 0 };
        /**
         * @brief store the function to invoke
         */
        Func func = nullptr;
        /**
         * @brief store the function for task copying
         */
        Copier copy = nullptr;
        /**
         * @brief store the function to clean up
         */
        Destroyer del = nullptr;

        /**
         * @brief Construct a new empty task
         */
        Task() = default;

        /**
         * @brief Construct a new Task
         * 
         * Copy constructor
         * 
         * @param other the task to copy from
         */
        Task(const Task& other) {
            //copy over the function table
            func = other.func;
            copy = other.copy;
            del = other.del;
            //copy over the data
            if (copy)
            {copy(storage, other.storage);}
        }

        /**
         * @brief Copy over a task
         * 
         * @param other the task to copy
         * @return Task& a reference to this task after filling out
         */
        Task& operator=(const Task& other) {
            //sanity check to prevent copy to self
            if (this == &other)
            {return *this;}

            //copy over the function table
            func = other.func;
            copy = other.copy;
            del = other.del;
            //copy over the data
            if (copy)
            {copy(storage, other.storage);}

            //return a reference to this task
            return *this;
        }

        /**
         * @brief Construct a new Task
         * 
         * Move constructor
         * 
         * @param other a move reference to the task to move from
         */
        Task(Task&& other) noexcept {
            //here, memcpy is safe
            memcpy(this, &other, sizeof(Task));
            //clean up the other's function table
            other.func = nullptr;
            other.copy = nullptr;
            other.del = nullptr;
        }

        /**
         * @brief copy assignment for a task
         * 
         * @param other the task to move from
         * @return Task& a reference to the new task
         */
        Task& operator=(Task&& other) noexcept {
            //sanity check to prevent move to self
            if (this == &other)
            {return *this;}

            //copy over the data
            memcpy(this, &other, sizeof(Task));
            //clean up the function table
            other.func = nullptr;
            other.copy = nullptr;
            other.del = nullptr;

            //return a reference to self
            return *this;
        }

        /**
         * @brief create a new task
         * 
         * @tparam Fn the function type to execute
         * @tparam Ret the return type of the function
         * @tparam Args the argument types to parse to the function
         * @param fn the callable to execute
         * @param handle a handle to store the argument type and store if it was finished
         * @param args the arguments to parse to the function
         * @return Task the task that stores the function to execute
         */
        template <typename Ret, typename... Args, typename Fn>
        static Task create(Fn&& fn, TaskHandle<Ret>* handle, Args&&... args) {
            //store the type to use for the function with correct args
            using F = FnWithArgs<Fn, Ret, Args...>;
            //sanity check the size
            static_assert(sizeof(F) <= sizeof(storage), "The function is too large");

            //if the handle exists, mark it as not done
            if (handle)
            {handle->m_done.store(false, std::memory_order_release);}

            //create the new task
            Task task;
            //fill in the function with arguments into the storage
            (void) new (task.storage) FnWithArgs<Fn, Ret, Args...>(std::forward<Fn>(fn), handle, std::forward<Args>(args)...);
            //create the call lambda
            task.func = [](void* dataPtr) {
                //get and re-interpret the function with arguments
                auto* fptr = reinterpret_cast<F*>(dataPtr);
                //call the function
                fptr->call();
            };

            //type-erased copy
            task.copy = [](void* dst, const void* src) {
                //get the actual typed pointer
                const F* srcF = reinterpret_cast<const F*>(src);
                //call the real copy constructor
                new (dst) F(*srcF);
            };

            //type-erased destructor
            task.del = [](void* ptr) {
                //delete the object
                reinterpret_cast<F*>(ptr)->~F();
            };

            //return the new task
            return task;
        }

        /**
         * @brief call the task
         */
        void call() {
            //call the stored function
            (*func)(storage);
        }

    private:

        /**
         * @brief a templated helper function to store a callable with arguments
         * 
         * @tparam Fn the type of the function to call
         * @tparam Ret the type expected to be returned by the function
         * @tparam Args the arguments to parse to the callable
         */
        template <typename Fn, typename Ret, typename... Args>
        struct FnWithArgs {
            /**
             * @brief store the actual function
             */
            Fn func;
            /**
             * @brief store the arguments for the function
             */
            std::tuple<Args...> args;
            /**
             * @brief store a pointer to the function handle for the return value
             */
            TaskHandle<Ret>* handle;

            /**
             * @brief Construct a new function with arguments
             * 
             * @param _func the function to call
             * @param _args the arguments to prase to the function
             */
            FnWithArgs(Fn&& _func, TaskHandle<Ret>* h, Args&&... _args)
             : func(std::forward<Fn>(_func)), 
               handle(h),
               args(std::forward<Args>(_args)...)
            {}

            /**
             * @brief Construct a new function with arguments
             * 
             * Default copy constructor
             */
            FnWithArgs(const FnWithArgs&) = default;
            /**
             * @brief Construct a new function with arguments
             * 
             * Default move constructor
             */
            FnWithArgs(FnWithArgs&&) = default;

            /**
             * @brief Destroy the function with arguments
             */
            ~FnWithArgs() = default;

            /**
             * @brief call the function
             */
            void call() {
                //sanity check if void is returned
                if constexpr (std::is_void_v<Ret>) {
                    //call the function and parse the arguments
                    std::apply(func, args);
                    //if a handle exists mark that it is done
                    if (handle) 
                    {handle->m_done.store(true, std::memory_order_release);}
                } else {
                    //check if a handle exists
                    if (handle) {
                        //store the return value of the function
                        new (handle->m_storage) Ret(std::apply(func, args));
                        //store that the task is done
                        handle->m_done.store(true, std::memory_order_release);
                    } else {
                        //else, discard the return value
                        (void)std::apply(func, args);
                    }
                }
            }
        };

    };

}

#endif
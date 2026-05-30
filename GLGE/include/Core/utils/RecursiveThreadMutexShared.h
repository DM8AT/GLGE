/**
 * @file RecursiveThreadMutexShared.h
 * @author DM8AT
 * @brief define a locking primitive that works similar to a shared mutex but says "this THREAD locks the mutex" and not this SCOPE locks the mutex
 * @version 0.1
 * @date 2026-05-29
 * 
 * @copyright Copyright (c) 2026
 * 
 */
//header guard
#ifndef _GLGE_CORE_THREAD_MUTEX_
#define _GLGE_CORE_THREAD_MUTEX_

//add common stuff
#include "Core/Common.h"
//threads are needed for thread IDs
#include <thread>
//a shared mutex is used as locking primitive
#include <shared_mutex>

//use the library namespace
namespace GLGE {

    /**
     * @brief a locking primitive that says that the THREAD owns the lock, not the scope
     * 
     * This works by asigning the lock to the thread, not the lock primitive. 
     */
    class RecursiveThreadMutexShared {
    public:

        /**
         * @brief Construct a new Recursive Thread Mutex
         */
        RecursiveThreadMutexShared() = default;

        /**
         * @brief Destroy the Recursive Thread Mutex Shared
         */
        ~RecursiveThreadMutexShared();

        /**
         * @brief lock the thread mutex
         * 
         * This may perform an upgrade. If an upgrade is done, the old lock is dropped and it may be required to wait for the lock to be able to be acquired. 
         * 
         * This lock upgrade is NOT atomic. 
         * It is performed by droping the old lock, acquiring the new lock and then sanity-checking the lock. 
         * This may allow another thread to acquire a shared or unique lock in this time window. 
         * If this is the case, this function may wait until the unique lock is acquired successfully. 
         */
        void lock();

        /**
         * @brief unlock the thread mutex
         * 
         * This may perform an downgrade. If a downgrade is done, the old lock is dropped and it may be required to wait for the lock to be able to be acquired. 
         * 
         * This lock downgrade is NOT atomic. 
         * It is performed by droping the old lock, acquiring the new lock and then sanity-checking the lock. 
         * This may allow another thread to acquire a shared or unique lock in this time window. 
         * If this is the case, this function may wait until the shared lock is acquired successfully. 
         */
        void unlock();

        /**
         * @brief lock the mutex in shared mode
         * 
         * This works even if it was locked uniquely by this thread before. 
         * The shared lock is still counted while a unique lock is active. 
         */
        void lock_shared();

        /**
         * @brief unlock the thread in shared mode
         * 
         * This works even while a unique lock is active on this thread. 
         * The shared lock release is still counted while a unique lock is active. 
         */
        void unlock_shared();

    protected:

        /**
         * @brief store all the thread-local data
         */
        struct ThreadLocalData {
            /**
             * @brief store the amount of shared locks
             * 
             * This is a virtual amount of shared locks. This means "at least one shared lock or stringer exists", not that the shared lock was called n times. 
             */
            u32 sharedLockCount = 0;
            /**
             * @brief store the amount of unique locks
             * 
             * This is a virtual amount of unique locks. This means "at least one unique lock exists", not that unique lock was called n times. 
             */
            u32 uniqueLockCount = 0;
        };

        /**
         * @brief wait until this thread owns upgrade ownership
         */
        void waitForUpgradeOwnership();

        /**
         * @brief Get a copy of the local thread state
         * 
         * @return `ThreadLocalData` a copy of the current thread's state
         */
        ThreadLocalData getThreadStateCopy() const {
            //lock the map
            std::lock_guard lock(m_mapMtx);
            //map can now be accessed

            //try to fetch from the map
            auto it = m_perThreadData.find(t_threadToken);

            //if it does not exist, return a copy to an empty element
            //the new element will be zero-initialized. 
            if (it == m_perThreadData.end())
            {return ThreadLocalData{};}

            //else, return the data
            return it->second;
        };

        /**
         * @brief a function that is used to call a function that modifies the thread-local data
         * 
         * @tparam Fn the type of the function to call
         * @param fn the function to call
         */
        template <typename Fn>
        void modifyThreadState(Fn&& fn) {
            //lock the map
            std::lock_guard lock(m_mapMtx);
            //map can now be accessed

            //get a REFERENCE to the state
            //the `operator[]` function creates a new entry if none exists. 
            //the new entry will be zero-initialized. 
            auto& state = m_perThreadData[t_threadToken];

            //call the inputted function
            fn(state);

            //if no more locks exist, erase the element
            if ((state.sharedLockCount == 0) && (state.uniqueLockCount == 0))
            {m_perThreadData.erase(t_threadToken);}
        }

        /**
         * @brief store the next token given to a thread
         * 
         * Threads are identified using these tokens
         */
        inline static std::atomic_uint64_t s_nextToken {1};
        /**
         * @brief store the thread token used for upgrading
         */
        inline static thread_local uint64_t t_threadToken = s_nextToken.fetch_add(1, std::memory_order_acq_rel);

        /**
         * @brief store a mutex that protects the per thread data map
         * 
         * It is forbidden to hold a lock to this while creating a lock on `m_mtx`. 
         */
        mutable std::mutex m_mapMtx;
        /**
         * @brief for each thread store the thread local data
         * 
         * `thread_local` cannot be used on member elements, so an unordered map indexed with the thread's token is used. 
         * This is an `unordered_map` for fast and easy arbitrary access (using the `operator[]` function). 
         */
        std::unordered_map<u64, ThreadLocalData> m_perThreadData;

        /**
         * @brief store the thread token of the thread that is upgrading
         */
        std::atomic_uint64_t m_upgradingThread = 0;

        /**
         * @brief store the mutex used for locking
         */
        std::shared_mutex m_mtx;

    };

}

#endif
/**
 * @file RecursiveThreadMutexShared.cpp
 * @author DM8AT
 * @brief implement the recursive thread-owned shared mutex
 * @version 0.1
 * @date 2026-05-29
 * 
 * @copyright Copyright (c) 2026
 * 
 */
//add the header
#include "Core/utils/RecursiveThreadMutexShared.h"
//add exceptions
#include "Core/Exception.h"

GLGE::RecursiveThreadMutexShared::~RecursiveThreadMutexShared() {
    //obtain a unique lock on the map
    std::lock_guard otherLock(m_mtx);
    std::lock_guard lock(m_mapMtx);
}

void GLGE::RecursiveThreadMutexShared::lock_shared() {
    //get the per-thread state
    ThreadLocalData state = getThreadStateCopy();

    //check for recursion
    if ((state.sharedLockCount > 0) || (state.uniqueLockCount > 0)) {
        //simply modify the data
        modifyThreadState([](ThreadLocalData& state) {++state.sharedLockCount;});
        //stop
        return;
    }

    //else, this is the first acquiring for the thread (e.g. neither unique nor shared lock is used)
    //so get a shared lock. This is safe here (no current lock on `m_mapMtx` exists). 
    m_mtx.lock_shared();

    //then, modify the map
    modifyThreadState([](ThreadLocalData& state) {state.sharedLockCount = 1;});
}

void GLGE::RecursiveThreadMutexShared::unlock_shared() {
    //get a copy of the current thread state
    ThreadLocalData state = getThreadStateCopy();

    //sanity check
    if (state.sharedLockCount == 0) 
    {throw GLGE::Exception("Tried to unlock a recursive shared thread mutex, but no unique lock was recorded", "GLGE::RecursiveThreadMutexShared::unlock_shared");}

    //check if the lock needs to be released (no other lock exists)
    const bool release = (state.sharedLockCount == 1) && (state.uniqueLockCount == 0);

    //update the per-thread state
    modifyThreadState([](ThreadLocalData& state) {--state.sharedLockCount;});

    //if requested, release the lock
    if (release)
    {m_mtx.unlock_shared();}
}

void GLGE::RecursiveThreadMutexShared::lock() {
    //get the state
    ThreadLocalData state = getThreadStateCopy();

    //check for recursive unique
    if (state.uniqueLockCount > 0) {
        modifyThreadState([](ThreadLocalData& state) {++state.uniqueLockCount;});
        return;
    }

    //check for required update
    if (state.sharedLockCount > 0) {
        //drop shared here -> avoids upgrade deadlock
        //it is well documented that ownership is NOT guaranteed to be continues. 
        m_mtx.unlock_shared();
        //wait for the ownership of the upgrade
        waitForUpgradeOwnership();
        //update the lock
        m_mtx.lock();
        //reset the upgrading thread token
        m_upgradingThread.store(0, std::memory_order_release);
        //modify the state
        modifyThreadState([](ThreadLocalData& state) {++state.uniqueLockCount;});
        //stop
        return;
    }

    //else, this is a fresh unique lock
    m_mtx.lock();

    //now this thread holds a unique lock
    //store that
    modifyThreadState([](ThreadLocalData& state) {state.uniqueLockCount = 1;});
}

void GLGE::RecursiveThreadMutexShared::unlock() {
    //get the state
    ThreadLocalData state = getThreadStateCopy();

    //sanity check
    if (state.uniqueLockCount == 0)
    {throw GLGE::Exception("Tried to unlock from a thread that was never locked", "GLGE::RecursiveThreadMutexShared::unlock");}

    //check for recursive unique
    if (state.uniqueLockCount > 1) {
        modifyThreadState([](ThreadLocalData& state) {--state.uniqueLockCount;}); 
        return;
    }

    //modify the thread state
    modifyThreadState([](ThreadLocalData& state) {--state.uniqueLockCount;});

    //if a downgrade is required, downgrade it
    if (state.sharedLockCount > 0) {
        //downgrade
        m_mtx.unlock();
        m_mtx.lock_shared();
        //stop
        return;
    }
    //else, just unlock
    m_mtx.unlock();
}

void GLGE::RecursiveThreadMutexShared::waitForUpgradeOwnership() {
    //iterate while the upgrade thread token is not this thread
    while (true) {
        uint64_t exp{};
        if (m_upgradingThread.compare_exchange_weak(exp, t_threadToken, std::memory_order_acq_rel)) 
        {return;}
        //if not acquired, yield
        std::this_thread::yield();
    }
}
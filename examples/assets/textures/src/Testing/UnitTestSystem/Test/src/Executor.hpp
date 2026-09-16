/**
 * @file Executor.hpp
 * @author DM8AT
 * @brief define what a test executor is internally
 * @version 0.1
 * @date 2026-05-19
 * 
 * @copyright Copyright (c) 2026
 * 
 */
//header guard
#pragma once

//add the test contract
#include "TestContract.h"
//add the test execution frontend
#include "TestExecutor.h"

//add helpers
#include "Helper.hpp"
//add tests
#include "Test.hpp"

//add vectors
#include <vector>

//to get information about the thread count
#include <thread>
//used for lock-free syncing
#include <atomic>
//used for thread syncing
#include <mutex>
//used for interrupted-based sleeping
#include <condition_variable>
//for time stuff
#include <chrono>

//add the dynamic library helper
#include "OSAbstraction/DynamicLibrary.hpp"

/**
 * @brief a namespace for the implementation
 */
namespace TestImpl {

/**
 * @brief define what an executor is
 */
class Executor {
public:

    /**
     * @brief Construct a new Executor
     * 
     * This creates the executor in an invalid state. Use `initialize` to setup the state. 
     */
    Executor() = default;

    /**
     * @brief Destroy the Executor
     */
    ~Executor() {
        //request shutdown
        m_running.store(false, std::memory_order_relaxed);
        //notify all that they should stop
        m_queueCv.notify_all();
        //wait for all tests to finish
        (void) waitIdle(UINT64_MAX);
        //if something did not finish, just also delete it

        //make sure to join back all threads
        for (auto& thread : m_worker) 
        {thread.join();}
        m_worker.clear();

        //clean up the temporary directory
        if (std::filesystem::is_directory(m_tmpDir))
        {std::filesystem::remove_all(m_tmpDir);}

        //free all remaining tests
        for (size_t i = 0; i < m_tests.size(); ++i) 
        {delete m_tests[i];}
    }

    TestExecResult initialize(const TestExecExecutorCreateInfo* pInfo) {
        //make sure the capabilities include only valid bits
        if (pInfo->capabilities & ~(
            TEST_EXEC_CAPABILITY_ASYNC_BIT | TEST_EXEC_CAPABILITY_FILE_IO_BIT | 
            TEST_EXEC_CAPABILITY_GPU_BIT | TEST_EXEC_CAPABILITY_NETWORK_BIT
        )) {
            //invalid bits detected
            return TEST_EXEC_INVALID_INPUT;
        }
        //store the valid flags
        m_capabilities = pInfo->capabilities;


        //check that the scheduler strategy is valid
        switch (pInfo->schedulerStrategy) {
        case TEST_EXEC_EXECUTOR_SCHEDULER_FIFO:
        case TEST_EXEC_EXECUTOR_SCHEDULER_FIFO_WEIGHTED:
            m_scheduleStrategy = pInfo->schedulerStrategy;
            break;
        
        default:
            //unknown mode
            return TEST_EXEC_INVALID_INPUT;
            break;
        }

        //load the root dir
        if (pInfo->rootDir == NULL)
        {m_rootDir = std::filesystem::current_path();}
        else {
            //load the inputted path and make it absolute
            m_rootDir = std::filesystem::absolute(pInfo->rootDir);
        }
        //load the temporary directory
        if (pInfo->tmpDir == NULL)
        {m_tmpDir = m_rootDir / "tmp";}
        else {
            //store the inputted path
            m_tmpDir = pInfo->tmpDir;
            //if the path is relative, it is in relation to the rootDir. 
            //make it to an absolute path
            if (m_tmpDir.is_relative()) 
            {m_tmpDir = m_rootDir / m_tmpDir;}
        }

        //make sure both directories exist
        if (!std::filesystem::is_directory(m_rootDir)) {
            if (!std::filesystem::create_directory(m_rootDir))
            {return TEST_EXEC_DIRECTORY_CREATE_ERROR;}
        }
        if (!std::filesystem::is_directory(m_tmpDir)) {
            if (!std::filesystem::create_directories(m_tmpDir))
            {return TEST_EXEC_DIRECTORY_CREATE_ERROR;}
        }

        //check the path lengths
        if (m_rootDir.string().size() >= (TEST_MAX_FILE_CHAR_COUNT-1))
        {return TEST_EXEC_FILE_LOAD_ERROR;}
        if (m_tmpDir.string().size() >= (TEST_MAX_FILE_CHAR_COUNT-1))
        {return TEST_EXEC_FILE_LOAD_ERROR;}

        //store the amount of tests to run in parallel at maximum
        if (pInfo->maxParallelTests == 0)
        {m_maxParallelTests = std::thread::hardware_concurrency();}
        else
        {m_maxParallelTests = pInfo->maxParallelTests;}

        //activate the executer
        m_running = true;
        //create all threads
        TestExecResult res = resizeVector(m_worker, m_maxParallelTests);
        if (res != TEST_EXEC_SUCCESS) {return res;}
        //run the worker function on all threads
        for (auto& thread : m_worker)
        {thread = std::thread(&Executor::workerTask, this);}

        //prepare the context
        memcpy(const_cast<char*>(m_ctx.rootDir), m_rootDir.c_str(), m_rootDir.string().size());
        memcpy(const_cast<char*>(m_ctx.tmpDir),  m_tmpDir.c_str(),  m_tmpDir.string().size());
        m_ctx.randomSeed = 0; //very random

        //success
        return TEST_EXEC_SUCCESS;
    }

    /**
     * @brief enumerate all loaded tests
     * 
     * @param pTestCount a pointer to an `uint64_t`. If `pTests` is `NULL`, the test count will be stored there. If `pTests` is not `NULL`, this says how many elements to pull. 
     * @param pTests a pointer to an array with `pTestCount` elements or `NULL` to query the test count
     * @param `TestExecResult` `TEST_EXEC_SUCCESS` on success, an error code on failure
     */
    TestExecResult enumerateTests(
        uint64_t* pTestCount,
        TestExecTest* pTests
    ) {
        //`pTestCount` shall not be null
        if (pTestCount == NULL)
        {return TEST_EXEC_INVALID_INPUT;}

        //if `pTests` is null, write the test count
        if (pTests == NULL) 
        {*pTestCount = static_cast<uint64_t>(m_tests.size());}
        else {
            //else, write the tests there
            for (size_t i = 0; i < *pTestCount; ++i)
            {pTests[i] = reinterpret_cast<::TestExecTest>(m_tests[i]);}
        }

        //success
        return TEST_EXEC_SUCCESS;
    }

    /**
     * @brief try to load the test from a dynamic library
     * 
     * @param path a path to the file to load from
     * @param `TestExecResult` `TEST_EXEC_SUCCESS` on success, an error code on failure
     */
    TestExecResult load(const char* path) {
        //add the correct file suffix
        std::string sPath = 
        #ifdef _WIN32
            std::string(path) +  ".dll"
        #elif __APPLE__
            std::string(path) + ".dylib"
        #else
            std::filesystem::path(path).remove_filename() / (std::filesystem::path(path).filename().string());
        #endif
        ;
        //make the path absolute
        std::filesystem::path absPath = std::filesystem::absolute(sPath);
        //check if the file exists
        if (!std::filesystem::is_regular_file(absPath))
        {return TEST_EXEC_FILE_NOT_FOUND;}
        //create the library
        DynamicLibrary lib;
        //try to load the library
        if (!lib.open(absPath))
        {return TEST_EXEC_FILE_LOAD_ERROR;}

        //make sure the file has a contract entry point
        auto regFn = lib.symbol<PFN_RegisterTest>(TEST_REGISTER_TEST_SYM);
        //sanity check if the function exists
        if (!regFn.second) {return TEST_EXEC_FILE_LOAD_ERROR;}

        //register the test
        uint64_t testCount = 0;
        (*regFn.first)(&testCount, NULL);
        //allocate enough test space
        std::vector<::Test> tests(testCount);
        (*regFn.first)(&testCount, tests.data());
        //try to load all tests
        size_t startIdx = m_tests.size();
        m_tests.resize(m_tests.size() + tests.size());
        for (size_t i = 0; i < tests.size(); ++i) {
            const auto& test = tests[i];
            m_tests[startIdx + i] = new Test();
            TestExecResult res = m_tests[startIdx + i]->initialize(
                std::filesystem::absolute(sPath),
                test.entry.name,
                test.entry.tags,
                test.entry.description,
                test.entry.timeout,
                test.entry.requirements
            );
            //sanity check the load
            if (res != TEST_EXEC_SUCCESS)
            {return res;}
        }

        //success
        return TEST_EXEC_SUCCESS;
    }

    /**
     * @brief destroy a specific test
     * 
     * @param pTest a pointer to the test to destroy
     * @param `TestExecResult` `TEST_EXEC_SUCCESS` on success, an error code on failure
     */
    TestExecResult destroyTest(Test* pTest) {
        //if the test is `NULL`, this is valid. 
        if (pTest == NULL)
        {return TEST_EXEC_SUCCESS;}

        //lookup the test
        for (size_t i = 0; i < m_tests.size(); ++i) {
            if (m_tests[i] == pTest) {
                //clean up the test
                delete m_tests[i];
                m_tests.erase(m_tests.begin() + i);
                //success
                return TEST_EXEC_SUCCESS;
            }
        }

        //if the test was not found, the handle was invalid
        return TEST_EXEC_INVALID_HANDLE;
    }

    /**
     * @brief wait until the executor is idle
     * 
     * @param timeout the amount of milliseconds to wait before timing out
     * @return `TestExecResult` `TEST_EXEC_SUCCESS` on success and `TEST_EXEC_GENERIC_ERROR` on timeout
     */
    TestExecResult waitIdle(uint64_t timeout) {
        //give the tests some time to start
        std::this_thread::sleep_for(std::chrono::milliseconds(1));
        //obtain a lock on the active task count
        std::unique_lock lock(m_currentActiveMtx);
        auto max_timeout = std::chrono::milliseconds::max();
        uint64_t cappedTimeout = (timeout > max_timeout.count()) ? max_timeout.count() : timeout;
        //wait for the count to be 0
        bool success = m_currentActiveCv.wait_for(lock, std::chrono::milliseconds(cappedTimeout), [&]{return (
            (m_currentlyActiveTasks.load(std::memory_order_acquire)==0) && 
            (m_inUse.activeTestCount == 0) &&
            (m_queuedTests.empty())
        );});
        //return if the wait was successful
        return (success ? TEST_EXEC_SUCCESS : TEST_EXEC_GENERIC_ERROR);
    }

    /**
     * @brief a function to schedule a test for execution
     * 
     * @param pExecuteTestInfo a pointer to the test to execution info
     * @return `TestExecResult` the result of the scheduling
     */
    TestExecResult executeTest(const TestExecExecuteTestInfo* pExecuteTestInfo) {
        //sanity check that the pointer exists
        if (pExecuteTestInfo == NULL) {return TEST_EXEC_INVALID_INPUT;}

        //make sure the test exists
        if (pExecuteTestInfo->test == TEST_EXEC_NULL_HANDLE) {return TEST_EXEC_INVALID_HANDLE;}

        //convert the test
        auto* t = reinterpret_cast<TestImpl::Test*>(pExecuteTestInfo->test);

        //make sure the test can even run
        if (t->getRequiredCapabilities() & ~m_capabilities) {
            t->skip();
            return TEST_EXEC_SUCCESS;
        }

        //set the log path of the test
        t->setLogPath(pExecuteTestInfo->logPath, m_rootDir);
        
        //schedule the test
        t->setWeight(pExecuteTestInfo->priority);
        {
        std::unique_lock lock(m_queueMtx);
        t->setState(TEST_EXEC_TEST_STATE_SCHEDULED);
        m_queuedTests.push_back(t);
        m_queueCv.notify_one();
        }

        //success
        return TEST_EXEC_SUCCESS;
    }

    /**
     * @brief Get the state of a test
     * 
     * It is fully safe to parse ANY value as pointer. If the test is not known by the executor, it is treated as invalid. 
     * 
     * @param test a pointer to the test to query the state from. The test must belong to this executor. 
     * @return `TestExecTestState` the state of the test, unknown 
     */
    inline TestExecTestState getTestState(TestImpl::Test* test) {
        //try to find the test
        bool found = false;
        for (size_t i = 0; i < m_tests.size(); ++i)
        {if (m_tests[i] == test) {found = true; break;}}
        //if not found -> unknown
        if (!found) {return TEST_EXEC_TEST_STATE_UNKNOWN;}

        //else, query the test state
        return test->getState();
    }

protected:

    /**
     * @brief check if a set of flags belonging to a task is currently runnable
     * 
     * This function assumes that the thread is holding a lock on the vector `m_queuedTests`. 
     * 
     * @param flags the flags to check
     * @return `true` if the flag set is currently runnable, `false` if not
     */
    bool isCurrentlyRunnable(TestRequirementFlags flags) {
        //check global exclusivity
        if (flags & TEST_REQUIREMENT_RUN_EXCLUSIVE_BIT) 
        {if (m_inUse.activeTestCount != 0) {return false;}}
        if (m_inUse.activeExclusiveUsers != 0) {return false;}

        //check GPU access
        if (flags & TEST_REQUIREMENT_GPU_ACCESS_EXCLUSIVE_BIT)
        {if ((m_inUse.activeGPUTests != 0) || (m_inUse.activeGPUExclusiveTests != 0)) {return false;}}
        else if (flags & TEST_REQUIREMENT_GPU_BIT)
        {if (m_inUse.activeGPUExclusiveTests != 0) {return false;}}

        //check file access
        if (flags & TEST_REQUIREMENT_FILE_ACCESS_EXCLUSIVE_BIT)
        {if ((m_inUse.activeFileTests != 0) || (m_inUse.activeFileExclusiveTests != 0)) {return false;}}
        else if (flags & TEST_REQUIREMENT_FILE_ACCESS_BIT)
        {if (m_inUse.activeFileExclusiveTests != 0) {return false;}}

        //check network access
        if (flags & TEST_REQUIREMENT_NETWORKING_EXCLUSIVE_BIT)
        {if ((m_inUse.activeNetworkTests != 0) || (m_inUse.activeNetworkExclusiveTests != 0)) {return false;}}
        else if (flags & TEST_REQUIREMENT_NETWORKING_BIT)
        {if (m_inUse.activeNetworkExclusiveTests != 0) {return false;}}

        //check ram test
        if (flags & TEST_REQUIREMENT_RAM_EXCLUSIVE_BIT)
        {if (m_inUse.activeRAMExclusiveCount != 0) {return false;}}

        //the test can be run
        return true;
    }

    /**
     * @brief mark a task as currently running based on the flags
     * 
     * This function assumes that the thread is holding a lock on the vector `m_queuedTests`. 
     * 
     * @param flags the flags to mark running
     */
    void registerRunning(TestRequirementFlags flags) {
        //increase the in use count
        m_currentlyActiveTasks.fetch_add(1, std::memory_order_release);

        //increase the amount of running tests
        ++m_inUse.activeTestCount;

        //exclusive tests
        if (flags & TEST_REQUIREMENT_RUN_EXCLUSIVE_BIT)
        {++m_inUse.activeExclusiveUsers;}

        //GPU
        if (flags & TEST_REQUIREMENT_GPU_ACCESS_EXCLUSIVE_BIT)
        {++m_inUse.activeGPUExclusiveTests;}
        if (flags & TEST_REQUIREMENT_GPU_BIT)
        {++m_inUse.activeGPUTests;}

        //File I/O
        if (flags & TEST_REQUIREMENT_FILE_ACCESS_EXCLUSIVE_BIT)
        {++m_inUse.activeFileExclusiveTests;}
        if (flags & TEST_REQUIREMENT_FILE_ACCESS_BIT)
        {++m_inUse.activeFileTests;}

        //Networking
        if (flags & TEST_REQUIREMENT_NETWORKING_EXCLUSIVE_BIT)
        {++m_inUse.activeNetworkExclusiveTests;}
        if (flags & TEST_REQUIREMENT_NETWORKING_BIT)
        {++m_inUse.activeNetworkTests;}

        //RAM
        if (flags & TEST_REQUIREMENT_RAM_EXCLUSIVE_BIT)
        {++m_inUse.activeRAMExclusiveCount;}
    }

    /**
     * @brief mark a task as finished running based on the flags
     * 
     * This function assumes that the thread is holding a lock on the vector `m_queuedTests`. 
     * 
     * @param flags the flags to mark finished
     */
    void removeRunning(TestRequirementFlags flags) {
        //increase the in use count
        m_currentlyActiveTasks.fetch_sub(1, std::memory_order_release);

        //decrease the amount of running tests
        --m_inUse.activeTestCount;

        //exclusive tests
        if (flags & TEST_REQUIREMENT_RUN_EXCLUSIVE_BIT)
        {--m_inUse.activeExclusiveUsers;}

        //GPU
        if (flags & TEST_REQUIREMENT_GPU_ACCESS_EXCLUSIVE_BIT)
        {--m_inUse.activeGPUExclusiveTests;}
        if (flags & TEST_REQUIREMENT_GPU_BIT)
        {--m_inUse.activeGPUTests;}

        //File I/O
        if (flags & TEST_REQUIREMENT_FILE_ACCESS_EXCLUSIVE_BIT)
        {--m_inUse.activeFileExclusiveTests;}
        if (flags & TEST_REQUIREMENT_FILE_ACCESS_BIT)
        {--m_inUse.activeFileTests;}

        //Networking
        if (flags & TEST_REQUIREMENT_NETWORKING_EXCLUSIVE_BIT)
        {--m_inUse.activeNetworkExclusiveTests;}
        if (flags & TEST_REQUIREMENT_NETWORKING_BIT)
        {--m_inUse.activeNetworkTests;}

        //RAM
        if (flags & TEST_REQUIREMENT_RAM_EXCLUSIVE_BIT)
        {--m_inUse.activeRAMExclusiveCount;}
    }

    /**
     * @brief Get the next runnable test using simple Fifo for scheduling
     * 
     * This function assumes that the thread is holding a lock on the vector `m_queuedTests`. 
     * 
     * @return `TestImpl::Test*` a pointer to the next runnable test or `nullptr` if none was found
     */
    TestImpl::Test* getRunnableTest_Fifo() {
        //iterate over all queued tests linearly
        for (size_t i = 0; i < m_queuedTests.size(); ++i) {
            //check if the current test can be run
            bool runnable = isCurrentlyRunnable(m_queuedTests[i]->getRequirements());

            if (runnable) {
                //fifo -> priority is ignored
                auto* ret = m_queuedTests[i];

                //remove the task from the queue
                m_queuedTests.erase(m_queuedTests.begin() + i);
                //return the test
                return ret;
            }
        }

        //no task was found
        return nullptr;
    }

    /**
     * @brief Get the Runnable test using a simple fifo with weighting for scheduling
     * 
     * This function assumes that the thread is holding a lock on the vector `m_queuedTests`. 
     * 
     * @return `TestImpl::Test*` a pointer to the next runnable test or `nullptr` if none was found
     */
    TestImpl::Test* getRunnableTest_FifoWeighted() {
        //store the currently best task
        TestImpl::Test* best = nullptr;
        size_t bestId = SIZE_MAX;

        //iterate over all queued tests linearly
        for (size_t i = 0; i < m_queuedTests.size(); ++i) {
            //check if the current test can be run
            bool runnable = isCurrentlyRunnable(m_queuedTests[i]->getRequirements());

            if (runnable) {
                //if no task is currently the best, this is now the best
                if (!best) {
                    best = m_queuedTests[i];
                    bestId = 0;
                }

                //else, compare the weights
                if (best->getWeight() < m_queuedTests[i]->getWeight()) {
                    //this is now the best test to run
                    best = m_queuedTests[i];
                    bestId = i;
                }
            }
        }

        //if a test was found, de-queue it
        if (bestId != SIZE_MAX) 
        {m_queuedTests.erase(m_queuedTests.begin() + bestId);}

        //return the best task
        return best;
    }

    /**
     * @brief Get the next runnable test
     * 
     * This function assumes that the thread is holding a lock on the vector `m_queuedTests`. 
     * 
     * @return `TestImpl::Test*` a pointer to the selected test. `nullptr` if no currently runnable test exists. 
     */
    TestImpl::Test* getRunnableTest() {
        //switch over the scheduler systems
        switch (m_scheduleStrategy) {
        case TEST_EXEC_EXECUTOR_SCHEDULER_FIFO: 
            return getRunnableTest_Fifo();
            break;
        case TEST_EXEC_EXECUTOR_SCHEDULER_FIFO_WEIGHTED:
            return getRunnableTest_FifoWeighted();
            break;
        
        default:
            //none found
            return nullptr;
            break;
        }
    }

    /**
     * @brief the function that runs on the worker threads
     */
    void workerTask() {
        //loop until the worker should join
        while (m_running.load(std::memory_order_relaxed)) {
            //store the currently running test
            TestImpl::Test* current = nullptr;

            //sleep and get the test
            {
                std::unique_lock lock(m_queueMtx);
                m_queueCv.wait(lock, [&]{return !m_running.load(std::memory_order_relaxed) || (m_queuedTests.size()>0);});

                //early-out on running stopped
                if (!m_running.load(std::memory_order_relaxed)) {return;}

                //get the next runnable test
                current = getRunnableTest();

                //if a test was gotten, mark it as running
                if (current) {registerRunning(current->getRequirements());}
            }

            //if a test was gotten, run it
            if (current) {
                //actually run the test
                current->executeTest(&m_ctx);

                //mark the test as done
                {
                    std::unique_lock lock(m_queueMtx);
                    removeRunning(current->getRequirements());
                    current = nullptr;
                }

                //notify about done
                m_currentActiveCv.notify_all();
            }
        }
    }

    /**
     * @brief store the context
     */
    TestContext m_ctx;

    /**
     * @brief store if the system is running
     */
    std::atomic_bool m_running = false;

    /**
     * @brief store the capabilites of the executor
     */
    TestExecCapabilityFlags m_capabilities;

    /**
     * @brief store all the tests
     * 
     * Tests are stored as pointer because they need memory stability since the handle is just a pointer
     */
    std::vector<TestImpl::Test*> m_tests;

    /**
     * @brief store the test executor scheduler strategy
     */
    TestExecExecutorScheduler m_scheduleStrategy = TEST_EXEC_EXECUTOR_SCHEDULER_MAX;

    /**
     * @brief store the local root of execution
     */
    std::filesystem::path m_rootDir;
    /**
     * @brief store a temporary that the tests can use
     */
    std::filesystem::path m_tmpDir;

    /**
     * @brief store the amount of tests to run in parallel at maximum
     */
    uint32_t m_maxParallelTests = 0;

    /**
     * @brief store all tests that are scheduled, but not currently running
     */
    std::vector<TestImpl::Test*> m_queuedTests;
    /**
     * @brief store the queue mutex
     */
    std::mutex m_queueMtx;
    /**
     * @brief store a conditional variable the workers use for interrupt-based sleeping
     */
    std::condition_variable m_queueCv;

    /**
     * @brief store all the worker threads
     */
    std::vector<std::thread> m_worker;

    /**
     * @brief store information about how many jobs currently use which system
     */
    struct CurrentlyUsed {
        uint32_t activeTestCount = 0;
        uint32_t activeExclusiveUsers = 0;
        uint32_t activeGPUTests = 0;
        uint32_t activeGPUExclusiveTests = 0;
        uint32_t activeFileTests = 0;
        uint32_t activeFileExclusiveTests = 0;
        uint32_t activeNetworkTests = 0;
        uint32_t activeNetworkExclusiveTests = 0;
        uint32_t activeRAMExclusiveCount = 0;
    } m_inUse;

    /**
     * @brief store the amount of currently active tasks
     */
    std::atomic_uint32_t m_currentlyActiveTasks;
    /**
     * @brief a mutex used to wait for the current active task conditional variable
     */
    std::mutex m_currentActiveMtx;
    /**
     * @brief a variable used for interrupt-based sleeping for the current active task count
     */
    std::condition_variable m_currentActiveCv;

};

}
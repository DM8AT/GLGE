/**
 * @file TestExecImpl.cpp
 * @author DM8AT
 * @brief implement all the test executor API functions
 * @version 0.1
 * @date 2026-05-20
 * 
 * @copyright Copyright (c) 2026
 * 
 */
//add the C++ implementations
#include "Executor.hpp"
#include "Test.hpp"

//all functions are for C
extern "C" {

/**
 * @brief create a test executor
 * 
 * @param pCreateInfo a pointer to the test executor create info
 * @param pExecutor a pointer to an opaque test executor handle
 * @return `TestExecResult` `TEST_EXEC_SUCCESS` on success, an error code on failure
 */
TestExecResult TEST_EXEC_CALL testExecCreateExecutor(
    const TestExecExecutorCreateInfo* pCreateInfo,
    TestExecExecutor* pExecutor
) {
    //sanity check the create info
    if (pCreateInfo == NULL) {return TEST_EXEC_INVALID_INPUT;}
    if (pCreateInfo->header.sType != TEST_EXEC_TYPE_TEST_EXECUTOR_CREATE_INFO) {return TEST_EXEC_INVALID_TYPE;}

    //create an executor and initialize it
    TestImpl::Executor* exec = new TestImpl::Executor();
    if (exec == nullptr) {return TEST_EXEC_OUT_OF_MEMORY;}
    TestExecResult res = exec->initialize(pCreateInfo);
    *pExecutor = reinterpret_cast<TestExecExecutor>(exec);
    return res;
}

/**
 * @brief destroy a test executor
 * 
 * if `executor` is invalid nothing happens
 * 
 * @param executor the test executor to destroy
 */
void TEST_EXEC_CALL testExecDestroyExecutor(
    TestExecExecutor executor
) {
    //just cast and delete
    //if `executor == nullptr`, it is still valid since `nullptr` can legally be passed into delete
    delete reinterpret_cast<TestImpl::Executor*>(executor);
}

/**
 * @brief enumerate all tests an executor instance currently stores
 * 
 * @param executor the executor to get the tests from
 * @param pTestCount a pointer to an `uint64_t`. If `pTests` is `NULL`, the test count will be stored there. If `pTests` is not `NULL`, this says how many elements to pull. 
 * @param pTests a pointer to an array with `pTestCount` elements or `NULL` to query the test count
 * @return `TestExecResult` `TEST_EXEC_SUCCESS` on success, an error code on failure
 */
TestExecResult TEST_EXEC_CALL testExecEnumerateExecutorTests(
    TestExecExecutor executor, 
    uint64_t* pTestCount,
    TestExecTest* pTests
) {
    //check that the executor is valid
    if (executor == TEST_EXEC_NULL_HANDLE)
    {return TEST_EXEC_INVALID_HANDLE;}
    //else, quarry the data
    return reinterpret_cast<TestImpl::Executor*>(executor)->enumerateTests(pTestCount, pTests);
}

/**
 * @brief a function to wait until a specific executor is idle
 * 
 * @param executor the executor to wait until idle
 * @param timeout the amount of milliseconds before timeout is triggered
 * @return `TestExecResult` `TEST_EXEC_SUCCESS` on success, an error code on failure
 */
TestExecResult TEST_EXEC_CALL testExecWaitExecutorIdle(
    TestExecExecutor executor, 
    uint64_t timeout
) {
    //make sure the executor is valid
    if (executor == NULL) {return TEST_EXEC_INVALID_HANDLE;}
    //valid -> just invoke the sleep function
    return reinterpret_cast<TestImpl::Executor*>(executor)->waitIdle(timeout);
}


/**
 * @brief load a test from a specific file
 * 
 * This function loads between 0 and n tests into the executor. Query the test handles using 
 * `testExecEnumerateExecutorTests`. 
 * 
 * @param executor the executor to load to
 * @param pLoadTestInfo a pointer to a structure that describes how to load a test
 * @return `TestExecResult` `TEST_EXEC_SUCCESS` on success, an error code on failure
 */
TestExecResult TEST_EXEC_CALL testExecLoadTests(
    TestExecExecutor executor, 
    const TestExecLoadTestInfo* pLoadTestInfo
) {
    //sanity check pLoadTestInfo
    if (pLoadTestInfo == NULL) {return TEST_EXEC_INVALID_INPUT;}
    if (pLoadTestInfo->header.sType != TEST_EXEC_TYPE_LOAD_TEST_INFO) {return TEST_EXEC_INVALID_TYPE;}

    //make sure the executor is valid
    if (executor == NULL) {return TEST_EXEC_INVALID_HANDLE;}
    //valid -> just load
    return reinterpret_cast<TestImpl::Executor*>(executor)->load(pLoadTestInfo->path);
}

/**
 * @brief unload a specific test
 * 
 * if `test` is invalid, nothing happens
 * 
 * @param executor the executor the test belongs to
 * @param test the test to free
 * @return `TestExecResult` `TEST_EXEC_SUCCESS` on success, an error code on failure
 */
TestExecResult TEST_EXEC_CALL testExecDestroyTest(
    TestExecExecutor executor,
    TestExecTest test
) {
    //make sure the executor is valid
    if (executor == NULL) {return TEST_EXEC_INVALID_HANDLE;}
    //valid -> destroy the test
    return reinterpret_cast<TestImpl::Executor*>(executor)->destroyTest(reinterpret_cast<TestImpl::Test*>(test));
}

/**
 * @brief execute a single test
 * 
 * If async is enabled this function returns immediately, else this function blocks until the test finishes. 
 * 
 * @param executor the executor to execute the test on
 * @param pExecuteTestInfo a pointer to a structure that holds information on how to execute the test
 * @return `TestExecResult` `TEST_EXEC_SUCCESS` on success, an error code on failure
 */
TestExecResult TEST_EXEC_CALL testExecExecuteTest(
    TestExecExecutor executor,
    const TestExecExecuteTestInfo* pExecuteTestInfo
) {
    //sanity check pExecuteTestInfo
    if (pExecuteTestInfo == NULL) {return TEST_EXEC_INVALID_INPUT;}
    if (pExecuteTestInfo->header.sType != TEST_EXEC_TYPE_EXECUTE_TEST_INFO) {return TEST_EXEC_INVALID_TYPE;}

    //make sure the executor is valid
    if (executor == NULL) {return TEST_EXEC_INVALID_HANDLE;}
    //valid -> invoke the scheduler
    return reinterpret_cast<TestImpl::Executor*>(executor)->executeTest(pExecuteTestInfo);
}

/**
 * @brief get the result of a test
 * 
 * @param executor the executor the test belongs to
 * @param test the test to query the result for
 * @param result a structure to fill with the test result
 * @return `TestExecResult` `TEST_EXEC_SUCCESS` on success, an error code on failure
 */
TestExecResult TEST_EXEC_CALL testExecGetTestResult(
    TestExecExecutor executor,
    TestExecTest test, 
    TestExecTestResultInfo* result
) {
    //sanity check result
    if (result == NULL) {return TEST_EXEC_INVALID_INPUT;}
    if (result->header.sType != TEST_EXEC_TYPE_TEST_RESULT_INFO) {return TEST_EXEC_INVALID_TYPE;}

    //check if the test is valid
    if (test == NULL) {return TEST_EXEC_INVALID_HANDLE;}

    //write the result
    result->result = reinterpret_cast<TestImpl::Test*>(test)->getExecResult();
    //write the return value
    result->returnValue = reinterpret_cast<TestImpl::Test*>(test)->getReturnValue();

    //success
    return TEST_EXEC_SUCCESS;
}

/**
 * @brief get the current state of a test
 * 
 * @param executor the executor the test belongs to
 * @param test the test to query the stat for
 * @param pTestStateInfo a pointer to fill with the test state
 * @return `TestExecResult` `TEST_EXEC_SUCCESS` on success, an error code on failure
 */
TestExecResult TEST_EXEC_CALL testExecGetTestState(
    TestExecExecutor executor,
    TestExecTest test,
    TestExecTestStateInfo* pTestStateInfo
) {
    //sanity check the pTestStateInfo
    if (pTestStateInfo == NULL) {return TEST_EXEC_INVALID_INPUT;}
    if (pTestStateInfo->header.sType != TEST_EXEC_TYPE_TEST_STATE_INFO) {return TEST_EXEC_INVALID_TYPE;}

    //sanity check the executor
    if (executor == NULL) {return TEST_EXEC_INVALID_HANDLE;}
    //write the state
    pTestStateInfo->state = reinterpret_cast<TestImpl::Executor*>(executor)->getTestState(reinterpret_cast<TestImpl::Test*>(test));
    //success
    return TEST_EXEC_SUCCESS;
}

/**
 * @brief get test metadata
 * 
 * @param executor the executor the test belongs to
 * @param test the test to query the metadata for
 * @param pMetadata a pointer to a structure to fill with the metadata
 * @return `TestExecResult` `TEST_EXEC_SUCCESS` on success, an error code on failure
 */
TestExecResult TEST_EXEC_CALL testExecGetTestMetadata(
    TestExecExecutor executor,
    TestExecTest test,
    TestExecTestMetadata* pMetadata
) {
    //sanity check the pMetadata
    if (pMetadata == NULL) {return TEST_EXEC_INVALID_INPUT;}
    if (pMetadata->header.sType != TEST_EXEC_TYPE_TEST_METADATA) {return TEST_EXEC_INVALID_TYPE;}

    //sanity check the executor
    if (executor == NULL) {return TEST_EXEC_INVALID_HANDLE;}
    //check if the test is valid
    if (test == NULL) {return TEST_EXEC_INVALID_HANDLE;}
    //get the test metadata
    return reinterpret_cast<TestImpl::Test*>(test)->fillMetadata(pMetadata);
}

}
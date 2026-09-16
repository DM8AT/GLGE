/**
 * @file main.cpp
 * @author DM8AT
 * @brief the main file
 * @version 0.1
 * @date 2026-05-19
 * 
 * @copyright Copyright (c) 2026
 * 
 */
#include <iostream>
#include <vector>

//add the unit testing API
#include "TestExecutor.h"

int main() {
    //create an executor
    TestExecExecutorCreateInfo execCreate;
    execCreate.rootDir = "root";
    execCreate.tmpDir = "tmp";
    execCreate.schedulerStrategy = TEST_EXEC_EXECUTOR_SCHEDULER_FIFO_WEIGHTED;
    execCreate.capabilities = TEST_EXEC_CAPABILITY_ASYNC_BIT | TEST_EXEC_CAPABILITY_FILE_IO_BIT | TEST_EXEC_CAPABILITY_GPU_BIT | TEST_EXEC_CAPABILITY_NETWORK_BIT;
    TestExecExecutor executor = TEST_EXEC_NULL_HANDLE;
    if (testExecCreateExecutor(&execCreate, &executor) != TEST_EXEC_SUCCESS)
    {throw std::runtime_error("Failed to create an executor");}

    //load a test bundle
    TestExecLoadTestInfo loadInfo;
    loadInfo.path = "TestLib/Test1";
    if (testExecLoadTests(executor, &loadInfo) != TEST_EXEC_SUCCESS)
    {throw std::runtime_error("Failed to load the requested test");}

    //get all tests loaded by the test bundle
    uint64_t testCount = 0;
    testExecEnumerateExecutorTests(executor, &testCount, nullptr);
    if (testCount == 0) {return 0;}
    std::vector<TestExecTest> tests(testCount);
    testExecEnumerateExecutorTests(executor, &testCount, tests.data());

    //print the amount of discovered tests
    std::cout << testCount << " test(s) loaded" << "\n";

    //select the first test to execute
    TestExecExecuteTestInfo execInfo;
    execInfo.priority = 0;
    execInfo.test = tests[0];
    testExecExecuteTest(executor, &execInfo);

    //wait until the test was executed
    testExecWaitExecutorIdle(executor, UINT64_MAX);

    //print the return value of the test
    TestExecTestResultInfo report;
    testExecGetTestResult(executor, tests[0], &report);
    std::cout << "Test finished with exit code " << report.returnValue << "\n";

    //clean up
    testExecDestroyExecutor(executor);
}
/**
 * @file TestLauncher.cpp
 * @author DM8AT
 * @brief implement a interface for the test launcher
 * @version 0.1
 * @date 2026-05-24
 * 
 * @copyright Copyright (c) 2026
 * 
 */
//add the header
#include "TestLauncher.h"
//add the test framework executor
#include "TestExecutor.h"

//add filesystem API
#include <filesystem>
//used for default i/o
#include <iostream>
//add vectors and strings
#include <vector>
#include <string>
//add string streams
#include <sstream>
//add maps
#include <map>

int launchUnitTests() {
    //configure root and temp dir
    auto rootDir = std::filesystem::current_path() / "root";
    auto tmpDir = std::filesystem::current_path() / "tmp";
    //create an executor
    TestExecExecutorCreateInfo execCreate;
    execCreate.capabilities = TEST_EXEC_CAPABILITY_ASYNC_BIT | TEST_EXEC_CAPABILITY_FILE_IO_BIT | TEST_EXEC_CAPABILITY_GPU_BIT | TEST_EXEC_CAPABILITY_NETWORK_BIT;
    execCreate.maxParallelTests = 16;
    execCreate.schedulerStrategy = TEST_EXEC_EXECUTOR_SCHEDULER_FIFO_WEIGHTED;
    execCreate.rootDir = rootDir.c_str();
    execCreate.tmpDir = tmpDir.c_str();
    TestExecExecutor executor = TEST_EXEC_NULL_HANDLE;
    if (testExecCreateExecutor(&execCreate, &executor) != TEST_EXEC_SUCCESS)
    {return 1;}

    //store all found test suites
    std::vector<std::filesystem::path> suites;
    //iterate over all tests to load (load from the folder "Tests")
    auto testPath = std::filesystem::current_path() / "Tests";
    for (const auto& entry : std::filesystem::directory_iterator(testPath)) {
        if (entry.path().extension() == 
            #ifdef _WIN32
            ".dll"
            #elifdef __APPLE__
            ".dylib"
            #else
            ".so"
            #endif
        ) {
            std::filesystem::path suiteName = entry.path();
            suiteName.replace_filename(entry.path().filename().stem().string());
            suites.push_back(suiteName);
        }
    }

    //try to load all tests
    std::vector<TestExecTest> tests;
    for (const auto& suite : suites) {
        TestExecLoadTestInfo loadInfo;
        loadInfo.path = suite.c_str();
        if (testExecLoadTests(executor, &loadInfo) != TEST_EXEC_SUCCESS)
        {throw std::runtime_error("Failed to load the test suite " + suite.string());}
    }
    uint64_t testCount = 0;
    testExecEnumerateExecutorTests(executor, &testCount, nullptr);
    tests.resize(testCount);
    testExecEnumerateExecutorTests(executor, &testCount, tests.data());

    //create a mapping from tag to a list of test IDs that have this tag
    std::map<std::string, std::vector<size_t>> tagIdMap;
    std::vector<std::string> testNames;
    for (size_t i = 0; i < tests.size(); ++i) {
        const auto& test = tests[i];
        //get the test metadata
        TestExecTestMetadata meta;
        testExecGetTestMetadata(executor, test, &meta);

        //convert the tag string into a vector
        std::stringstream tagStream(meta.tags);
        std::vector<std::string> tags;
        std::string singleTag;
        while (tagStream >> singleTag)
        {tags.push_back(singleTag);}

        //iterate over all found tags and add the test ID
        for (const auto& tag : tags)
        {tagIdMap[tag].push_back(i);}

        //add the test name
        testNames.push_back(meta.name);
    }
    //serialize the tag map
    std::vector<std::pair<const char*, std::vector<size_t>&>> tagIdSerialized;
    tagIdSerialized.reserve(tagIdMap.size());
    for (auto& [tag, ids] : tagIdMap) 
    {tagIdSerialized.push_back(std::pair<const char*, std::vector<size_t>&>(tag.c_str(), std::ref(ids)));}

    //get wether to run by tags or test name
    std::cout << "[0]     Run all loaded tests\n";
    std::cout << "[1]     Run a specific test based on its name\n";
    std::cout << "[2]     Run a specific subset of tests based on tags\n";
    size_t selectMode = SIZE_MAX;
    while (selectMode == SIZE_MAX) {
        try {
            //try to read a size_t (this may throw)
            selectMode = readSizeT();
        } catch (const std::invalid_argument& exception) {
            //input error - print and re-try
            std::cout << "----------------\n" << exception.what() << "\n----------------\n";
        } catch (const std::exception& exception) {
            //print the fatal error
            std::cout << "[FATAL ERROR] A fatal error was thrown during selection:\n" << exception.what() << "\n";
            //just stop
            return 0xff;
        }
        //sanity check the range
        if (selectMode > 2) {
            //print an error
            std::cout << "----------------\nPlease input either 0, 1 or 2\n----------------\n";
            selectMode = SIZE_MAX;
        }
    }

    //store a list of all tests running
    std::vector<TestExecTest> runningTests;

    //check for mode 0: All tests
    if (selectMode == 0) {
        //just schedule all tests
        runningTests.reserve(tests.size());
        for (const auto& test : tests) {
            TestExecExecuteTestInfo execInfo;
            execInfo.logPath = nullptr;
            execInfo.priority = 0;
            execInfo.test = test;
            testExecExecuteTest(executor, &execInfo);
            runningTests.push_back(test);
        }
    } else if (selectMode == 1) {
        //print all loaded tests
        std::cout << structureMapToString(testNames);
        //get which test to run
        size_t sel = SIZE_MAX;
        while (sel == SIZE_MAX) {
            try {
                //try to read a size_t (this may throw)
                sel = readSizeT();
            } catch (const std::invalid_argument& exception) {
                //input error - print and re-try
                std::cout << "----------------\n" << exception.what() << "\n----------------\n";
            } catch (const std::exception& exception) {
                //print the fatal error
                std::cout << "[FATAL ERROR] A fatal error was thrown during selection:\n" << exception.what() << "\n";
                //just stop
                return 0xff;
            }
            //range check
            if (sel >= testNames.size()) {
                //print an error
                std::cout << "----------------\nPlease input a value in range 0 to " << testNames.size()-1 << "\n----------------\n";
                sel = SIZE_MAX;
            }
        }

        //run the selected log
        const auto& test = tests[sel];
        runningTests.push_back(test);

        TestExecExecuteTestInfo execInfo;
        execInfo.logPath = nullptr;
        execInfo.priority = 0;
        execInfo.test = test;
        testExecExecuteTest(executor, &execInfo);
    } else if (selectMode == 2) {
        //print all loaded tags
        std::cout << structureMapToString(tagIdSerialized);

        //get which tag to run
        size_t sel = SIZE_MAX;
        while (sel == SIZE_MAX) {
            try {
                //try to read a size_t (this may throw)
                sel = readSizeT();
            } catch (const std::invalid_argument& exception) {
                //input error - print and re-try
                std::cout << "----------------\n" << exception.what() << "\n----------------\n";
            } catch (const std::exception& exception) {
                //print the fatal error
                std::cout << "[FATAL ERROR] A fatal error was thrown during selection:\n" << exception.what() << "\n";
                //just stop
                return 0xff;
            }
            //range check
            if (sel >= tagIdSerialized.size()) {
                //print an error
                std::cout << "----------------\nPlease input a value in range 0 to " << tagIdSerialized.size()-1 << "\n----------------\n";
                sel = SIZE_MAX;
            }
        }

        //get all tests with the tag
        auto& tagged = tagIdSerialized[sel].second;
        //and run the tests
        for (const auto& testId : tagged) {
            const auto& test = tests[testId];
            runningTests.push_back(test);

            TestExecExecuteTestInfo execInfo;
            execInfo.logPath = nullptr;
            execInfo.priority = 0;
            execInfo.test = test;
            testExecExecuteTest(executor, &execInfo);
        }
    }

    //wait for the tests to be done
    testExecWaitExecutorIdle(executor, UINT64_MAX);

    //all tests should now be done

    //if no tests are selected to run, just stop
    if (runningTests.size() == 0) {
        //clean up
        for (const auto& test : tests)
        {testExecDestroyTest(executor, test);}
        testExecDestroyExecutor(executor);
        return 0;
    }

    //for each test print the result
    std::cout << "----------------\n";

    //store how many tests resulted in what state
    size_t successCount = 0;
    size_t skippedCount = 0;
    size_t timeoutCount = 0;
    size_t warningCount = 0;
    size_t failedCount = 0;
    size_t uncontrolledFailedCount = 0;
    size_t unknownCount = 0;
    for (size_t i = 0; i < runningTests.size(); ++i) {
        const auto& test = runningTests[i];
        //get the name
        TestExecTestMetadata meta;
        testExecGetTestMetadata(executor, test, &meta);
        //get the result
        TestExecTestResultInfo resInfo;
        testExecGetTestResult(executor, test, &resInfo);

        std::cout << "Test \"" << meta.name << "\" resulted in: ";
        //add to what it finished as
        switch (resInfo.result) {
        case TEST_EXEC_TEST_RESULT_SUCCESS:           ++successCount;            std::cout << "success"; break;
        case TEST_EXEC_TEST_RESULT_SKIPPED:           ++skippedCount;            std::cout << "skipped"; break;
        case TEST_EXEC_TEST_RESULT_TIMED_OUT:         ++timeoutCount;            std::cout << "time out"; break;
        case TEST_EXEC_TEST_RESULT_WARNING:           ++warningCount;            std::cout << "warning"; break;
        case TEST_EXEC_TEST_RESULT_CONTROLLED_FAIL:   ++failedCount;             std::cout << "fail"; break;
        case TEST_EXEC_TEST_RESULT_UNCONTROLLED_FAIL: ++uncontrolledFailedCount; std::cout << "uncontrolled fail"; break;

        default: ++unknownCount; std::cout << "unknown"; break;
        }
        std::cout << "\n";
    }
    std::cout << "----------------\n";

    //print the test results
    std::cout << "Overall test results: \n";
    std::cout << "    Success:            " << successCount << ", " << (float(successCount) / float(runningTests.size()))*100.f << "%\n";
    std::cout << "    Skipped:            " << skippedCount << ", " << (float(skippedCount) / float(runningTests.size()))*100.f << "%\n";
    std::cout << "    Timed out:          " << timeoutCount << ", " << (float(timeoutCount) / float(runningTests.size()))*100.f << "%\n";
    std::cout << "    Warning:            " << warningCount << ", " << (float(warningCount) / float(runningTests.size()))*100.f << "%\n";
    std::cout << "    Fails:              " << failedCount  << ", " << (float(failedCount)  / float(runningTests.size()))*100.f << "%\n";
    std::cout << "    Uncontrolled Fails: " << uncontrolledFailedCount << ", " << (float(uncontrolledFailedCount) / float(runningTests.size()))*100.f << "%\n";

    //clean up
    for (const auto& test : tests)
    {testExecDestroyTest(executor, test);}
    testExecDestroyExecutor(executor);

    //success
    return 0;
}
/**
 * @file Test.hpp
 * @author DM8AT
 * @brief define the internal structure of a loaded test
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

//add the filesystem
#include <filesystem>
//add vectors
#include <vector>
//for replace
#include <algorithm>
//for printf debugging
#include <iostream>
//for file io
#include <fstream>

//add subprocesses
#include "OSAbstraction/Subprocess.hpp"
//add shared memory
#include "OSAbstraction/SharedMemory.hpp"

const char* RETURN_ENUM_STR[] = {
    "TEST_EXEC_TEST_RESULT_SUCCESS",
    "TEST_EXEC_TEST_RESULT_WARNING",
    "TEST_EXEC_TEST_RESULT_CONTROLLED_FAIL",
    "TEST_EXEC_TEST_RESULT_UNCONTROLLED_FAIL",
    "TEST_EXEC_TEST_RESULT_SKIPPED",
    "TEST_EXEC_TEST_RESULT_INVALID",
    "TEST_EXEC_TEST_RESULT_TIMED_OUT",
};

/**
 * @brief a namespace for the implementation
 */
namespace TestImpl {

/**
 * @brief a helper function to escape a string before printing to json
 * 
 * @param input the inputted, unescaped string
 * @return `std::string` the escaped string
 */
static std::string escapeJsonString(const std::string& input) {
    //store the escaped string
    std::string output;
    //pre-reserve the size
    output.reserve(input.length());
    //for each input: Check if it is allready legal in json or if it needs to be legalized
    for (char c : input) {
        //legalize potentially illegal character
        switch (c) {
            case '"':  output += "\\\""; break;
            case '\\': output += "\\\\"; break;
            case '\b': output += "\\b";  break;
            case '\f': output += "\\f";  break;
            case '\n': output += "\\n";  break;
            case '\r': output += "\\r";  break;
            case '\t': output += "\\t";  break;
            default:   output += c;      break;
        }
    }
    //return the escaped output
    return output;
}

/**
 * @brief define what a test is internally
 */
class Test {
public:

    /**
     * @brief Construct a new Test
     * 
     * This creates the test in an invalid state. Use `initialize` to setup the state
     */
    Test() = default;

    /**
     * @brief Destroy the Test
     */
    ~Test() = default;

    /**
     * @brief initialize the test
     * 
     * @param libPath the absolute path to the library that stores the test
     * @param name the name of the test
     * @param tags the name of the test
     * @param description the description of the test
     * @param timeout the amount of milliseconds after which the test is considered timed out
     * @param requirementFlags the scheduling flags for the test
     * @return `TestExecResult` `TEST_EXEC_RESULT` on success, an error flag on failure
     */
    TestExecResult initialize(
        std::filesystem::path libPath,
        const char* name,
        const char* tags,
        const char* description,
        uint64_t timeout,
        TestRequirementFlags requirementFlags
    ) {
        //name cannot be null
        if (name == NULL)
        {return TEST_EXEC_INVALID_INPUT;}
        m_name = name;
        //tags and description can legally be null
        //this is treated as an empty string by std::string
        m_tags = tags;
        m_description = description;
        //store the timeout time. Any time is valid. 
        m_maxDuration = timeout;
        //store the requirement flags. Check if all flags are known. 
        if (requirementFlags & ~(
            TEST_REQUIREMENT_RUN_EXCLUSIVE_BIT | 
            TEST_REQUIREMENT_GPU_BIT | TEST_REQUIREMENT_GPU_ACCESS_EXCLUSIVE_BIT | 
            TEST_REQUIREMENT_FILE_ACCESS_BIT | TEST_REQUIREMENT_FILE_ACCESS_EXCLUSIVE_BIT | 
            TEST_REQUIREMENT_RAM_EXCLUSIVE_BIT | 
            TEST_REQUIREMENT_NETWORKING_BIT | TEST_REQUIREMENT_NETWORKING_EXCLUSIVE_BIT | 
            TEST_REQUIREMENT_ASYNC_BIT
        )) {
            //some unknown bits are present
            return TEST_EXEC_INVALID_INPUT;
        }
        //store the valid combination
        m_required = requirementFlags;

        //compute the required capabilites from the requirements
        m_requiredCaps |= (m_required & TEST_REQUIREMENT_GPU_BIT         || m_required & TEST_REQUIREMENT_GPU_ACCESS_EXCLUSIVE_BIT)  ? TEST_EXEC_CAPABILITY_GPU_BIT     : 0;
        m_requiredCaps |= (m_required & TEST_REQUIREMENT_FILE_ACCESS_BIT || m_required & TEST_REQUIREMENT_FILE_ACCESS_EXCLUSIVE_BIT) ? TEST_EXEC_CAPABILITY_FILE_IO_BIT : 0;
        m_requiredCaps |= (m_required & TEST_REQUIREMENT_NETWORKING_BIT  || m_required & TEST_REQUIREMENT_NETWORKING_EXCLUSIVE_BIT)  ? TEST_EXEC_CAPABILITY_NETWORK_BIT : 0;
        m_requiredCaps |= (m_required & TEST_REQUIREMENT_ASYNC_BIT)                                                                  ? TEST_EXEC_CAPABILITY_ASYNC_BIT   : 0;

        //store the library path
        m_libPath = libPath;

        //success
        return TEST_EXEC_SUCCESS;
    }

    /**
     * @brief Get the Requirements for this test
     * 
     * @return `TestRequirementFlags` the requirement flags for this test
     */
    inline TestRequirementFlags getRequirements() const noexcept 
    {return m_required;}

    /**
     * @brief Get the Required Capabilities
     * 
     * @return `TestExecCapabilityFlags` the required capability flags
     */
    inline TestExecCapabilityFlags getRequiredCapabilities() const noexcept
    {return m_requiredCaps;}

    /**
     * @brief Set the weight of the test
     * 
     * @param weight the weight of the test
     */
    inline void setWeight(uint64_t weight) noexcept
    {m_weight = weight;}

    /**
     * @brief Get the weight of the test
     * 
     * @return `uint64_t` the weight value (higher -> more important)
     */
    inline uint64_t getWeight() const noexcept
    {return m_weight;}

    /**
     * @brief actually execute the test
     * 
     * This task runs on a worker thread, but not in a subprocess. Do NOT invoke the test invoker directly. 
     * 
     * @param ctx a pointer to the context of the executer
     */
    TestExecResult executeTest(const TestContext* ctx) {
        //mark the test as running
        m_state = TEST_EXEC_TEST_STATE_RUNNING;
        //store the report
        TestReport report;

        //try to find the worker
        std::filesystem::path p = Subprocess::getCurrentExecutablePath().remove_filename();
        p = p / "TestLib" / 
        #if _WIN32
        "TestWorker.exe";
        #else
        "TestWorker";
        #endif
        if (!std::filesystem::is_regular_file(p)) {
            m_state = TEST_EXEC_TEST_STATE_DONE;
            m_execReport = TEST_EXEC_TEST_RESULT_INVALID;
            m_returnValue = -3;
            return TEST_EXEC_FILE_NOT_FOUND;
        }

        //create the shared memory UUID
        std::string memId = m_libPath.string() + "_" + m_name;
        std::replace(memId.begin(), memId.end(), '/', '_');
        std::replace(memId.begin(), memId.end(), '\\', '_');
        memId = std::string("/") + memId;

        //create the shared memory arena
        SharedMemory sharedMem;
        if (!sharedMem.initialize(memId, sizeof(*ctx) + sizeof(report), SharedMemory::Mode::Create))
        {return TEST_EXEC_GENERIC_ERROR;}

        //write the context and initial report
        if (!sharedMem.writeObject(*ctx))
        {return TEST_EXEC_OUT_OF_MEMORY;}
        if (!sharedMem.writeObject(report, sizeof(*ctx)))
        {return TEST_EXEC_OUT_OF_MEMORY;}

        //record the starting timestamp
        auto startTime = std::chrono::system_clock::now();
        //open the log file
        std::ofstream log(m_logPath, std::ofstream::out);

        auto printHelper = [&](const std::string& from, char separator, auto invoker) {
            //store the current position
            size_t pos = 0;
            //iterate while there are characters left
            while (pos < from.size()) {
                //search the span to print
                size_t nextPos = from.find(separator, pos+1);
                //if the next is the end, make it the max
                if (nextPos == std::string::npos) {nextPos = from.size();}
                //substring the to print data
                std::string sub = escapeJsonString(from.substr(pos, (nextPos - pos)));
                //add it to the log
                log << invoker(pos, sub);
                //advance the position
                pos = nextPos+1;
            }
        };

        log.clear();
        log << "{\n";
        log << "    \"schema_version\": 1,\n";
        log << "    \"meta\": {\n";
        log << "        \"name\": \"" << m_name << "\",\n";
        log << "        \"tags\": [\n";
        printHelper(m_tags, ' ', [&](size_t p, const std::string& str) -> std::string {
            return std::string((p == 0) ? "" : ",\n") + std::string("            \"") + str + "\"";
        });
        log << "\n        ],\n";
        log << "        \"description\": \"" << m_description << "\"\n";
        log << "    },\n";
        log << "    \"start_time_unix_ms\": " << std::chrono::duration_cast<std::chrono::milliseconds>(startTime.time_since_epoch()).count() << ",\n";
        log << "    \"log\": [\n";
        log.close();

        //create the subprocess to use
        Subprocess proc;
        std::vector<std::string> args = {
                //pass the worker name
                m_name,
                //pass the library location
                m_libPath.string(),
                //pass the memory UUID
                memId,
                //pass the log file path
                m_logPath.string()
            };
        if (!proc.launch(
            p, args
        )) 
        {return TEST_EXEC_GENERIC_ERROR;}

        //await the process
        uint64_t timeout = m_maxDuration;
        bool timedOut = false;
        if (!proc.wait(timeout)) {
            proc.terminate();
            timedOut = true;
        }
        auto endTime = std::chrono::system_clock::now();

        //open the log file again
        log.open(m_logPath, std::ofstream::out | std::ofstream::app);
        //end the log and start the std out dump
        log << "\n    ],\n    \"stdout\": [\n";
        //get the std out dump and prepare printing
        std::string out = proc.getStandardOutMessages();
        printHelper(out, '\n', [&](size_t pos, const std::string& str) -> std::string {
            return std::string((pos == 0) ? "" : ",\n") + "        \"" + str + "\"";
        });
        //finalize the std out dump and prepare the std err dump
        log << "\n    ],\n    \"stderr\": [\n";
        //get the err messages and reset last newline
        out = proc.getErrorOutMessages();
        printHelper(out, '\n', [&](size_t pos, const std::string& str) -> std::string {
            return std::string((pos == 0) ? "" : ",\n") + "        \"" + str + "\"";
        });

        //get the exit method
        if (timedOut) {
            report.result = TEST_TIMED_OUT;
        } else {
            if (proc.didCloseNormally()) {
                //report can be trusted
                //read back the report
                sharedMem.readObject(report, sizeof(*ctx));
            } else {
                report.result = TEST_UNCONTROLLED_FAIL;
            }
        }

        //store the result
        memcpy(&m_report, &report, sizeof(report));

        //create the execution report
        switch (m_report.result) {
        case TestResult::TEST_CONTROLLED_FAIL:   m_execReport = TEST_EXEC_TEST_RESULT_CONTROLLED_FAIL; break;
        case TestResult::TEST_INVALID:           m_execReport = TEST_EXEC_TEST_RESULT_INVALID; break;
        case TestResult::TEST_SKIPPED:           m_execReport = TEST_EXEC_TEST_RESULT_SKIPPED; break;
        case TestResult::TEST_SUCCESS:           m_execReport = TEST_EXEC_TEST_RESULT_SUCCESS; break;
        case TestResult::TEST_TIMED_OUT:         m_execReport = TEST_EXEC_TEST_RESULT_TIMED_OUT; break;
        case TestResult::TEST_UNCONTROLLED_FAIL: m_execReport = TEST_EXEC_TEST_RESULT_UNCONTROLLED_FAIL; break;
        case TestResult::TEST_WARNING:           m_execReport = TEST_EXEC_TEST_RESULT_WARNING; break;
        
        default:
            m_execReport = TEST_EXEC_TEST_RESULT_INVALID; 
            break;
        }

        //get the return value
        m_returnValue = proc.getExitCode();
        //get the termination signal (0 = nothing to do)
        int termSig = proc.getTerminationSignal();

        //add the end time
        log << "\n    ],\n    \"end_time_unix_ms\": " << (std::chrono::duration_cast<std::chrono::milliseconds>(endTime.time_since_epoch()).count()) << ",\n";
        log << "    \"duration_ms\": " << std::chrono::duration_cast<std::chrono::milliseconds>(endTime - startTime).count() << ",\n";
        //finalize the printing
        log << "    \"return\": {\n        \"exit_code\":" << m_returnValue << ",\n        \"state\": \"" << RETURN_ENUM_STR[static_cast<uint32_t>(m_execReport)] << "\",\n";
        //print the termination information
        log << "        \"termination_signal\": " << termSig << ",\n";
        log << "        \"termination_reason\": \"" << ((termSig == 11) ? "segmentation_fault" : ((termSig == 15) ? "termination" : ((termSig == 9) ? "killed" : ((termSig == 0) ? "exited" : "unknown")))) << "\"\n";
        log << "    }\n";
        //close the log
        log << "}";
        log.close();

        //the test is now done (regardless of success state)
        m_state = TEST_EXEC_TEST_STATE_DONE;

        //success
        return TEST_EXEC_SUCCESS;
    }

    /**
     * @brief skip the test
     */
    void skip() {
        //mark as skipped
        m_execReport = TEST_EXEC_TEST_RESULT_SKIPPED;
    }

    /**
     * @brief Set the log path
     * 
     * @param path the path used for logging, `NULL` for default
     * @param root the local root path
     */
    inline void setLogPath(const char* path, std::filesystem::path root) {
        //check for NULL
        if (path == NULL) {
            //use default auto
            std::filesystem::path libPath = m_libPath;
            libPath.remove_filename();
            m_logPath = libPath / (m_name + ".json");
        } else {
            //use the inputted log path
            m_logPath = std::filesystem::path(path) / (m_name + ".json");
            //make sure the path is absolute
            if (m_logPath.is_relative())
            {m_logPath = root / m_logPath;}
        }
    }

    /**
     * @brief Set the test state
     * 
     * @param state the new state of the test
     */
    void setState(TestExecTestState state)
    {m_state = state;}

    /**
     * @brief Get the test execution state
     * 
     * @return `TestExecTestState` the current test execution state
     */
    TestExecTestState getState() const
    {return m_state;}

    /**
     * @brief Get the execution result
     * 
     * @return `TestExecTestResult` the execution result
     */
    inline TestExecTestResult getExecResult() const noexcept
    {return m_execReport;}

    /**
     * @brief Get the Return Value
     * 
     * @return `int` the return value
     */
    inline int getReturnValue() const noexcept
    {return m_returnValue;}

    /**
     * @brief fill a metadata structure with some metadata
     * 
     * @param pMeta a pointer to the metadata to fill
     * @return `TestExecResult` `TEST_EXEC_RESULT` on success, an error flag on failure
     */
    inline TestExecResult fillMetadata(TestExecTestMetadata* pMeta) {
        //sanity check the metadata pointer
        if (pMeta == NULL) {return TEST_EXEC_INVALID_INPUT;}

        //write the metadata
        pMeta->name = m_name.c_str();
        pMeta->tags = m_tags.c_str();
        pMeta->description = m_description.c_str();

        //success
        return TEST_EXEC_SUCCESS;
    }

protected:

    /**
     * @brief store a path to the library that stores this test
     */
    std::filesystem::path m_libPath;
    /**
     * @brief store the path to log to
     */
    std::filesystem::path m_logPath;
    /**
     * @brief store the name of the test
     */
    std::string m_name;
    /**
     * @brief store the tags of the test
     */
    std::string m_tags;
    /**
     * @brief store the description of the test
     */
    std::string m_description;
    /**
     * @brief store the weight of the task
     */
    uint64_t m_weight = 0;
    /**
     * @brief store the duration after which the test is considered timed out
     */
    uint64_t m_maxDuration = UINT64_MAX;
    /**
     * @brief store the required capabilites for the test
     */
    TestRequirementFlags m_required = 0;
    /**
     * @brief store the required capabilities
     */
    TestExecCapabilityFlags m_requiredCaps = 0;
    /**
     * @brief store the result
     */
    TestReport m_report;
    /**
     * @brief store the execution report
     */
    TestExecTestResult m_execReport;
    /**
     * @brief store the return value
     */
    int m_returnValue = 0;

    /**
     * @brief store the state of the test (alone the test instance means that the test is loaded)
     */
    TestExecTestState m_state = TEST_EXEC_TEST_STATE_LOADED;

};

}
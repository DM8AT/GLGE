/**
 * @file Worker.cpp
 * @author DM8AT
 * @brief implement the sandbox executable for the worker
 * @version 0.1
 * @date 2026-05-20
 * 
 * @copyright Copyright (c) 2026
 * 
 */
//add dynamic libraries
#include "OSAbstraction/DynamicLibrary.hpp"
//add shared memory
#include "OSAbstraction/SharedMemory.hpp"

//add the contract
#include "TestContract.h"

#include <iostream>
//add vectors
#include <vector>
//add file i/o
#include <fstream>
//for async stuff
#include <atomic>
#include <mutex>

#ifndef _WIN32
#include <sys/prctl.h>
#include <signal.h>
#endif

/**
 * @brief store the output file to log to
 */
std::ofstream logFile;

//this must be atomic: optimization may introduce invalid caches otherwise (set to true not visible across caches on different cores)
std::atomic_bool isFirstLogEntry {true};
std::mutex logFileMtx;

std::string escapeJsonString(const std::string& input) {
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

void logFn(const TestMessage* msg) {
    //if the message is invalid, stop
    if (msg == NULL) {return;}

    //start the message log block
    //since this can be called from multiple threads, cache the to-print block locally
    std::stringstream stream;
    stream << "        {\n";
    stream << "            \"msg_type\": \"message\",\n";
    //if existing, log the color
    if (msg->color) {
        stream << "            \"color\": {\n";
        stream << "                \"red\": " << static_cast<uint32_t>(msg->color->red) << ",\n";
        stream << "                \"green\": " << static_cast<uint32_t>(msg->color->green) << ",\n";
        stream << "                \"blue\": " << static_cast<uint32_t>(msg->color->blue) << ",\n";
        stream << "                \"alpha\": " << static_cast<uint32_t>(msg->color->alpha) << "\n";
        stream << "            },\n";
    }
    //if existing, log the location
    if (msg->at) {
        stream << "            \"at\": {\n";
        stream << "                \"file:\": \"" << escapeJsonString(msg->at->file) << "\",\n";
        stream << "                \"line:\": " << msg->at->line << ",\n";
        stream << "                \"expression:\": \"" << escapeJsonString(msg->at->expression) << "\",\n";
        stream << "            },\n";
    }
    //log the timestamp
    auto timeStamp = std::chrono::system_clock::now();
    stream << "            \"time_stamp_unix_ms\": " << std::chrono::duration_cast<std::chrono::milliseconds>(timeStamp.time_since_epoch()).count() << ",\n";
    //log the actual message
    stream << "            \"msg\": \"" << escapeJsonString(msg->msg) << "\"\n        }";
    //output the stream to the file
    std::unique_lock lock(logFileMtx);
    //finalize stringstream used to guarantee that json is always valid -> no flush can happen if nothing is written to the file
    std::stringstream finalized;
    finalized << (isFirstLogEntry.exchange(false, std::memory_order_relaxed) ? "" : ",\n") << stream.str();
    logFile << finalized.str() << std::flush;
    //release lock
}

void assertFn(const TestAssertion* assert) {
    //if the assertion is invalid, stop
    if (assert == NULL) {return;}

    //start the assertion log block
    //since this can be called from multiple threads, cache the to-print block locally
    std::stringstream stream;
    stream << "        {\n";
    stream << "            \"msg_type\": \"assertion\",\n";
    //if existing, log the color
    if (assert->color) {
        stream << "            \"color\": {\n";
        stream << "                \"red\": " << static_cast<uint32_t>(assert->color->red) << ",\n";
        stream << "                \"green\": " << static_cast<uint32_t>(assert->color->green) << ",\n";
        stream << "                \"blue\": " << static_cast<uint32_t>(assert->color->blue) << ",\n";
        stream << "                \"alpha\": " << static_cast<uint32_t>(assert->color->alpha) << "\n";
        stream << "            },\n";
    }
    //if existing, log the location
    if (assert->at) {
        stream << "            \"at\": {\n";
        stream << "                \"file:\": \"" << escapeJsonString(assert->at->file) << "\",\n";
        stream << "                \"line:\": " << assert->at->line << ",\n";
        stream << "                \"expression:\": \"" << escapeJsonString(assert->at->expression) << "\"\n";
        stream << "            },\n";
    }
    //log the timestamp
    auto timeStamp = std::chrono::system_clock::now();
    stream << "            \"time_stamp_unix_ms\": " << std::chrono::duration_cast<std::chrono::milliseconds>(timeStamp.time_since_epoch()).count() << ",\n";
    //log the expected and actual value
    stream << "            \"expected\": \"" << escapeJsonString(assert->expected) << "\",\n";
    stream << "            \"actual\": \"" << escapeJsonString(assert->actual) << "\",\n";
    stream << "            \"passed\": " << ((assert->passed == 1) ? 1 : 0) << "\n";
    stream << "        }";
    {
    //output the stream to the file
    std::unique_lock lock(logFileMtx);
    //finalize stringstream used to guarantee that json is always valid -> no flush can happen if nothing is written to the file
    std::stringstream finalized;
    finalized << (isFirstLogEntry.exchange(false, std::memory_order_relaxed) ? "" : ",\n") << stream.str();
    logFile << finalized.str() << std::flush;
    //release lock
    }

    //if the assertion did NOT pass, return with the assertion value as the exit code
    if (assert->passed != 0)
    {exit(assert->passed);}
}

/**
 * @brief the main entry point for the worker sandbox
 * 
 * @param argc the argument count (expected = 4)
 * @param argv the argument strings
 * @return `int` process return value
 */
int main(int argc, char const *argv[]) {
    #ifndef _WIN32
        //make sure this process does not create a zombie process
        prctl(PR_SET_PDEATHSIG, SIGKILL);
        if (getppid() == 1) {
            //parent is allready dead -> kill this process
            int(raise(SIGKILL));
        }
    #endif
    //sanity check the argument count
    if (argc != 5)
    {return 0xf0 + argc;}

    //get the start time
    auto start = std::chrono::system_clock::now();

    //create the logging file
    logFile.open(argv[4], std::ofstream::out | std::ofstream::app);
    if (!logFile.is_open()) {return 0xf0;}

    //establish shared memory connection
    SharedMemory mem;
    if (!mem.initialize(argv[3], sizeof(TestContext) + sizeof(TestReport), SharedMemory::Mode::Open))
    {return 1;}

    TestContext* ctx = reinterpret_cast<TestContext*>(mem.data());
    TestReport* report = reinterpret_cast<TestReport*>((reinterpret_cast<TestContext*>(mem.data()))+1);

    //open the dynamic library
    DynamicLibrary lib;
    if (!lib.open(std::filesystem::path(argv[2])))
    {return 2;}

    //load the test enumeration function
    auto regFunc = lib.symbol<PFN_RegisterTest>(TEST_REGISTER_TEST_SYM);
    if (!regFunc.second) {return 3;}

    //get the tests
    uint64_t testCount = 0;
    (*regFunc.first)(&testCount, nullptr);
    std::vector<Test> tests(testCount);
    (*regFunc.first)(&testCount, tests.data());

    //get the requested test
    PFN_TestInvoker invoker = nullptr;
    for (const auto& test : tests) {
        if (strcmp(test.entry.name, argv[1]) == 0) {
            //found
            invoker = test.invoker;
        }
    }
    //if the invoker was not found, stop
    if (invoker == nullptr) {return 4;}

    //write the test functions
    TestFunctions funcs {
        .log = &logFn,
        .assertion = &assertFn
    };

    //else, invoke the invoker
    (*invoker)(ctx, report, &funcs);

    //return success
    return 0;
}
/**
 * @file TestExecutor.h
 * @author DM8AT
 * @brief define the API that is used to implement the test system into a launcher
 * @version 0.1
 * @date 2026-05-19
 * 
 * @copyright Copyright (c) 2026
 * 
 * All returned string pointers remain valid until:
 * - the associated object is destroyed
 * - or the executor is destroyed
 * whichever occurs first.
 * 
 * The ABI assumes that C23 or higher is used. 
 * 
 * All provided structures have clearly defined defaults. 
 */
//header guard
#ifndef _TEST_EXECUTOR_
#define _TEST_EXECUTOR_

#ifdef __cplusplus
extern "C" {
#endif

/**
 * @brief define the version of the test executor
 */
#define TEST_EXEC_VERSION 1

//fixed integers are required
#include <stdint.h>
//add bools
#include <stdbool.h>
//used for NULL
#include <stddef.h>
//used for static assertion
#include <assert.h>

//unify calling behaviour
//All functions used in the ABI must be marked with this. 
#if defined(_WIN32)
    /**
     * @brief A macro to specify how the call should be done
     */
    #define TEST_EXEC_CALL __cdecl
#else
    /**
     * @brief A macro to specify how the call should be done
     */
    #define TEST_EXEC_CALL
#endif

/**
 * @brief define a unified null handle
 */
#define TEST_EXEC_NULL_HANDLE NULL

/**
 * @brief a helper to create a non-dispatchble handle
 */
#define TEST_EXEC_DEFINE_NONDISPATCHABLE_HANDLE(object) typedef struct object##_T* object;

/**
 * @brief a helper for compile-time ABI comptability tests
 * 
 * @param x the expression to evaluate
 * @param msg the message to print in the error if `x` evaluated to `false`
 */
#define TEST_EXEC_STATIC_ASSERT(x, msg) static_assert(x, msg);

/**
 * @brief store all test executor types
 */
typedef enum e_TestExecType {
    /**
     * @brief the type is a test base
     * 
     * Available since version 1
     */
    TEST_EXEC_TEST_BASE = 0,
    /**
     * @brief the type is a test executor create info
     * 
     * Available since version 1
     */
    TEST_EXEC_TYPE_TEST_EXECUTOR_CREATE_INFO = 1,
    /**
     * @brief the type is a load test info
     * 
     * Available since version 1
     */
    TEST_EXEC_TYPE_LOAD_TEST_INFO = 2,
    /**
     * @brief the type is an execute test info
     * 
     * Available since version 1
     */
    TEST_EXEC_TYPE_EXECUTE_TEST_INFO = 3,
    /**
     * @brief the type is a report
     * 
     * Available since version 1
     */
    TEST_EXEC_TYPE_TEST_REPORT = 4,
    /**
     * @brief the type is a test state
     * 
     * Available since version 1
     */
    TEST_EXEC_TYPE_TEST_STATE_INFO = 5,
    /**
     * @brief the type is a test result info
     * 
     * Available since version 1
     */
    TEST_EXEC_TYPE_TEST_RESULT_INFO = 6,
    /**
     * @brief the type is a test metadata
     * 
     * Available since version 1
     */
    TEST_EXEC_TYPE_TEST_METADATA = 7,
    /**
     * @brief the type is a message color
     * 
     * Available since version 1
     */
    TEST_EXEC_TYPE_MESSAGE_COLOR = 8,
    /**
     * @brief the type is a timestamp
     * 
     * Available since version 1
     */
    TEST_EXEC_TYPE_TIMESTAMP = 9,

    /**
     * @brief maximum test type value
     * 
     * Used solely to force enum storage size compatibility. Not a valid runtime value.
     */
    TEST_EXEC_TYPE_MAX = 0x7fffffff
} TestExecType;

/**
 * @brief define the possible result values for testExec calls
 */
typedef enum e_TestExecResult {
    /**
     * @brief the function ran successful
     */
    TEST_EXEC_SUCCESS = 0,
    /**
     * @brief something went wrong, but it is not sure what
     */
    TEST_EXEC_GENERIC_ERROR = 1,
    /**
     * @brief no more memory is available
     */
    TEST_EXEC_OUT_OF_MEMORY = 2,
    /**
     * @brief the `pNext` chain contained unrecognized types
     */
    TEST_EXEC_INVALID_PNEXT = 3,
    /**
     * @brief an input contained invalid data
     */
    TEST_EXEC_INVALID_INPUT = 4,
    /**
     * @brief an input contained invalid data
     */
    TEST_EXEC_INVALID_OPERATION = 5,
    /**
     * @brief a specified file was not found
     */
    TEST_EXEC_FILE_NOT_FOUND = 6,
    /**
     * @brief something went wrong while loading a file
     */
    TEST_EXEC_FILE_LOAD_ERROR = 7,
    /**
     * @brief an invalid object handle was passed
     */
    TEST_EXEC_INVALID_HANDLE = 8,
    /**
     * @brief failed to create a directory
     */
    TEST_EXEC_DIRECTORY_CREATE_ERROR = 9,
    /**
     * @brief invalid type
     */
    TEST_EXEC_INVALID_TYPE = 10,

    /**
     * @brief maximum test type value
     * 
     * Used solely to force enum storage size compatibility. Not a valid runtime value.
     */
    TEST_EXEC_RESULT_MAX = 0x7fffffff
} TestExecResult;

/**
 * @brief define a base structure for all test executor structors
 */
typedef struct s_TestExecBase {
    /**
     * @brief store the structure type
     */
    TestExecType sType = TEST_EXEC_TEST_BASE;
    /**
     * @brief store a pointer to a structure that extends this structure
     * 
     * `NULL` marks the end of the extension chain. 
     * 
     * To use an extension it needs to be valid in the test version. 
     * If an invalid `sType` is found, the extension chain is considered "broken" and traversal fails. 
     * 
     * Every structure in a `pNext` chain must begin with TestBase.
     */
    const void* pNext = NULL;
} TestExecBase;

/**
 * @brief define all capability flags of the executor
 */
typedef enum e_TestExecCapabilityFlagBits {
    /**
     * @brief define that the execution platform has a GPU
     */
    TEST_EXEC_CAPABILITY_GPU_BIT = 0x00000001,
    /**
     * @brief define that the execution platform permits tests to launch threads or subjobs
     */
    TEST_EXEC_CAPABILITY_ASYNC_BIT = 0x00000002,
    /**
     * @brief define that the execution platform permits tests to run with file i/o
     */
    TEST_EXEC_CAPABILITY_FILE_IO_BIT = 0x00000004,
    /**
     * @brief define that the execution platform permits tests to run with network access
     */
    TEST_EXEC_CAPABILITY_NETWORK_BIT = 0x00000008,

    /**
     * @brief maximum test executor capability value
     * 
     * Used solely to force enum storage size compatibility. Not a valid runtime value.
     */
    TEST_EXEC_CAPABILITY_MAX_BIT = 0x7fffffff
} TestExecCapabilityFlagBits;

/**
 * @brief define the type used to store test executor capability flags
 */
typedef uint32_t TestExecCapabilityFlags;

/**
 * @brief define how the test executor scheduler operates
 */
typedef enum e_TestExecExecutorScheduler {
    /**
     * @brief define that the scheduler will use a fifo queue for scheduling
     * 
     * With this setting the `priority` element of a task is ignored
     */
    TEST_EXEC_EXECUTOR_SCHEDULER_FIFO = 1,
    /**
     * @brief define that the scheduler will use a fifo queue that respects priorities for scheduling
     * 
     * With this setting the `priority` element of a task influences execution order
     */
    TEST_EXEC_EXECUTOR_SCHEDULER_FIFO_WEIGHTED = 2,

    /**
     * @brief maximum test executor capability value
     * 
     * Used solely to force enum storage size compatibility. Not a valid runtime value.
     */
    TEST_EXEC_EXECUTOR_SCHEDULER_MAX = 0x7fffffff
} TestExecExecutorScheduler;

/**
 * @brief define different states for a test
 */
typedef enum e_TestExecTestState {
    /**
     * @brief the test is not known
     */
    TEST_EXEC_TEST_STATE_UNKNOWN = 0,
    /**
     * @brief the test is loaded, but currently not scheduled to run
     */
    TEST_EXEC_TEST_STATE_LOADED = 1,
    /**
     * @brief the test is scheduled, but currently not running
     */
    TEST_EXEC_TEST_STATE_SCHEDULED = 2,
    /**
     * @brief the test is currently running
     */
    TEST_EXEC_TEST_STATE_RUNNING = 3,
    /**
     * @brief the test finished running
     */
    TEST_EXEC_TEST_STATE_DONE = 4
} TestExecTestState;

/**
 * @brief store the resulting state of a test
 */
typedef enum e_TestExecTestResult {
    /**
     * @brief the test finished successfully
     * 
     * This means the expected result was reached, NOT that no errors were thrown. 
     */
    TEST_EXEC_TEST_RESULT_SUCCESS = 0,
    /**
     * @brief something went unexpectedly, but in a way that did not hinder the test execution
     */
    TEST_EXEC_TEST_RESULT_WARNING = 1,
    /**
     * @brief the test failed and the system detected the failure
     */
    TEST_EXEC_TEST_RESULT_CONTROLLED_FAIL = 2,
    /**
     * @brief the system terminated the test
     * 
     * This is e.g. X force-closing the app or segmentation fault
     */
    TEST_EXEC_TEST_RESULT_UNCONTROLLED_FAIL = 3,
    /**
     * @brief the test was not executed
     */
    TEST_EXEC_TEST_RESULT_SKIPPED = 4,
    /**
     * @brief something about the test report seems wrong, the report was deemed not reliable. 
     */
    TEST_EXEC_TEST_RESULT_INVALID = 5,
    /**
     * @brief the test took longer than the specified `timeout`
     */
    TEST_EXEC_TEST_RESULT_TIMED_OUT = 6,



    /**
     * @brief maximum enum value
     * 
     * Used solely to force enum storage size compatibility. Not a valid runtime value.
     */
    TEST_EXEC_TEST_RESULT_MAX = 0x7fffffff
} TestExecTestResult;

/**
 * @brief define the type of send message
 */
typedef enum e_TestExecMessageType {
    /**
     * @brief debugging information
     */
    TEST_EXEC_MESSAGE_TYPE_DEBUG = 0,
    /**
     * @brief general information
     */
    TEST_EXEC_MESSAGE_TYPE_INFO = 1,
    /**
     * @brief a simple warning
     */
    TEST_EXEC_MESSAGE_TYPE_WARNING = 2,
    /**
     * @brief a higher severity warning
     */
    TEST_EXEC_MESSAGE_TYPE_CRITICAL_WARNING = 3,
    /**
     * @brief something went wrong
     */
    TEST_EXEC_MESSAGE_TYPE_ERROR = 4,
    /**
     * @brief something went really wrong
     */
    TEST_EXEC_MESSAGE_TYPE_FATAL_ERROR = 5,

    /**
     * @brief the maximum test message type enum
     * 
     * Used solely to force enum storage size compatibility. Not a valid runtime value.
     */
    TEST_EXEC_MESSAGE_TYPE_MAX = 0x7fffffff
} TestExecMessageType;

/**
 * @brief define a test executor handle
 */
TEST_EXEC_DEFINE_NONDISPATCHABLE_HANDLE(TestExecExecutor)
/**
 * @brief define a test handle
 */
TEST_EXEC_DEFINE_NONDISPATCHABLE_HANDLE(TestExecTest)

/**
 * @brief define the test message color
 * 
 * The color is stored in the sRGB color space. 
 */
typedef struct s_TestExecMessageColor {
    /**
     * @brief include the test type header
     */
    TestExecBase header = {
        .sType = TEST_EXEC_TYPE_MESSAGE_COLOR,
        .pNext = NULL
    };

    /**
     * @brief store the amount of red in the color. 0 = no red, 0xff = maximum red
     */
    uint8_t red = 0;
    /**
     * @brief store the amount of green in the color. 0 = no green, 0xff = maximum green
     */
    uint8_t green = 0;
    /**
     * @brief store the amount of blue in the color. 0 = no blue, 0xff = maximum blue
     */
    uint8_t blue = 0;
    /**
     * @brief store the transparency of the color. 0 = fully transparent, 0xff = fully opaque
     * 
     * May systems (especially consoles) cannot represent transparency. This value may be interpreted as actual blending or as alpha-clipping. 
     * 
     * If alpha clipping is used, `bool visible = alpha > 127` is used to evaluate visibility. 
     */
    uint8_t alpha = 0;
} TestExecMessageColor;

/**
 * @brief define a structure that stores a timestamp
 */
typedef struct s_TestExecTimestamp {
    /**
     * @brief include the test type header
     */
    TestExecBase header = {
        .sType = TEST_EXEC_TYPE_TIMESTAMP,
        .pNext = NULL
    };

    /**
     * @brief store the amount of seconds since the unit epoch
     * 
     * The unit epoch is currently Jan 01 1970 in UTC. 
     */
    uint64_t unixSeconds;
} TestExecTimestamp;

/**
 * @brief define the create info for a test executor
 */
typedef struct s_TestExecExecutorCreateInfo {
    /**
     * @brief include the header
     */
    TestExecBase header {
        .sType = TEST_EXEC_TYPE_TEST_EXECUTOR_CREATE_INFO,
        .pNext = NULL
    };
    /**
     * @brief store the capabilities
     */
    TestExecCapabilityFlags capabilities = 0;
    /**
     * @brief define the used scheduler strategy
     */
    TestExecExecutorScheduler schedulerStrategy = TEST_EXEC_EXECUTOR_SCHEDULER_FIFO_WEIGHTED;
    /**
     * @brief define a path to a directory to use for temporary files
     * 
     * This directory is not required to currently exist. If it does not exist it will be created. 
     * It will be deleted once the executor is deleted. 
     * 
     * If this is `NULL`, the directory ${rootDir}/tmp is considered `tmpDir`. 
     * 
     * If this is not an absolute path it will be considered relative to the `rootDir` path. 
     */
    const char* tmpDir = NULL;
    /**
     * @brief store the directory to use as the root of execution for all tests. 
     * 
     * If this is `NULL`, the current working directory is considered the root of execution. 
     * 
     * If a path is provided it may be a relative path to the current working directory or an absolute path. 
     * 
     * If this directory does not exist, it will be created. 
     */
    const char* rootDir = NULL;
    /**
     * @brief define the amount of parallel tests that a single executor run in parallel
     * 
     * The value `0` is considered auto. If `0` is selected the implementation decides how many tests
     * to run in parallel. 
     */
    uint32_t maxParallelTests = 0;
} TestExecExecutorCreateInfo;

/**
 * @brief define a structure that describes how a test is loaded
 */
typedef struct s_TestExecLoadTestInfo {
    /**
     * @brief include the header
     */
    TestExecBase header {
        .sType = TEST_EXEC_TYPE_LOAD_TEST_INFO,
        .pNext = NULL
    };
    /**
     * @brief store a path to the file
     * 
     * `NULL` is illegal
     */
    const char* path = NULL;
} TestExecLoadTestInfo;

/**
 * @brief define a structure that describes how a test is dispatched
 */
typedef struct s_TestExecExecuteTestInfo {
    /**
     * @brief include the header
     */
    TestExecBase header = {
        .sType = TEST_EXEC_TYPE_EXECUTE_TEST_INFO,
        .pNext = NULL
    };
    /**
     * @brief store the priority of the test
     * 
     * This may hint the scheduler on when to execute this. Depending on the scheduling strategy it is ignored. 
     * 
     * 0 = lowest priority, `UINT64_MAX` = highest priority
     */
    uint64_t priority = 0;
    /**
     * @brief store the test to execute
     */
    TestExecTest test = NULL;
    /**
     * @brief store the path to the log file
     * 
     * If this is `NULL`, the default path of the path to the shared library the test originated from is used. 
     * 
     * The path may be absolute or relative to the specified `rootDir`.
     */
    const char* logPath = NULL;
} TestExecExecuteTestInfo;

/**
 * @brief define a structure that is used to query a test state
 */
typedef struct s_TestExecTestStateInfo {
    /**
     * @brief include the header
     */
    TestExecBase header = {
        .sType = TEST_EXEC_TYPE_TEST_STATE_INFO,
        .pNext = NULL
    };
    /**
     * @brief store the current test state
     */
    TestExecTestState state = TEST_EXEC_TEST_STATE_UNKNOWN;
} TestExecTestStateInfo;

/**
 * @brief define a structure that holds info about how many tests passed
 */
typedef struct s_TestExecTestReport {
    /**
     * @brief include the header
     */
    TestExecBase header = {
        .sType = TEST_EXEC_TYPE_TEST_REPORT,
        .pNext = NULL
    };

    /**
     * @brief store the total amount of tests invoked
     */
    uint64_t total = 0;
    /**
     * @brief store the amount of invoked tests that passed
     */
    uint64_t passed = 0;
    /**
     * @brief store the amount of invoked tests that failed
     */
    uint64_t failed = 0;
    /**
     * @brief store the amount of invoked tests that where skipped
     */
    uint64_t skipped = 0;
} TestExecTestReport;

/**
 * @brief define a structure to query information about the result of an test
 */
typedef struct s_TestExecTestResultInfo {
    /**
     * @brief include the header
     */
    TestExecBase header = {
        .sType = TEST_EXEC_TYPE_TEST_RESULT_INFO,
        .pNext = NULL
    };

    /**
     * @brief store the test result
     */
    TestExecTestResult result = TEST_EXEC_TEST_RESULT_UNCONTROLLED_FAIL;
    /**
     * @brief store the return value
     */
    int returnValue = 0;
} TestExecTestResultInfo;

/**
 * @brief a structure used to get test metadata
 */
typedef struct s_TestExecTestMetadata {
    /**
     * @brief include the header
     */
    TestExecBase header = {
        .sType = TEST_EXEC_TYPE_TEST_METADATA,
        .pNext = NULL
    };
    /**
     * @brief store the human-readable name of the test
     * 
     * `NULL` is illegal
     */
    const char* name = NULL;
    /**
     * @brief store the 
     */
    const char* tags = NULL;
    /**
     * @brief store the human-readable description of the test
     * 
     * `NULL` is legal
     */
    const char* description = NULL;
} TestExecTestMetadata;


//check if function prototypes are requested
#ifndef TEST_EXEC_NO_FUNCTION_PROTOTYPES

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
);

/**
 * @brief destroy a test executor
 * 
 * if `executor` is invalid nothing happens
 * 
 * @param executor the test executor to destroy
 */
void TEST_EXEC_CALL testExecDestroyExecutor(
    TestExecExecutor executor
);

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
);

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
);



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
);

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
);

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
);

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
);

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
);

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
);

#endif

//Testing: TestExecType
TEST_EXEC_STATIC_ASSERT(sizeof(TestExecType) == 4, "ABI mismatch: Expected size of type TestExecType was 4 bytes, but an invalid size was reported.")
TEST_EXEC_STATIC_ASSERT(alignof(TestExecType) == 4, "ABI mismatch: Expected alignment of type TestExecType was 4 bytes, but an invalid alignment was reported.")
//Testing: TestExecResult
TEST_EXEC_STATIC_ASSERT(sizeof(TestExecResult) == 4, "ABI mismatch: Expected size of type TestExecResult was 4 bytes, but an invalid size was reported.")
TEST_EXEC_STATIC_ASSERT(alignof(TestExecResult) == 4, "ABI mismatch: Expected alignment of type TestExecResult was 4 bytes, but an invalid alignment was reported.")
//Testing: TestExecCapabilityFlagBits
TEST_EXEC_STATIC_ASSERT(sizeof(TestExecCapabilityFlagBits) == 4, "ABI mismatch: Expected size of type TestExecCapabilityFlagBits was 4 bytes, but an invalid size was reported.")
TEST_EXEC_STATIC_ASSERT(alignof(TestExecCapabilityFlagBits) == 4, "ABI mismatch: Expected alignment of type TestExecCapabilityFlagBits was 4 bytes, but an invalid alignment was reported.")
//Testing: TestExecCapabilityFlags
TEST_EXEC_STATIC_ASSERT(sizeof(TestExecCapabilityFlags) == 4, "ABI mismatch: Expected size of type TestExecCapabilityFlags was 4 bytes, but an invalid size was reported.")
TEST_EXEC_STATIC_ASSERT(alignof(TestExecCapabilityFlags) == 4, "ABI mismatch: Expected alignment of type TestExecCapabilityFlags was 4 bytes, but an invalid alignment was reported.")
//Testing: TestExecExecutorScheduler
TEST_EXEC_STATIC_ASSERT(sizeof(TestExecExecutorScheduler) == 4, "ABI mismatch: Expected size of type TestExecExecutorScheduler was 4 bytes, but an invalid size was reported.")
TEST_EXEC_STATIC_ASSERT(alignof(TestExecExecutorScheduler) == 4, "ABI mismatch: Expected alignment of type TestExecExecutorScheduler was 4 bytes, but an invalid alignment was reported.")
//Testing: TestExecTestState
TEST_EXEC_STATIC_ASSERT(sizeof(TestExecTestState) == 4, "ABI mismatch: Expected size of type TestExecTestState was 4 bytes, but an invalid size was reported.")
TEST_EXEC_STATIC_ASSERT(alignof(TestExecTestState) == 4, "ABI mismatch: Expected alignment of type TestExecTestState was 4 bytes, but an invalid alignment was reported.")
//Testing: TestExecTestResult
TEST_EXEC_STATIC_ASSERT(sizeof(TestExecTestResult) == 4, "ABI mismatch: Expected size of type TestExecTestResult was 4 bytes, but an invalid size was reported.")
TEST_EXEC_STATIC_ASSERT(alignof(TestExecTestResult) == 4, "ABI mismatch: Expected alignment of type TestExecTestResult was 4 bytes, but an invalid alignment was reported.")
//Testing: TestExecMessageType
TEST_EXEC_STATIC_ASSERT(sizeof(TestExecMessageType) == 4, "ABI mismatch: Expected size of type TestExecMessageType was 4 bytes, but an invalid size was reported.")
TEST_EXEC_STATIC_ASSERT(alignof(TestExecMessageType) == 4, "ABI mismatch: Expected alignment of type TestExecMessageType was 4 bytes, but an invalid alignment was reported.")

//Testing: TestExecBase
TEST_EXEC_STATIC_ASSERT(sizeof(TestExecBase) == 16, "ABI mismatch: Expected size of type TestExecBase was 16 bytes, but an invalid size was reported.")
TEST_EXEC_STATIC_ASSERT(alignof(TestExecBase) == 8, "ABI mismatch: Expected alignment of type TestExecBase was 8 bytes, but an invalid alignment was reported.")
TEST_EXEC_STATIC_ASSERT(offsetof(TestExecBase, sType) == 0, "ABI mismatch: Expected the offset of the element sType in the type TestExecBase to be 0, but a different offset was reported.")
TEST_EXEC_STATIC_ASSERT(offsetof(TestExecBase, pNext) == 8, "ABI mismatch: Expected the offset of the element pNext in the type TestExecBase to be 8, but a different offset was reported.")
//Testing: TestExecBase
TEST_EXEC_STATIC_ASSERT(sizeof(TestExecMessageColor) == 24, "ABI mismatch: Expected size of type TestExecMessageColor was 24 bytes, but an invalid size was reported.")
TEST_EXEC_STATIC_ASSERT(alignof(TestExecMessageColor) == 8, "ABI mismatch: Expected alignment of type TestExecMessageColor was 8 bytes, but an invalid alignment was reported.")
TEST_EXEC_STATIC_ASSERT(offsetof(TestExecMessageColor, header) == 0, "ABI mismatch: Expected the offset of the element header in the type TestExecMessageColor to be 0, but a different offset was reported.")
TEST_EXEC_STATIC_ASSERT(offsetof(TestExecMessageColor, red) == 16, "ABI mismatch: Expected the offset of the element red in the type TestExecMessageColor to be 16, but a different offset was reported.")
TEST_EXEC_STATIC_ASSERT(offsetof(TestExecMessageColor, green) == 17, "ABI mismatch: Expected the offset of the element green in the type TestExecMessageColor to be 17, but a different offset was reported.")
TEST_EXEC_STATIC_ASSERT(offsetof(TestExecMessageColor, blue) == 18, "ABI mismatch: Expected the offset of the element blue in the type TestExecMessageColor to be 18, but a different offset was reported.")
TEST_EXEC_STATIC_ASSERT(offsetof(TestExecMessageColor, alpha) == 19, "ABI mismatch: Expected the offset of the element alpha in the type TestExecMessageColor to be 19, but a different offset was reported.")
//Testing: TestExecTimestamp
TEST_EXEC_STATIC_ASSERT(sizeof(TestExecTimestamp) == 24, "ABI mismatch: Expected size of type TestExecTimestamp was 24 bytes, but an invalid size was reported.")
TEST_EXEC_STATIC_ASSERT(alignof(TestExecTimestamp) == 8, "ABI mismatch: Expected alignment of type TestExecTimestamp was 8 bytes, but an invalid alignment was reported.")
TEST_EXEC_STATIC_ASSERT(offsetof(TestExecTimestamp, header) == 0, "ABI mismatch: Expected the offset of the element header in the type TestExecTimestamp to be 0, but a different offset was reported.")
TEST_EXEC_STATIC_ASSERT(offsetof(TestExecTimestamp, unixSeconds) == 16, "ABI mismatch: Expected the offset of the element unixSeconds in the type TestExecTimestamp to be 16, but a different offset was reported.")
//Testing: TestExecExecutorCreateInfo
TEST_EXEC_STATIC_ASSERT(sizeof(TestExecExecutorCreateInfo) == 48, "ABI mismatch: Expected size of type TestExecExecutorCreateInfo was 48 bytes, but an invalid size was reported.")
TEST_EXEC_STATIC_ASSERT(alignof(TestExecExecutorCreateInfo) == 8, "ABI mismatch: Expected alignment of type TestExecExecutorCreateInfo was 8 bytes, but an invalid alignment was reported.")
TEST_EXEC_STATIC_ASSERT(offsetof(TestExecExecutorCreateInfo, header) == 0, "ABI mismatch: Expected the offset of the element header in the type TestExecExecutorCreateInfo to be 0, but a different offset was reported.")
TEST_EXEC_STATIC_ASSERT(offsetof(TestExecExecutorCreateInfo, capabilities) == 16, "ABI mismatch: Expected the offset of the element capabilities in the type TestExecExecutorCreateInfo to be 16, but a different offset was reported.")
TEST_EXEC_STATIC_ASSERT(offsetof(TestExecExecutorCreateInfo, schedulerStrategy) == 20, "ABI mismatch: Expected the offset of the element schedulerStrategy in the type TestExecExecutorCreateInfo to be 20, but a different offset was reported.")
TEST_EXEC_STATIC_ASSERT(offsetof(TestExecExecutorCreateInfo, tmpDir) == 24, "ABI mismatch: Expected the offset of the element tmpDir in the type TestExecExecutorCreateInfo to be 24, but a different offset was reported.")
TEST_EXEC_STATIC_ASSERT(offsetof(TestExecExecutorCreateInfo, rootDir) == 32, "ABI mismatch: Expected the offset of the element rootDir in the type TestExecExecutorCreateInfo to be 32, but a different offset was reported.")
TEST_EXEC_STATIC_ASSERT(offsetof(TestExecExecutorCreateInfo, maxParallelTests) == 40, "ABI mismatch: Expected the offset of the element maxParallelTests in the type TestExecExecutorCreateInfo to be 40, but a different offset was reported.")
//Testing: TestExecExecutorCreateInfo
TEST_EXEC_STATIC_ASSERT(sizeof(TestExecLoadTestInfo) == 24, "ABI mismatch: Expected size of type TestExecLoadTestInfo was 24 bytes, but an invalid size was reported.")
TEST_EXEC_STATIC_ASSERT(alignof(TestExecLoadTestInfo) == 8, "ABI mismatch: Expected alignment of type TestExecLoadTestInfo was 8 bytes, but an invalid alignment was reported.")
TEST_EXEC_STATIC_ASSERT(offsetof(TestExecLoadTestInfo, header) == 0, "ABI mismatch: Expected the offset of the element header in the type TestExecLoadTestInfo to be 0, but a different offset was reported.")
TEST_EXEC_STATIC_ASSERT(offsetof(TestExecLoadTestInfo, path) == 16, "ABI mismatch: Expected the offset of the element path in the type TestExecLoadTestInfo to be 16, but a different offset was reported.")
//Testing: TestExecExecuteTestInfo
TEST_EXEC_STATIC_ASSERT(sizeof(TestExecExecuteTestInfo) == 40, "ABI mismatch: Expected size of type TestExecExecuteTestInfo was 32 bytes, but an invalid size was reported.")
TEST_EXEC_STATIC_ASSERT(alignof(TestExecExecuteTestInfo) == 8, "ABI mismatch: Expected alignment of type TestExecExecuteTestInfo was 8 bytes, but an invalid alignment was reported.")
TEST_EXEC_STATIC_ASSERT(offsetof(TestExecExecuteTestInfo, header) == 0, "ABI mismatch: Expected the offset of the element header in the type TestExecExecuteTestInfo to be 0, but a different offset was reported.")
TEST_EXEC_STATIC_ASSERT(offsetof(TestExecExecuteTestInfo, priority) == 16, "ABI mismatch: Expected the offset of the element priority in the type TestExecExecuteTestInfo to be 16, but a different offset was reported.")
TEST_EXEC_STATIC_ASSERT(offsetof(TestExecExecuteTestInfo, test) == 24, "ABI mismatch: Expected the offset of the element test in the type TestExecExecuteTestInfo to be 24, but a different offset was reported.")
TEST_EXEC_STATIC_ASSERT(offsetof(TestExecExecuteTestInfo, logPath) == 32, "ABI mismatch: Expected the offset of the element logPath in the type TestExecExecuteTestInfo to be 32, but a different offset was reported.")
//Testing: TestExecExecuteTestInfo
TEST_EXEC_STATIC_ASSERT(sizeof(TestExecTestStateInfo) == 24, "ABI mismatch: Expected size of type TestExecTestStateInfo was 24 bytes, but an invalid size was reported.")
TEST_EXEC_STATIC_ASSERT(alignof(TestExecTestStateInfo) == 8, "ABI mismatch: Expected alignment of type TestExecTestStateInfo was 8 bytes, but an invalid alignment was reported.")
TEST_EXEC_STATIC_ASSERT(offsetof(TestExecTestStateInfo, header) == 0, "ABI mismatch: Expected the offset of the element header in the type TestExecTestStateInfo to be 0, but a different offset was reported.")
TEST_EXEC_STATIC_ASSERT(offsetof(TestExecTestStateInfo, state) == 16, "ABI mismatch: Expected the offset of the element state in the type TestExecTestStateInfo to be 16, but a different offset was reported.")
//Testing: TestExecExecuteTestInfo
TEST_EXEC_STATIC_ASSERT(sizeof(TestExecTestReport) == 48, "ABI mismatch: Expected size of type TestExecTestReport was 48 bytes, but an invalid size was reported.")
TEST_EXEC_STATIC_ASSERT(alignof(TestExecTestReport) == 8, "ABI mismatch: Expected alignment of type TestExecTestReport was 8 bytes, but an invalid alignment was reported.")
TEST_EXEC_STATIC_ASSERT(offsetof(TestExecTestReport, header) == 0, "ABI mismatch: Expected the offset of the element header in the type TestExecTestReport to be 0, but a different offset was reported.")
TEST_EXEC_STATIC_ASSERT(offsetof(TestExecTestReport, total) == 16, "ABI mismatch: Expected the offset of the element total in the type TestExecTestReport to be 16, but a different offset was reported.")
TEST_EXEC_STATIC_ASSERT(offsetof(TestExecTestReport, passed) == 24, "ABI mismatch: Expected the offset of the element passed in the type TestExecTestReport to be 24, but a different offset was reported.")
TEST_EXEC_STATIC_ASSERT(offsetof(TestExecTestReport, failed) == 32, "ABI mismatch: Expected the offset of the element failed in the type TestExecTestReport to be 32, but a different offset was reported.")
TEST_EXEC_STATIC_ASSERT(offsetof(TestExecTestReport, skipped) == 40, "ABI mismatch: Expected the offset of the element skipped in the type TestExecTestReport to be 40, but a different offset was reported.")
//Testing: TestExecExecuteTestInfo
TEST_EXEC_STATIC_ASSERT(sizeof(TestExecTestResultInfo) == 24, "ABI mismatch: Expected size of type TestExecTestResultInfo was 24 bytes, but an invalid size was reported.")
TEST_EXEC_STATIC_ASSERT(alignof(TestExecTestResultInfo) == 8, "ABI mismatch: Expected alignment of type TestExecTestResultInfo was 8 bytes, but an invalid alignment was reported.")
TEST_EXEC_STATIC_ASSERT(offsetof(TestExecTestResultInfo, header) == 0, "ABI mismatch: Expected the offset of the element header in the type TestExecTestResultInfo to be 0, but a different offset was reported.")
TEST_EXEC_STATIC_ASSERT(offsetof(TestExecTestResultInfo, result) == 16, "ABI mismatch: Expected the offset of the element result in the type TestExecTestResultInfo to be 16, but a different offset was reported.")
//Testing: TestExecTestMetadata
TEST_EXEC_STATIC_ASSERT(sizeof(TestExecTestMetadata) == 40, "ABI mismatch: Expected size of type TestExecTestMetadata was 40 bytes, but an invalid size was reported.")
TEST_EXEC_STATIC_ASSERT(alignof(TestExecTestMetadata) == 8, "ABI mismatch: Expected alignment of type TestExecTestMetadata was 8 bytes, but an invalid alignment was reported.")
TEST_EXEC_STATIC_ASSERT(offsetof(TestExecTestMetadata, header) == 0, "ABI mismatch: Expected the offset of the element header in the type TestExecTestMetadata to be 0, but a different offset was reported.")
TEST_EXEC_STATIC_ASSERT(offsetof(TestExecTestMetadata, name) == 16, "ABI mismatch: Expected the offset of the element name in the type TestExecTestMetadata to be 16, but a different offset was reported.")
TEST_EXEC_STATIC_ASSERT(offsetof(TestExecTestMetadata, tags) == 24, "ABI mismatch: Expected the offset of the element tags in the type TestExecTestMetadata to be 24, but a different offset was reported.")
TEST_EXEC_STATIC_ASSERT(offsetof(TestExecTestMetadata, description) == 32, "ABI mismatch: Expected the offset of the element description in the type TestExecTestMetadata to be 32, but a different offset was reported.")

#ifdef __cplusplus
}
#endif

#endif
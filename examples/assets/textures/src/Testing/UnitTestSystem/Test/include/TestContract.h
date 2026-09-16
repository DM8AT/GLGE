/**
 * @file TestContract.h
 * @author DM8AT
 * @brief define a test contract all tests need to satisfy
 * @version 0.1
 * @date 2026-05-19
 * 
 * @copyright Copyright (c) 2026
 * 
 * @warning this file is written in C, not C++ because C is easier for shared libraries
 * 
 * On extension of this system: If a structure is introduced in a version and the version is released, then the structure may not be changed in the future. 
 * If the structure is extended in the future, the `pNext` chain should be used. 
 * 
 * ABI assumptions:
 * - All structures use default compiler alignment.
 * - Structure packing must not be modified.
 * - Enums are assumed to be 32-bit signed integers.
 * - All ABI participants must compile with compatible calling conventions.
 * 
 * Supported ABI model assumptions:
 * - 64-bit pointers
 * - 64-bit function pointers
 * - LP64 or LLP64 data model
 * - 8-byte alignment for pointers and double
 * - little-endian byte order for integers
 * 
 * Unsupported:
 * - nonstandard enum packing
 * - modified struct packing
 * - freestanding implementations with incompatible ABI rules
 * 
 * All floating-point types are assumed to be IEEE754-compatable. This holds for floats (IEEE-754 32 bit value) and doubles (IEEE-754 64 bit value). 
 * 
 * The ABI assumes that C23 or higher is used. 
 * 
 * All provided structures have clearly defined defaults. 
 */
//header guard
#ifndef _TEST_CONTRACT_
#define _TEST_CONTRACT_

#ifdef __cplusplus
extern "C" {
#endif

//fixed integers are required
#include <stdint.h>
//add bools
#include <stdbool.h>
//used for NULL
#include <stddef.h>
//used for assertions
#include <assert.h>

//helper to convert the contents of a define to a string

/**
 * @brief a helper to stringify defines
 */
#define TEST_STRINGIFY_HELPER(x) #x
/**
 * @brief a macro to convert the contents of a define into a string
 */
#define TEST_STRINGIFY(x) TEST_STRINGIFY_HELPER(x)

/**
 * @brief define the version of the test interface
 */
#define TEST_ABI_VERSION 1

//unify calling behaviour
//All functions used in the ABI must be marked with this. 
#if defined(_WIN32)
    /**
     * @brief A macro to specify how the call should be done
     */
    #define TEST_CALL __cdecl
#else
    /**
     * @brief A macro to specify how the call should be done
     */
    #define TEST_CALL
#endif

#if defined(_WIN32)
    //windows differentiates between import and export
    #if defined(TEST_BUILD)
        #define TEST_ABI __declspec(dllexport)
    #else
        #define TEST_ABI __declspec(dllimport)
    #endif
#else
    /**
     * @brief define that the call is an ABI call
     */
    #define TEST_ABI __attribute__((visibility("default")))
#endif

/**
 * @brief define the name of the test registration function
 * 
 * Assumed type: `PFN_RegisterTest`
 */
#define TEST_REGISTER_TEST test_registerTest
/**
 * @brief define the symbol for the test registration function
 */
#define TEST_REGISTER_TEST_SYM TEST_STRINGIFY(TEST_REGISTER_TEST)
/**
 * @brief a helper to define the function to register tests from an dynamic library
 */
#define TEST_REGISTER_TEST_FN extern "C" TEST_ABI void TEST_CALL TEST_REGISTER_TEST(uint64_t *size, Test *data)

/**
 * @brief define the name of the get abi function
 * 
 * Assumed type: `PFN_GetTestABIVersion`
 */
#define TEST_GET_ABI_VERSION test_getABIVersion
/**
 * @brief define the symbol for the test get abi function
 */
#define TEST_GET_ABI_VERSION_SYM TEST_STRINGIFY(TEST_GET_ABI_VERSION)
/**
 * @brief a helper to define the function to get the ABI version used by an dynamic library
 */
#define TEST_GET_ABI_VERSION_FN extern "C" TEST_ABI uint64_t TEST_CALL TEST_GET_ABI_VERSION()

/**
 * @brief a helper for compile-time ABI comptability tests
 * 
 * @param x the expression to evaluate
 * @param msg the message to print in the error if `x` evaluated to `false`
 */
#define TEST_STATIC_ASSERT(x, msg) static_assert(x, msg);

/**
 * @brief define a null handle
 */
#define TEST_NULL_HANDLE UINT64_MAX
/**
 * @brief define an invalid test ID
 */
#define TEST_INVALID_TEST_ID UINT64_MAX

/**
 * @brief define the size of the test report message in amount of characters
 */
#define TEST_REPORT_MSG_SIZE 1024
/**
 * @brief define the maximum amount of characters a path may have
 */
#define TEST_MAX_FILE_CHAR_COUNT 512

/**
 * @brief store all test types
 */
typedef enum e_TestType {
    /**
     * @brief the type is just a test base
     * 
     * Introduced in ABI version 1
     */
    TEST_BASE = 0,
    /**
     * @brief the type is a test report
     * 
     * Introduced in ABI version 1
     */
    TEST_REPORT = 1,
    /**
     * @brief the type is a test entry
     * 
     * Introduced in ABI version 1
     */
    TEST_ENTRY = 2,
    /**
     * @brief the type is a test color
     * 
     * Introduced in ABI version 1
     */
    TEST_COLOR = 3,
    /**
     * @brief the type is a test message
     * 
     * Introduced in ABI version 1
     */
    TEST_MESSAGE = 4,
    /**
     * @brief the type is a test file marker
     * 
     * Introduced in ABI version 1
     */
    TEST_FILE_MARKER = 5,
    /**
     * @brief the type is an assertion
     * 
     * Introduced in ABI version 1
     */
    TEST_ASSERTION = 6,
    /**
     * @brief the type is a test (fptr + metadata container)
     * 
     * Introduced in ABI version 1
     */
    TEST_TEST = 7,
    /**
     * @brief the type is test functions
     * 
     * Introduced in ABI version 1
     */
    TEST_TEST_FUNCTIONS = 8,

    /**
     * @brief maximum test type value
     * 
     * Used solely to force enum storage size compatibility. Not a valid runtime value.
     */
    TEST_TYPE_MAX = 0x7fffffff
} TestType;

/**
 * @brief define a base structure for all test structures. This structure is always required to exist as the first element. 
 * 
 * If this structure exists it must be directly at the beginning of the structure
 */
typedef struct  s_TestBase {
    /**
     * @brief store an identification of the type this structure is contained in
     */
    TestType sType = TEST_BASE;
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
} TestBase;

/**
 * @brief store the resulting state of a test
 */
typedef enum e_TestResult {
    /**
     * @brief the test finished successfully
     * 
     * This means the expected result was reached, NOT that no errors were thrown. 
     */
    TEST_SUCCESS = 0,
    /**
     * @brief something went unexpectedly, but in a way that did not hinder the test execution
     */
    TEST_WARNING = 1,
    /**
     * @brief the test failed and the system detected the failure
     */
    TEST_CONTROLLED_FAIL = 2,
    /**
     * @brief the system terminated the test
     * 
     * This is e.g. X force-closing the app or segmentation fault
     */
    TEST_UNCONTROLLED_FAIL = 3,
    /**
     * @brief the test was not executed
     */
    TEST_SKIPPED = 4,
    /**
     * @brief something about the test report seems wrong, the report was deemed not reliable. 
     */
    TEST_INVALID = 5,
    /**
     * @brief the test took longer than the specified `timeout`
     */
    TEST_TIMED_OUT = 6,



    /**
     * @brief maximum enum value
     * 
     * Used solely to force enum storage size compatibility. Not a valid runtime value.
     */
    TEST_RESULT_MAX = 0x7fffffff
} TestResult;

/**
 * @brief define flags that instruct the scheduler what to keep in mind when scheduling the tests
 */
typedef enum e_TestRequirementBits {
    /**
     * @brief This test should run when no other tests are running
     */
    TEST_REQUIREMENT_RUN_EXCLUSIVE_BIT = 0x00000001,
    /**
     * @brief This test wants to access the GPU
     * 
     * This means the test should be skipped if no GPU is detected. 
     */
    TEST_REQUIREMENT_GPU_BIT = 0x00000002,
    /**
     * @brief This test wants to exclusively access the GPU
     * 
     * This means the test should be skipped if no GPU is detected and the test should be scheduled to run while no other test with this flag or the `TEST_REQUIREMENT_GPU_BIT` is running. 
     */
    TEST_REQUIREMENT_GPU_ACCESS_EXCLUSIVE_BIT = 0x00000004,
    /**
     * @brief This test wants to access files
     * 
     * This means the test should be run with access to the asset directory
     */
    TEST_REQUIREMENT_FILE_ACCESS_BIT = 0x00000008,
    /**
     * @brief This test wants to access files exclusively
     * 
     * This means the test should be run with access to the asset directory and not while other tests that have this flag or `TEST_REQUIREMENT_FILE_ACCESS_BIT` set are running. 
     */
    TEST_REQUIREMENT_FILE_ACCESS_EXCLUSIVE_BIT = 0x00000010,
    /**
     * @brief This test potentially consumes huge amount of RAM
     * 
     * This means the test should be run while no other tests with this flag are running
     */
    TEST_REQUIREMENT_RAM_EXCLUSIVE_BIT = 0x00000020,
    /**
     * @brief The test needs to access the network
     * 
     * This means the test must be skipped in offline mode
     */
    TEST_REQUIREMENT_NETWORKING_BIT = 0x00000040,
    /**
     * @brief The test needs to access the network and wants to access it exclusively
     * 
     * This means the test must be skipped in offline mode and must run while no other tests with this flag or `TEST_REQUIREMENT_NETWORKING_BIT` are running. 
     */
    TEST_REQUIREMENT_NETWORKING_EXCLUSIVE_BIT = 0x00000080,
    /**
     * @brief The test needs to be able to run stuff async, e.g. using threads or subjobs. 
     * 
     * This means the test must be skipped if async tests are not allowed. 
     */
    TEST_REQUIREMENT_ASYNC_BIT = 0x00000100,

    /**
     * @brief maximum enum value
     * 
     * Used solely to force enum storage size compatibility. Not a valid runtime value.
     */
    TEST_REQUIREMENT_MAX_BIT = 0x7fffffff
} TestRequirementBits;

/**
 * @brief define the mask for all test requirement bits
 */
typedef uint32_t TestRequirementFlags;

/**
 * @brief a structure to store information about where something happened
 */
typedef struct s_TestFileMarker {
    /**
     * @brief include the test type header
     */
    TestBase header = {
        .sType = TEST_FILE_MARKER,
        .pNext = NULL
    };

    /**
     * @brief a marker for "what"
     */
    const char* expression = NULL;
    /**
     * @brief store the path to the file
     * 
     * the root of the path is `rootDir`. Absolute paths are not allowed. Going back a directory can be done with `..`. The home directory marker (`~`) is not allowed. 
     */
    const char* file = NULL;
    /**
     * @brief store in what line
     * 
     * The first line if the file is 1. Line 0 is considered invalid. 
     */
    uint64_t line = 0;
} TestFileMarker;

/**
 * @brief store a report for a single test
 * 
 * Important: All strings are assumed to be static. 
 */
typedef struct s_TestReport {
    /**
     * @brief include the test type header
     */
    TestBase header = {
        .sType = TEST_REPORT,
        .pNext = NULL
    };

    /**
     * @brief store the result of the test
     */
    TestResult result = TEST_UNCONTROLLED_FAIL;
    /**
     * @brief store a reported message
     * 
     * This may include further information about an error. 
     */
    const char msg[TEST_REPORT_MSG_SIZE] {};
} TestReport;

/**
 * @brief define the type of send message
 */
typedef enum e_TestMessageType {
    /**
     * @brief debugging information
     */
    TEST_MESSAGE_TYPE_DEBUG = 0,
    /**
     * @brief general information
     */
    TEST_MESSAGE_TYPE_INFO = 1,
    /**
     * @brief a simple warning
     */
    TEST_MESSAGE_TYPE_WARNING = 2,
    /**
     * @brief a higher severity warning
     */
    TEST_MESSAGE_TYPE_CRITICAL_WARNING = 3,
    /**
     * @brief something went wrong
     */
    TEST_MESSAGE_TYPE_ERROR = 4,
    /**
     * @brief something went really wrong
     */
    TEST_MESSAGE_TYPE_FATAL_ERROR = 5,

    /**
     * @brief the maximum test message type enum
     * 
     * Used solely to force enum storage size compatibility. Not a valid runtime value.
     */
    TEST_MESSAGE_TYPE_MAX = 0x7fffffff
} TestMessageType;

/**
 * @brief define the test message color
 * 
 * The color is stored in the sRGB color space. 
 */
typedef struct s_TestMessageColor {
    /**
     * @brief include the test type header
     */
    TestBase header = {
        .sType = TEST_COLOR,
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
} TestMessageColor;

/**
 * @brief define a message from a test
 */
typedef struct s_TestMessage {
    /**
     * @brief include the test type header
     */
    TestBase header {
        .sType = TEST_MESSAGE,
        .pNext = NULL
    };

    /**
     * @brief store from where the message is
     * 
     * If this is `NULL` no location information is given
     */
    TestFileMarker* at = NULL;
    /**
     * @brief store the color for the test message
     * 
     * If this is `NULL`, a default color of true black (red -> none, green -> none, blue -> none, alpha -> none) is assumed. 
     */
    TestMessageColor* color = NULL;
    /**
     * @brief store the test message
     * 
     * The message is only referenced. It must stay valid while the test message structure exists. 
     */
    const char* msg = NULL;
    /**
     * @brief store what type of message this is
     */
    TestMessageType messageType = TEST_MESSAGE_TYPE_FATAL_ERROR;
} TestMessage;

/**
 * @brief a structure to define what an assertion stores
 * 
 * The log message for an assertion looks like this:
 * [TIMESTAMP] at(if `at != NULL`) [OPTIONAL LOCATION]: Expected [expected] and got [actual]. Assertion passed: `passed ? "true" : "false"`
 * 
 * Assertions are always included in all logging forms. 
 * 
 * If an assertion with `passed = false` is received, the test will be killed immediately and the test will be considered `TEST_CONTROLLED_FAIL`.
 */
typedef struct s_TestAssertion {
    /**
     * @brief include the test type header
     */
    TestBase header {
        .sType = TEST_ASSERTION,
        .pNext = NULL
    };

    /**
     * @brief define the location the assertion originates from
     * 
     * If this is `NULL`, no location information will be logged
     */
    TestFileMarker* at = NULL;

    /**
     * @brief store the color for the test message
     * 
     * If this is `NULL`, a default color of true black (red -> none, green -> none, blue -> none, alpha -> none) is assumed. 
     */
    TestMessageColor* color = NULL;

    /**
     * @brief describe what was expected
     */
    const char* expected = NULL;
    /**
     * @brief describe what was the gotten value
     */
    const char* actual = NULL;
    /**
     * @brief say if the assertion passed or failed
     * 
     * `true` if passed, `false` if failed. 
     * 
     * This is essentially a bool. Due to portability it is stored as an `uint8_t`. 0 is mapped to false and everything else is mapped to true. 
     */
    uint8_t passed = 0;
} TestAssertion;

/**
 * @brief define a context for a test
 * 
 * This structure is not extendable by design
 */
typedef struct s_TestContext {
    /**
     * @brief a directory that only exists while the test runs
     * 
     * The test system creates it and deletes it when the test finishes. Temporary files can be stored here. 
     */
    const char tmpDir[TEST_MAX_FILE_CHAR_COUNT] {};
    /**
     * @brief a directory to consider the root of execution. This must be an absolute path. 
     * 
     * This is specified by the test system invoker. 
     */
    const char rootDir[TEST_MAX_FILE_CHAR_COUNT] {};

    /**
     * @brief a random input seed
     */
    uint64_t randomSeed = 0;
} TestContext;

/**
 * @brief store all test API specific functions a test may call
 */
typedef struct s_TestFunctions {
    /**
     * @brief include the header
     */
    TestBase header = {
        .sType = TEST_TEST_FUNCTIONS,
        .pNext = NULL
    };

    /**
     * @brief a function to log a simple message
     * 
     * This function may be called concurrently. 
     * 
     * If the test message pointer is `NULL`, the function will return immediately and safely. This is legal. 
     */
    void (*log)(const TestMessage*) = NULL;

    /**
     * @brief a function to log and potentially throw an assertion
     * 
     * This function may be called concurrently. 
     * 
     * If the test assertion pointer is `NULL`, the function will return immediately and safely. This is legal. 
     * 
     * This function may stop the test if TestAssertion::passed is not 1. 
     */
    void (*assertion)(const TestAssertion*) = NULL;
} TestFunctions;

/**
 * @brief the type of a function pointer that is used to invoke the test
 * 
 * the passed test context is owned by the invoker of the test. It must live in static memory. 
 */
typedef void (TEST_CALL *PFN_TestInvoker)(const TestContext*, TestReport*, const TestFunctions*);

/**
 * @brief define information about a single test
 */
typedef struct s_TestEntry {
    /**
     * @brief include the test type header
     */
    TestBase header {
        .sType = TEST_ENTRY,
        .pNext = NULL
    };

    /**
     * @brief store the test-system registered name of the test
     * 
     * Inside a single dynamic library this name must be unique. 
     * 
     * @warning `NULL` is illegal. `NULL` will skip the test. 
     */
    const char* name = NULL;
    /**
     * @brief store the tags of the test
     * 
     * A tag is used for filtering. The `tags` string may contain multiple tags. All tags must be separated by a space. 
     * Legal characters inside a single tag are alphanumeric + special characters. Spaces are invalid and used to separate tags. 
     * 
     * A `tags` string may look like this: "Graphic Performance Vulkan SDL3" \ 
     * this would be interpreted as the tags "Graphic", "Performance", "Vulkan" and "SDL3". 
     * 
     * A test is shown and run during filtering when at least a single tag matches. 
     */
    const char* tags = NULL;
    /**
     * @brief store a description of the test
     * 
     * This includes what the test tested and how. `NULL` is legal. 
     */
    const char* description = NULL;
    /**
     * @brief the amount of milliseconds after which the test is considered timed out
     * 
     * This timeout specifies the time AT LEAST taken before timing it. The timeout happens if the scheduler detects the task ran for more than `timeout` milliseconds. 
     * If a timeout is detected, the task is killed immediately and the test is considered `TEST_TIMED_OUT`.
     */
    uint64_t timeout = UINT64_MAX;
    /**
     * @brief store the requirements for the test
     */
    TestRequirementFlags requirements = 0;
} TestEntry;

/**
 * @brief a structure that stores all information to invoke and reflect on a single test
 */
typedef struct s_Test {
    /**
     * @brief include the header
     */
    TestBase header {
        .sType = TEST_TEST,
        .pNext = NULL
    };

    /**
     * @brief store the test metadata
     */
    TestEntry entry;
    /**
     * @brief store the function pointer used to invoke the test
     */
    PFN_TestInvoker invoker = NULL;
} Test;

/**
 * @brief get the test ABI version from a test
 */
typedef uint64_t (TEST_CALL *PFN_GetTestABIVersion)();

/**
 * @brief the type of a function pointer to a function to register a test
 * 
 * This works similar to `vkEnumerate*` calls. 
 * 
 * This call quarries the data. The `Test*` must be owned by the caller. It is only accessed by the callee. 
 * 
 * @param size a pointer to the size of an existing array. Filled with the required size if `data` is `NULL`.
 * @param data a pointer to an array with at least `size` elements to write the invoker functions to. If `data` is `NULL`, `size` will be filled with the required size. 
 */
typedef void (TEST_CALL *PFN_RegisterTest)(uint64_t* size, Test* data);

//test compatibility
//Testing: TestType
TEST_STATIC_ASSERT(sizeof(TestType) == 4, "ABI mismatch: Expected size of type TestType was 4 bytes, but an invalid size was reported.")
TEST_STATIC_ASSERT(alignof(TestType) == 4, "ABI mismatch: Expected alignment of type TestType was 4 bytes, but an invalid alignment was reported.")
//Testing: TestBase
TEST_STATIC_ASSERT(sizeof(TestBase) == 16, "ABI mismatch: Expected size of type TestBase was 16 bytes, but an invalid size was reported.")
TEST_STATIC_ASSERT(alignof(TestBase) == 8, "ABI mismatch: Expected alignment of type TestBase was 8 bytes, but an invalid alignment was reported.")
TEST_STATIC_ASSERT(offsetof(TestBase, sType) == 0, "ABI mismatch: Expected the offset of the element sType in the type TestBase to be 0, but a different offset was reported.")
TEST_STATIC_ASSERT(offsetof(TestBase, pNext) == 8, "ABI mismatch: Expected the offset of the element pNext in the type TestBase to be 8, but a different offset was reported.")
//Testing: TestResult
TEST_STATIC_ASSERT(sizeof(TestResult) == 4, "ABI mismatch: Expected size of type TestResult was 4 bytes, but an invalid size was reported.")
TEST_STATIC_ASSERT(alignof(TestResult) == 4, "ABI mismatch: Expected alignment of type TestResult was 4 bytes, but an invalid alignment was reported.")
//Testing: TestRequirement flags
TEST_STATIC_ASSERT(sizeof(TestRequirementBits) == 4, "ABI mismatch: Expected size of type TestRequirementBits was 4 bytes, but an invalid size was reported.")
TEST_STATIC_ASSERT(alignof(TestRequirementBits) == 4, "ABI mismatch: Expected alignment of type TestRequirementBits was 4 bytes, but an invalid alignment was reported.")
TEST_STATIC_ASSERT(sizeof(TestRequirementFlags) == 4, "ABI mismatch: Expected size of type TestRequirementFlags was 4 bytes, but an invalid size was reported.")
TEST_STATIC_ASSERT(alignof(TestRequirementFlags) == 4, "ABI mismatch: Expected alignment of type TestRequirementFlags was 4 bytes, but an invalid alignment was reported.")
//Testing: TestFileMarker
TEST_STATIC_ASSERT(sizeof(TestFileMarker) == 40, "ABI mismatch: Expected size of type TestFileMarker was 40 bytes, but an invalid size was reported.")
TEST_STATIC_ASSERT(alignof(TestFileMarker) == 8, "ABI mismatch: Expected alignment of type TestFileMarker was 8 bytes, but an invalid alignment was reported.")
TEST_STATIC_ASSERT(offsetof(TestFileMarker, header) == 0, "ABI mismatch: Expected the offset of the element header in the type TestFileMarker to be 0, but a different offset was reported.")
TEST_STATIC_ASSERT(offsetof(TestFileMarker, expression) == 16, "ABI mismatch: Expected the offset of the element expression in the type TestFileMarker to be 16, but a different offset was reported.")
TEST_STATIC_ASSERT(offsetof(TestFileMarker, file) == 24, "ABI mismatch: Expected the offset of the element file in the type TestFileMarker to be 24, but a different offset was reported.")
TEST_STATIC_ASSERT(offsetof(TestFileMarker, line) == 32, "ABI mismatch: Expected the offset of the element line in the type TestFileMarker to be 32, but a different offset was reported.")
//Testing: TestReport
/**
 * @brief test the TestReport
 */
TEST_STATIC_ASSERT(sizeof(TestReport) == 24 + (sizeof(char)*TEST_REPORT_MSG_SIZE), "ABI mismatch: Expected size of type TestReport was ${24 + (sizeof(char)*TEST_REPORT_MSG_SIZE)} bytes, but an invalid size was reported.")
TEST_STATIC_ASSERT(alignof(TestReport) == 8, "ABI mismatch: Expected alignment of type TestReport was 8 bytes, but an invalid alignment was reported.")
TEST_STATIC_ASSERT(offsetof(TestReport, header) == 0, "ABI mismatch: Expected the offset of the element header in the type TestReport to be 0, but a different offset was reported.")
TEST_STATIC_ASSERT(offsetof(TestReport, result) == 16, "ABI mismatch: Expected the offset of the element result in the type TestReport to be 16, but a different offset was reported.")
TEST_STATIC_ASSERT(offsetof(TestReport, msg) == 20, "ABI mismatch: Expected the offset of the element msg in the type TestReport to be 20, but a different offset was reported.")
//Testing: MessageType
TEST_STATIC_ASSERT(sizeof(TestMessageType) == 4, "ABI mismatch: Expected size of type TestMessageType was 4 bytes, but an invalid size was reported.")
TEST_STATIC_ASSERT(alignof(TestMessageType) == 4, "ABI mismatch: Expected alignment of type TestMessageType was 4 bytes, but an invalid alignment was reported.")
//Testing: TestMessageColor
TEST_STATIC_ASSERT(sizeof(TestMessageColor) == 24, "ABI mismatch: Expected size of type TestMessageColor was 24 bytes, but an invalid size was reported.")
TEST_STATIC_ASSERT(alignof(TestMessageColor) == 8, "ABI mismatch: Expected alignment of type TestMessageColor was 8 bytes, but an invalid alignment was reported.")
TEST_STATIC_ASSERT(offsetof(TestMessageColor, header) == 0, "ABI mismatch: Expected the offset of the element header in the type TestMessageColor to be 0, but a different offset was reported.")
TEST_STATIC_ASSERT(offsetof(TestMessageColor, red) == 16, "ABI mismatch: Expected the offset of the element red in the type TestMessageColor to be 16, but a different offset was reported.")
TEST_STATIC_ASSERT(offsetof(TestMessageColor, green) == 17, "ABI mismatch: Expected the offset of the element green in the type TestMessageColor to be 17, but a different offset was reported.")
TEST_STATIC_ASSERT(offsetof(TestMessageColor, blue) == 18, "ABI mismatch: Expected the offset of the element blue in the type TestMessageColor to be 18, but a different offset was reported.")
TEST_STATIC_ASSERT(offsetof(TestMessageColor, alpha) == 19, "ABI mismatch: Expected the offset of the element alpha in the type TestMessageColor to be 19, but a different offset was reported.")
//Testing: TestMessage
TEST_STATIC_ASSERT(sizeof(TestMessage) == 48, "ABI mismatch: Expected size of type TestMessage was 56 bytes, but an invalid size was reported.")
TEST_STATIC_ASSERT(alignof(TestMessage) == 8, "ABI mismatch: Expected alignment of type TestMessage was 8 bytes, but an invalid alignment was reported.")
TEST_STATIC_ASSERT(offsetof(TestMessage, header) == 0, "ABI mismatch: Expected the offset of the element header in the type TestMessage to be 0, but a different offset was reported.")
TEST_STATIC_ASSERT(offsetof(TestMessage, at) == 16, "ABI mismatch: Expected the offset of the element at in the type TestMessage to be 16, but a different offset was reported.")
TEST_STATIC_ASSERT(offsetof(TestMessage, color) == 24, "ABI mismatch: Expected the offset of the element color in the type TestMessage to be 24, but a different offset was reported.")
TEST_STATIC_ASSERT(offsetof(TestMessage, msg) == 32, "ABI mismatch: Expected the offset of the element msg in the type TestMessage to be 32, but a different offset was reported.")
TEST_STATIC_ASSERT(offsetof(TestMessage, messageType) == 40, "ABI mismatch: Expected the offset of the element messageType in the type TestMessage to be 40, but a different offset was reported.")
//Testing: TestAssertion
TEST_STATIC_ASSERT(sizeof(TestAssertion) == 56, "ABI mismatch: Expected size of type TestAssertion was 64 bytes, but an invalid size was reported.")
TEST_STATIC_ASSERT(alignof(TestAssertion) == 8, "ABI mismatch: Expected alignment of type TestResult was 8 bytes, but an invalid alignment was reported.")
TEST_STATIC_ASSERT(offsetof(TestAssertion, header) == 0, "ABI mismatch: Expected the offset of the element header in the type TestAssertion to be 0, but a different offset was reported.")
TEST_STATIC_ASSERT(offsetof(TestAssertion, at) == 16, "ABI mismatch: Expected the offset of the element at in the type TestAssertion to be 16, but a different offset was reported.")
TEST_STATIC_ASSERT(offsetof(TestAssertion, color) == 24, "ABI mismatch: Expected the offset of the element color in the type TestAssertion to be 24, but a different offset was reported.")
TEST_STATIC_ASSERT(offsetof(TestAssertion, expected) == 32, "ABI mismatch: Expected the offset of the element expected in the type TestAssertion to be 32, but a different offset was reported.")
TEST_STATIC_ASSERT(offsetof(TestAssertion, actual) == 40, "ABI mismatch: Expected the offset of the element actual in the type TestAssertion to be 40, but a different offset was reported.")
TEST_STATIC_ASSERT(offsetof(TestAssertion, passed) == 48, "ABI mismatch: Expected the offset of the element passed in the type TestAssertion to be 48, but a different offset was reported.")
//Testing: TestContext
TEST_STATIC_ASSERT(sizeof(TestContext) == 8 + (sizeof(char)*(TEST_MAX_FILE_CHAR_COUNT*2)), "ABI mismatch: Expected size of type TestContext was ${8 + (sizeof(char)*(TEST_MAX_FILE_CHAR_COUNT*2))} bytes, but an invalid size was reported.")
TEST_STATIC_ASSERT(alignof(TestContext) == 8, "ABI mismatch: Expected alignment of type TestContext was 8 bytes, but an invalid alignment was reported.")
TEST_STATIC_ASSERT(offsetof(TestContext, tmpDir) == 0, "ABI mismatch: Expected the offset of the element tmpDir in the type TestContext to be 0, but a different offset was reported.")
TEST_STATIC_ASSERT(offsetof(TestContext, rootDir) == (sizeof(char)*TEST_MAX_FILE_CHAR_COUNT), "ABI mismatch: Expected the offset of the element rootDir in the type TestContext to be ${sizeof(char)*TEST_MAX_FILE_CHAR_COUNT}, but a different offset was reported.")
TEST_STATIC_ASSERT(offsetof(TestContext, randomSeed) == (sizeof(char)*TEST_MAX_FILE_CHAR_COUNT*2), "ABI mismatch: Expected the offset of the element randomSeed in the type TestContext to be ${(sizeof(char)*TEST_MAX_FILE_CHAR_COUNT*2)}, but a different offset was reported.")
//Testing: Test Functions
TEST_STATIC_ASSERT(sizeof(TestFunctions) == 32, "ABI mismatch: Expected size of type TestFunctions was 24 bytes, but an invalid size was reported.")
TEST_STATIC_ASSERT(alignof(TestFunctions) == 8, "ABI mismatch: Expected alignment of type TestFunctions was 8 bytes, but an invalid alignment was reported.")
TEST_STATIC_ASSERT(offsetof(TestFunctions, header) == 0, "ABI mismatch: Expected the offset of the element header in the type TestFunctions to be 0, but a different offset was reported.")
TEST_STATIC_ASSERT(offsetof(TestFunctions, log) == 16, "ABI mismatch: Expected the offset of the element log in the type TestFunctions to be 16, but a different offset was reported.")
TEST_STATIC_ASSERT(offsetof(TestFunctions, assertion) == 24, "ABI mismatch: Expected the offset of the element assertion in the type TestFunctions to be 24, but a different offset was reported.")
//Testing: PFN_TestInvoker
TEST_STATIC_ASSERT(sizeof(PFN_TestInvoker) == 8, "ABI mismatch: Expected size of type PFN_TestInvoker was 8 bytes, but an invalid size was reported.")
TEST_STATIC_ASSERT(alignof(PFN_TestInvoker) == 8, "ABI mismatch: Expected alignment of type PFN_TestInvoker was 8 bytes, but an invalid alignment was reported.")
//Testing: TestEntry
TEST_STATIC_ASSERT(sizeof(TestEntry) == 56, "ABI mismatch: Expected size of type TestEntry was 56 bytes, but an invalid size was reported.")
TEST_STATIC_ASSERT(alignof(TestEntry) == 8, "ABI mismatch: Expected alignment of type TestEntry was 8 bytes, but an invalid alignment was reported.")
TEST_STATIC_ASSERT(offsetof(TestEntry, header) == 0, "ABI mismatch: Expected the offset of the element header in the type TestEntry to be 0, but a different offset was reported.")
TEST_STATIC_ASSERT(offsetof(TestEntry, name) == 16, "ABI mismatch: Expected the offset of the element name in the type TestEntry to be 16, but a different offset was reported.")
TEST_STATIC_ASSERT(offsetof(TestEntry, tags) == 24, "ABI mismatch: Expected the offset of the element tags in the type TestEntry to be 24, but a different offset was reported.")
TEST_STATIC_ASSERT(offsetof(TestEntry, description) == 32, "ABI mismatch: Expected the offset of the description expected in the type TestEntry to be 32, but a different offset was reported.")
TEST_STATIC_ASSERT(offsetof(TestEntry, timeout) == 40, "ABI mismatch: Expected the offset of the element timeout in the type TestEntry to be 40, but a different offset was reported.")
TEST_STATIC_ASSERT(offsetof(TestEntry, requirements) == 48, "ABI mismatch: Expected the offset of the element requirements in the type TestEntry to be 48, but a different offset was reported.")
//Testing: Test
TEST_STATIC_ASSERT(sizeof(Test) == 80, "ABI mismatch: Expected size of type Test was 80 bytes, but an invalid size was reported.")
TEST_STATIC_ASSERT(alignof(Test) == 8, "ABI mismatch: Expected alignment of type Test was 8 bytes, but an invalid alignment was reported.")
TEST_STATIC_ASSERT(offsetof(Test, header) == 0, "ABI mismatch: Expected the offset of the element header in the type Test to be 0, but a different offset was reported.")
TEST_STATIC_ASSERT(offsetof(Test, entry) == 16, "ABI mismatch: Expected the offset of the element entry in the type Test to be 16, but a different offset was reported.")
TEST_STATIC_ASSERT(offsetof(Test, invoker) == 72, "ABI mismatch: Expected the offset of the element invoker in the type Test to be 72, but a different offset was reported.")
//Testing: PFN_GetTestABIVersion
TEST_STATIC_ASSERT(sizeof(PFN_GetTestABIVersion) == 8, "ABI mismatch: Expected size of type PFN_GetTestABIVersion was 8 bytes, but an invalid size was reported.")
TEST_STATIC_ASSERT(alignof(PFN_GetTestABIVersion) == 8, "ABI mismatch: Expected alignment of type PFN_GetTestABIVersion was 8 bytes, but an invalid alignment was reported.")
//Testing: PFN_RegisterTest
TEST_STATIC_ASSERT(sizeof(PFN_RegisterTest) == 8, "ABI mismatch: Expected size of type PFN_RegisterTest was 8 bytes, but an invalid size was reported.")
TEST_STATIC_ASSERT(alignof(PFN_RegisterTest) == 8, "ABI mismatch: Expected alignment of type PFN_RegisterTest was 8 bytes, but an invalid alignment was reported.")

#ifdef __cplusplus
}
#endif

#endif
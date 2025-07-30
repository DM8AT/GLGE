/**
 * @file main.cpp
 * @author DM8AT
 * @brief a list of automated tests for GLGE
 * @version 0.1
 * @date 2025-07-30
 * 
 * @copyright Copyright (c) 2025
 * 
 */

//include GLGE - it should be tested
#include "../../GLGE/GLGE.h"

//inlcude the test functions
#include "tests.h"

//store how many successfull tests where run
uint64_t successfull_tests = 0;
//store the overall count of tests
uint64_t test_count = 0;

/**
 * @brief run a single unit test
 * 
 * @param func a pointer to the function to run
 * @param name the name of the unit test to identify the test in the console
 * @param expectError true : the expected result of the test is an error | false : the expected result is no error (default)
 */
void runUnitTest(void (*func)(), const char* name, bool expectError = false)
{
    //reset the error flag
    error = false;

    //try-catch to get all exceptions
    try
    {
        //run the function
        (*func)();
    }
    catch (...)
    {
        //now, defenetly something went wrong (exceptions are not part of GLGE for C compatability)
        error = true;
    }
    
    //if an error is expected, invert the result
    if (expectError) {error = !error;}

    //print if the test was succesfull
    std::cout << "Result of test \"" << name << "\" is: " << (error ? "\x1B[0;91mFailure\x1B[0m" : "\x1B[0;92mSuccess\x1B[0m") << "\n";
    //if the test was successfull, increment the success count
    successfull_tests += !error;
    //regardless of success increment the amount of run tests
    ++test_count;
}

int main()
{
    //message that unit testing was started
    std::cout << "---------------------------------------\nStarted unit tests\n---------------------------------------\n";
    
    //run all the tests
    runUnitTest(testSimpleWindow, "Simple window test");
    runUnitTest(testCleardWindow, "Clearing window test");
    runUnitTest(testInvalidAPI, "Invalid API test", true);
    runUnitTest(testMat4, "Matrix 4x4 functionality test");

    //print the overall result
    std::cout << "---------------------------------------\nOverall:\n    Successfull: "
              << successfull_tests << "\n    Failed: " << (test_count - successfull_tests)
              << "\n---------------------------------------\n";
}
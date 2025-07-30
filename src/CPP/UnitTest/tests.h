/**
 * @file tests.h
 * @author DM8AT
 * @brief all tests available for the unit testing
 * @version 0.1
 * @date 2025-07-30
 * 
 * @copyright Copyright (c) 2025
 * 
 */

//only include once
#pragma once

//store if the error logger detected an error
extern bool error;

//test if a simple window can be constructed
void testSimpleWindow();

//store that a window can be constructed and exist for some time while it is filled with a constant color
void testCleardWindow();

//test that creating a window with an API that does not support windows (API_NONE is always undefined)
//results in errors
// WARNING: Expected result is an error
void testInvalidAPI();

//test all the mat4 functions
//internal prints to narrow down error searching
void testMat4();
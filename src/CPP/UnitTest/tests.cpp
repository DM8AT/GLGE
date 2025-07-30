/**
 * @file tests.cpp
 * @author DM8AT
 * @brief implement all test functions for the unit testing
 * @version 0.1
 * @date 2025-07-30
 * 
 * @copyright Copyright (c) 2025
 * 
 */

//include the header for all the functions
#include "tests.h"

//store if the error logger detected an error
bool error = false;

//include GLGE for the tests
#include "../../GLGE/GLGE.h"

/**
 * @brief a class to set the error flag if one of the following messages occours:
 *  - Fatal Error
 *  - Error
 *  - Critical Warning
 *  - Warning (to be extra shure everything works)
 */
class ErrorLogger final : public Logger
{
public:

    /**
     * @brief Destroy the Error Logger
     * print everything on destruction
     */
    virtual ~ErrorLogger() {printAll();}

    /**
     * @brief update the error flag, dosn't actually print
     */
    virtual void print() noexcept
    {
        //get the top most message
        Message msg = m_messages[0];
        //update the error flag acordingly
        error |= (msg.getMessageType() == MESSAGE_TYPE_FATAL_ERROR) ||
                 (msg.getMessageType() == MESSAGE_TYPE_FATAL_ERROR) ||
                 (msg.getMessageType() == MESSAGE_TYPE_CRITICAL_WARNING) || 
                 (msg.getMessageType() == MESSAGE_TYPE_WARNING);
    }

};

//test if a simple window can be constructed
void testSimpleWindow()
{
    //create an instance
    Instance inst("Simple instance", API_OPENGL_4_6);
    //insert the error logger
    inst.setLogger(new ErrorLogger());
    //add a window
    Window win("Simple Window", 600, 0, WINDOW_SETTINGS_DEFAULT, inst);
    inst.syncGraphicSetup();
}

//store that a window can be constructed and exist for some time while it is filled with a constant color
void testCleardWindow()
{
    //create an instance
    Instance inst("Clear instance", API_OPENGL_4_6);
    //insert the error logger
    inst.setLogger(new ErrorLogger());
    //add a window
    Window win("Clear window", 600, 0, WINDOW_SETTINGS_DEFAULT, inst);
    inst.syncGraphicSetup();

    //create a simple framebuffer
    Framebuffer fbuff(1, false, false, false, win.getSize(), inst);

    //construct a simple render pipeline
    RenderStage stages[] = {
        RenderStage(RENDER_STAGE_CLEAR, ClearStageData(&fbuff), 0,0,0),
        RenderStage(RENDER_STAGE_BLIT_TO_WINDOW, BlitToWindowStageData(&fbuff, &win), 0,0,0),
        RenderStage(RENDER_STAGE_SWAP_WINDOW, SwapWindowData(&win), 0,0,0)
    };
    RenderPipeline pipe(stages, sizeof(stages)/sizeof(*stages), true, inst);

    //wait a bit while the window is open
    std::this_thread::sleep_for(std::chrono::milliseconds(500));
}

//test that creating a window with an API that does not support windows (API_NONE is always undefined)
//results in errors
// WARNING: Expected result is an error
void testInvalidAPI()
{
    //create an instance with no correct graphic instance
    Instance inst("Invalid instance", API_NONE);
    inst.setLogger(new ErrorLogger());
    //create a window with the invalid API
    Window win("Invalid window", 600, 0, WINDOW_SETTINGS_DEFAULT, inst);
    inst.syncGraphicSetup();
}

/**
 * @brief a simple utility function to compare two 4x4 matricies without the build-in operator (bad to use functions if they may be buggy)
 * 
 * @param a a pointer to the first matrix interperted as a float pointer
 * @param b a pointer to the second matrix interperted as a float pointer
 * @return true : the matricies are NOT identically
 * @return false : the matriceies are identical
 */
static inline constexpr bool util_compareMat4Inv(const float* a, const float* b)
{
    //compare all values of the matricies
    return !(
        a[0] == b[0]  && a[1] == b[1]  && a[2] == b[2]  && a[3] == b[3]  &&
        a[4] == b[4]  && a[5] == b[5]  && a[6] == b[6]  && a[7] == b[7]  &&
        a[8] == b[8]  && a[9] == b[9]  && a[10]== b[10] && a[11]== b[11] &&
        a[12]== b[12] && a[13]== b[13] && a[14]== b[14] && a[15]== b[15]
    );
}

//test all the mat4 functions
//internal prints to narrow down error searching
void testMat4()
{
    //store the temporary error value
    bool tmpError = false;
    //print a seperator line
    std::cout << "---------------------\n";

    //store a constant identity matrix to use later as utility
    const float mat_identity[16] = {
        1.f,0.f,0.f,0.f,
        0.f,1.f,0.f,0.f,
        0.f,0.f,1.f,0.f,
        0.f,0.f,0.f,1.f,
    };

    //test if the default is an identity matrix
    {
        //expect the default to be an identity matrix
        mat4 identity;
        //compare all values to the identity matrix
        tmpError = util_compareMat4Inv((float*)&identity, (float*)mat_identity);
    }
    //check if this is the error and print a message accordingly
    std::cout << "Sub-Test of Mat4x4 test: Test if default is the identity matrix. Result: " << (tmpError ? "\x1B[0;91mFailure\x1B[0m" : "\x1B[0;92mSuccess\x1B[0m") << "\n";
    //update the error flag and reset the temporary error
    error |= tmpError;
    tmpError = false;

    //test the matrix from value constructor
    {
        //create the matrix
        mat4 matrix(
            0, 1, 2, 3,
            4, 5, 6, 7,
            8, 9, 10,11,
            12,13,14,15
        );
        //store the expected values
        const float expected[] = {
            0, 1, 2, 3,
            4, 5, 6, 7,
            8, 9, 10,11,
            12,13,14,15
        };
        //check if the matrix values are correct
        tmpError = util_compareMat4Inv((float*)&matrix, expected);
    }
    //check if this is the error and print a message accordingly
    std::cout << "Sub-Test of Mat4x4 test: Test if the constructor from 16 values works. Result: " << (tmpError ? "\x1B[0;91mFailure\x1B[0m" : "\x1B[0;92mSuccess\x1B[0m") << "\n";
    //update the error flag and reset the temporary error
    error |= tmpError;
    tmpError = false;

    //test the matrix from float* constructor
    {
        //store the float list and expected result
        const float expected[] = {
            15,15,13,12,
            11,10, 9, 8,
             7, 6, 5, 4,
             3, 2, 1, 0
        };
        //create the matrix
        mat4 matrix((float*)expected);
        //check if the matrix values are correct
        tmpError = util_compareMat4Inv((float*)&matrix, expected);
    }
    //check if this is the error and print a message accordingly
    std::cout << "Sub-Test of Mat4x4 test: Test if the constructor from a float* works. Result: " << (tmpError ? "\x1B[0;91mFailure\x1B[0m" : "\x1B[0;92mSuccess\x1B[0m") << "\n";
    //update the error flag and reset the temporary error
    error |= tmpError;
    tmpError = false;

    //test the + operator for matricies
    {
        //store the expected result
        const float expected[] = {
            2,0,0,0,
            0,2,0,0,
            0,0,2,0,
            0,0,0,2
        };
        //create the first matrix
        mat4 matrix;
        //calculate the sum
        mat4 sum = matrix + matrix;
        //check if the matrix values are correct
        tmpError = util_compareMat4Inv((float*)&sum, expected);
    }
    //check if this is the error and print a message accordingly
    std::cout << "Sub-Test of Mat4x4 test: Test if the addition of two matricies works. Result: " << (tmpError ? "\x1B[0;91mFailure\x1B[0m" : "\x1B[0;92mSuccess\x1B[0m") << "\n";
    //update the error flag and reset the temporary error
    error |= tmpError;
    tmpError = false;

    //test the + operator for a matrix and a scalar
    {
        //store the expected result
        const float expected[] = {
            2,1,1,1,
            1,2,1,1,
            1,1,2,1,
            1,1,1,2
        };
        //create the matrix
        mat4 matrix;
        //calculate the sum
        mat4 sum = matrix + 1;
        //check if the matrix values are correct
        tmpError = util_compareMat4Inv((float*)&sum, expected);
    }
    //check if this is the error and print a message accordingly
    std::cout << "Sub-Test of Mat4x4 test: Test if the addition of a matrix and a scalar. Result: " << (tmpError ? "\x1B[0;91mFailure\x1B[0m" : "\x1B[0;92mSuccess\x1B[0m") << "\n";
    //update the error flag and reset the temporary error
    error |= tmpError;
    tmpError = false;

    //test the - operator for matricies
    {
        //store the expected result
        const float expected[] = {
            0,0,0,0,
            0,0,0,0,
            0,0,0,0,
            0,0,0,0
        };
        //create the first matrix
        mat4 matrix;
        //calculate the difference
        mat4 diff = matrix - matrix;
        //check if the matrix values are correct
        tmpError = util_compareMat4Inv((float*)&diff, expected);
    }
    //check if this is the error and print a message accordingly
    std::cout << "Sub-Test of Mat4x4 test: Test if the subtraction of two matricies works. Result: " << (tmpError ? "\x1B[0;91mFailure\x1B[0m" : "\x1B[0;92mSuccess\x1B[0m") << "\n";
    //update the error flag and reset the temporary error
    error |= tmpError;
    tmpError = false;

    //test the - operator for a matrix and a scalar
    {
        //store the expected result
        const float expected[] = {
             0,-1,-1,-1,
            -1, 0,-1,-1,
            -1,-1, 0,-1,
            -1,-1,-1, 0
        };
        //create the matrix
        mat4 matrix;
        //calculate the difference
        mat4 diff = matrix - 1;
        //check if the matrix values are correct
        tmpError = util_compareMat4Inv((float*)&diff, expected);
    }
    //check if this is the error and print a message accordingly
    std::cout << "Sub-Test of Mat4x4 test: Test if the subtraciton of a matrix and a scalar. Result: " << (tmpError ? "\x1B[0;91mFailure\x1B[0m" : "\x1B[0;92mSuccess\x1B[0m") << "\n";
    //update the error flag and reset the temporary error
    error |= tmpError;
    tmpError = false;

    //test the * operator for matricies
    {
        //store the expected result
        const float expected[] = {
            100, 110, 120, 130, 
            228, 254, 280, 306, 
            356, 398, 440, 482, 
            484, 542, 600, 658
        };
        //create the first matrix
        mat4 matrix(
            1, 2, 3, 4,
            5, 6, 7, 8,
            9,10,11,12,
            13,14,15,16
        );
        mat4 other(
            2, 3, 4, 5,
            6, 7, 8, 9,
            10,11,12,13,
            14,15,16,17
        );
        //calculate the product
        mat4 prod = matrix * other;
        //check if the matrix values are correct
        tmpError = util_compareMat4Inv((float*)&prod, expected);
    }
    //check if this is the error and print a message accordingly
    std::cout << "Sub-Test of Mat4x4 test: Test if the multiplication of two matricies works. Result: " << (tmpError ? "\x1B[0;91mFailure\x1B[0m" : "\x1B[0;92mSuccess\x1B[0m") << "\n";
    //update the error flag and reset the temporary error
    error |= tmpError;
    tmpError = false;

    //test the * operator for a matrix and a scalar
    {
        //store the expected result
        const float expected[] = {
            5,0,0,0,
            0,5,0,0,
            0,0,5,0,
            0,0,0,5
        };
        //create the matrix
        mat4 matrix;
        //scale the matrix with the scalar
        mat4 diff = matrix * 5;
        //check if the matrix values are correct
        tmpError = util_compareMat4Inv((float*)&diff, expected);
    }
    //check if this is the error and print a message accordingly
    std::cout << "Sub-Test of Mat4x4 test: Test if the scalig of a matrix by an scalar. Result: " << (tmpError ? "\x1B[0;91mFailure\x1B[0m" : "\x1B[0;92mSuccess\x1B[0m") << "\n";
    //update the error flag and reset the temporary error
    error |= tmpError;
    tmpError = false;

    //test if operator== works
    {
        //create the matrix
        mat4 matrix(1,2,3,4,5,6,7,8,9,10,11,12,13,14,15,16);
        //create the matrix to compare with
        mat4 other(1,2,3,4,5,6,7,8,9,10,11,12,13,14,15,16);
        //check if the matrix values are equal (expected result)
        tmpError = !(matrix == other);
    }
    //check if this is the error and print a message accordingly
    std::cout << "Sub-Test of Mat4x4 test: Test if the scalig of a matrix by an scalar. Result: " << (tmpError ? "\x1B[0;91mFailure\x1B[0m" : "\x1B[0;92mSuccess\x1B[0m") << "\n";
    //update the error flag and reset the temporary error
    error |= tmpError;
    tmpError = false;

    //print a seperator line to end the block
    std::cout << "---------------------\n";
}
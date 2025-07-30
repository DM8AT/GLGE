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

//test all the mat4 functions
//internal prints to narrow down error searching
void testMat4()
{
    //store the temporary error value
    bool tmpError = false;
    //print a seperator line
    std::cout << "---------------------\n";
    //test if the default is an identity matrix
    {
        //expect the default to be an identity matrix
        mat4 identity;
        //convert the matrix to a float[4][4] to be sure that no errors with the operator[] function exist
        float* ident = (float*)(&identity);
        //compare all values to the identity matrix
        tmpError = !(
            ident[0] == 1 && ident[1] == 0 && ident[2] == 0 && ident[3] == 0 &&
            ident[4] == 0 && ident[5] == 1 && ident[6] == 0 && ident[7] == 0 &&
            ident[8] == 0 && ident[9] == 0 && ident[10]== 1 && ident[11]== 0 &&
            ident[12]== 0 && ident[13]== 0 && ident[14]== 0 && ident[15]== 1
        );
    }
    //check if this is the error and print a message accordingly
    std::cout << "Sub-Test of Mat4x4 test: Test if default is the identity matrix. Result: " << (tmpError ? "\x1B[0;91mFailure\x1B[0m" : "\x1B[0;92mSuccess\x1B[0m") << "\n";
    //update the error flag and reset the temporary error
    error |= tmpError;
    tmpError = false;

    //print a seperator line to end the block
    std::cout << "---------------------\n";
}
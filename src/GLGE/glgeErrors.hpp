/**
 * @file glgeErrors.h
 * @author DM8AT
 * @brief this file stores all the defines for errors outputed by GLGE
 * 
 * only used behind the sceens
 * 
 * @date 2022-12-17
 * 
 * @copyright Copyright DM8AT 2023. All rights reserved. This project is released under the MIT license. 
 * 
 */

//check for double include
#ifndef _GLGE_ERRORS_H_
#define _GLGE_ERRORS_H_

////////////////////////////////////////////////////////
//Defines for importand errors (allways crash & print)//
////////////////////////////////////////////////////////

//these errors will always be printed, even if normal errors aren't. If one of them occures, the program is very broken. 

/**
 * this error occures if the frame buffer could not be created. 
 */
#define GLGE_FATAL_ERROR_FRAMEBUFFER_NOT_COMPILED "[GLGE FATAL ERROR] could not compile the Framebuffer. Error code by OpenGL: "

////////////////////////
//Error string defines//
////////////////////////

/**
 * this error occures if an window was allready created but a second one should be created. 
 * GLGE supports only a single window. 
*/
#define GLGE_ERROR_TWO_WINDOWS "[GLGE ERROR] GLGE allready has an window, two windows are not supported\n"

/**
 * this error is outputed if the initalisation of glew failes. 
 */
#define GLGE_ERROR_GLEW_INIT_FAILED "[GLGE ERROR] could not initalise glew, glew error string: %s\n"

/**
 * this error is printed if argc is an value that defenetly dosn't come from the main function input
 */
#define GLGE_ERROR_ARGC_OUT_OF_BOUNDS "[GLGE ERROR] Invalid number for argc in init function: %d\n"

/**
 * this error is called if the input to the init function into argv is a nullpointer
 */
#define GLGE_ERROR_NULLPOINTER_INP_INIT "[GLGE ERROR] nullpointer as input for init function\n"

/**
 * this error is printed if the size of an window is negative
 */
#define GLGE_ERROR_WINDOW_SIZE_NEGATIVE "[GLGE ERROR] invalid parameters for input to window cration function, a negative size is not allowed for a window\n"

/**
 * this error is printed if the size of an window is 0
 */
#define GLGE_ERROR_WINDOW_SIZE_ZERO "[GLGE ERROR] invalid parameters for input to window cration function, a size of 0 is not allowed for a window\n"

/**
 * this error is outputed if the name for a window should be the nullpointer
 */
#define GLGE_ERROR_WINDOW_NAME_NULLPOINTER "[GLGE ERROR] nullpointer as name for window is invalide\n"

/**
 * this error is outputed if the position of an window is negative
 */
#define GLGE_ERROR_WINDOW_NEGATIVE_POSITION "[GLGE ERROR] an negative window position dosn't work\n"

/**
 * this error is outputed if the values inputed as color are negative
 */
#define GLGE_ERROR_NEGAITVE_COLOR_VAL "[GLGE ERROR] negative color values are not supported\n"

/**
 * this error is called if the values for a color are too large
 */
#define GLGE_ERROR_COLOR_VALUE_OUT_OF_BOUNDS "[GLGE ERROR] color value is to large, value is not supported\n"

/**
 * this error occures if an inputed function is the nullpointer
 */
#define GLGE_ERROR_FUNC_IS_NULLPOINTER "[GLGE ERROR] inputed function is the nullpointer\n"

/**
 * this error is printed if the maximal frames per second should be set to something less or equal to zero
 */
#define GLGE_ERROR_FPS_LESS_ONE "[GLGE ERROR] the inputed maximal frames per second are less or equal to 0\n"

/**
 * this error occures if a file cant be found
 */
#define GLGE_ERROR_FILE_NOT_FOUND "[GLGE ERROR] could not find the requested file: '%s'\n"

/**
 * this error message says that the inputed argument is not a valide argument for the function
 */
#define GLGE_ERROR_INVALIDE_INPUT "[GLGE ERROR] the inputed argument is not valide, read the description for the function for information\n"

/**
 * this error is outputed if GLGE failes to create a shader
 */
#define GLGE_ERROR_COULD_NOT_CREATE_SHADER "[GLGE ERROR] could not create shader of type " //followed by shader type

/**
 * this error occures if the shader source code can't be compiled
 */
#define GLGE_ERROR_SHADER_COMPILE_ERROR "[GLGE ERROR] could not compile shader of type %d because an unexpected error occured. Error generated by OpenGL: \n%s\n"

/**
 * this error is printed if GLGE failes to get the uniform variable out of an shader
 */
#define GLGE_ERROR_UNIFORM_VAR_NOT_FOUND "[GLGE ERROR] could not get location of uniform variable %s\n"

/**
 * this error occures if a shader program can't be validated
 */
#define GLGE_ERROR_SHADER_VALIDATE_ERROR "[GLGE ERROR] could not validate shader program, OpenGL error: %s\n"

/**
 * this error is printed if the inputed value is 0 or less, but shader id values are only one or above
 */
#define GLGE_ERROR_INVALIDE_SHADER_INPUT "[GLGE ERROR] the inputed value is not an valide value for an shader\n"

/**
 * this error is outputed if an OpenGL error occures.
 */
#define GLGE_ERROR_OPEN_GL_ERROR "[GLGE ERROR] an OpenGL error occured. Error created by OpenGL: \n%s\n"

/**
 * this error is outputed if an pointer inputed into an function is the nullpointer
 */
#define GLGE_ERROR_INPUT_NULLPOINTER "[GLGE ERROR] an nullpointer is not an valide function input\n"

/**
 * this error is outouted if the field of view is less or equal to zero
 */
#define GLGE_ERROR_CAMERA_FOV_LESS_EQUAL_ZERO "[GLGE ERROR] the field of view can't be zero or less. Inputed Value: %f\n"

/**
 * this error occures if the near cliping plane is less or equal to zero
 */
#define GLGE_ERROR_CAMERA_NEAR_LESS_EQUAL_ZERO "[GLGE ERROR] the near cliping plane can't be less or equal to zero. Inputed Value: %f\n"

/**
 * this error is printed if the far cliping plane is less or equal to zero
 */
#define GLGE_ERROR_CAMERA_FAR_LESS_EQUAL_ZERO "[GLGE ERROR] the far cliping plane can't be less or equal to zero. Inputed Value: %f\n"

/**
 * this error occures if the far cliping plane is less or equal to the near cliping plane
 */
#define GLGE_ERROR_CAMERA_FAR_LESS_EQUAL_NEAR "[GLGE ERROR] the far cliping plane can't be less or equal to the near cliping plane. \n"

//////////////////////
//Error string exits//
//////////////////////

/**
 * this messages is the message at wich point the window sub function "createWindow" stops the C++ program. 
 * HINT:
 * are two windows being created?
 */
#define GLGE_ERROR_STR_WINDOW_SUB_FUNC "[GLGE ERROR RETURN]: Error in glgeCreateWindow sub function 'createWindow', please check for repeat of window cration"

/**
 * this message is send if the program is being stopped because the initalisation of GLEW failed. 
 * HINT:
 * is glew correctly installed?
 */
#define GLGE_ERROR_STR_GLEW_INIT "[GLGE ERROR RETURN]: Error in glgeCreateWindow sub function 'createWindow', is glew installed on the system?"

/**
 * this error is called if the program stops because of an error in the init function
 */
#define GLGE_ERROR_STR_GLGE_INIT_ERROR "[GLGE ERROR RETURN]: Error in glgeInit"

/**
 * this error is printed if the program stops because of invalide inputs for a window creation
 */
#define GLGE_ERROR_STR_WINDOW_INPUT_ERROR "[GLGE ERROR RETURN]: error in glgeCreateWindow | invalide input parameters"

/**
 * this message is printed if an error occured in the function "glgeSetClearColor" that causes the program to fail
 */
#define GLGE_ERROR_STR_SET_CLEAR_COLOR "[GLGE ERROR RETURN]: an error occured in function 'glgeSetClearColor'"

/**
 * this message is outputed if an error occured in the function "glgeBindDisplayFunc" that caused the program to fail
 */
#define GLGE_ERROR_STR_BIND_DISPLAY_CALLBACK "[GLGE ERROR RETURN]: an error occured in function 'glgeBindDisplayFunc'"

/**
 * this message is outputed if an error occured in the function "glgeBindMainFunc" that caused the program to fail
 */
#define GLGE_ERROR_STR_BIND_MAIN_CALLBACK "[GLGE ERROR RETURN]: an error occured in the function 'glgeBindMainFunc'"

/**
 * this message is send if an error in the function "glgeSetMaxFPS" caused the program to crash
 */
#define GLGE_ERROR_STR_SET_MAX_FPS "[GLGE ERROR RETURN]: an error occured in function 'glgeSetMaxFPS'"

/**
 * this message says that the error printed above occured in an constructor for an Object
 */
#define GLGE_ERROR_STR_OBJECT_OBJECT "[GLGE ERROR RETURN]: an error occured in the constructor for an object, function 'Object::Object'"

/**
 * this message is outputed if an error occures in the add shader sub function
 */
#define GLGE_ERROR_STR_OBJECT_ADD_SHADER "[GLGE ERROR RETURN]: shader compiling did return an error, function 'Object::addShader', sub function of function 'Object::compileShader'"

/**
 * this message is printed if an error occures in the function for getting uniform variables out of shader scripts
 */
#define GLGE_ERROR_STR_OBJECT_GET_UNIFORM_VARIABLE "[GLGE ERROR RETURN]: error occured in function Object::getUniformVar"

/**
 * this message is outputed if an error occures in the function for compiling shaders in the object
 */
#define GLGE_ERROR_STR_OBJECT_COMPILE_SHADERS "[GLGE ERROR RETURN]: error occured in function 'Object::compileShader'"

/**
 * this message is printed if an error occures in the read file function
 */
#define GLGE_ERROR_STR_READ_FILE "[GLGE ERROR RETURN]: error occured in the read file function, function name: 'readFile'"

/**
 * this message is outputed if an error occures while glge is trying to set the name for the move matrix in the shaders
 */
#define GLGE_ERROR_STR_GLGE_SET_MOV_MAT_NAME "[GLGE ERROR RETURN]: error occured in function 'glgeSetMoveMatrixName'"

/**
 * this message is printed if an error occures in the function to bind a shader
 */
#define GLGE_ERROR_STR_OBJECT_BIND_SHADER "[GLGE ERROR RETURN]: error occured in function 'Object::bindShader'"

/**
 * this message is printed if an error occures in the function to bind a main camera
 */
#define GLGE_ERROR_STR_BIND_MAIN_CAMERA "[GLGE ERROR RETURN]: error occured in function 'glgeBindCamera'"

/**
 * this message is printed if an error occures in the Camera constructor
 */
#define GLGE_ERROR_STR_CAMERA_CAMERA "[GLGE ERROR RETURN]: error occured in function 'Camera::Camera'"

////////////////////////
//GLGE Warning Strings//
////////////////////////

/**
 * this warning occures if a not esential part of GLGE gets a nullpointer insted of an valide pointer
 */
#define GLGE_WARNING_POINTER_IS_NULLPOINTER "[GLGE WARNING] the inputed pointer is the nullpointer\n"

////////////////////////////////
//GLGE Warning Position String//
////////////////////////////////

/**
 * this message says that the warnings above this text occured in a struct for an Mesh
 */
#define GLGE_WARNING_POS_MESH_MESH "[GLGE WARNING IN] warning occured in function 'Mesh::Mesh'"

#endif
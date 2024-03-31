/**
 * @file openglGLGEComputeShader.cpp
 * @author DM8AT
 * @brief 
 * @version 0.1
 * @date 2024-02-27
 * 
 * @copyright Copyright DM8AT 2024. All rights reserved. This project is released under the MIT license. 
 * 
 */
//include the header file
#include "openglGLGEComputeShader.hpp"
//include the private functions
#include "../GLGEIndependend/glgeInternalFuncs.h"
//include the private vars to get OpenGL acess
#include "openglGLGEVars.hpp"
//include the errors
#include "../GLGEIndependend/glgeErrors.hpp"

//default includes
#include <iostream>

ComputeShader::ComputeShader()
{}

ComputeShader::ComputeShader(const char* shaderFile)
{
    //store the contend of the file
    std::string str;
    //read the shader file
    readFile(shaderFile, str);
    //cast to another constructor
    *this = ComputeShader(str);
}

ComputeShader::ComputeShader(std::string shaderCode)
{
    //store the source code
    this->src = shaderCode;
    //create a new shader program
    unsigned int shaderProgram = glCreateProgram();

    //check if the shader could be created
    if (shaderProgram == 0)
    {
        //output an error message
        if (glgeErrorOutput)
        {
            printf(GLGE_ERROR_COULD_NOT_CREATE_SHADER);
            //say where the error occured
            std::cerr << GLGE_ERROR_STR_OBJECT_COMPILE_SHADERS << "\n";
        }
        //stop the program
        if (glgeExitOnError)
        {
            //only exit the program if glge is tolled to exit on an error
            exit(1);
        };
    }

    //create a new shader with the inputed type
    unsigned int shaderObj = glCreateShader(GL_COMPUTE_SHADER);

    //check if the shader object could be created
    if (shaderObj == 0)
    {
        //output an error message
        if (glgeErrorOutput)
        {
            std::cout << GLGE_ERROR_COULD_NOT_CREATE_SHADER << GL_COMPUTE_SHADER << "\n";
            //print where the error occured
            std::cerr << GLGE_ERROR_STR_OBJECT_ADD_SHADER << "\n";
        }
        //stop the script
        if (glgeExitOnError)
        {
            //only exit the program if glge is tolled to exit on an error
            exit(1);
        };
    }

    //set a GLchar to the inputed text
    const GLchar* p[1];
    p[0] = shaderCode.c_str();

    //store the length of the text
    int lengths[1];
    lengths[0] = strlen(p[0]);

    //set the shader source code
    glShaderSource(shaderObj, 1, p, lengths);

    //compile to the new shader object
    glCompileShader(shaderObj);

    //check for compiling errors
    int success;
    glGetShaderiv(shaderObj, GL_COMPILE_STATUS, &success);

    //if there was an error, print a message and exit
    if (!success)
    {
        //output an error message
        if (glgeErrorOutput)
        {
            //create an info log to store the error created by open gl
            GLchar InfoLog[1024];
            glGetShaderInfoLog(shaderObj, 1024, NULL, InfoLog);
            //print the message
            printf(GLGE_ERROR_SHADER_COMPILE_ERROR, GL_COMPUTE_SHADER, InfoLog);
            //print where the error occured
            std::cerr << GLGE_ERROR_STR_OBJECT_ADD_SHADER << "\n";
        }
        //stop the script
        if (glgeExitOnError)
        {
            //only exit the program if glge is tolled to exit on an error
            exit(1);
        };
    }

    //attach the shader object to the final program
    glAttachShader(shaderProgram, shaderObj);

    //create an variable to check for success
    success = 0;
    //setup an error log
    GLchar ErrorLog[1024] = {0};

    //link the shader program
    glLinkProgram(shaderProgram);

    //get the program iv from the shader
    glGetProgramiv(shaderProgram, GL_LINK_STATUS, &success);
    //check if the program linking was no success
    if (success == 0)
    {
        //output an error message
        if (glgeErrorOutput)
        {
            //get the error from open gl and output it with an custom message
            glGetProgramInfoLog(shaderProgram, sizeof(ErrorLog), NULL, ErrorLog);
            printf(GLGE_ERROR_SHADER_VALIDATE_ERROR, ErrorLog);
        }
        //stop the program
        if (glgeExitOnError)
        {
            //only exit the program if glge is tolled to exit on an error
            exit(1);
        };
    }

    //check if the program is valide
    glValidateProgram(shaderProgram);
    //get the program iv again
    glGetProgramiv(shaderProgram, GL_VALIDATE_STATUS, &success);
    //check for success
    if (!success)
    {
        //output an error message
        if (glgeErrorOutput)
        {
            //get the error from open gl and output it with an custom message
            glGetProgramInfoLog(shaderProgram, sizeof(ErrorLog), NULL, ErrorLog);
            printf(GLGE_ERROR_SHADER_VALIDATE_ERROR, ErrorLog);
        }
        //stop the program
        if (glgeExitOnError)
        {
            //only exit the program if glge is tolled to exit on an error
            exit(1);
        };
    }

    //store the shader program
    this->shader = shaderProgram;
}

void ComputeShader::dispatch(vec3 workgroups)
{
    //bind the shader program
    this->applyShader();
    //dispatch the compute shader
    glDispatchCompute((unsigned int)workgroups.x, (unsigned int)workgroups.y, (unsigned int)workgroups.z);
    // make sure execution has finished before continuing
    glMemoryBarrier(GL_SHADER_IMAGE_ACCESS_BARRIER_BIT);
    //unbind the shader
    this->removeShader();
}

void ComputeShader::encode(Data* data)
{
    //encode the source code
    data->writeString(this->src);

    //encode the uniforms
    this->encodeUniforms(data);
}

void ComputeShader::decode(Data* data)
{
    //deocde the source
    std::string src = data->readString();
    //delete the shader
    this->deleteShader();
    //construct a new shader from the source
    *this = ComputeShader(src);

    //decode the uniforms
    this->decodeUniforms(data);
}

std::string ComputeShader::getSRC()
{
    //return the shader code
    return this->src;
}
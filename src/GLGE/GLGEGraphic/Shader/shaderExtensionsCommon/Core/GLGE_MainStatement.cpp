/**
 * @file GLGE_MainStatement.cpp
 * @author DM8AT
 * @brief implement the main statement handle
 * @version 0.1
 * @date 2025-05-22
 * 
 * @copyright Copyright (c) 2025
 * 
 */

//include the shader extensions
#include "GLGE_MainStatement.h"
//include shaders
#include "../../GLGEShader.h"
//include processors
#include "../../GLGEShaderProcessor.h"

bool ShaderExt_Main::onFind(std::string& source, ShaderProcessor*, Shader* shader, uint64_t elementIdx) noexcept
{
    //store the position and skip the @main statement
    uint64_t p = elementIdx + sizeof("@main")-1;
    //search the next element that is nether a space nor a (
    while ((source[p] == ' ') || (source[p] == '('))
    {
        //increase the position
        ++p;
        //return an error if the index is out of range
        if (p >= source.length()) {return false;}
    }
    //store the start of the string
    uint64_t start = p;
    //search the next element that is ether a space or a )
    while ((source[p] != ' ') && (source[p] != ')'))
    {
        //increase the position
        ++p;
        //return an error if the index is out of range
        if (p >= source.length()) {return false;}
    }
    //extract the substring
    std::string substr = source.substr(start, p-start);

    //search the end of the line
    while (source[p] != '\n')
    {
        //increase the position
        ++p;
        //return an error if the index is out of range
        if (p >= source.length()) {return false;}
    }
    //store the line end position
    uint64_t lineEnd = p;

    //search the next function by searching the next curly brackets or semicolon
    while ((source[p] != '{') && (source[p] != ';'))
    {
        //increase the position
        ++p;
        //return an error if the index is out of range
        if (p >= source.length()) {return false;}
    }
    //track back to the last open bracket
    while (source[p] != '(') {--p;}
    //track also back till the last element is not a space
    do {--p;} while (source[p] == ' ');
    //store the function name end
    uint64_t funcNameEnd = p;
    //track back to the last space
    while (source[p] != ' ') {--p;}

    //extract the function name
    std::string fName = source.substr(p+1, funcNameEnd-p);

    //add the element as an entry point

    //store the shader type
    ShaderType type = (ShaderType)UINT8_MAX;
    //check if the type is a vertex shader
    if (substr == "stage_vertex")
    {
        //store the correct type
        type = SHADER_TYPE_VERTEX;
    }
    //else, check if the type is a fragment shader
    else if (substr == "stage_fragment")
    {
        //store the correct type
        type = SHADER_TYPE_FRAGMENT;
    }
    //else, check if the type is a geometry shader
    else if (substr == "stage_geometry")
    {
        //store the correct type
        type = SHADER_TYPE_GEOMETRY;
    }
    //else, check if the type is a tesselation controll shader
    else if (substr == "stage_tess_controll")
    {
        //store the correct type
        type = SHADER_TYPE_TESSELATION_CONTROLL;
    }
    //else, check if the type is a tesselation evaluation shader
    else if (substr == "stage_tess_evaluate")
    {
        //store the correct type
        type = SHADER_TYPE_TESSELATION_EVALUATION;
    }
    //else, check if the type is a compute shader
    else if (substr == "stage_compute")
    {
        //store the correct type
        type = SHADER_TYPE_COMPUTE;
    }
    
    //check for undefined
    if (type == (ShaderType)UINT8_MAX)
    {
        //print an error
        shader->getInstance()->log(std::string("Failed to identify shader type ") + substr.data(), MESSAGE_TYPE_ERROR);
        return false;
    }

    //store the entry point
    shader->setEntryPoint(type, fName.data());

    //erase the whole line
    source.erase(elementIdx, lineEnd - elementIdx);

    //return the success
    return true;
}

/**
 * @brief store the identifyer for the main directive
 */
const char* ShaderExt_Main::identifyer = "@main";

/**
 * @file GLGE_VersionStatement.cpp
 * @author DM8AT
 * @brief define how the version statement for GLGE works
 * @version 0.1
 * @date 2025-05-28
 * 
 * @copyright Copyright (c) 2025
 * 
 */

//include the version statement
#include "GLGE_VersionStatement.h"
//include shader
#include "../../GLGEShader.h"

bool ShaderExt_Version::onFind(std::string& source, ShaderProcessor* processor, Shader* shader, uint64_t elementIdx) noexcept
{
    //search the end of the line
    uint64_t lineEnd = source.find('\n', elementIdx);
    //error if the line end was not found
    if (lineEnd == std::string::npos) {return false;}

    //calcualte where the version identifyer starts
    uint64_t vStart = elementIdx + strlen(identifyer);
    vStart = source.find(' ', vStart);
    vStart = source.find_first_not_of(' ', vStart);
    //error if the position is not valid
    if (vStart == std::string::npos) {return false;}
    //extract the substring containing the version
    std::string vers = source.substr(vStart, lineEnd - vStart);
    //store the version numbers
    uint32_t maj = 0, min = 0, patch = 0;
    /**
     * get the version numbers
     */
    //store the start and end of the current number
    uint64_t start = 0, end = 0;
    //extract the string for the major version
    start = vStart;
    end = source.find('.', start);
    std::string majStr = source.substr(start, end - start);
    //extract the string for the minor version
    start = end+1;
    end = source.find('.', start);
    std::string minStr = source.substr(start, end - start);
    //extract the string for the major version
    start = end+1;
    end = lineEnd;
    std::string patchStr = source.substr(start, end - start);

    //read the integers from all the strings
    maj = std::stoi(majStr);
    min = std::stoi(minStr);
    patch = std::stoi(patchStr);

    //calculate the GLGE version
    uint64_t glgeVers = GLGE_TO_VERSION(maj, min, patch);

    //check if the version is newer
    if (glgeVers > GLGE_VERSION)
    {
        //print a warning
        shader->getInstance()->log("A shader that uses GLGE versions is in a newer version than the version you're compiling with. It MIGHT use features not implemented in this version.", MESSAGE_TYPE_ERROR);
    }
    
    //remove the GLGE version
    source.erase(elementIdx + 9, (lineEnd - (elementIdx + 9)));
    source.insert(elementIdx + 9, "450 core");

    std::cout << source << "\n";

    //return the success
    return true;
}

/**
 * @brief store the identifyer for the version statement
 */
const char* ShaderExt_Version::identifyer = "#version GLGE";
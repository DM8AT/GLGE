/**
 * @file GLGE_IncludeStatement.cpp
 * @author DM8AT
 * @brief define how the #incldue statement for the GLGE GLSL wrapper works
 * @version 0.1
 * @date 2025-05-23
 * 
 * @copyright Copyright (c) 2025
 * 
 */

//include the include statement extension
#include "GLGE_IncludeStatement.h"
//include shaders
#include "../../GLGEShader.h"
//include the processor
#include "../../GLGEShaderProcessor.h"
//include file management stuff
#include "../../../../GLGECore/Files/GLGEFile.h"

//include the main directive
#include "../Core/GLGE_CoreExt.h"

bool ShaderExt_Include::onFind(std::string& source, ShaderProcessor* processor, Shader* shader, uint64_t elementIdx) noexcept
{
    //store the current position
    uint64_t p = elementIdx;
    //search the next function by searching the next curly brackets or semicolon
    while ((source[p] != '\n') && (p != source.length()))
    {
        //increase the position
        ++p;
    }
    
    //search back to a " or >
    while ((source[p] != '>') && (source[p] != '"')) {--p;}
    //store the position
    uint64_t end = p;
    //search back to another " or <
    do {--p;} while ((source[p] != '<') && (source[p] != '"'));
    //store the start position
    uint64_t start = p;

    //check if the enclosement signes don't match up
    if (!(((source[start] == '"') && (source[end] == '"')) || ((source[start] == '<') && (source[end] == '>'))))
    {
        //error! Opening and closing don't match up
        shader->getInstance()->log("Failed to process include statement - Opening and closing type don't match up", MESSAGE_TYPE_ERROR);
        return false;
    }

    //store if a package should be included
    bool package = (source[start] == '<');

    //extract the name of the element to include
    std::string_view toInclude = source.substr(start+1, end-start - 1);

    //erase the line
    source.erase(elementIdx, (end-elementIdx) + 1);

    //check if this is a package
    if (package)
    {
        //get the iterator
        auto it = processor->getPackages().find(toInclude);
        //check if the package is found
        if (it == processor->getPackages().end())
        {
            //error! Failed to find the requested package in this processor
            shader->getInstance()->log(std::string("Failed to process include statement - Failed to find the requested package named '") + toInclude.data() + "'", MESSAGE_TYPE_ERROR);
            return false;
        }
        //insert the content of the package at this position
        source.insert(elementIdx, it->second);
    }
    else
    {
        //else, this is a relative path to a file. First, calculate the absolute file path to the to including file
        Path shaderPath = Path(shader->getFile(), PATH_LAYOUT_NATIVE);
        std::string sPathStr = shaderPath.getPath(PATH_LAYOUT_UNIX);
        //remove the last element from that string. It is an absolute file path, so there's guaranteed to be at least one /
        uint64_t last = sPathStr.find_last_of('/');
        //get everything before the slash
        std::string absPath = sPathStr.substr(0, last+1);
        
        //create the path to the file to include. File pathes in the include statement must be in unix layout
        Path path = Path(absPath + toInclude.data(), PATH_LAYOUT_UNIX);

        //check if the file exists
        if (!std::filesystem::exists(path.getPath(PATH_LAYOUT_NATIVE)))
        {
            //error! failed to find the file to include
            shader->getInstance()->log(std::string("Failed to process include statement - Failed to locate file to include '") + path.getPath(PATH_LAYOUT_NATIVE) + "'", MESSAGE_TYPE_ERROR);
            return false;
        }

        //store the content of the file
        std::string incSrc = "";
        
        //load the file. The file will close at the end of the scope. 
        {
            //load the content of the file and remove all comments
            File f(path, false);
            //load the file's contents
            incSrc = f.content().data;
        }
        //remove all comments
        ShaderProcessor::removeComments(incSrc);

        /**
         * REMOVE THE VERSION DIRECTOVE 
         */
        //store the location of the version directive (GLSL keyword)
        uint64_t vLoc = incSrc.find("#version ");
        //check if a version directive exists
        if (vLoc != std::string::npos)
        {
            //store the end of the line
            uint64_t endP = vLoc;
            //search the next end of line
            endP = incSrc.find('\n', endP);
            //remove everything between the version directive and the end
            incSrc.erase(vLoc, (endP - vLoc) + 1);
        }

        //store the position of a version directive
        uint64_t mainPos = incSrc.find(ShaderExt_Main::identifyer);
        //store the line end after the main directive
        uint64_t mainLEnd = 0;
        //loop while there are main directives
        while (mainPos != std::string::npos)
        {
            //store the line end 
            mainLEnd = incSrc.find('\n', mainPos);
            //erase everything between the two positions
            incSrc.erase(mainPos, (mainLEnd - mainPos) + 1);
        }

        //insert the file contents at the include statements
        source.insert(elementIdx, incSrc);
    }

    //return that the exit was successfull
    return true;
}

/**
 * @brief store the identifyer for the include directive
 */
const char* ShaderExt_Include::identifyer = "#include";
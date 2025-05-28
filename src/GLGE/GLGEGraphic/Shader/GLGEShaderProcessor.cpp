/**
 * @file GLGEShaderProcessor.cpp
 * @author DM8AT
 * @brief implement the shader processor
 * @version 0.1
 * @date 2025-05-13
 * 
 * @copyright Copyright (c) 2025
 * 
 */

//include the shader processor
#include "GLGEShaderProcessor.h"
//include files
#include "../../GLGECore/Files/GLGEFile.h"

//include the core extensions
#include "shaderExtensionsCommon/Core/GLGE_CoreExt.h"
//include the util extensions
#include "shaderExtensionsCommon/Util/GLGE_UtilExt.h"

void ShaderProcessor::removeComments(std::string& source)
{
    //store if the current element is commented out
    bool inComment = false;
    //store if this is a multi-line comment
    bool multLineCmt = false;
    //store the start element
    uint64_t start = 0;
    //iterate over all characters
    for (uint64_t i = 0; i < source.length(); ++i)
    {
        //check if the current element is commented out
        if (inComment)
        {
            //check for a comment end
            if (multLineCmt) {
                //check for a star
                if (source[i] == '*') {
                    //check if the end is reached
                    if (i+1 < source.length()) {
                        //check if the element is a slash
                        if (source[i+1] == '/') {
                            //end the comment
                            inComment = false;
                            //erase the content
                            source = source.erase(start, (i-start) + 2);
                            i -= (i-start);
                        }
                    }
                }
            }
            else
            {
                //else, check for a new line character
                if (source[i] == '\n')
                {
                    //end the comment
                    inComment = false;
                    //erase the content
                    source = source.erase(start, (i-start)+1);
                    i -= (i-start);
                }
            }

            //go to the next element
            continue;
        }

        //check for a comment start (both types start with a slash)
        if (source[i] == '/')
        {
            //check if a next character exists
            if (i+1 < source.length())
            {
                //if the next element is a star, it is multi-line. 
                //If the next element is a slash, it is single line. 
                
                //check for a slash
                if (source[i+1] == '/')
                {
                    //store the start of a single line comment
                    inComment = true;
                    multLineCmt = false;
                    start = i;
                    //skip to the next iteration
                    continue;
                }
                //else, check for a star
                else if (source[i+1] == '*')
                {
                    //store the start of a single line comment
                    inComment = true;
                    multLineCmt = true;
                    start = i;
                    //skip to the next iteration
                    continue;
                }
            }
        }
    }

    //store the position before the end position of the file
    uint64_t i = source.length()-1;
    //check if a comment is still active
    if (inComment)
    {
        //erase the content
        source = source.erase(start, (i-start) + 1);
    }
}

ShaderProcessor::ShaderProcessor()
{
    //load all default extensions

    //load the handeling of main
    m_extensions.emplace_back(new ShaderExt_Main());
    //load the handeling of include statements
    m_extensions.emplace_back(new ShaderExt_Include());
    //load the handeling of GLGE versions
    m_extensions.emplace_back(new ShaderExt_Version());

    //store how many own extensions exist
    m_uniqueExtensions = 3;
}

ShaderProcessor::~ShaderProcessor()
{
    //iterate over all unique shader extensions
    for (uint64_t i = 0; i < m_uniqueExtensions; ++i)
    {
        //delete the shader extension
        delete m_extensions[i];
    }
    //clear the extension vector
    m_extensions.clear();
}

bool ShaderProcessor::loadPackage(const Path& path, const std::string_view& name)
{
    //check if the file exists
    if (!File::exists(path.getPath(PATH_LAYOUT_NATIVE)))
    {
        //return an error
        return false;
    }

    //load the file
    File f(path, false);
    //extract the source code
    std::string source(f.content().data);
    //remove the comments
    removeComments(source);
    //add the file content string to the map
    m_packages[name] = std::string(source.data());

    //return that the addition was successfull
    return true;
}

bool ShaderProcessor::processShader(Shader& shader) noexcept
{
    //remove all comments
    //store a reference to the source code
    std::string& source = shader.m_glgeSource;
    //remove the comments
    removeComments(source);

    //store if there could be extensions left to process
    bool run = (m_extensions.size() > 0);
    //loop while extensions are left
    while (run)
    {
        //first, assume that there are no more extensions to process
        bool found = false;
        //iterate over all characters
        for (uint64_t i = 0; i < source.length(); ++i)
        {
            //iterate over all extensions
            for (uint64_t j = 0; j < m_extensions.size(); ++j)
            {
                //store if the names match
                bool match = true;
                //loop over all elements of the name
                for (uint64_t k = 0; k < m_extensions[j]->getIdentifyer().length(); ++k)
                {
                    //check if enough space is free
                    if (i+k < source.length())
                    {
                        //check if the characters don't match
                        if (source[i+k] != m_extensions[j]->getIdentifyer()[k])
                        {
                            //stop, this is not a match
                            match = false;
                            break;
                        }
                    }
                }
                //skip if this is not a match
                if (!match) {continue;}
                //if this is a match, the extension should be called
                if (!m_extensions[j]->onFind(source, this, &shader, i))
                {
                    //if an error occoured, imediatly stop
                    return false;
                }
                //store that a extension was found
                found = true;
                //stop the search for extensions, the execution of this
                //extension may use extensions
                break;
            }
            //stop if a extension was processed
            if (found) {break;}
        }
        //update if the search should be continued
        run = found;
    }
    
    //return the successfull processing
    return true;
}

std::ostream& operator<<(std::ostream& os, const ShaderProcessor& sp) noexcept
{
    //add the header
    os << "shaderProcessor{packages:{\n";
    //iterate over all packages
    for (auto it = sp.m_packages.begin(); it != sp.m_packages.end(); ++it)
    {
        //check if this is not the first package
        if (it != sp.m_packages.begin())
        {
            //print a comma and new line
            os << ",\n";
        }
        //print the package
        os << "    {name: \"" << it->first << "\"}";
    }
    //print the suffix and return
    return (os << "}");
}
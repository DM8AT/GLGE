/**
 * @file GLGEPath.cpp
 * @author DM8AT
 * @brief implement the path management class
 * @version 0.1
 * @date 2025-05-28
 * 
 * @copyright Copyright (c) 2025
 * 
 */

//include pathes
#include "GLGEPath.h"

Path::Path(const std::string& path, PathLayout layout, PathLayout prefearedLayout) noexcept
{
    //return if the layout is undefined
    if (layout == PATH_LAYOUT_UNDEFINED) {return;}

    //store the absolute path
    std::string abs = "";
    //check if the path is absolute
    if ((layout == PATH_LAYOUT_UNIX) && (path[0] == '/'))
    {
        //store this as the absolute path
        abs = path;
    }
    else if ((layout == PATH_LAYOUT_WINDOWS) && (path[1] == ':'))
    {
        //store this as the absolute path
        abs = path;
    }
    //else, the path is relative
    else
    {
        //get the working directory in the requested format
        std::string workDir = getWorkingDirectoryIn(layout);
        //add the two strings together, but seperated with the correct symbol
        abs = workDir + ((layout == PATH_LAYOUT_UNIX) ? '/' : '\\') + path;
    }

    //tokenize the absolute path
    tokenizePath(abs, layout, m_pathTokens);

    //store the format the path will be recreated and bufferd in
    m_layout = prefearedLayout;
    //recreate the path in the new layout
    createPathFromTokens(m_path, m_layout, m_pathTokens);
}

std::string Path::getPath(PathLayout layout) const noexcept
{
    //return the bufferd path if the layout is the same
    if (layout == m_layout) {return m_path;}
    //else, the path needs to be translated. 
    //store the string to return
    std::string path = "";
    //fill the string
    createPathFromTokens(path, layout, m_pathTokens);
    //return the final path
    return path;
}

PathLayout Path::getLayoutOf(const std::string& path) noexcept
{
    //if the first element is a /, this is a unix file path
    if (path[0] == '/') {return PATH_LAYOUT_UNIX;}
    //if the second element is a collon (:), this is a windows file path
    if (path[1] == ':') {return PATH_LAYOUT_WINDOWS;}
    //if the type could not be determined, return undefined
    return PATH_LAYOUT_UNDEFINED;
}

bool Path::tokenizePath(const std::string& path, PathLayout layout, std::vector<std::string>& tokens) noexcept
{
    //error if the layout is undefined
    if (layout == PATH_LAYOUT_UNDEFINED) {return false;}

    //store the seperator token. It depends on windows or unix
    char seperator = (layout == PATH_LAYOUT_UNIX) ? '/' : '\\';

    //store the amount of symbols since the last seperator
    uint64_t len = 0;
    //store the start of the seperator. On unix, skip the first element
    uint64_t start = (layout == PATH_LAYOUT_UNIX) ? 1 : 0;
    //loop over all characters in the string
    for (uint64_t i = 0; i < path.size(); ++i)
    {
        //check for a seperator token
        if (path[i] == seperator)
        {
            //check if the length is valid
            if (len > 0)
            {
                //store the new element
                tokens.push_back(path.substr(start, len));
                //reset the length
                len = 0;
                //the next element is the start of the new word
                start = i + 1;
            }
        }
        else
        {
            //increase the length by 1
            ++len;
        }
    }

    //check if there are still tokens to store
    if (len > 0)
    {
        //store the new element
        tokens.push_back(path.substr(start, len));
    }

    //return the success
    return true;
}

bool Path::createPathFromTokens(std::string& str, PathLayout layout, const std::vector<std::string>& tokens) noexcept
{
    //error if the layout is undefined
    if (layout == PATH_LAYOUT_UNDEFINED) {return false;}

    //store the seperator token. It depends on windows or unix
    char seperator = (layout == PATH_LAYOUT_UNIX) ? '/' : '\\';

    //clear the string
    str.clear();
    
    //store the start token
    uint64_t start = 0;

    //check for unix layout
    if (layout == PATH_LAYOUT_UNIX)
    {
        //check if the first token has a collon as the second character
        if (tokens[0][1] == ':')
        {
            //this is the windows root, skip it
            start = 1;
        }
    }
    else if (layout == PATH_LAYOUT_WINDOWS)
    {
        //check if the first token does not have a collon as the second character
        if (tokens[0][1] != ':')
        {
            //check if this is a windows system
            #if GLGE_WINDOWS
            //the windows root is missing! extract the root (first 2 characters) from the absolute path and add them. 
            str += std::string(std::filesystem::current_path().c_str()).substr(0, 2);
            #else
            //just guess the C drive. There's no more that can be done here. 
            str += "C:";
            #endif
        }
    }

    //iterate over all tokens
    for (uint64_t i = start; i < tokens.size(); ++i)
    {
        //store the string to insert
        std::string insert = tokens[i];
        //add the current token to the string prefixed with the correct seperator
        str += seperator + insert;
    }

    //return the success
    return true;
}

std::string Path::getWorkingDirectoryIn(PathLayout layout)
{
    //get the working directory
    std::string workDir = std::string(std::filesystem::current_path());
    //return the working directory if the layout is undefined or native
    if ((layout == PATH_LAYOUT_UNDEFINED) || (layout == PATH_LAYOUT_NATIVE)) {return workDir;}

    //else, tokenize the working directory
    //store the tokens
    std::vector<std::string> tokens;
    //fully tokenize the path
    tokenizePath(workDir, PATH_LAYOUT_NATIVE, tokens);
    //restore the path in the requested format and override the original string
    createPathFromTokens(workDir, layout, tokens);

    //return the reconstructed string
    return workDir;
}






Path_t* GLGE_C_FUNC(makePath)(const char* str, PathLayout layout, PathLayout prefearedLayout)
{
    //create a new path and cast it to the opaque C type
    return (Path_t*)(new Path(std::string(str), layout, prefearedLayout));
}

PathLayout GLGE_C_FUNC(getPathLayout)(const Path_t* path)
{
    //return the path layout stored in the C++ class
    return ((Path*)path)->getLayout();
}

const char* GLGE_C_FUNC(getRawPath)(const Path_t* path)
{
    //return the raw path string stored in the C++ class as C string
    return ((Path*)path)->getRawPath().c_str();
}

char* GLGE_C_FUNC(getPath)(PathLayout layout, const Path_t* path)
{
    //store the C++ string of the path stored in the path class
    std::string str = ((Path*)path)->getPath(layout);
    //create a new char* that has enough characters
    char* ret = (char*)calloc(str.size(), sizeof(char));
    //copy the data from the C++ string to the char
    memcpy(ret, str.data(), str.size()*sizeof(char));
    //return the C string
    return ret;
}

PathLayout GLGE_C_FUNC(getLayoutOfPath)(const char* str)
{
    //return the path calculated
    return Path::getLayoutOf(std::string(str));
}

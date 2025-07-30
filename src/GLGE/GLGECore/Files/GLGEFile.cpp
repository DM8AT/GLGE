/**
 * @file GLGEFile.cpp
 * @author DM8AT
 * @brief implement the file management for the file class
 * @version 0.1
 * @date 2025-04-12
 * 
 * @copyright Copyright (c) 2025
 * 
 */

//include the file class
#include "GLGEFile.h"


void s_FileContents::operator=(const std::string& str)
{
    //resize the buffer
    resize(str.size());
    //copy the string data
    memcpy(data, str.data(), str.size());
}

void s_FileContents::operator+=(const std::string& str)
{
    //add a string to the content
    //create the contents as a string
    std::string dat = data;
    //add the new string
    dat += str;
    //store the data
    *this = dat;
}

void s_FileContents::resize(uint64_t size)
{
    //check if data exists
    if (data)
    {
        //delete the old data
        free(data);
    }
    //allocate the own data
    data = (char*)malloc(size+1);
    //clear the data
    bzero(data, size+1);
    //store the size 
    length = size;
}






File::File(const Path& path, bool create) : m_path(path.getPath(PATH_LAYOUT_NATIVE))
{
    //check if the file exists
    if (!exists(m_path) && create)
    {
        //create a temporary std::ofstream
        std::ofstream tmp(m_path.data());
        //just close it
        tmp.close();
    }
    //sync with the file
    reload();
}

File::~File()
{
    //reset all parameters
    m_path = "";
    m_content = "";
    m_potentialChange = false;
}

void File::save()
{
    //open an output file
    std::ofstream out(m_path.data());
    //write the data
    out.write(m_content.data, m_content.length);
    //close the file
    out.close();
}

bool File::didUpdate()
{
    //get the last write time
    auto last = std::filesystem::last_write_time(m_path);
    //check if they are not equal
    if (last > m_last)
    {
        //store the new time
        m_last = last;
        //say that an update occoured
        return true;
    }
    //else, no update occoured
    return false;
}

void File::reload()
{
    //open the input file
    std::ifstream in(m_path.data(), std::fstream::in | std::fstream::binary);
    //stop if the file could not be opend
    if (!in.is_open()) {return;}

    //get the size of the file
    uint64_t len = std::filesystem::file_size(m_path.data());
    
    //resize the string
    m_content.resize(len);
    //read the content of the file into the string
    in.read(m_content.data, len);
    //close the file
    in.close();

    //get the last write time
    m_last = std::filesystem::last_write_time(m_path);
}







File_t* GLGE_C_FUNC(openFile)(const Path* path, bool create) GLGE_FUNC_NOEXCEPT
{
    //create the new file instance, cast and return it
    return (File_t*)(new File(*path, create));
}

void GLGE_C_FUNC(closeFile)(File_t* file) GLGE_FUNC_NOEXCEPT
{
    //cast and delete the file
    delete ((File*)file);
}

FileContents* GLGE_C_FUNC(getFileContents)(File_t* file) GLGE_FUNC_NOEXCEPT
{
    //use the C++ function to read the contensts
    return &(((File*)file)->content());
}

uint64_t GLGE_C_FUNC(getFileLength)(File_t* file) GLGE_FUNC_NOEXCEPT
{
    //use the C++ function to read the length
    return ((File*)file)->getLength();
}

bool GLGE_C_FUNC(didFileUpdate)(File_t* file) GLGE_FUNC_NOEXCEPT
{
    //use the C++ function to check for an update
    return ((File*)file)->didUpdate();
}

void GLGE_C_FUNC(reloadFile)(File_t* file) GLGE_FUNC_NOEXCEPT
{
    //use the C++ function to reload the file
    ((File*)file)->reload();
}

void GLGE_C_FUNC(saveFile)(File_t* file) GLGE_FUNC_NOEXCEPT
{
    //use the C++ function to save the file
    ((File*)file)->save();
}

bool GLGE_C_FUNC(doesFileExist)(const char* path) GLGE_FUNC_NOEXCEPT
{

    //use the C++ function to check for an update
    return File::exists(path);
}
/**
 * @file GLGEFile.h
 * @author DM8AT
 * @brief handeling files in GLGE
 * @version 0.1
 * @date 2025-04-12
 * 
 * @copyright Copyright (c) 2025
 * 
 */

//header guard
#ifndef _GLGE_FILE_
#define _GLGE_FILE_

//include the settings
#include "../GLGESettings.h"
//include the utilities
#include "../GLGEUtil.h"

/**
 * @brief manage the contents of a file
 */
typedef struct s_FileContents {
    /**
     * @brief store the data of the file
     */
    char* data;
    /**
     * @brief store the length of the data
     */
    uint64_t length;

    //check if this is C++
    #if GLGE_CPP

    /**
     * @brief Construct a new FileContent
     */
    s_FileContents() : data(0), length(0) {}

    /**
     * @brief Destroy the FileContents
     */
    inline ~s_FileContents() {if (data) {free(data);} data = 0;}

    /**
     * @brief set the data of the file content
     * 
     * @param str a constant reference to the string of contents
     */
    void operator=(const std::string& str);

    /**
     * @brief add a string to the file contents
     * 
     * @param str the string to add
     */
    void operator+=(const std::string& str);

    /**
     * @brief change the size of the contents
     * @warning this clears the data! To transfear data, save it before resizing
     * 
     * @param size the new size of the contents
     */
    void resize(uint64_t size);

    /**
     * @brief print the file contents into an output stream
     * 
     * @param os the output stream to print to
     * @param fc a constant reference to the file contents to print
     * @return std::ostream& a reference to the new output stream
     */
    inline friend std::ostream& operator<<(std::ostream& os, const s_FileContents& fc) {return os << "fileContents{data: " << fc.data << ", length: " << fc.length << "}";}

    #endif //C++ section

} FileContents;

//check if this is C++
#if GLGE_CPP

/**
 * @brief a class to manage files
 */
class File
{
public:

    /**
     * @brief Construct a new File
     */
    File() = default;

    /**
     * @brief Open a file
     * 
     * @param path the path to the file to open
     * @param create true : the file can be created | false : the file can not be created
     */
    File(std::string_view path, bool create);

    /**
     * @brief Destroy the File
     */
    ~File();

    /**
     * @brief Get the Length of the file
     * 
     * @return const uint64_t& the length of the file's contents
     */
    inline uint64_t getLength() const noexcept {return m_content.length;}

    /**
     * @brief get the content of the file
     * 
     * @return std::string& a reference to the string containing the content
     */
    inline FileContents& content() noexcept {m_potentialChange = true; return m_content;}

    /**
     * @brief save the content of the file to the disk
     */
    void save();

    /**
     * @brief get if the file on the disk did update without changing this file
     * 
     * @return true : the file did update
     * @return false : the file did not update
     */
    bool didUpdate();

    /**
     * @brief reload the content from the file on the disk to the file
     */
    void reload();

    /**
     * @brief check if a file exists
     * 
     * @param path the path to check
     * @return true : the path exists
     * @return false : the path does not exist
     */
    inline static bool exists(std::string_view path) {return std::filesystem::exists(path) && !std::filesystem::is_directory(path);}
    
private:

    /**
     * @brief store the contents of the file
     */
    FileContents m_content;
    /**
     * @brief store the path of the own file
     */
    std::string_view m_path = "";
    
    /**
     * @brief store if a potential change occoured
     */
    bool m_potentialChange = false;
    /**
     * @brief store the last time changes where made
     */
    std::filesystem::file_time_type m_last;
};

#endif //C++ section

//check if this is C++
#if GLGE_CPP
extern "C" {
#endif //C++ section

    /**
     * @brief an opaque struct to store an instance of the File class
     */
    typedef struct s_File sFile;

    /**
     * @brief open a file
     * 
     * @param path the path for the file to open
     * @param create true : the file will be created | false : the file will not be created
     */
    sFile* GLGE_C_FUNC(openFile)(const char* path, bool create) GLGE_FUNC_NOEXCEPT;

    /**
     * @brief close a file
     * 
     * @param file a pointer to the file to close
     */
    void GLGE_C_FUNC(closeFile)(sFile* file) GLGE_FUNC_NOEXCEPT;

    /**
     * @brief get the file contents from a file
     * 
     * @param file a pointer to the file to read from
     * @return FileContents* a pointer to the contents of the file, can be used for reading and writing
     */
    FileContents* GLGE_C_FUNC(getFileContents)(sFile* file) GLGE_FUNC_NOEXCEPT;

    /**
     * @brief get the length of a file in bytes
     * 
     * @param file a pointer to the file to read the length from
     * @return uint64_t the length of the file in bytes
     */
    uint64_t GLGE_C_FUNC(getFileLength)(sFile* file) GLGE_FUNC_NOEXCEPT;

    /**
     * @brief check if a file was updated externaly
     * 
     * @param file a pointer to the file to check
     * @return true : the file was updated externaly
     * @return false : the file was not updated externaly
     */
    bool GLGE_C_FUNC(didFileUpdate)(sFile* file) GLGE_FUNC_NOEXCEPT;

    /**
     * @brief reload a file with the contents stored on disk
     * All pointers to the file contents will remain valid, there is no need to re-fetch them
     * 
     * @param file a pointer to the file to reload
     */
    void GLGE_C_FUNC(reloadFile)(sFile* file) GLGE_FUNC_NOEXCEPT;

    /**
     * @brief save a file to the disk
     * 
     * @param file a pointer to the file to save
     */
    void GLGE_C_FUNC(saveFile)(sFile* file) GLGE_FUNC_NOEXCEPT;

    /**
     * @brief check if a specific path is a file
     * 
     * @param path the path to check
     * @return true : the file does exist
     * @return false : the file does not exist
     */
    bool GLGE_C_FUNC(doesFileExist)(const char* path) GLGE_FUNC_NOEXCEPT;

//check if this is C++
#if GLGE_CPP
}
#endif //C++ section

#endif
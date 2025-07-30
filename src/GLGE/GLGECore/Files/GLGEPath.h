/**
 * @file GLGEPath.h
 * @author DM8AT
 * @brief handle file system pathes for different path layouts
 * @version 0.1
 * @date 2025-05-28
 * 
 * @copyright Copyright (c) 2025
 * 
 */

//header guard
#ifndef _GLGE_PATH_
#define _GLGE_PATH_

//include the settings
#include "../GLGESettings.h"
//include utilities
#include "../GLGEUtil.h"

/**
 * @brief specify the type of a path
 */
typedef enum e_PathLayout {
    /**
     * @brief specify that the layout is undefined. Default value for all inputs. 
     * @warning not all functions may handle this type correctly. If it is not handeld, an error will be thrown. 
     */
    PATH_LAYOUT_UNDEFINED = 0,
    /**
     * @brief specifiy that the layout is like on unix-based systems
     * - The root of the file system is identifyed by a /
     * - A / character is used to seperate folders
     */
    PATH_LAYOUT_UNIX,
    /**
     * @brief specify that the layout is like on windows. 
     * - The root of the file system is identifyed by ${CHARACTER}:
     * - A \ character is used to seperate folders
     */
    PATH_LAYOUT_WINDOWS,

    ////////////
    // ALIASES//
    ////////////

    /**
     * @brief linux is based on unix
     */
    PATH_LAYOUT_LINUX = PATH_LAYOUT_UNIX,
    /**
     * @brief debian is a linux distro
     */
    PATH_LAYOUT_DEBIAN = PATH_LAYOUT_LINUX,
    /**
     * @brief ubuntu is a debian based linux distro
     */
    PATH_LAYOUT_UBUNTU = PATH_LAYOUT_LINUX,
    /**
     * @brief arch is a linux distro
     */
    PATH_LAYOUT_ARCH = PATH_LAYOUT_LINUX,

    /**
     * @brief windows 10 is a windows system
     */
    PATH_LAYOUT_WIN_10 = PATH_LAYOUT_WINDOWS,
    /**
     * @brief windows 11 is a windows system
     */
    PATH_LAYOUT_WIN_11 = PATH_LAYOUT_WINDOWS,

    /**
     * @brief MacOS is unix based
     */
    PATH_LAYOUT_MAC = PATH_LAYOUT_UNIX,

    //check if this is windows
    #if GLGE_WINDOWS
    //define that the native path is using windows
    PATH_LAYOUT_NATIVE = PATH_LAYOUT_WINDOWS
    #else
    //define that the native path is using unix
    PATH_LAYOUT_NATIVE = PATH_LAYOUT_UNIX
    #endif

} PathLayout;

//check for C++
#if GLGE_CPP

/**
 * @brief store the path to handle
 */
class Path
{
public:

    /**
     * @brief Construct a new Path
     */
    Path() = default;

    /**
     * @brief Construct a new Path
     * @warning if the path is not in the format specified by layout, it will be changed to be in the format. This is because layout is the expected layout that will be used for internal storage
     * @warning the path creation will fail if the path layout is undefined
     * 
     * @param path the string of the path to store
     * @param layout the layout of the inputed path. CAN NOT BE UNDEFINED!
     * @param prefearedLayout the layout the path will be mostly used as or PATH_LAYOUT_UNDEFINED to use the format of the inputed path
     */
    Path(const std::string& path, PathLayout layout = PATH_LAYOUT_UNIX, PathLayout prefearedLayout = PATH_LAYOUT_NATIVE) noexcept;

    /**
     * @brief Destroy the Path
     */
    ~Path() {}

    /**
     * @brief Get the Layout of the path
     * 
     * @return const PathLayout& the layout of the path
     */
    const PathLayout& getLayout() const noexcept {return m_layout;}

    /**
     * @brief Get the raw stored path. It is always an absolute path. 
     * 
     * @return const std::string& the raw stored path
     */
    const std::string& getRawPath() const noexcept {return m_path;}

    /**
     * @brief Get the Path in a specific layout. It is always an absolute path. 
     * 
     * @param layout the layout for the path
     * @return std::string the string in the specified layout
     */
    std::string getPath(PathLayout layout) const noexcept;

    /**
     * @brief Get the layout of a specifc ABSOLUTE path
     * 
     * @param path the path to identify. Must be an absolute path!
     * @return PathLayout the layout of the path
     */
    static PathLayout getLayoutOf(const std::string& path) noexcept;

    /**
     * @brief convert a path into a bunch of directory tokens. The -1th element is the Root. The last element is the file
     * 
     * @param path the path to convert
     * @param layout the layout of the path to convert. CAN NOT BE PATH_LAYOUT_UNDEFINED!
     * @param tokens a vector containing all directorys of the path. The last element is the file to access
     * 
     * @return true : successfully tokenized the path
     * @return false : failed to tokenize the path
     */
    static bool tokenizePath(const std::string& path, PathLayout layout, std::vector<std::string>& tokens) noexcept;

    /**
     * @brief Create a path from a list of tokens
     * 
     * @param str the string to store the path to
     * @param layout the layout to create the path in
     * @param tokens the list of tokens to use for creation
     * 
     * @return true : successfully created the string
     * @return false : errors during string recreation
     */
    static bool createPathFromTokens(std::string& str, PathLayout layout, const std::vector<std::string>& tokens) noexcept;

    /**
     * @brief Get the path to the current working directory in the requested layout
     * 
     * @param layout the layout the working directory will be outputed as
     * @return std::string the string for the working directory in the requested format
     */
    static std::string getWorkingDirectoryIn(PathLayout layout);
    
    /**
     * @brief print the path into an output stream
     * 
     * @param o the output stream to print the path to
     * @param p the path to print
     * @return std::ostream& the filled output stream
     */
    friend std::ostream& operator<<(std::ostream& o, const Path& p) noexcept {return o << p.getPath(PATH_LAYOUT_NATIVE);}

protected:

    /**
     * @brief store the own layout string
     */
    std::string m_path = "";
    /**
     * @brief store the tokenized absolute path
     */
    std::vector<std::string> m_pathTokens;
    /**
     * @brief store the layout of the own string
     */
    PathLayout m_layout = PATH_LAYOUT_UNDEFINED;

};

//start the C section
extern "C" {
#endif //C++ section

/**
 * @brief store the path in C
 */
typedef struct s_Path Path_t;

/**
 * @brief create a new path from a string
 * 
 * @param str the path in string form
 * @param layout the layout of the inputed path
 * @param prefearedLayout the layout the path will be mostly used as or PATH_LAYOUT_UNDEFINED to use the format of the inputed path
 * 
 * @return Path* a pointer to a new path object
 */
Path_t* GLGE_C_FUNC(makePath)(const char* str, PathLayout layout, PathLayout prefearedLayout);

/**
 * @brief get the layout of the path object
 * 
 * @param path the path object to querry the layout information from
 * 
 * @return layout the layout stored in the path object
 */
PathLayout GLGE_C_FUNC(getPathLayout)(const Path_t* path);

/**
 * @brief get the raw path that is stored in a path object
 * 
 * @param path the path object to querry the raw path string from
 * 
 * @return const char* the path C string. It is valid as long as the path object exists. 
 */
const char* GLGE_C_FUNC(getRawPath)(const Path_t* path);

/**
 * @brief get the path that is stored in a path object in a specifc layout
 * 
 * @param layout the layout the path should have
 * @param path the path to querry the layout from
 * 
 * @return char* the path as a C string. It must be free'd on your own. 
 */
char* GLGE_C_FUNC(getPath)(PathLayout layout, const Path_t* path);

/**
 * @brief get the path layout of a path that is stored in string form
 * 
 * @param str the string path to querry the layout information from
 * 
 * @return PathLayout the layout of the path string
 */
PathLayout GLGE_C_FUNC(getLayoutOfPath)(const char* str);

//end the C section
#if GLGE_CPP
};
#endif // end of C section ending C++ block

#endif
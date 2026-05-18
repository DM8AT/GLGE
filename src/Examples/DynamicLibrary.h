/**
 * @file DynamicLibrary.h
 * @author DM8AT
 * @brief define a helper for managing dynamic libraries
 * @version 0.1
 * @date 2026-05-18
 * 
 * @copyright Copyright (c) 2026
 * 
 */
//header guard
#ifndef _EXAMPLE_DYNAMIC_LIBRARY_
#define _EXAMPLE_DYNAMIC_LIBRARY_

//add the required os-dependent dynamic library loaders
#include <filesystem>
#if defined(_WIN32)
    #ifndef NOMINMAX
        #define NOMINMAX
    #endif
    #include <windows.h>
#else
    #include <dlfcn.h>
#endif

//for type checking
#include <type_traits>

/**
 * @brief define an abstraction over the OS-specific dynamic library
 */
class DynamicLibrary {
public:

    /**
     * @brief Construct a new Dynamic Library
     */
    DynamicLibrary() = default;
    
    //dynamic library handles cannot be copied
    DynamicLibrary(const DynamicLibrary&) = delete;
    DynamicLibrary& operator=(const DynamicLibrary&) = delete;

    /**
     * @brief Construct a new Dynamic
     * 
     * Move constructor
     * 
     * @param o the other element to move from
     */
    DynamicLibrary(DynamicLibrary&& o) noexcept {
        //new element, no dynamic library can be open

        //move over and invalidate other
        m_handle = o.m_handle;
        o.m_handle = nullptr;
    }

    /**
     * @brief move operator
     * 
     * @param o the other library to move from
     * @return `DynamicLibrary&` a reference to this
     */
    DynamicLibrary& operator=(DynamicLibrary&& o) noexcept {
        //make sure to close any old library
        close();

        //move over and invalidate other
        m_handle = o.m_handle;
        o.m_handle = nullptr;

        //return a reference to self
        return *this;
    }

    /**
     * @brief Destroy the Dynamic Library
     */
    ~DynamicLibrary()
    {close();}

    /**
     * @brief open a dynamic library from a specific path
     * 
     * @param path the path to load from
     * @return `true` if the load was successful, `false` on error
     */
    bool open(const std::filesystem::path& path) {
        //make sure to close any old handles
        close();
        //control path must respect the load model
    #if defined(_WIN32)
        //load the windows handle
        m_handle = LoadLibraryW(path.c_str());
        //handle remains `nullptr` on failure
        return m_handle != nullptr;
    #else
        //load the so / dynlib handle
        m_handle = dlopen(path.c_str(), RTLD_NOW | RTLD_LOCAL);
        //handle remains `nullptr` on failure
        return m_handle != nullptr;
    #endif
    }

    /**
     * @brief get a specific symbol from the dynamic library
     * 
     * @tparam T the type of the function to load
     * @param name the name of the symbol to load
     * @return `T` the loaded function, `nullptr` on failure
     */
    template <typename T>
    requires (std::is_pointer_v<T> && std::is_function_v<std::remove_pointer_t<T>>)
    T symbol(const char* name) const {
        //sanity check that the handle is not null
        if (m_handle == nullptr)
        {return nullptr;}
        //get the address and reinterpret-cast it to provided type
    #if defined(_WIN32)
        return reinterpret_cast<T>(GetProcAddress(m_handle, name));
    #else
        return reinterpret_cast<T>(dlsym(m_handle, name));
    #endif
    }

    /**
     * @brief close any open dynamic library
     */
    void close() noexcept {
        //if a handle exists, close the library and reset the handle
        if (m_handle != nullptr) {
    #if defined(_WIN32)
            FreeLibrary(m_handle);
            m_handle = nullptr;
    #else
            dlclose(m_handle);
            m_handle = nullptr;
    #endif
        }
    }

protected:

#if defined(_WIN32)
    /**
     * @brief store the dll handle
     */
    HMODULE m_handle = nullptr;
#else
    /**
     * @brief store the handle for the so / dynamic library (dynlib)
     */
    void* m_handle = nullptr;
#endif

};

#endif
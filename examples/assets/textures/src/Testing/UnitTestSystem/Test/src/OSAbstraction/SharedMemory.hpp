/**
 * @file SharedMemory.hpp
 * @author DM8AT
 * @brief implement a shared memory helper
 * @version 0.1
 * @date 2026-05-21
 * 
 * @copyright Copyright (c) 2026
 * 
 */
//header guard
#pragma once

#ifndef OS_ABSTRACTION_DETECT_OS_MSG
#define OS_ABSTRACTION_DETECT_OS_MSG
    #if defined(_WIN32)
        #pragma message "Detected platform: Windows"
    #elif defined(__linux__)
        #pragma message "Detected platform: Linux (assuming posix)"
    #elif defined(__APPLE__)
        #pragma message "Detected platform: MaCOS"
    #else
        #error Unsupported platform
    #endif
#endif

//include the OS-Dependent headers
#ifdef _WIN32
    //add the windows API
    #define NOMINMAX
    #include <windows.h>
#else
    //add all libs for linux
    #include <fcntl.h>
    #include <sys/mman.h>
    #include <sys/stat.h>
    #include <unistd.h>
#endif

//add fixed sized integers
#include <cstdint>
//add cstrings for memory operations
#include <cstring>
//add strings
#include <string>

class SharedMemory {
public:

    /**
     * @brief define the the shared memory should be created or attached to
     */
    enum class Mode {
        /**
         * @brief create a new shared memory block
         */
        Create = 0,
        /**
         * @brief open a shared memory block
         */
        Open = 1
    };

    /**
     * @brief Construct a new Shared Memory
     */
    SharedMemory() = default;

    /**
     * @brief initialize the shared memory container
     * 
     * @param name the name of the shared memory block to create or attach to
     * @param size the size of the region to create or attach to
     * @param mode the mode to of the initialization
     * @return `true` if the shared memory was allocated, `false` if something went wrong
     */
    bool initialize(const std::string& name, size_t size, Mode mode) {
        //store the inputted state
        m_name = name;
        m_size = size;

    #ifdef _WIN32
        //if the mode is create, create a new file mapping
        if (mode == Mode::Create) {
            //create the file mapping
            m_handle = CreateFileMappingA(
                INVALID_HANDLE_VALUE,
                nullptr,
                PAGE_READWRITE,
                static_cast<DWORD>((uint64_t(size) >> 32) & 0xFFFFFFFF),
                static_cast<DWORD>(uint64_t(size) & 0xFFFFFFFF),
                m_name.c_str()
            );

            //if the handle was not created, return a failure
            if (!m_handle)
            {return false;}
        } else {
            //try to open the file mapping
            m_handle = OpenFileMappingA(
                FILE_MAP_ALL_ACCESS,
                FALSE,
                m_name.c_str()
            );

            //if the file mapping could not be opened, return a failure
            if (!m_handle)
            {return false;}
        }

        //create a view into the file
        m_ptr = MapViewOfFile(m_handle, FILE_MAP_ALL_ACCESS, 0, 0, m_size);
        //if the view could not be created, clean up and return a failure
        if (!m_ptr) {
            CloseHandle(m_handle);
            return false;
        }

    #else

        //setup the opening flag based on the mode
        int flags = (mode == Mode::Create) ? (O_CREAT | O_RDWR) : O_RDWR;
        //open the requested file
        m_fd = shm_open(m_name.c_str(), flags, 0666);

        //if the file descriptor was not opened, return a failure
        if (m_fd < 0) {
            perror("shm_open");
            return false;
        }

        //if the mode was create, check for truncation
        if (mode == Mode::Create) {
            if (ftruncate(m_fd, static_cast<off_t>(m_size)) != 0) {
                //if truncation happened, close the file and clean up
                close(m_fd);
                shm_unlink(m_name.c_str());
                //also, return a failure
                return false;
            }
        }

        //map a pointer into the shared memory
        m_ptr = mmap(nullptr, m_size, PROT_READ | PROT_WRITE, MAP_SHARED, m_fd,0);

        //if the pointer is the mapped failed pointer, clean up and return a failure
        if (m_ptr == MAP_FAILED) {
            close(m_fd);
            return false;
        }

    #endif

        //success
        return true;
    }

    /**
     * @brief Destroy the Shared Memory
     */
    ~SharedMemory()
    {
    #ifdef _WIN32

        //if a mapped pointer exists, close the view
        if (m_ptr)
        {UnmapViewOfFile(m_ptr);}
        //close the file handle
        if (m_handle)
        {CloseHandle(m_handle);}

    #else

        //if a valid mapped pointer exists, unmap the pointer
        if (m_ptr && m_ptr != MAP_FAILED)
        {munmap(m_ptr, m_size);}
        //close the file descriptor
        if (m_fd >= 0)
        {close(m_fd);}

    #endif
    }

    /**
     * @brief write data into the shared memory
     * 
     * @param data a pointer to the data to write
     * @param bytes the amount of bytes to write
     * @param offset the offset to write to
     * @return `true` on success, `false` on failure
     */
    bool write(const void* data, size_t bytes, size_t offset = 0) {
        //check the bounds, return a failure on fit failing
        if (boundsCheck(bytes, offset)) {
            //copy the data over and return success
            memcpy(static_cast<uint8_t*>(m_ptr) + offset, data, bytes);
            return true;
        } else {
            return false;
        }
    }

    /**
     * @brief read data from the shared memory
     * 
     * @param dst the destination to write to
     * @param bytes the amount of bytes to read
     * @param offset the offset to read from
     * @return `true` on success, `false` on failure
     */
    bool read(void* dst, std::size_t bytes, std::size_t offset = 0) const {
        //check the bounds, return a failure on fit failing
        if (boundsCheck(bytes, offset)) {
            //copy the data to the output and return success
            memcpy(dst, static_cast<uint8_t*>(m_ptr) + offset, bytes);
            return true;
        } else {
            return false;
        }
    }

    /**
     * @brief write an object into the shared memory
     * 
     * @tparam T the type of data to write
     * @param value a constant reference to the data to write
     * @param offset the byte offset to write to
     * @return `true` on success, `false` on failure
     */
    template<typename T>
    bool writeObject(const T& value, std::size_t offset = 0)
    {return write(&value, sizeof(T), offset);}

    /**
     * @brief read an object from the data
     * 
     * @tparam T the type of the object to read
     * @param value a reference to the value to read to
     * @param offset the byte offset to read from
     * @return `true` on success, `false` on failure
     */
    template<typename T>
    bool readObject(T& value, std::size_t offset = 0) const 
    {return read(&value, sizeof(T), offset);}

    /**
     * @brief get the raw shared memory data
     * 
     * @return `void*` a pointer to the raw data
     */
    void* data() 
    {return m_ptr;}

    /**
     * @brief get the raw shared memory data
     * 
     * @return `const void*` a constant pointer to the raw data
     */
    const void* data() const 
    {return m_ptr;}

    /**
     * @brief get the size of the shared memory
     * 
     * @return `size_t` the size of the shared memory in bytes
     */
    size_t size() const
    {return m_size;}

    /**
     * @brief Get the name of the accessed shared memory region
     * 
     * @return `const std::string&` a constant reference to the name
     */
    const std::string& name() const 
    {return m_name;}

protected:

    /**
     * @brief a helper to check bounds
     * 
     * @param bytes the amount of bytes to write
     * @param offset the byte offset the write is queued to
     * @return `true` if it fits in bounds, `false` if it does not
     */
    bool boundsCheck(size_t bytes, size_t offset) const {
        //check if the offset and byte count fits into size
        if ((offset + bytes) > m_size) 
        {return false;}
        //it fits
        return true;
    }

    /**
     * @brief store the shared memory name
     */
    std::string m_name;
    /**
     * @brief store the size
     */
    size_t m_size = 0;
    /**
     * @brief store a pointer into the shared memory
     */
    void* m_ptr = nullptr;

    #ifdef _WIN32
        /**
         * @brief store the shared memory handle
         */
        HANDLE m_handle = nullptr;
    #else
        /**
         * @brief store the shared memory file descriptor
         */
        int m_fd = -1;
    #endif

};
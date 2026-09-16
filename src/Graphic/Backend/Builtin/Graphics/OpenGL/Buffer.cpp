/**
 * @file Buffer.cpp
 * @author DM8AT
 * @brief implement the OpenGL buffer backend
 * @version 0.1
 * @date 2026-03-21
 * 
 * @copyright Copyright (c) 2026
 * 
 */
//add the buffer backend
#include "Graphic/Backend/Builtin/Graphics/OpenGL/Buffer.h"
//add the mappings
#include "__Mapper.h"
//add OpenGL resource sets
#include "Graphic/Backend/Builtin/Graphics/OpenGL/ResourceSet.h"

GLGE::Graphic::Backend::Graphic::OpenGL::Buffer::Buffer(Type type, const void* initial, size_t size, GLGE::Graphic::Backend::Graphic::Instance* instance, Usage usage) 
 : GLGE::Graphic::Backend::Graphic::Buffer(type, initial, size, instance, usage)
{
    //for CPU only just stop
    if (m_usage == Usage::CPU_ONLY) {return;}

    //create the buffer
    glCreateBuffers(1, &m_handle);
    //in debug name the buffer
    #if GLGE_DEBUG
    std::string name = std::string("Buffer_") + std::to_string(m_name);
    glObjectLabel(GL_BUFFER, m_handle, name.length(), name.c_str());
    #endif

    //create the buffer with the correct usage flags
    switch (m_usage) {
        case Usage::STREAMING: {
                //flags
                GLbitfield flags = GL_MAP_PERSISTENT_BIT | GL_MAP_COHERENT_BIT | GL_MAP_READ_BIT | GL_MAP_WRITE_BIT;
                //setup the buffer for constant mapping (e.g. streaming)
                glNamedBufferStorage(m_handle, m_size, initial, flags);
                //for streaming map the buffer
                m_data = glMapNamedBufferRange(m_handle, 0, m_size, flags);
                //store the flags
                m_flags = flags;
            } break;
        case Usage::STREAMING_DOWNLOAD: {
                //flags
                GLbitfield flags = GL_MAP_PERSISTENT_BIT | GL_MAP_COHERENT_BIT | GL_MAP_READ_BIT;
                //setup the buffer for constant mapping (e.g. streaming)
                glNamedBufferStorage(m_handle, m_size, initial, flags);
                //for streaming map the buffer
                m_data = glMapNamedBufferRange(m_handle, 0, m_size, flags);
                //store the flags
                m_flags = flags;
            } break;
        case Usage::STREAMING_UPLOAD: {
                //flags
                GLbitfield flags = GL_MAP_PERSISTENT_BIT | GL_MAP_COHERENT_BIT | GL_MAP_WRITE_BIT;
                //setup the buffer for constant mapping (e.g. streaming)
                glNamedBufferStorage(m_handle, m_size, initial, flags);
                //for streaming map the buffer
                m_data = glMapNamedBufferRange(m_handle, 0, m_size, flags);
                //store the flags
                m_flags = flags;
            } break;
        case Usage::CPU_GPU: {
                //flags
                GLbitfield flags = GL_MAP_PERSISTENT_BIT | GL_MAP_WRITE_BIT | GL_MAP_READ_BIT;
                //setup the buffer so that it may be updated from the client (the CPU)
                glNamedBufferStorage(m_handle, m_size, initial, flags);
                //for the buffer
                m_data = glMapNamedBufferRange(m_handle, 0, m_size, flags | GL_MAP_FLUSH_EXPLICIT_BIT);
                //store the flags
                m_flags = flags;
            } break;
        case Usage::GPU_ONLY: {
                //setup the buffer to prevent CPU side updates and fully prevent mapping (this uses dynamic GPU storage)
                glNamedBufferStorage(m_handle, m_size, initial, GL_DYNAMIC_STORAGE_BIT);
                //store the flags
                m_flags = GL_DYNAMIC_STORAGE_BIT;
            } break;

        default:
            //error
            throw GLGE::Exception("Invalid buffer usage", "GLGE::Graphic::Backend::Graphic::OpenGL::Buffer::Buffer");
            break;
    };

    //if this is a streaming buffer sanity check the mapping
    if ((m_flags & GL_MAP_PERSISTENT_BIT) && !m_data) 
    {throw GLGE::Exception("Failed to map the GPU buffer for a streaming buffer", "GLGE::Graphic::Backend::Graphic::OpenGL::Buffer::Buffer");}
}

GLGE::Graphic::Backend::Graphic::OpenGL::Buffer::~Buffer() {
    //if a buffer exists, delete it
    if (m_handle) {
        //if this is a streaming buffer, unmap it
        if ((m_flags & GL_MAP_PERSISTENT_BIT) && m_data)
        {glUnmapNamedBuffer(m_handle);}
        //clean up
        glDeleteBuffers(1, &m_handle);
        m_handle = 0;
    }
}

void GLGE::Graphic::Backend::Graphic::OpenGL::Buffer::write(const void* data, size_t size, size_t offset) {
    //if no data exists, nothing can be written
    if (!data || size == 0) {return;}

    //bounds check in debug
    #if GLGE_DEBUG
    if (offset + size > m_size) 
    {throw GLGE::Exception("Write out of bounds", "GLGE::Graphic::Backend::Graphic::OpenGL::Buffer::write");}
    #endif

    //blacklist some usages
    if ((m_usage == Usage::GPU_ONLY) || (m_usage == Usage::STREAMING_DOWNLOAD)) 
    {throw GLGE::Exception("Invalid usage flag provided for CPU-Side writing", "GLGE::Graphic::Backend::Graphic::OpenGL::Buffer::write");}

    //store the data
    memcpy(reinterpret_cast<u8*>(m_data) + offset, data, size);

    //mark dirty for CPU GPU shadowing system
    if (m_usage == Usage::CPU_GPU)
    {m_cpu_outOfDate.emplace_back(offset, size);}
}

void GLGE::Graphic::Backend::Graphic::OpenGL::Buffer::read(void* data, size_t size, size_t offset) {
    //sanity check the target buffer
    if (!data || size == 0) {return;}

    //bounds check in debug
    #if GLGE_DEBUG
    if (offset + size > m_size) 
    {throw GLGE::Exception("Read out of bounds", "GLGE::Graphic::Backend::Graphic::OpenGL::Buffer::read");}
    #endif

    //switch over the usage
    switch (m_usage) {
        //CPU only and CPU GPU use the exact same read pattern
        case Usage::CPU_ONLY:
        case Usage::CPU_GPU:
            //CPU copy exists
            memcpy(data, reinterpret_cast<u8*>(m_data) + offset, size);
            break;

        //GPU only cannot be read
        case Usage::GPU_ONLY: {
                //throw an error
                throw GLGE::Exception("Cannot read from GPU_ONLY buffer", "GLGE::Graphic::Backend::Graphic::OpenGL::Buffer::read");
            } break;

        //bidirectional streaming and stream downloading use the same download pattern
        case Usage::STREAMING:
        case Usage::STREAMING_DOWNLOAD: {
                //reading from a mapped buffer
                if (!(m_flags & GL_MAP_READ_BIT)) 
                {throw GLGE::Exception("Buffer not readable for CPU", "GLGE::Graphic::Backend::Graphic::OpenGL::Buffer::read");}
                memcpy(data, reinterpret_cast<u8*>(m_data) + offset, size);
            } break;

        //streaming upload cannot download
        case Usage::STREAMING_UPLOAD: {
                //writing-only streaming: cannot read
                throw GLGE::Exception("Cannot read from STREAMING_UPLOAD buffer", "GLGE::Graphic::Backend::Graphic::OpenGL::Buffer::read");
            } break;

        default:
            throw GLGE::Exception("Unknown buffer usage in read()", "GLGE::Graphic::Backend::Graphic::OpenGL::Buffer::read");
            break;
    }
}

void GLGE::Graphic::Backend::Graphic::OpenGL::Buffer::resize(size_t size, bool preserve) {
    //sanity check if resize is required
    if (m_size == size) {return;}

    //check for CPU only buffer (special case as no GPU buffer exists)
    if (m_usage == Usage::CPU_ONLY) {
        //allocate a new buffer
        u8* newBuff = new u8[size];
        memset(newBuff, 0, size);
        //if preserving is requested, copy the data
        if (preserve)
        {memcpy(newBuff, m_data, glm::min(size, m_size));}
        //swap the buffers
        u8* swap = reinterpret_cast<u8*>(m_data);
        m_data = newBuff;
        //clean up
        delete[] swap;
    } else {
        //create a new buffer with the current flags
        u32 newBuff = 0;
        glCreateBuffers(1, &newBuff);
        //in debug name the buffer
        #if GLGE_DEBUG
        std::string name = std::string("Buffer_") + std::to_string(m_name);
        glObjectLabel(GL_BUFFER, newBuff, name.length(), name.c_str());
        #endif
        glNamedBufferStorage(newBuff, size, nullptr, m_flags);
        //if mapping is requested, map the buffer
        void* newMap = nullptr;
        if (m_flags & GL_MAP_PERSISTENT_BIT)
        {newMap = glMapNamedBufferRange(newBuff, 0, size, m_flags | ((m_usage == Usage::CPU_GPU) ? GL_MAP_FLUSH_EXPLICIT_BIT : ((m_usage == Usage::GPU_ONLY) ? GL_DYNAMIC_STORAGE_BIT : 0)));}
        //if the preserving is requested, preserve the data
        if (preserve) 
        {memcpy(newMap, m_data, glm::min(size, m_size));}

        //store the new map
        m_data = newMap;
        //clean up the old buffer
        std::swap(newBuff, m_handle);
        glDeleteBuffers(1, &newBuff);
    }

    //store the new size
    m_size = size;
}

void GLGE::Graphic::Backend::Graphic::OpenGL::Buffer::fill(const void* pattern, size_t patternSize, size_t n, size_t offset) {
    //quick sanity check
    if (offset > m_size) {return;}

    //set the fill size if full is selected
    if (n == 0)
    {n = (m_size - offset) / ((patternSize == 0) ? 1 : patternSize);}

    //compute total bytes to operate on, stop if 0 bytes are modified
    size_t total = patternSize * n;
    if (total == 0) {return;}

    //shrink to fit
    if (offset + total > m_size)
    {total = (m_size - offset) / ((patternSize == 0) ? 1 : patternSize);}

    //special casing for null-clear: Allowed on everything
    if (!pattern && (patternSize == 0)) {
        //clear CPU buffer
        if (m_data) 
        {memset(reinterpret_cast<u8*>(m_data) + offset, 0, total);}
        //if a GPU buffer exists and CPU to GPU streaming is disabled, clear the GPU buffer
        if ((m_usage != Usage::CPU_ONLY) && (isStreamBuffer() && (m_usage != Usage::STREAMING_DOWNLOAD))) {
            u8 zero = 0;
            glClearNamedBufferSubData(m_handle, GL_R8UI, offset, total, GL_RED_INTEGER, GL_UNSIGNED_BYTE, &zero);
        }
    }

    //blacklist some usages
    if ((m_usage == Usage::GPU_ONLY) || (m_usage == Usage::STREAMING_DOWNLOAD)) 
    {throw GLGE::Exception("Invalid usage flag provided for pattern writing", "GLGE::Graphic::Backend::Graphic::OpenGL::Buffer::fill");}

    //if data is stored on the CPU use CPU writes
    if (m_data) {
        //update the CPU buffer
        u8* dst = reinterpret_cast<u8*>(m_data) + offset;
        for (size_t i = 0; i < n; ++i) 
        {memcpy(dst + (i * patternSize), pattern, patternSize);}

        //for CPU shadowing mark as dirty
        if (m_usage == Usage::CPU_GPU)
        {m_cpu_outOfDate.emplace_back(offset, total);}
    }
}

void GLGE::Graphic::Backend::Graphic::OpenGL::Buffer::sync(bool fromGPU) {
    //only required for CPU_GPU usage
    if (m_usage != Usage::CPU_GPU) {return;}

    //if no data exist nothing can be synced
    if (!m_data) {return;}

    //check the direction
    if (fromGPU) {
        //read back the full buffer
        glGetNamedBufferSubData(m_handle, 0, m_size, m_data);
        m_cpu_outOfDate.clear();
    } else {
        //iterate over all ranges and flush them
        for (const auto& range : m_cpu_outOfDate)
        {glFlushMappedNamedBufferRange(m_handle, range.offset, range.size);}
        //all ranges up to date
        m_cpu_outOfDate.clear();
    }
}

void GLGE::Graphic::Backend::Graphic::OpenGL::Buffer::onBuildBinding(GLGE::Graphic::ResourceSet* set, u32 unit) {
    //store the binding function
    static void (*func)(u32, void*) = [](u32 unit, void* self) {
        //get the handle
        u32 handle = reinterpret_cast<GLGE::Graphic::Backend::Graphic::OpenGL::Buffer*>(self)->getHandle();
        //get the type
        GLenum t = __translate_bufferType(reinterpret_cast<GLGE::Graphic::Backend::Graphic::OpenGL::Buffer*>(self)->getType());
        //map to the unit
        glBindBufferBase(t, unit, handle);
    };
    //register the binding function
    reinterpret_cast<GLGE::Graphic::Backend::Graphic::OpenGL::ResourceSet*>(set->getBackend().get())->registerBinder(func, this, unit);
}
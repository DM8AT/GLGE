/**
 * @file GLGEGraphicAPI_MemoryArena.h
 * @author DM8AT
 * @brief define the layout for a GPU memory management arena
 * @version 0.1
 * @date 2025-06-10
 * 
 * @copyright Copyright (c) 2025
 * 
 */

#ifndef _GLGE_GRAPHIC_API_MEMORY_ARENA_
#define _GLGE_GRAPHIC_API_MEMORY_ARENA_

//incldue the settings
#include "../../GLGECore/GLGESettings.h"
//include instances
#include "GLGEGraphicAPI_Instance.h"

/**
 * @brief inform about the intent of a memory reagion's use case
 */
typedef enum e_MemoryUsage {
    /**
     * @brief the usage of a memory reagion is unknown
     */
    MEMORY_USAGE_UNKNOWN = 0,
    /**
     * @brief specify that the memory is only used for reading
     * @warning this is used for optimization. Writing to a memory reagion markt as read only may resault in undefined behaviour
     */
    MEMORY_USAGE_READ,
    /**
     * @brief specify that the memory is only written to
     * @warning this is used for optimization. Reading a memory reagion markt as write only may resault in undefined behaviour
     */
    MEMORY_USAGE_WRITE,
    /**
     * @brief specify that the memory is used for reading and writing
     */
    MEMORY_USAGE_READ_WRITE,
    /**
     * @brief specify that a memory reagion is used as a uniform buffer
     */
    MEMORY_USAGE_UNIFORM
} MemoryUsage;

//check for C++
#if GLGE_CPP

/**
 * @brief store an continues area in VRam for arbituary data storage
 */
class GraphicMemoryArena : public GraphicInstAttatchable
{
public:

    /**
     * @brief store a pointer into the memory arena
     * 
     * A nullpointer is spezified by the size being 0. 
     */
    struct GraphicPointer
    {
        /**
         * @brief the start index of the reagion
         */
        uint64_t startIdx;
        /**
         * @brief the size of the reagion
         */
        uint64_t size;

        /**
         * @brief check if two graphic pointer point to the same memory reagion
         * @warning the information if the memory arena is the same isn't recorded
         * 
         * @param ptr a constant reference to the other pointer
         * @return true : both pointers point to the same reagion
         * @return false : they point to different reagions
         */
        inline bool operator==(const GraphicPointer& ptr) const noexcept {return (startIdx == ptr.startIdx) && (size == ptr.size);}

        /**
         * @brief print a graphic pointer into the default output
         * 
         * @param os the output stream to print to
         * @param ptr the pointer to print
         * @return std::ostream& the filled output stream
         */
        inline friend std::ostream& operator<<(std::ostream& os, const GraphicPointer& ptr) noexcept {return os << "GraphicPointer{index: " << ptr.startIdx << ", size: " << ptr.size << "}";}
    };

    /**
     * @brief Construct a new Graphic Memory Arena
     */
    GraphicMemoryArena() = default;

    /**
     * @brief Construct a new Graphic Memory Arena
     * 
     * @param size the size of the arena in bytes
     * @param allowResize spezify if dynamic resizing is allowed
     * @param usage specify how the memory reagion will be used
     * @param instance a reference to the instance the graphic memory will belong to
     */
    GraphicMemoryArena(uint64_t size, bool allowResize, MemoryUsage usage, Instance& instance) noexcept;

    /**
     * @brief Destroy the Graphic Memory Arena
     */
    virtual ~GraphicMemoryArena();

    /**
     * @brief a hook that is called when the memory arena is created
     */
    virtual void onCreate() noexcept {}

    /**
     * @brief a hook that is called when the memory arena is destroyed
     */
    virtual void onDestroy() noexcept {}

    /**
     * @brief allocate some amount of memory from the memory arena
     * 
     * @param size the size of the area to allocate
     * @return const GraphicPointer* a pointer to the requested area. The pointer belongs to the memory arena. 
     */
    GraphicPointer allocate(uint64_t size) noexcept;

    /**
     * @brief free a pointer allocated from a memory arena
     * 
     * @param ptr a reference to a pointer to the reagion
     * 
     * @return true : managed to free the pointer
     * @return false : failed to release the the pointer
     */
    bool release(GraphicPointer& ptr) noexcept;

    /**
     * @brief write new data into a section of VRam
     * 
     * @param ptr a pointer to the area to write
     * @param data the data to write
     */
    void update(const GraphicPointer& ptr, void* data) noexcept;

    /**
     * @brief change the size of the memory arena
     * 
     * @param size the new size of the memory arena in bytes
     * 
     * @return true : the resize was successfull
     * @return false : the resizing failed, possibly out of memory
     */
    bool resize(uint64_t size) noexcept;

    /**
     * @brief lock or unlock the memory
     * @warning only use if you know what your doing
     */
    void lock() noexcept;

    /**
     * @brief unlock the memory
     * @warning only use this if YOU locked the memory previously
     */
    void unlock() noexcept;

    /**
     * @brief get if the memory state is locked or unlocked
     * 
     * @return true : the memory is currently locked and being written or read
     * @return false : the memory is currently free
     */
    inline bool isLocked() const noexcept {return m_locked;}

    /**
     * @brief Get the Size of the memory arena
     * 
     * @return const uint64_t& the size of the memory arena
     */
    inline const uint64_t& getSize() const noexcept {return m_size;}

    /**
     * @brief Get the memory usage of the memory arena
     * 
     * @return const MemoryUsage& the memory usage of the memory arena
     */
    inline const MemoryUsage& getUsage() const noexcept {return m_usage;}

    /**
     * @brief bind the buffer to a specific unit on a command buffer
     * 
     * @param unit the unit to bind to
     * @param cmdBuff the commandbuffer the binding should occoure in
     */
    virtual void onBind(uint64_t, GraphicCommandBuffer*) noexcept {}

    /**
     * @brief unbind the buffer from a command buffer
     * 
     * @param cmdBuff the commandbuffer to de-couple from
     */
    virtual void onUnbind(GraphicCommandBuffer*) noexcept {}

protected:

    /**
     * @brief update all changed sections of GPU memory
     */
    virtual void gpuUpdate() noexcept {}

    /**
     * @brief register that the size of the whole arena changed
     */
    virtual void gpuSizeChanged() noexcept {}

    /**
     * @brief store the complete size of the memory arena
     */
    uint64_t m_size;
    /**
     * @brief store if dynamic resizing is allowed
     */
    bool m_allowResize = true;
    /**
     * @brief store a vector of free reagions
     */
    std::vector<GraphicPointer> m_free;
    /**
     * @brief store a vector of pointer pointing to changed reagions
     */
    std::vector<GraphicPointer> m_changed;

    /**
     * @brief stores if this memory arena is used by background stuff. This will silence the warnings for freeing it while content exists
     */
    bool m_isAPI = false;

    /**
     * @brief store a full CPU copy of the VRam reagion for fast write access
     * @warning if data is manipulated, this might not be up-to-date with the data
     */
    void* m_data = 0;
    /**
     * @brief store if the data is currently being read or written
     */
    std::mutex m_mutex;
    /**
     * @brief store if the mutex is locked or unlocked
     */
    bool m_locked = false;
    /**
     * @brief store the usage of the memory reagion
     */
    MemoryUsage m_usage = MEMORY_USAGE_UNKNOWN;
};

#endif //end of C++ section

#endif
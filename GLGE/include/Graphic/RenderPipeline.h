/**
 * @file RenderPipeline.h
 * @author DM8AT
 * @brief define what a render pipeline is
 * 
 * A render pipeline is a class that controls how rendering flows. It is responsible for telling the graphic backend abstract commands that 
 * are then translated into actual Graphic API commands by the backend implementation. Different backend implementations may translate
 * the commands differently, but the the final result should look visually the same on all implementations. But the performance of the backends may 
 * vary. 
 * 
 * @version 0.1
 * @date 2025-12-30
 * 
 * @copyright Copyright (c) 2025
 * 
 */
//header guard
#ifndef _GLGE_GRAPHIC_RENDER_PIPELINE_
#define _GLGE_GRAPHIC_RENDER_PIPELINE_

//include common stuff
#include "Core/Common.h"
//add the base class
#include "Core/BaseClass.h"
//add commands
#include "Command.h"
//add graphic instance
#include "Instance.h"

//add command buffers
#include "Backend/Graphics/CommandBuffer.h"
//add command handles
#include "Backend/Graphics/CommandHandle.h"

//use the library namespace
namespace GLGE::Graphic {

    /**
     * @brief a class that describes how rendering should happen
     * 
     * This class has a list of abstract commands it issues to the graphic backend. The graphic backend is then responsible to translate the
     * abstract commands into actual Graphic API commands (or handle them themself). The render pipeline records those commands into a buffer. 
     * Then, they can be replayed to create a frame. Each render pipeline has a specific target it renders. If the target is a window it then 
     * swaps it after rendering. 
     */
    class RenderPipeline : public GLGE::BaseClass {
    public:

        RenderPipeline(const RenderPipeline&) = delete;
        RenderPipeline(RenderPipeline&&) = delete;
        RenderPipeline& operator=(const RenderPipeline&) = delete;
        RenderPipeline& operator=(RenderPipeline&&) = delete;

        /**
         * @brief Destroy the Render Pipeline
         */
        ~RenderPipeline();

        /**
         * @brief create a render pipeline from a list of commands
         * 
         * @tparam Commands the types for the commands
         * @param cmds the actual command instances to store
         * @return `RenderPipeline` the render pipeline that uses the inputted commands
         */
        template <typename... Commands>
        requires (GLGE::Graphic::is_command_v<Commands> && ...)
        static RenderPipeline create(std::pair<const char*, Commands>... cmds) {
            //compute the exact size needed for the storage
            constexpr size_t s = (sizeof(cmds.second.payload) + ... + 0);
            //store a pool for the data
            u8* pool = new u8[s];
            //store a vector for the types and handles
            std::vector<CommandType> types;
            std::vector<Backend::Graphic::CommandHandle> handles;
            std::vector<std::string_view> names;
            types.reserve(sizeof...(Commands));
            handles.reserve(sizeof...(Commands));
            names.reserve(sizeof...(Commands));
            //fill in all the data
            size_t pos = 0;
            (storeCommand(cmds.second, types, handles, pool, pos), ...);
            //store all the names
            ((names.push_back(cmds.first)), ...);
            //iterate over the 
            return RenderPipeline(types, handles, names, pool);
        }

        /**
         * @brief Get the Graphic Instance the render pipeline belongs to
         * 
         * @return `Instance*` a pointer to the graphic instance the render pipeline belongs to
         */
        inline Instance* getGraphicInstance() const noexcept
        {return m_inst;}

        /**
         * @brief record the render pipeline
         * 
         * If the render pipeline is allready recorded, then the pipeline will first clear the previous recording
         * 
         * @param allowThreading `true` if threading is allowed, `false` if everything should be done on the current thread
         */
        void record(bool allowThreading = false);

        /**
         * @brief play back the render pipeline
         */
        void play()
        {m_cmdBuff->play();}

        /**
         * @brief check if the render pipeline is recorded
         * 
         * @return `true` if the render pipeline is currently recorded, `false` otherwise
         */
        inline bool isRecorded() const noexcept
        {return m_cmdBuff->isRecorded();}

        /**
         * @brief Get a specific command
         * 
         * @param name the name of the command to get
         * @return `Backend::Graphic::CommandHandle*` a pointer to the command handle
         */
        inline Backend::Graphic::CommandHandle* getCommand(const std::string_view& name)
        {return (m_nameMap.contains(name)) ? &m_handles[m_nameMap[name]] : nullptr;}

    private:

        /**
         * @brief a helper function to store a single command
         * 
         * @tparam Args the argument types of the command
         * @param cmd a constant reference to the command to store
         * @param types a list of the command types to store the command type to
         * @param handles a list of command handles to store the command handle to
         * @param data a pointer to an array of raw binary data to store parameters to
         * @param pos the byte position to write to in the data array. This will be advanced by the used size. 
         */
        template <typename ...Args>
        static void storeCommand(const Command<Args...>& cmd, std::vector<CommandType>& types, std::vector<Backend::Graphic::CommandHandle>& handles, u8* data, size_t& pos) {
            //copy over the payload
            memcpy(data + pos, cmd.payload, sizeof(cmd.payload));
            //store the type
            types.push_back(cmd.type);
            //create a new handle
            handles.emplace_back(sizeof(cmd.payload), data + pos);
            //go to the next element
            pos += sizeof(cmd.payload);
        }

        /**
         * @brief Construct a new Render Pipeline
         */
        RenderPipeline() = default;

        /**
         * @brief Construct a new Render Pipeline
         * 
         * @param types a list of the types of the commands
         * @param handles a vector that contains the handles for the commands
         * @param names a vector that contains names for all the stages
         * @param storage the raw argument storage
         */
        RenderPipeline(const std::vector<CommandType>& types, const std::vector<Backend::Graphic::CommandHandle>& handles, const std::vector<std::string_view>& names, u8* storage);

        /**
         * @brief store the graphic instance
         */
        Instance* m_inst = nullptr;

        /**
         * @brief store the command handle 
         */
        Reference<GLGE::Graphic::Backend::Graphic::CommandBuffer> m_cmdBuff = nullptr;

        /**
         * @brief store the types for all the commands
         */
        std::vector<CommandType> m_types;
        /**
         * @brief store the handles for all the commands
         */
        std::vector<Backend::Graphic::CommandHandle> m_handles;
        /**
         * @brief store names for all the commands
         */
        std::vector<std::string_view> m_names;
        /**
         * @brief store a name lookup map
         */
        std::unordered_map<std::string_view, size_t> m_nameMap;
        /**
         * @brief store the data pool that belongs to the render pipeline
         */
        u8* m_pool = nullptr;


    };

}

#endif
/**
 * @file RenderPipeline.cpp
 * @author DM8AT
 * @brief implement the render pipeline frontend
 * @version 0.1
 * @date 2026-01-01
 * 
 * @copyright Copyright (c) 2026
 * 
 */
//include the render pipeline
#include "Graphic/RenderPipeline.h"
//add profiling
#include "Core/Profiler.h"

//use the namespace
using namespace GLGE::Graphic;

RenderPipeline::RenderPipeline(const std::vector<CommandType>& types, const std::vector<Backend::Graphic::CommandHandle>& handles, 
                               const std::vector<std::string_view>& names, u8* storage) 
 : m_types(types), m_handles(handles), m_names(names), m_pool(storage)
{
    //get the graphic instance
    m_inst = getInstance()->getExtension<GLGE::Graphic::Instance>();
    //in debug do a sanity check
    #if GLGE_DEBUG
    if (!m_inst)
    {throw GLGE::Exception("Tried to create a render pipeline without first creating a graphic instance", "GLGE::Graphic::RenderPipeline::RenderPipeline");}
    #endif

    //fill the name lookup table
    for (size_t i = 0; i < m_names.size(); ++i)
    {m_nameMap[m_names[i]] = i;}

    //create the command buffer
    m_cmdBuff = m_inst->m_gDesc->createCommandBuffer(this);
}

RenderPipeline::~RenderPipeline() {
    //clean up the pool and set it to null
    delete m_pool;
    m_pool = nullptr;
}

void RenderPipeline::record(bool allowThreading) {
    GLGE_PROFILER_SCOPE();

    //if the command buffer is recorded, clear it
    if (m_cmdBuff->isRecorded())
    {m_cmdBuff->clear();}

    //Iterate over all commands
    for (size_t i = 0; i < m_names.size(); ++i) {
        GLGE_PROFILER_SCOPE_NAMED("GLGE::Graphic::RenderPipeline::record::recordSingle");

        //get the correct entry from the translation table
        auto* entry = m_inst->m_gDesc->getCommandTable()->getCommand((u32)m_types[i]);
        //sanity check
        if (!entry) {
            //in debug mode throw an error, in release just skip it
            #if GLGE_DEBUG
            throw GLGE::Exception("Tried to execute an unsupported command", "GLGE::Graphic::RenderPipeline::record");
            #endif
            continue;
        }

        //get the correct handle
        const auto& handle = m_handles[i];
        //run the command
        entry->func(*m_cmdBuff.get(), handle);
    }

    //finalize the command buffer
    m_cmdBuff->finalize();
}
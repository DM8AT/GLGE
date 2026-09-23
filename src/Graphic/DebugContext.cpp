/**
 * @file DebugContext.cpp
 * @author DM8AT
 * @brief implement the debug context
 * @version 0.1
 * @date 2026-09-23
 * 
 * @copyright Copyright (c) 2026
 * 
 */
//add the frontend
#include "GLGE/Graphic/DebugContext.h"

void GLGE::Graphic::DebugContext::beginRecording() {
    //in debug: sanity check
    #if GLGE_DEBUG
    if (m_state == State::RECORDING) {throw GLGE::Exception("Tried to start a debug context that was allready in a recording state", "GLGE::Graphic::DebugContext::beginRecording");}
    #endif

    //update the state
    m_state = State::RECORDING;

    //clean up all currently recorded meta-data
    m_cmds.clear();
    m_drawCmdRecords.clear();
    m_vertices.clear();
    m_indices.clear();
}

void GLGE::Graphic::DebugContext::setCamera(const Component::Camera& camera, const vec3& pos) {
    //in debug: sanity check
    #if GLGE_DEBUG
    if (m_state != State::RECORDING) {throw GLGE::Exception("Cannot record a set camera command on a debug context that is not currently in a recording state", "GLGE::Graphic::DebugContext::setCamera");}
    #endif

    //add a new camera set command
    Command cmd {
        .command = Command::SetCamera {
            .camera = camera,
            .pos = pos
        }
    };
    m_cmds.push_back(cmd);
}

void GLGE::Graphic::DebugContext::setTarget(const RenderTarget& target) {
    //in debug: sanity check
    #if GLGE_DEBUG
    if (m_state != State::RECORDING) {throw GLGE::Exception("Cannot record a set target command on a debug context that is not currently in a recording state", "GLGE::Graphic::DebugContext::setTarget");}
    #endif

    //add a new target set command
    Command cmd {
        .command = Command::SetTarget {
            .target = target
        }
    };
    m_cmds.push_back(cmd);
}

void GLGE::Graphic::DebugContext::draw(const DebugDrawDataProvider* drawData) {
    //in debug: sanity check
    #if GLGE_DEBUG
    if (m_state != State::RECORDING) {throw GLGE::Exception("Cannot record a draw command on a debug context that is not currently in a recording state", "GLGE::Graphic::DebugContext::draw");}
    #endif

    //create the draw command
    Command cmd {
        .command = Command::Draw {
            .drawCount = static_cast<u32>(drawData->getRecords().size()),
            .cmdStartIdx = static_cast<u32>(m_drawCmdRecords.size())
        }
    };

    //iterate over all command records to copy them over. While doing that also update the index and vertex offsets. 
    m_drawCmdRecords.reserve(m_drawCmdRecords.size() + drawData->getRecords().size());
    for (const auto& record : drawData->getRecords()) {
        DebugDrawDataProvider::CommandRecord copy = record;
        copy.indexStart += m_indices.size();
        copy.pointStart += m_vertices.size();
        m_drawCmdRecords.push_back(copy);
    }

    //just add the vertex and index data to the end of the unified record
    m_vertices.insert(m_vertices.end(), drawData->getPointBuffer().begin(), drawData->getPointBuffer().end());
    m_indices.insert(m_indices.end(), drawData->getIndexBuffer().begin(), drawData->getIndexBuffer().end());
}

void GLGE::Graphic::DebugContext::endRecording() {
    //in debug: sanity check
    #if GLGE_DEBUG
    if (m_state != State::RECORDING) {throw GLGE::Exception("Cannot end recording of a debug context that is not in a recording state", "GLGE::Graphic::DebugContext::endRecording");}
    #endif

    //update the state
    m_state = State::RECORDED;
    //invalidate self
    invalidate();
}
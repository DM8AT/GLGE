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
//add windows
#include "GLGE/Graphic/Window.h"

GLGE::Graphic::DebugContext::~DebugContext() {
    //remove from all shaders
    for (const auto& shader : m_currentlyReferencedShader) 
    {shader->getResources(0)->removeFrom(*this);}

    //invoke the cleanup function
    if (m_cleanupFn) {(*m_cleanupFn)(this);}
}

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
    //reset tracking objects
    m_camMatCount = 0;
    m_targetCount = 0;
    m_newReferencedShader.clear();
    m_newReferencedTargets.clear();
    m_drawCallCount = 0;

    //if a default shader is set, apply it
    if (m_defaultShader != nullptr)
    {setShader(m_defaultShader);}
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

    //increase the camera matrix count
    ++m_camMatCount;
}

void GLGE::Graphic::DebugContext::setTarget(const RenderTarget& target) {
    //in debug: sanity check
    #if GLGE_DEBUG
    if (m_state != State::RECORDING) {throw GLGE::Exception("Cannot record a set target command on a debug context that is not currently in a recording state", "GLGE::Graphic::DebugContext::setTarget");}
    #endif

    //at least 1 color target required
    if (target.getType()) {
        if (reinterpret_cast<GLGE::Graphic::Framebuffer*>(target.getTarget())->getBackend()->getColorAttachmentCount() == 0)
        {throw GLGE::Exception("All render targets require at least one color attachment", "GLGE::Graphic::DebugContext::setTarget");}
    }

    //add a new target set command
    Command cmd {
        .command = Command::SetTarget {
            .target = target
        }
    };
    m_cmds.push_back(cmd);
    //record the target binding
    m_newReferencedTargets.push_back(target);

    //increase the amount of used targets
    ++m_targetCount;
}

void GLGE::Graphic::DebugContext::setShader(GLGE::Graphic::Shader* shader) {
    //in debug: sanity check
    #if GLGE_DEBUG
    if (m_state != State::RECORDING) {throw GLGE::Exception("Cannot record a draw command on a debug context that is not currently in a recording state", "GLGE::Graphic::DebugContext::setShader");}

    //nullptr is invalid
    if (shader == nullptr)
    {throw GLGE::Exception("Cannot set the current to nullptr", "GLGE::Graphic::DebugContext::setShader");}
    #endif

    //create the set command
    Command cmd {
        .command = Command::SetShader {
            .shader = shader
        }
    };

    //store the command
    m_cmds.push_back(cmd);

    //update the current shader
    m_currentShader = shader;
}

void GLGE::Graphic::DebugContext::draw(const DebugDrawDataProvider* drawData) {
    //in debug: sanity check
    #if GLGE_DEBUG
    if (m_state != State::RECORDING) {throw GLGE::Exception("Cannot record a draw command on a debug context that is not currently in a recording state", "GLGE::Graphic::DebugContext::draw");}
    #endif

    //state sanity check
    if (m_camMatCount == 0)
    {throw GLGE::Exception("A camera must be bound before a draw command can be executed", "GLGE::Graphic::DebugContext::draw");}
    if (m_targetCount == 0)
    {throw GLGE::Exception("A render target must be set before a draw command can be executed", "GLGE::Graphic::DebugContext::draw");}
    if (m_currentShader == nullptr)
    {throw GLGE::Exception("A shader must be set before a draw command can be executed", "GLGE::Graphic::DebugContext::draw");}

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

    //increase the draw call count
    m_drawCallCount += drawData->getRecords().size();

    //just add the vertex and index data to the end of the unified record
    m_vertices.insert(m_vertices.end(), drawData->getPointBuffer().begin(), drawData->getPointBuffer().end());
    m_indices.insert(m_indices.end(), drawData->getIndexBuffer().begin(), drawData->getIndexBuffer().end());

    //store the command
    m_cmds.push_back(cmd);
}

void GLGE::Graphic::DebugContext::endRecording() {
    //in debug: sanity check
    #if GLGE_DEBUG
    if (m_state != State::RECORDING) {throw GLGE::Exception("Cannot end recording of a debug context that is not in a recording state", "GLGE::Graphic::DebugContext::endRecording");}
    #endif

    //update the state
    m_state = State::RECORDED;

    //only upload vertex data if data exists
    if (m_vertices.size() > 0) {
        //upload the vertex buffer
        m_vbo.resize(m_vertices.size() * sizeof(*m_vertices.data()), false);
        m_vbo.write(m_vertices.data(), m_vertices.size() * sizeof(*m_vertices.data()), 0);
    }
    //only upload index data if data exists
    if (m_indices.size() > 0) {
        //upload the index buffer
        m_ibo.resize(m_indices.size() * sizeof(*m_indices.data()), false);
        m_ibo.write(m_indices.data(), m_indices.size() * sizeof(*m_indices.data()), 0);
    }
    //prepare the camera matrix storage if camera matrices exist
    if (m_camMatCount > 0) {
        m_camBuff.resize(m_camMatCount * sizeof(glm::mat4), false);
    }

    //keep track of the active camera index and current target
    u32 camera = 0;
    u32 target = 0;
    //compute the per-draw data
    std::vector<PerDraw> drawData;
    drawData.reserve(m_drawCallCount);
    for (const auto& cmd : m_cmds) {
        if (std::holds_alternative<Command::Draw>(cmd.command)) {
            const auto& data = std::get<Command::Draw>(cmd.command);
            //record the per-draw data
            for (size_t i = 0; i < data.drawCount; ++i) {
                PerDraw draw {};
                const auto& style = m_drawCmdRecords[data.cmdStartIdx + i].style;
                draw.color = style.color;
                draw.cameraIdx = camera - 1; //index is camera discovered count minus 1
                draw.targetIdx = target - 1; //index is target discoverd count minus 1
                draw.pointSize = (style.renderMode == DebugDrawDataProvider::Style::RenderMode::VERTICES) ? style.pointSize : 0.f;
                drawData.push_back(draw);
            }
        } else if (std::holds_alternative<Command::SetCamera>(cmd.command)) {
            //next camera
            ++camera;
        } else if (std::holds_alternative<Command::SetTarget>(cmd.command)) {
            //next target
            ++target;
        }
    }

    //upload the per draw data if it exists
    if (drawData.size() > 0) {
        m_perDrawBuff.resize(drawData.size() * sizeof(*drawData.data()));
        m_perDrawBuff.write(drawData.data(), drawData.size() * sizeof(*drawData.data()), 0);
    }

    //remove from all old shader
    for (const auto& shader : m_currentlyReferencedShader) 
    {shader->getResources(0)->removeFrom(*this);}
    //attach to all new referenced shaders
    for (const auto& shader : m_newReferencedShader)
    {shader->getResources(0)->attachTo(*this);}

    //remove from all old targets
    for (const auto& target : m_currentlyReferencedTargets) {
        if (target.getType() == RenderTarget::WINDOW) {
            reinterpret_cast<Window*>(target.getTarget())->detachListener(this);
        } else if (target.getType() == RenderTarget::FRAMEBUFFER) {
            reinterpret_cast<Framebuffer*>(target.getTarget())->detachListener(this);
        } else {
            std::unreachable();
        }
    }
    //attach to all new targets
    for (const auto& target : m_newReferencedTargets) {
        if (target.getType() == RenderTarget::WINDOW) {
            reinterpret_cast<Window*>(target.getTarget())->attachListener(this);
        } else if (target.getType() == RenderTarget::FRAMEBUFFER) {
            reinterpret_cast<Framebuffer*>(target.getTarget())->attachListener(this);
        } else {
            std::unreachable();
        }
    }

    //update the current lists
    m_currentlyReferencedShader = m_newReferencedShader;
    m_currentlyReferencedTargets = m_newReferencedTargets;

    //pre-resize the target buffer if applicable
    if (m_targetCount > 0) {
        m_targetInfoBuff.resize(sizeof(uvec2) * m_targetCount);
    }

    //invalidate self
    invalidate();
}
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
    m_camMatrices.clear();
    m_currentTarget = static_cast<GLGE::Graphic::Window*>(nullptr);
    m_newReferencedShader.clear();
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

    //state sanity check
    if (m_currentTarget.getTarget() == nullptr)
    {throw GLGE::Exception("Cannot set a camera without binding a render target", "GLGE::Graphic::DebugContext::setCamera");}

    //add a new camera set command
    Command cmd {
        .command = Command::SetCamera {
            .camera = camera,
            .pos = pos
        }
    };
    m_cmds.push_back(cmd);

    //compute the target aspect
    float aspect = 1.f;
    switch (m_currentTarget.getType()) {
        case GLGE::Graphic::RenderTarget::WINDOW: 
            aspect = reinterpret_cast<GLGE::Graphic::Window*>(m_currentTarget.getTarget())->getResolution().x / float(reinterpret_cast<GLGE::Graphic::Window*>(m_currentTarget.getTarget())->getResolution().y);
            break;
        case GLGE::Graphic::RenderTarget::FRAMEBUFFER: 
            aspect = reinterpret_cast<GLGE::Graphic::Framebuffer*>(m_currentTarget.getTarget())->getBackend()->getColorAttachment(0)->getSize().x / float(reinterpret_cast<GLGE::Graphic::Framebuffer*>(m_currentTarget.getTarget())->getBackend()->getColorAttachment(0)->getSize().y);
            break;
        default: std::unreachable();
    }

    //compute the projection matrix
    glm::mat4 proj = glm::perspective(glm::radians(camera.FOV), aspect, camera.clip_near, camera.clip_far);
    //compute the transformation matrix
    glm::mat4 transf = glm::translate(glm::mat4(GLGE::Quaternion(camera.eulerAngles)), -pos);
    //combine into one matrix
    glm::mat4 camMatrix = proj * transf;

    //store in the list
    m_camMatrices.push_back(camMatrix);
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

    //store the current target
    m_currentTarget = target;
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
    if (m_camMatrices.size() == 0)
    {throw GLGE::Exception("A camera must be bound before a draw command can be executed", "GLGE::Graphic::DebugContext::draw");}
    if (m_currentTarget.getTarget() == nullptr)
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

    //upload the vertex buffer
    m_vbo.resize(m_vertices.size() * sizeof(*m_vertices.data()), false);
    m_vbo.write(m_vertices.data(), m_vertices.size() * sizeof(*m_vertices.data()), 0);
    //upload the index buffer
    m_ibo.resize(m_indices.size() * sizeof(*m_indices.data()), false);
    m_ibo.write(m_indices.data(), m_indices.size() * sizeof(*m_indices.data()), 0);
    //upload the camera matrices
    m_camBuff.resize(m_camMatrices.size() * sizeof(*m_camMatrices.data()));
    m_camBuff.write(m_camMatrices.data(), m_camMatrices.size() * sizeof(*m_camMatrices.data()), 0);

    //keep track of the active camera index
    u32 camera = 0;
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
                draw.pointSize = (style.renderMode == DebugDrawDataProvider::Style::RenderMode::VERTICES) ? style.pointSize : 0.f;
                drawData.push_back(draw);
            }
        } else if (std::holds_alternative<Command::SetCamera>(cmd.command)) {
            //next camera
            ++camera;
        }
    }

    //upload the per draw data
    m_perDrawBuff.resize(drawData.size() * sizeof(*drawData.data()));
    m_perDrawBuff.write(drawData.data(), drawData.size() * sizeof(*drawData.data()), 0);

    //remove from all old shader
    for (const auto& shader : m_currentlyReferencedShader) 
    {shader->getResources(0)->removeFrom(*this);}
    //attach to all new referenced shaders
    for (const auto& shader : m_newReferencedShader)
    {shader->getResources(0)->attachTo(*this);}

    //update the current list
    m_currentlyReferencedShader = m_newReferencedShader;

    //invalidate self
    invalidate();
}
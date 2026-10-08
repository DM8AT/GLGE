/**
 * @file GUIContext.cpp
 * @author DM8AT
 * @brief implement the GUI context
 * @version 0.1
 * @date 2026-09-29
 * 
 * @copyright Copyright (c) 2026
 * 
 */
//add the gui context
#include "GLGE/Graphic/GUIContext.h"

//add windows and framebuffers
#include "GLGE/Graphic/Window.h"
#include "GLGE/Graphic/Framebuffer.h"

GLGE::Graphic::GUIContext::GUIContext() 
 : BaseClass(),
   m_vbo(Buffer::Type::STORAGE_VERTEX, nullptr, 64, Buffer::Usage::STREAMING_UPLOAD), 
   m_ibo(Buffer::Type::STORAGE_INDEX, nullptr, 64, Buffer::Usage::STREAMING_UPLOAD),
   m_projMatBuff(Buffer::Type::STORAGE, nullptr, 64, Buffer::Usage::STREAMING_UPLOAD)
{}

GLGE::Graphic::GUIContext::~GUIContext() {
    if (m_cleanupFn)
    {(*m_cleanupFn)(this);}
    //drop references to everything
    //assume that the images where destroyed
    for (const auto& target : m_currentlyReferencedTargets) {
        if (target.getType() == RenderTarget::WINDOW) {
            reinterpret_cast<Window*>(target.getTarget())->attachListener(this);
        } else if (target.getType() == RenderTarget::FRAMEBUFFER) {
            reinterpret_cast<Framebuffer*>(target.getTarget())->attachListener(this);
        } else {
            std::unreachable();
        }
    }
}

void GLGE::Graphic::GUIContext::beginRecording() {
    //in debug: sanity check
    #if GLGE_DEBUG
    if (m_state == State::RECORDING) {throw GLGE::Exception("Tried to start a gui context that was allready in a recording state", "GLGE::Graphic::GUIContext::beginRecording");}
    #endif

    //update the state
    m_state = State::RECORDING;

    //clean up all currently recorded meta-data
    m_cmds.clear();
    m_vertices.clear();
    m_indices.clear();
    //reset tracking objects
    m_newReferencedTargets.clear();
    m_newReferencedImages.clear();
    m_drawCallCount = 0;
}

void GLGE::Graphic::GUIContext::draw(const GUIProvider* provider) {
    //make sure to store the current vertex / index buffer start
    Command cmd {
        .command = Command::SetDrawSubsection {
            .baseVertexOffset = static_cast<u32>(m_vertices.size()),
            .baseIndexOffset = static_cast<u32>(m_indices.size())
        }
    };
    m_cmds.push_back(cmd);

    //add the vertex and index buffers
    m_vertices.insert(m_vertices.end(), provider->getVertices().begin(), provider->getVertices().end());
    m_indices.insert(m_indices.end(), provider->getIndices().begin(), provider->getIndices().end());

    //iterate over all supplied commands and track them
    m_cmds.reserve(m_cmds.size() + provider->getCommands().size());
    for (const auto& cmd : provider->getCommands()) {
        //just insert the command
        m_cmds.push_back(Command {.command = cmd});

        //track some state
        if (std::holds_alternative<GUIProvider::Command::SetTarget>(cmd.command)) 
        {m_newReferencedTargets.push_back(std::get<GUIProvider::Command::SetTarget>(cmd.command).target);}
        if (std::holds_alternative<GUIProvider::Command::SetTexture>(cmd.command)) 
        {m_newReferencedImages.push_back(std::get<GUIProvider::Command::SetTexture>(cmd.command).image);}
    }
}

void GLGE::Graphic::GUIContext::endRecording() {
    //in debug: sanity check
    #if GLGE_DEBUG
    if (m_state != State::RECORDING) {throw GLGE::Exception("Cannot end recording of a goi context that is not in a recording state", "GLGE::Graphic::GUIContext::endRecording");}
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

    //track the projection matrices
    std::vector<glm::mat4> projMats;
    for (const auto& cmd : m_cmds) {
        if (std::holds_alternative<Command::DrawSubcmd>(cmd.command)) {
            const auto& c = std::get<Command::DrawSubcmd>(cmd.command).command;
            if (std::holds_alternative<Command::DrawSubcmd::SetProjection>(c)) {
                const auto& mat = std::get<Command::DrawSubcmd::SetProjection>(c);
                projMats.push_back(glm::mat4(
                    vec4{mat.matrix[ 0], mat.matrix[ 1], mat.matrix[ 2], mat.matrix[ 3]},
                    vec4{mat.matrix[ 4], mat.matrix[ 5], mat.matrix[ 6], mat.matrix[ 7]},
                    vec4{mat.matrix[ 8], mat.matrix[ 9], mat.matrix[10], mat.matrix[11]},
                    vec4{mat.matrix[12], mat.matrix[13], mat.matrix[14], mat.matrix[15]}
                ));
            }
        }
    }
    //if projection matrices exist, upload the buffer. Else, add an identity matrix and then upload
    if (projMats.size() == 0) {
        projMats.push_back(glm::mat4(1));
        m_cmds.insert(m_cmds.begin(), Command {
            .command = Command::DrawSubcmd {
                .command = Command::DrawSubcmd::SetProjection {
                    .matrix = {
                        1,0,0,0,
                        0,1,0,0,
                        0,0,1,0,
                        0,0,0,1
                    }
                }
            }
        }); //emulate a setting at the start
    }
    m_projMatBuff.resize(projMats.size() * sizeof(glm::mat4), false);
    m_projMatBuff.write(projMats.data(), sizeof(glm::mat4)*projMats.size(), 0);

    //Images: pointer identity, so use an unordered_set for de-duplication
    {
        //No longer used: Just assume that it was deleted
        //second pass: Fill old unordered set
        std::unordered_set<const Image*> oldImages;
        oldImages.reserve(m_currentlyReferencedImages.size());
        for (const auto& img : m_currentlyReferencedImages)
        {oldImages.insert(img);}
        //then, attach all elements that do not occur in the old set
        for (const auto& img : m_newReferencedImages) {
            if (!oldImages.contains(img))
            {img->attachTo(*this);}
        }
    }

    //Targets: compare the RenderTarget objects directly.
    //No hashing required; std::find gives O(n*m), but avoids requiring
    //operator<, hashing, or changing RenderTarget.
    {
        //just iterate and try to look up in new list
        for (const auto& oldTarget : m_currentlyReferencedTargets) {
            if (std::find(m_newReferencedTargets.begin(), m_newReferencedTargets.end(), oldTarget) == m_newReferencedTargets.end()) {
                if (oldTarget.getType() == RenderTarget::WINDOW) {
                    reinterpret_cast<Window*>(oldTarget.getTarget())->detachListener(this);
                } else if (oldTarget.getType() == RenderTarget::FRAMEBUFFER) {
                    reinterpret_cast<Framebuffer*>(oldTarget.getTarget())->detachListener(this);
                } else {
                    std::unreachable();
                }
            }
        }
        //just iterate and try to look up in old list
        for (const auto& newTarget : m_newReferencedTargets) {
            if (std::find(m_currentlyReferencedTargets.begin(), m_currentlyReferencedTargets.end(), newTarget) == m_currentlyReferencedTargets.end()) {
                if (newTarget.getType() == RenderTarget::WINDOW) {
                    reinterpret_cast<Window*>(newTarget.getTarget())->attachListener(this);
                } else if (newTarget.getType() == RenderTarget::FRAMEBUFFER) {
                    reinterpret_cast<Framebuffer*>(newTarget.getTarget())->attachListener(this);
                } else {
                    std::unreachable();
                }
            }
        }
    }
    //update the current lists
    m_currentlyReferencedTargets = m_newReferencedTargets;
    m_currentlyReferencedImages = m_newReferencedImages;

    //invalidate self
    invalidate();
}
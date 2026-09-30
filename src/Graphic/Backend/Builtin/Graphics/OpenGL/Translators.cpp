/**
 * @file Translators.cpp
 * @author DM8AT
 * @brief implement the translator functions from render pipeline commands to API calls
 * @version 0.1
 * @date 2026-01-01
 * 
 * @copyright Copyright (c) 2026
 * 
 */
//include the translator functions
#include "Translators.h"

//include video backend windows
#include "GLGE/Graphic/Backend/Video/Window.h"

//add OpenGL
#include "glad/glad.h"

//include profiling
#include "GLGE/Core/Profiler.h"

//add the OpenGL framebuffer implementation
#include "GLGE/Graphic/Backend/Builtin/Graphics/OpenGL/Framebuffer.h"
//add the OpenGL shader implementation
#include "GLGE/Graphic/Backend/Builtin/Graphics/OpenGL/Shader.h"
//add the OpenGL buffer implementation
#include "GLGE/Graphic/Backend/Builtin/Graphics/OpenGL/Buffer.h"
//add the OpenGL image implementation
#include "GLGE/Graphic/Backend/Builtin/Graphics/OpenGL/Image.h"

//add the shader frontend
#include "GLGE/Graphic/Shader.h"
//add the material frontend
#include "GLGE/Graphic/Material.h"
//add the mesh frontend
#include "GLGE/Graphic/Mesh.h"
//add render targets
#include "GLGE/Graphic/RenderTarget.h"
//add renderers
#include "GLGE/Graphic/Renderer.h"
//add debug context
#include "GLGE/Graphic/DebugContext.h"
//add gui context
#include "GLGE/Graphic/GUIContext.h"

//add the OpenGL contract
#include "GLGE/Graphic/Backend/Video/APIContracts/OpenGL.h"

//work in the implementation namespace

//helper to convert to depth function
static GLenum __toGLDepthFunc(GLGE::Graphic::DebugDrawDataProvider::Style::DepthTest mode) {
    switch (mode) {
        case GLGE::Graphic::DebugDrawDataProvider::Style::DepthTest::DEPTH_COMPARE_ALWAYS:         return GL_ALWAYS;
        case GLGE::Graphic::DebugDrawDataProvider::Style::DepthTest::DEPTH_COMPARE_NEVER:          return GL_NEVER;
        case GLGE::Graphic::DebugDrawDataProvider::Style::DepthTest::DEPTH_COMPARE_LESS:           return GL_LESS;
        case GLGE::Graphic::DebugDrawDataProvider::Style::DepthTest::DEPTH_COMPARE_LESS_EQUALS:    return GL_LEQUAL;
        case GLGE::Graphic::DebugDrawDataProvider::Style::DepthTest::DEPTH_COMPARE_GREATER:        return GL_GREATER;
        case GLGE::Graphic::DebugDrawDataProvider::Style::DepthTest::DEPTH_COMPARE_GREATER_EQUALS: return GL_GEQUAL;
        case GLGE::Graphic::DebugDrawDataProvider::Style::DepthTest::DEPTH_COMPARE_EQUALS:         return GL_EQUAL;
        case GLGE::Graphic::DebugDrawDataProvider::Style::DepthTest::DEPTH_COMPARE_NOT_EQUAL:      return GL_NOTEQUAL;
    }
    //fallback
    return GL_LESS;
}

//helper to convert GUI blend factor to GL enum
static GLenum __toGLBlendFac(GLGE::Graphic::GUIProvider::BlendFactor bf) {
    switch (bf) {
        case GLGE::Graphic::GUIProvider::BlendFactor::ZERO: return GL_ZERO;
        case GLGE::Graphic::GUIProvider::BlendFactor::ONE: return GL_ONE;
        case GLGE::Graphic::GUIProvider::BlendFactor::SRC_ALPHA: return GL_SRC_ALPHA;
        case GLGE::Graphic::GUIProvider::BlendFactor::ONE_MINUS_SRC_ALPHA: return GL_ONE_MINUS_SRC_ALPHA;
        
        default: std::unreachable();
    }
}

namespace OglImpl {

bool clear(GLGE::Graphic::Backend::Graphic::CommandBuffer& cmdBuff, const GLGE::Graphic::Backend::Graphic::CommandHandle& handle) {
    GLGE_PROFILER_SCOPE_NAMED("GLGE::Graphic::Backend::Graphic::OpenGL::Translators::clear");

    //store a helper function that is called when the clear is invoked
    static void (*helper_window)(GLGE::Graphic::Window*, GLGE::vec4, GLGE::f32, GLGE::u32) = [](GLGE::Graphic::Window* window, GLGE::vec4 color, float depth, GLGE::u32 stencil) {
            GLGE_PROFILER_SCOPE_NAMED("GLGE::Graphic::Backend::Graphic::OpenGL::Translators::clear::clear");
            {
                GLGE_PROFILER_SCOPE_NAMED("GLGE::Graphic::Backend::Graphic::OpenGL::Translators::clear::clear::colorClearPass");
                glBindFramebuffer(GL_FRAMEBUFFER, 0);
                glClearColor(color.r, color.g, color.b, color.a);
                glClearDepthf(depth);
                glClearStencil(stencil);
                glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT | GL_STENCIL_BUFFER_BIT);
            }
        };
    static void (*helper_fbuff)(GLGE::Graphic::Backend::Graphic::OpenGL::Framebuffer*, GLGE::u8, GLGE::vec4, float, GLGE::u32)
         = [](GLGE::Graphic::Backend::Graphic::OpenGL::Framebuffer* fbuff, GLGE::u8 idx, GLGE::vec4 color, float depth, GLGE::u32 stencil) {
            GLGE_PROFILER_SCOPE_NAMED("GLGE::Graphic::Backend::Graphic::OpenGL::Translators::clear::clear");

            {
                GLGE_PROFILER_SCOPE_NAMED("GLGE::Graphic::Backend::Graphic::OpenGL::Translators::clear::clear::colorClearPass");
                glClearNamedFramebufferfv(fbuff->getHandle(), GL_COLOR, idx, (GLGE::f32*)&color);
            }
            {
                GLGE_PROFILER_SCOPE_NAMED("GLGE::Graphic::Backend::Graphic::OpenGL::Translators::clear::clear::depthStencilClearPass");
                glClearNamedFramebufferfi(fbuff->getHandle(), GL_DEPTH_STENCIL, 0, depth, stencil);
            }
        };
        
    //extract the actual arguments
    const auto& [target, idx, color, depth, stencil] = handle.getArguments<GLGE::Graphic::RenderTarget, GLGE::u8, GLGE::vec4, GLGE::f32, GLGE::u32>();
    //queue the actual command
    if (target.getType() == GLGE::Graphic::RenderTarget::WINDOW) 
    {cmdBuff.addCommand(helper_window, (GLGE::Graphic::Window*)target.getTarget(), (GLGE::vec4)color, (GLGE::f32)depth, (GLGE::u32)stencil);}
    else 
    {cmdBuff.addCommand(helper_fbuff, (GLGE::Graphic::Backend::Graphic::OpenGL::Framebuffer*)(reinterpret_cast<GLGE::Graphic::Framebuffer*>(target.getTarget())->getBackend().get()), 
                        GLGE::u8(idx), (GLGE::vec4)color, (GLGE::f32)depth, (GLGE::u32)stencil);}

    //success
    return true;
}

bool copy(GLGE::Graphic::Backend::Graphic::CommandBuffer& cmdBuff, const GLGE::Graphic::Backend::Graphic::CommandHandle& handle) {
    GLGE_PROFILER_SCOPE_NAMED("GLGE::Graphic::Backend::Graphic::OpenGL::Translators::copy");

    //store a helper function that is called when the copy is invoked
    static void (*helper_f_to_w)(GLGE::Graphic::Backend::Graphic::Framebuffer*, GLGE::Graphic::Window*, GLGE::u8) = 
        [](GLGE::Graphic::Backend::Graphic::Framebuffer* fbuff, GLGE::Graphic::Window* win, GLGE::u8 idx) {
            GLGE_PROFILER_SCOPE_NAMED("GLGE::Graphic::Backend::Graphic::OpenGL::Translators::copy::copy");
            //then, copy the actual data
            {
                GLGE_PROFILER_SCOPE_NAMED("GLGE::Graphic::Backend::Graphic::OpenGL::Translators::copy::copy::copy");
                GLGE::u32 fbo = reinterpret_cast<GLGE::Graphic::Backend::Graphic::OpenGL::Framebuffer*>(fbuff)->getHandle();
                //blit command relies on read state for the read framebuffer
                glNamedFramebufferReadBuffer(fbo, GL_COLOR_ATTACHMENT0 + idx); //this is NOT the window
                glBindFramebuffer(GL_READ_FRAMEBUFFER, fbo);
                glBindFramebuffer(GL_DRAW_FRAMEBUFFER, 0);
                glDrawBuffer(GL_BACK);
                glBlitFramebuffer(0, 0, fbuff->getColorAttachment(0)->getSize().x, fbuff->getColorAttachment(0)->getSize().y, 
                                  0, 0, win->getResolution().x, win->getResolution().y, 
                                  GL_COLOR_BUFFER_BIT, GL_NEAREST);
            }
        };
    static void (*helper_f_to_f)(GLGE::Graphic::Backend::Graphic::OpenGL::Framebuffer*, GLGE::u8, GLGE::Graphic::Backend::Graphic::OpenGL::Framebuffer*, GLGE::u8) = 
        [](GLGE::Graphic::Backend::Graphic::OpenGL::Framebuffer* from, GLGE::u8 from_idx, GLGE::Graphic::Backend::Graphic::OpenGL::Framebuffer* to, GLGE::u8 to_idx) {
            GLGE_PROFILER_SCOPE_NAMED("GLGE::Graphic::Backend::Graphic::OpenGL::Translators::copy::copy");
            GLGE::u32 f = from->getHandle();
            GLGE::u32 t = to->getHandle();
            //blit command relies on read state for the read framebuffer
            //these both are NOT the window
            glNamedFramebufferReadBuffer(f, GL_COLOR_ATTACHMENT0 + from_idx);
            glNamedFramebufferDrawBuffer(t, GL_COLOR_ATTACHMENT0 + to_idx);
            glBlitNamedFramebuffer(f, t, 
                                    0, 0, from->getColorAttachment(0)->getSize().x, from->getColorAttachment(0)->getSize().y, 
                                    0, 0, to->getColorAttachment(0)->getSize().x, to->getColorAttachment(0)->getSize().y, 
                                    GL_COLOR_BUFFER_BIT, GL_NEAREST);
        };
    static void (*helper_w_to_f)(GLGE::Graphic::Backend::Graphic::Framebuffer*, GLGE::Graphic::Window*, GLGE::u8) = 
        [](GLGE::Graphic::Backend::Graphic::Framebuffer* fbuff, GLGE::Graphic::Window* win, GLGE::u8 idx) {
            GLGE_PROFILER_SCOPE_NAMED("GLGE::Graphic::Backend::Graphic::OpenGL::Translators::copy::copy");
            //then, copy the actual data
            {
                GLGE_PROFILER_SCOPE_NAMED("GLGE::Graphic::Backend::Graphic::OpenGL::Translators::copy::copy::copy");
                GLGE::u32 fbo = reinterpret_cast<GLGE::Graphic::Backend::Graphic::OpenGL::Framebuffer*>(fbuff)->getHandle();
                //configure FBO 0 correctly
                glBindFramebuffer(GL_READ_FRAMEBUFFER, 0);
                glReadBuffer(GL_BACK);
                glBindFramebuffer(GL_DRAW_FRAMEBUFFER, fbo);
                glDrawBuffer(GL_COLOR_ATTACHMENT0 + idx);
                glBlitFramebuffer(0, 0, win->getResolution().x, win->getResolution().y, 
                                  0, 0, fbuff->getColorAttachment(0)->getSize().x, fbuff->getColorAttachment(0)->getSize().y, 
                                  GL_COLOR_BUFFER_BIT, GL_NEAREST);
            }
        };
    //extract the actual arguments
    const auto& [from, from_idx, to, to_idx] = handle.getArguments<GLGE::Graphic::RenderTarget, GLGE::u8, GLGE::Graphic::RenderTarget, GLGE::u8>();
    
    //use the correct copy implementation
    if ((from.getType() == GLGE::Graphic::RenderTarget::FRAMEBUFFER) && (to.getType() == GLGE::Graphic::RenderTarget::WINDOW)) {
        cmdBuff.addCommand(helper_f_to_w, reinterpret_cast<GLGE::Graphic::Backend::Graphic::Framebuffer*>(reinterpret_cast<GLGE::Graphic::Framebuffer*>(from.getTarget())->getBackend().get()),
                           reinterpret_cast<GLGE::Graphic::Window*>(to.getTarget()), GLGE::u8(from_idx));
    } else if ((from.getType() == GLGE::Graphic::RenderTarget::FRAMEBUFFER) && (to.getType() == GLGE::Graphic::RenderTarget::FRAMEBUFFER)) {
        cmdBuff.addCommand(helper_f_to_f, 
                           reinterpret_cast<GLGE::Graphic::Backend::Graphic::OpenGL::Framebuffer*>(reinterpret_cast<GLGE::Graphic::Framebuffer*>(from.getTarget())->getBackend().get()), GLGE::u8(from_idx),
                           reinterpret_cast<GLGE::Graphic::Backend::Graphic::OpenGL::Framebuffer*>(reinterpret_cast<GLGE::Graphic::Framebuffer*>(to.getTarget())->getBackend().get()),   GLGE::u8(to_idx));

    } else if ((from.getType() == GLGE::Graphic::RenderTarget::WINDOW) && (to.getType() == GLGE::Graphic::RenderTarget::FRAMEBUFFER)) {
        cmdBuff.addCommand(helper_w_to_f, reinterpret_cast<GLGE::Graphic::Backend::Graphic::Framebuffer*>(reinterpret_cast<GLGE::Graphic::Framebuffer*>(to.getTarget())->getBackend().get()),
                           reinterpret_cast<GLGE::Graphic::Window*>(from.getTarget()), GLGE::u8(to_idx));
    } else {
        //invalid combination
        throw GLGE::Exception("Failed to build command buffer: Invalid target combination for copy", "GLGE::Graphic::Backend::Graphic::OpenGL::Translators::copy");
        return false;
    }

    //success
    return true;
}

bool dispatchCompute(GLGE::Graphic::Backend::Graphic::CommandBuffer& cmdBuff, const GLGE::Graphic::Backend::Graphic::CommandHandle& handle) {
    GLGE_PROFILER_SCOPE_NAMED("GLGE::Graphic::Backend::Graphic::OpenGL::Translators::dispatchCompute");

    //store a helper function that is called when the dispatch is invoked
    static void (*helper)(GLGE::Graphic::Backend::Graphic::OpenGL::Shader*, GLGE::uvec3) = [](GLGE::Graphic::Backend::Graphic::OpenGL::Shader* shader, GLGE::uvec3 invocations) {
        //bind the shader
        glUseProgram(shader->getProgram());
        //invoke the compute shader
        glDispatchCompute(invocations.x, invocations.y, invocations.z);
        //wait for the compute shader to be done
        glMemoryBarrier(GL_ALL_BARRIER_BITS);
    };

    //extract the actual arguments
    const auto& [shader, invocations] = handle.getArguments<GLGE::Graphic::Shader*, GLGE::uvec3>();

    //get the resource for all required sets and bind them
    const std::vector<GLGE::u32>& sets = shader->getOfType(GLGE::Graphic::Shader::Type::COMPUTE).sets;
    for (GLGE::u32 set : sets) {
        auto* resources = shader->getResources(set);
        if (!resources)
        {throw GLGE::Exception("Tried to dispatch a compute shader, but the compute shader requested resources for a set that was not set", "dispatchCompute"); return false;}
        resources->getBackend()->bind(&cmdBuff);
    }

    //queue the actual command
    cmdBuff.addCommand(helper, (GLGE::Graphic::Backend::Graphic::OpenGL::Shader*)shader->getBackend().get(), (GLGE::uvec3)invocations);
    //success
    return true;
}

bool drawWorld(GLGE::Graphic::Backend::Graphic::CommandBuffer& cmdBuff, const GLGE::Graphic::Backend::Graphic::CommandHandle& handle) {
    GLGE_PROFILER_SCOPE_NAMED("GLGE::Graphic::Backend::Graphic::OpenGL::Translators::drawWorld");

    //extract all arguments
    const auto& [renderer] = handle.getArguments<GLGE::Graphic::Renderer*>();

    //draw
    renderer->getBackend()->record(cmdBuff);
    
    //success
    return true;
}

bool drawDebug(GLGE::Graphic::Backend::Graphic::CommandBuffer& cmdBuff, const GLGE::Graphic::Backend::Graphic::CommandHandle& handle) {
    GLGE_PROFILER_SCOPE_NAMED("GLGE::Graphic::Backend::Graphic::OpenGL::Translators::drawDebug");

    //structure used to store persistent data

    struct Persistent {
        //store the used VAO
        GLuint vao;
    };

    //structure that represents GPU-Side per-draw data
    //lambda functions that are actually called

    //lambda called once for init
    void (*initCmd)(GLuint) = [](GLuint vao) {
        GLGE_PROFILER_SCOPE_NAMED("GLGE::Graphic::Backend::Graphic::OpenGL::Translators::drawDebug::init");
        //bind the VAO
        glBindVertexArray(vao);
    };

    //lambda called at the end for clean up
    void (*finishCmd)() = []() {
        GLGE_PROFILER_SCOPE_NAMED("GLGE::Graphic::Backend::Graphic::OpenGL::Translators::drawDebug::setTarget");
        //reset the depth write to true
        glDepthMask(GL_TRUE);
    };

    //lambda to update the target
    void (*setTarget)(GLGE::Graphic::RenderTarget) = [](GLGE::Graphic::RenderTarget target) {
        GLGE_PROFILER_SCOPE_NAMED("GLGE::Graphic::Backend::Graphic::OpenGL::Translators::drawDebug::setTarget");
        if (target.getType() == GLGE::Graphic::RenderTarget::WINDOW)
        {glBindFramebuffer(GL_FRAMEBUFFER, 0);}
        else if (target.getType() == GLGE::Graphic::RenderTarget::FRAMEBUFFER) 
        {glBindFramebuffer(GL_FRAMEBUFFER, static_cast<GLGE::Graphic::Backend::Graphic::OpenGL::Framebuffer*>(reinterpret_cast<GLGE::Graphic::Framebuffer*>(target.getTarget())->getBackend().get())->getHandle());}
    };

    //lambda to set the shader
    void (*setShader)(GLGE::Graphic::Shader*) = [](GLGE::Graphic::Shader* shader) {
        GLGE_PROFILER_SCOPE_NAMED("GLGE::Graphic::Backend::Graphic::OpenGL::Translators::drawDebug::setShader");
        //get the program
        GLuint prog = static_cast<GLGE::Graphic::Backend::Graphic::OpenGL::Shader*>(shader->getBackend().get())->getProgram();
        glUseProgram(prog);
    };

    //lambda to draw something
    void (*drawCmd)(GLGE::Graphic::DebugDrawDataProvider::Style, GLGE::u32, GLGE::u32, GLGE::u32, GLGE::u32, GLGE::u32) = [](GLGE::Graphic::DebugDrawDataProvider::Style style, GLGE::u32 firstVertex, GLGE::u32 vertexCount, GLGE::u32 firstIndex, GLGE::u32 indexCount, GLGE::u32 drawIdx) {
        //prepare for the style
        switch (style.blendMode) {
            case GLGE::Graphic::DebugDrawDataProvider::Style::BlendMode::OPAQUE: glDisable(GL_BLEND); break;
            case GLGE::Graphic::DebugDrawDataProvider::Style::BlendMode::NORMAL: glEnable(GL_BLEND); glBlendFunc(GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA); break;
            case GLGE::Graphic::DebugDrawDataProvider::Style::BlendMode::ADDITIVE: glEnable(GL_BLEND); glBlendFunc(GL_SRC_ALPHA, GL_ONE); break;
            default: glDisable(GL_BLEND);
        }
        glDepthMask((style.depthWrite) ? GL_TRUE : GL_FALSE);
        glDepthFunc(__toGLDepthFunc(style.depthTest));
        switch (style.cullMode) {
            case GLGE::Graphic::DebugDrawDataProvider::Style::CullMode::OFF: glDisable(GL_CULL_FACE); break;
            case GLGE::Graphic::DebugDrawDataProvider::Style::CullMode::BACK: glEnable(GL_CULL_FACE); glCullFace(GL_BACK); break;
            case GLGE::Graphic::DebugDrawDataProvider::Style::CullMode::FRONT: glEnable(GL_CULL_FACE); glCullFace(GL_FRONT); break;
            default: glDisable(GL_CULL_FACE);
        }

        //switch over the used render mode to select how to render the data
        switch (style.renderMode) {
            case GLGE::Graphic::DebugDrawDataProvider::Style::RenderMode::SOLID:
                glDrawElementsInstancedBaseVertexBaseInstance(GL_TRIANGLES, indexCount, GL_UNSIGNED_INT, reinterpret_cast<const void*>(firstIndex*sizeof(GLGE::u32)), 1, firstVertex, drawIdx);
                break;
            case GLGE::Graphic::DebugDrawDataProvider::Style::RenderMode::WIREFRAME:
                glDrawElementsInstancedBaseVertexBaseInstance(GL_LINES, indexCount, GL_UNSIGNED_INT, reinterpret_cast<const void*>(firstIndex*sizeof(GLGE::u32)), 1, firstVertex, drawIdx);
                break;
            case GLGE::Graphic::DebugDrawDataProvider::Style::RenderMode::VERTICES:
                glDrawArraysInstancedBaseInstance(GL_POINTS, firstVertex, vertexCount, 1, drawIdx);
                break;
            default: std::unreachable();
        }
    };

    //extract all arguments
    const auto& [context] = handle.getArguments<GLGE::Graphic::DebugContext*>();

    //check if a VAO exists
    if (context->getBackendData() != nullptr) {
        //clean up the old data
        Persistent* old = reinterpret_cast<Persistent*>(context->getBackendData());
        glDeleteVertexArrays(1, &old->vao);
        delete old;
    }
    //create the new VAO
    GLuint vao = 0;
    glCreateVertexArrays(1, &vao);
    
    glEnableVertexArrayAttrib(vao, 0);
    glVertexArrayAttribFormat(vao, 0, 3, GL_FLOAT, GL_FALSE, 0);
    glVertexArrayAttribBinding(vao, 0, 0);
    glVertexArrayVertexBuffer(vao, 0, static_cast<GLGE::Graphic::Backend::Graphic::OpenGL::Buffer*>(context->getVBO()->getBackendReference().get())->getHandle(), 0, 12);
    glVertexArrayElementBuffer(vao, static_cast<GLGE::Graphic::Backend::Graphic::OpenGL::Buffer*>(context->getIBO()->getBackendReference().get())->getHandle());

    //store the persistent data
    context->setBackendData(new Persistent {
        .vao = vao
    });
    Persistent* persistent = reinterpret_cast<Persistent*>(context->getBackendData());

    //get the commands from the context
    const auto& cmds = context->getCommands();

    //keep track of the current draw index
    GLGE::u32 currentDrawIdx = 0;

    //record the init command
    cmdBuff.addCommand(initCmd, persistent->vao);

    //keep track of the dimensions of all targets
    std::vector<GLGE::uvec2> extents;
    extents.reserve(context->getTargetInfoBuffer()->getSize() / sizeof(GLGE::uvec2));
    //keep track of the camera matrices
    std::vector<glm::mat4> camMatrices;
    camMatrices.reserve(context->getCameraBuffer()->getSize() / sizeof(glm::mat4));

    //iterate over all commands
    for (const auto& cmd : cmds) {
        //check which type of command this is
        if (std::holds_alternative<GLGE::Graphic::DebugContext::Command::Draw>(cmd.command)) {
            //get the draw list data
            const auto& draw = std::get<GLGE::Graphic::DebugContext::Command::Draw>(cmd.command);

            //iterate over all requested draws
            for (size_t i = draw.cmdStartIdx; i < (draw.cmdStartIdx + draw.drawCount); ++i) {
                //get the draw sub-command
                const auto& sub = context->getDrawSubCommands()[i];
                //record the function to draw the element
                cmdBuff.addCommand(drawCmd, sub.style, sub.pointStart, sub.pointCount, sub.indexStart, sub.indexCount, currentDrawIdx);
                //step the draw index
                ++currentDrawIdx;
            }
        } else if (std::holds_alternative<GLGE::Graphic::DebugContext::Command::SetCamera>(cmd.command)) {
            //get the camera
            const auto& camera = std::get<GLGE::Graphic::DebugContext::Command::SetCamera>(cmd.command).camera;
            const auto& pos = std::get<GLGE::Graphic::DebugContext::Command::SetCamera>(cmd.command).pos;
            //compute the projection matrix
            glm::mat4 proj = glm::perspective(glm::radians(camera.FOV), 1.f, camera.clip_near, camera.clip_far);
            //compute the transformation matrix
            glm::mat4 transf = glm::translate(glm::mat4(GLGE::Quaternion(camera.eulerAngles)), -pos);
            //combine into one matrix
            glm::mat4 camMatrix = proj * transf;

            //store in the list
            camMatrices.push_back(camMatrix);
        } else if (std::holds_alternative<GLGE::Graphic::DebugContext::Command::SetTarget>(cmd.command)) {
            //no requirements

            //get the target
            const auto& newTarget = std::get<GLGE::Graphic::DebugContext::Command::SetTarget>(cmd.command).target;
            //record the target update
            cmdBuff.addCommand(setTarget, GLGE::Graphic::RenderTarget(newTarget));
            //store the extent of the target
            GLGE::uvec2 extent;
            if (newTarget.getType() == GLGE::Graphic::RenderTarget::WINDOW) {
                extent = reinterpret_cast<GLGE::Graphic::Window*>(newTarget.getTarget())->getResolution();
            } else if (newTarget.getType() == GLGE::Graphic::RenderTarget::FRAMEBUFFER) {
                extent = reinterpret_cast<GLGE::Graphic::Framebuffer*>(newTarget.getTarget())->getBackend()->getColorAttachment(0)->getSize();
            } else {
                std::unreachable();
            }
            extents.push_back(extent);
        } else if (std::holds_alternative<GLGE::Graphic::DebugContext::Command::SetShader>(cmd.command)) {
            //get the shader
            GLGE::Graphic::Shader* shader = std::get<GLGE::Graphic::DebugContext::Command::SetShader>(cmd.command).shader;
            //bind the data
            shader->getResources(0)->getBackend()->bind(&cmdBuff);
            //set the shader
            cmdBuff.addCommand(setShader, shader);
        } else {
            std::unreachable();
        }
    }

    //record clean up
    cmdBuff.addCommand(finishCmd);

    //upload the recorded data
    context->getTargetInfoBuffer()->write(extents.data(), extents.size() * sizeof(*extents.data()), 0);
    context->getCameraBuffer()->write(camMatrices.data(), camMatrices.size() * sizeof(*camMatrices.data()), 0);

    //success
    return true;
}

bool drawGui(GLGE::Graphic::Backend::Graphic::CommandBuffer& cmdBuff, const GLGE::Graphic::Backend::Graphic::CommandHandle& handle) {
    GLGE_PROFILER_SCOPE_NAMED("GLGE::Graphic::Backend::Graphic::OpenGL::Translators::drawGUI");

    //structure used to store persistent data

    struct Persistent {
        //store the used VAO
        GLuint vao;
    };

    //structure that represents GPU-Side per-draw data
    //lambda functions that are actually called

    //lambda called once for init
    void (*initCmd)(GLuint, GLGE::u32) = [](GLuint vao, GLGE::u32 projMatBuff) {
        GLGE_PROFILER_SCOPE_NAMED("GLGE::Graphic::Backend::Graphic::OpenGL::Translators::drawGUI::init");
        //bind the VAO
        glBindVertexArray(vao);
        //bind the projection matrix buffer
        glBindBufferBase(GL_SHADER_STORAGE_BUFFER, 0, projMatBuff);
        //disable depth stuff
        glDisable(GL_DEPTH_TEST);
        //enable blending
        glEnable(GL_BLEND);
        //disable culling
        glDisable(GL_CULL_FACE);
        //enable scissoring
        glEnable(GL_SCISSOR_TEST);
        //make sure that the clip origin is lower left
        glClipControl(GL_LOWER_LEFT, GL_NEGATIVE_ONE_TO_ONE);
    };

    //lambda called at the end for clean up
    void (*finishCmd)() = []() {
        GLGE_PROFILER_SCOPE_NAMED("GLGE::Graphic::Backend::Graphic::OpenGL::Translators::drawGUI::setTarget");
        //reset the depth test to true
        glEnable(GL_DEPTH_TEST);
        //enable culling
        glEnable(GL_CULL_FACE);
        //disable scissoring
        glDisable(GL_SCISSOR_TEST);
    };

    //lambda to update the target
    void (*setTarget)(GLGE::Graphic::RenderTarget) = [](GLGE::Graphic::RenderTarget target) {
        GLGE_PROFILER_SCOPE_NAMED("GLGE::Graphic::Backend::Graphic::OpenGL::Translators::drawGUI::setTarget");
        if (target.getType() == GLGE::Graphic::RenderTarget::WINDOW)
        {glBindFramebuffer(GL_FRAMEBUFFER, 0);}
        else if (target.getType() == GLGE::Graphic::RenderTarget::FRAMEBUFFER) 
        {glBindFramebuffer(GL_FRAMEBUFFER, static_cast<GLGE::Graphic::Backend::Graphic::OpenGL::Framebuffer*>(reinterpret_cast<GLGE::Graphic::Framebuffer*>(target.getTarget())->getBackend().get())->getHandle());}
    };

    //lambda to set the shader
    void (*setShader)(GLGE::Graphic::Shader*) = [](GLGE::Graphic::Shader* shader) {
        GLGE_PROFILER_SCOPE_NAMED("GLGE::Graphic::Backend::Graphic::OpenGL::Translators::drawGUI::setShader");
        //get the program
        GLuint prog = static_cast<GLGE::Graphic::Backend::Graphic::OpenGL::Shader*>(shader->getBackend().get())->getProgram();
        glUseProgram(prog);
    };

    //lambda to set a texture
    void (*setTexture)(GLGE::u32, GLGE::u32) = [](GLGE::u32 tex, GLGE::u32 binding) {
        GLGE_PROFILER_SCOPE_NAMED("GLGE::Graphic::Backend::Graphic::OpenGL::Translators::drawGUI::setTexture");
        //just set it
        glBindTextureUnit(binding, tex);
    };

    //lambda to set a blend mode
    void (*setBlending)(GLenum, GLenum) = [](GLenum f1, GLenum f2) {
        GLGE_PROFILER_SCOPE_NAMED("GLGE::Graphic::Backend::Graphic::OpenGL::Translators::drawGUI::setBlending");
        //set the blending
        glBlendFunc(f1, f2);
    };

    //lambda to draw something
    void (*drawCmd)(GLGE::Graphic::GUIProvider::RenderMode, GLGE::u32, GLGE::u32, GLGE::u32, GLGE::u32, GLGE::u32) = [](GLGE::Graphic::GUIProvider::RenderMode mode, GLGE::u32 firstVertex, GLGE::u32 vertexCount, GLGE::u32 firstIndex, GLGE::u32 indexCount, GLGE::u32 projMatIdx) {
        GLGE_PROFILER_SCOPE_NAMED("GLGE::Graphic::Backend::Graphic::OpenGL::Translators::drawGUI::draw");
        //switch over the used render mode to select how to render the data
        switch (mode) {
            case GLGE::Graphic::GUIProvider::RenderMode::TRIANGLES:
                glDrawElementsInstancedBaseVertexBaseInstance(GL_TRIANGLES, indexCount, GL_UNSIGNED_INT, reinterpret_cast<const void*>(firstIndex*sizeof(GLGE::u32)), 1, firstVertex, projMatIdx);
                break;
            case GLGE::Graphic::GUIProvider::RenderMode::LINES:
                glDrawElementsInstancedBaseVertexBaseInstance(GL_LINES, indexCount, GL_UNSIGNED_INT, reinterpret_cast<const void*>(firstIndex*sizeof(GLGE::u32)), 1, firstVertex, projMatIdx);
                break;
            case GLGE::Graphic::GUIProvider::RenderMode::POINTS:
                glDrawArraysInstancedBaseInstance(GL_POINTS, firstVertex, vertexCount, 1, projMatIdx);
                break;
            default: std::unreachable();
        }
    };

    //lambda to set scissor
    void (*setScissor)(GLGE::vec2, GLGE::vec2, GLGE::uvec2) = [](GLGE::vec2 from, GLGE::vec2 to, GLGE::uvec2 extent) {
        GLGE_PROFILER_SCOPE_NAMED("GLGE::Graphic::Backend::Graphic::OpenGL::Translators::drawGUI::setScissor");
        
        //set state
        glScissor(from.x, extent.y - to.y, to.x - from.x, to.y - from.y);
    };

    //extract all arguments
    const auto& [context] = handle.getArguments<GLGE::Graphic::GUIContext*>();

    //check if a VAO exists
    if (context->getBackendData() != nullptr) {
        //clean up the old data
        Persistent* old = reinterpret_cast<Persistent*>(context->getBackendData());
        glDeleteVertexArrays(1, &old->vao);
        delete old;
    }
    //create the new VAO
    GLuint vao = 0;
    glCreateVertexArrays(1, &vao);
    
    glEnableVertexArrayAttrib(vao, 0);
    glEnableVertexArrayAttrib(vao, 1);
    glEnableVertexArrayAttrib(vao, 2);
    glVertexArrayAttribFormat(vao, 0, 2, GL_FLOAT, GL_FALSE, 0);
    glVertexArrayAttribFormat(vao, 1, 2, GL_FLOAT, GL_FALSE, 8);
    glVertexArrayAttribFormat(vao, 2, 4, GL_UNSIGNED_BYTE, GL_TRUE, 16);
    glVertexArrayAttribBinding(vao, 0, 0);
    glVertexArrayAttribBinding(vao, 1, 0);
    glVertexArrayAttribBinding(vao, 2, 0);
    glVertexArrayVertexBuffer(vao, 0, static_cast<GLGE::Graphic::Backend::Graphic::OpenGL::Buffer*>(context->getVBO()->getBackendReference().get())->getHandle(), 0, 20);
    glVertexArrayElementBuffer(vao, static_cast<GLGE::Graphic::Backend::Graphic::OpenGL::Buffer*>(context->getIBO()->getBackendReference().get())->getHandle());

    //store the persistent data
    context->setBackendData(new Persistent {
        .vao = vao
    });
    Persistent* persistent = reinterpret_cast<Persistent*>(context->getBackendData());

    //store the current projection matrix
    //Use UINT32_MAX since it will work as a poison and automatically wrap-around to 0 once a set occurs
    GLGE::u32 currProjMat = std::numeric_limits<GLGE::u32>::max();

    //init
    cmdBuff.addCommand(initCmd, persistent->vao, static_cast<GLGE::Graphic::Backend::Graphic::OpenGL::Buffer*>(context->getProjMatBuffer()->getBackendReference().get())->getHandle());

    //store the current render target
    GLGE::Graphic::RenderTarget currTarget = static_cast<GLGE::Graphic::Window*>(nullptr);
    //store the extent of the current render target
    GLGE::uvec2 extent = {0,0};
    //store the base offsets
    GLGE::u32 vtxBase = 0;
    GLGE::u32 idxBase = 0;
    //iterate over all commands
    for (const auto& cmd : context->getCommands()) {
        //check if the base is being set
        if (std::holds_alternative<GLGE::Graphic::GUIContext::Command::SetDrawSubsection>(cmd.command)) {
            const auto& val = std::get<GLGE::Graphic::GUIContext::Command::SetDrawSubsection>(cmd.command);
            vtxBase = val.baseVertexOffset;
            idxBase = val.baseIndexOffset;
        }
        //check if this is a draw sub-command
        else if (std::holds_alternative<GLGE::Graphic::GUIContext::Command::DrawSubcmd>(cmd.command)) {
            const auto& subCmd = std::get<GLGE::Graphic::GUIContext::Command::DrawSubcmd>(cmd.command).command;

            //switch over the sub-command
            if (std::holds_alternative<GLGE::Graphic::GUIContext::Command::DrawSubcmd::SetTexture>(subCmd)) {
                //get the set texture command
                const auto& setTex = std::get<GLGE::Graphic::GUIContext::Command::DrawSubcmd::SetTexture>(subCmd);
                //record the texture set
                GLGE::u32 handle = static_cast<GLGE::Graphic::Backend::Graphic::OpenGL::Image*>(setTex.image->getBackend().get())->getHandle();
                //record the set command
                cmdBuff.addCommand(setTexture, handle, GLGE::u32(setTex.slot));
            } else if (std::holds_alternative<GLGE::Graphic::GUIContext::Command::DrawSubcmd::SetShader>(subCmd)) {
                //get the shader
                const auto& setSdr = std::get<GLGE::Graphic::GUIContext::Command::DrawSubcmd::SetShader>(subCmd);
                //record the set command
                cmdBuff.addCommand(setShader, setSdr.shader);
            } else if (std::holds_alternative<GLGE::Graphic::GUIContext::Command::DrawSubcmd::SetTarget>(subCmd)) {
                //keep track
                currTarget = std::get<GLGE::Graphic::GUIContext::Command::DrawSubcmd::SetTarget>(subCmd).target;

                //store the extent
                if (currTarget.getType() == GLGE::Graphic::RenderTarget::WINDOW) {
                    extent = reinterpret_cast<GLGE::Graphic::Window*>(currTarget.getTarget())->getResolution();
                } else if (currTarget.getType() == GLGE::Graphic::RenderTarget::FRAMEBUFFER) {
                    extent = reinterpret_cast<GLGE::Graphic::Framebuffer*>(currTarget.getTarget())->getBackend()->getColorAttachment(0)->getSize();
                } else {
                    std::unreachable();
                }

                //record the setting
                cmdBuff.addCommand(setTarget, std::get<GLGE::Graphic::GUIContext::Command::DrawSubcmd::SetTarget>(subCmd).target);
            } else if (std::holds_alternative<GLGE::Graphic::GUIContext::Command::DrawSubcmd::SetProjection>(subCmd)) {
                //step to the next one
                ++currProjMat;
            } else if (std::holds_alternative<GLGE::Graphic::GUIContext::Command::DrawSubcmd::Draw>(subCmd)) {
                //get the draw data
                const auto& drawData = std::get<GLGE::Graphic::GUIContext::Command::DrawSubcmd::Draw>(subCmd);
                //record the draw command
                cmdBuff.addCommand(drawCmd, drawData.renderMode, GLGE::u32(drawData.firstVertex + vtxBase), drawData.drawElements, GLGE::u32(drawData.firstIndex + idxBase), drawData.drawElements, currProjMat);
            } else if (std::holds_alternative<GLGE::Graphic::GUIContext::Command::DrawSubcmd::SetRenderRegion>(subCmd)) {
                //get the scissor extent
                const auto& scissor = std::get<GLGE::Graphic::GUIContext::Command::DrawSubcmd::SetRenderRegion>(subCmd);
                //record the command
                cmdBuff.addCommand(setScissor, scissor.from, scissor.to, extent);
            } else if (std::holds_alternative<GLGE::Graphic::GUIContext::Command::DrawSubcmd::SetBlending>(subCmd)) {
                //get the blending state
                const auto& blending = std::get<GLGE::Graphic::GUIContext::Command::DrawSubcmd::SetBlending>(subCmd);
                //map both to GL state and record
                cmdBuff.addCommand(setBlending, __toGLBlendFac(blending.inFactor), __toGLBlendFac(blending.currFactor));
            } else {
                std::unreachable();
            }
        }
    }

    //clean up scissor state
    if (currTarget.getTarget() != nullptr) {
        GLGE::uvec2 extent;
        if (currTarget.getType() == GLGE::Graphic::RenderTarget::WINDOW) {
            extent = reinterpret_cast<GLGE::Graphic::Window*>(currTarget.getTarget())->getResolution();
        } else if (currTarget.getType() == GLGE::Graphic::RenderTarget::FRAMEBUFFER) {
            extent = reinterpret_cast<GLGE::Graphic::Framebuffer*>(currTarget.getTarget())->getBackend()->getColorAttachment(0)->getSize();
        } else {
            std::unreachable();
        }
        cmdBuff.addCommand(setScissor, GLGE::vec2{0,0}, GLGE::vec2(extent), extent);
    }

    //finish
    cmdBuff.addCommand(finishCmd);

    //success
    return true;
}

}
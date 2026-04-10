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
#include "Graphic/Backend/Video/Window.h"

//add OpenGL
#include "glad/glad.h"

//include profiling
#include "Core/Profiler.h"

//add the OpenGL framebuffer implementation
#include "Graphic/Backend/Builtin/Graphics/OpenGL/Framebuffer.h"
//add the OpenGL shader implementation
#include "Graphic/Backend/Builtin/Graphics/OpenGL/Shader.h"
//add the OpenGL mesh pool
#include "Graphic/Backend/Builtin/Graphics/OpenGL/MeshPool.h"

//add the shader frontend
#include "Graphic/Shader.h"
//add the material frontend
#include "Graphic/Material.h"
//add the mesh frontend
#include "Graphic/Mesh.h"
//add render targets
#include "Graphic/RenderTarget.h"
//add renderers
#include "Graphic/Renderer.h"

//work in the implementation namespace

namespace Impl {

bool swapWindow(GLGE::Graphic::Backend::Graphic::CommandBuffer& cmdBuff, const GLGE::Graphic::Backend::Graphic::CommandHandle& handle) {
    GLGE_PROFILER_SCOPE_NAMED("GLGE::Graphic::Backend::Graphic::OpenGL::Translators::swapWindow");

    //store a helper function that is called when the swap is invoked
    static void (*helper)(GLGE::Graphic::Window*) = [](GLGE::Graphic::Window* window) {
        GLGE_PROFILER_SCOPE_NAMED("GLGE::Graphic::Backend::Graphic::OpenGL::Translators::swapWindow::swapWindow");
        window->getVideoWindow()->onSwapWindow();
    };
    //extract the actual arguments
    const auto& [win] = handle.getArguments<GLGE::Graphic::Window*>();
    //queue the actual command
    cmdBuff.addCommand(helper, (GLGE::Graphic::Window*)win);
    //success
    return true;
}

bool clear(GLGE::Graphic::Backend::Graphic::CommandBuffer& cmdBuff, const GLGE::Graphic::Backend::Graphic::CommandHandle& handle) {
    GLGE_PROFILER_SCOPE_NAMED("GLGE::Graphic::Backend::Graphic::OpenGL::Translators::clear");

    //store a helper function that is called when the clear is invoked
    static void (*helper_window)(GLGE::Graphic::Window*, GLGE::vec4, GLGE::f32, GLGE::u32) = [](GLGE::Graphic::Window* window, GLGE::vec4 color, float depth, GLGE::u32 stencil) {
            GLGE_PROFILER_SCOPE_NAMED("GLGE::Graphic::Backend::Graphic::OpenGL::Translators::clear::clear");
            {
                GLGE_PROFILER_SCOPE_NAMED("GLGE::Graphic::Backend::Graphic::OpenGL::Translators::clear::clear::makeCurrent");
                window->getVideoWindow()->onMakeCurrent(window->getGraphicInstance()->getGraphicBackendInstance()->getContext());
            }
            {
                GLGE_PROFILER_SCOPE_NAMED("GLGE::Graphic::Backend::Graphic::OpenGL::Translators::clear::clear::colorClearPass");
                glClearNamedFramebufferfv(0, GL_COLOR, 0, (GLGE::f32*)&color);
            }
            {
                GLGE_PROFILER_SCOPE_NAMED("GLGE::Graphic::Backend::Graphic::OpenGL::Translators::clear::clear::depthStencilClearPass");
                glClearNamedFramebufferfi(0, GL_DEPTH_STENCIL, 0, depth, stencil);
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
            //first, ensure that the correct window is bound
            {
                GLGE_PROFILER_SCOPE_NAMED("GLGE::Graphic::Backend::Graphic::OpenGL::Translators::copy::copy::makeCurrent");
                win->getVideoWindow()->onMakeCurrent(win->getGraphicInstance()->getGraphicBackendInstance()->getContext());
            }
            //then, copy the actual data
            {
                GLGE_PROFILER_SCOPE_NAMED("GLGE::Graphic::Backend::Graphic::OpenGL::Translators::copy::copy::copy");
                GLGE::u32 fbo = reinterpret_cast<GLGE::Graphic::Backend::Graphic::OpenGL::Framebuffer*>(fbuff)->getHandle();
                //blit command relies on read state for the read framebuffer
                glNamedFramebufferReadBuffer(fbo, GL_COLOR_ATTACHMENT0 + idx);
                glBlitNamedFramebuffer(fbo, 0, 
                                       0, 0, fbuff->getColorAttachment(0)->getSize().x, fbuff->getColorAttachment(0)->getSize().y, 
                                       0, 0, win->getResolution().x, win->getResolution().y, GL_COLOR_BUFFER_BIT, GL_LINEAR);
            }
        };
    static void (*helper_f_to_f)(GLGE::Graphic::Backend::Graphic::OpenGL::Framebuffer*, GLGE::u8, GLGE::Graphic::Backend::Graphic::OpenGL::Framebuffer*, GLGE::u8) = 
        [](GLGE::Graphic::Backend::Graphic::OpenGL::Framebuffer* from, GLGE::u8 from_idx, GLGE::Graphic::Backend::Graphic::OpenGL::Framebuffer* to, GLGE::u8 to_idx) {
            GLGE_PROFILER_SCOPE_NAMED("GLGE::Graphic::Backend::Graphic::OpenGL::Translators::copy::copy");
            GLGE::u32 f = from->getHandle();
            GLGE::u32 t = to->getHandle();
            //blit command relies on read state for the read framebuffer
            glNamedFramebufferReadBuffer(f, GL_COLOR_ATTACHMENT0 + from_idx);
            glNamedFramebufferDrawBuffer(t, GL_COLOR_ATTACHMENT0 + to_idx);
            glBlitNamedFramebuffer(f, t, 
                                    0, 0, from->getColorAttachment(0)->getSize().x, from->getColorAttachment(0)->getSize().y, 
                                    0, 0, to->getColorAttachment(0)->getSize().x, to->getColorAttachment(0)->getSize().y, GL_COLOR_BUFFER_BIT, GL_LINEAR);
        };
    static void (*helper_w_to_f)(GLGE::Graphic::Backend::Graphic::Framebuffer*, GLGE::Graphic::Window*, GLGE::u8) = 
        [](GLGE::Graphic::Backend::Graphic::Framebuffer* fbuff, GLGE::Graphic::Window* win, GLGE::u8 idx) {
            GLGE_PROFILER_SCOPE_NAMED("GLGE::Graphic::Backend::Graphic::OpenGL::Translators::copy::copy");
            //first, ensure that the correct window is bound
            {
                GLGE_PROFILER_SCOPE_NAMED("GLGE::Graphic::Backend::Graphic::OpenGL::Translators::copy::copy::makeCurrent");
                win->getVideoWindow()->onMakeCurrent(win->getGraphicInstance()->getGraphicBackendInstance()->getContext());
            }
            //then, copy the actual data
            {
                GLGE_PROFILER_SCOPE_NAMED("GLGE::Graphic::Backend::Graphic::OpenGL::Translators::copy::copy::copy");
                GLGE::u32 fbo = reinterpret_cast<GLGE::Graphic::Backend::Graphic::OpenGL::Framebuffer*>(fbuff)->getHandle();
                //blit command relies on read state for the read framebuffer
                glNamedFramebufferDrawBuffer(fbo, GL_COLOR_ATTACHMENT0 + idx);
                glBlitNamedFramebuffer(0, fbo, 
                                       0, 0, win->getResolution().x, win->getResolution().y, 
                                       0, 0, fbuff->getColorAttachment(0)->getSize().x, fbuff->getColorAttachment(0)->getSize().y, GL_COLOR_BUFFER_BIT, GL_LINEAR);
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

bool drawSimpleMesh(GLGE::Graphic::Backend::Graphic::CommandBuffer& cmdBuff, const GLGE::Graphic::Backend::Graphic::CommandHandle& handle) {
    GLGE_PROFILER_SCOPE_NAMED("GLGE::Graphic::Backend::Graphic::OpenGL::Translators::drawSimpleMesh");

    //store a helper function that is called when the draw is invoked
    static void (*helper)(GLGE::u64, GLGE::u8, GLGE::u32, GLGE::Graphic::RenderTarget, GLGE::Graphic::Backend::Graphic::MeshPool::LODInfo)
     = [](GLGE::u64 meshId, GLGE::u8 LOD, GLGE::u32 instances, GLGE::Graphic::RenderTarget target, GLGE::Graphic::Backend::Graphic::MeshPool::LODInfo lod) {
        //bind the correct buffer type
        if (target.getType() == GLGE::Graphic::RenderTarget::WINDOW) {
            GLGE::Graphic::Window* win = reinterpret_cast<GLGE::Graphic::Window*>(target.getTarget());
            win->getVideoWindow()->onMakeCurrent(win->getGraphicInstance()->getGraphicBackendInstance()->getContext());
            glBindFramebuffer(GL_FRAMEBUFFER, 0);
        } else {
            auto* fbuff = reinterpret_cast<GLGE::Graphic::Backend::Graphic::OpenGL::Framebuffer*>(reinterpret_cast<GLGE::Graphic::Framebuffer*>(target.getTarget())->getBackend().get());
            GLGE::u32 fbo = fbuff->getHandle();
            //activate all color attachments
            static constexpr const GLenum bufs[16] = {GL_COLOR_ATTACHMENT0, GL_COLOR_ATTACHMENT1, GL_COLOR_ATTACHMENT2, GL_COLOR_ATTACHMENT3, GL_COLOR_ATTACHMENT4, GL_COLOR_ATTACHMENT5, GL_COLOR_ATTACHMENT6, GL_COLOR_ATTACHMENT7, GL_COLOR_ATTACHMENT8, GL_COLOR_ATTACHMENT9, GL_COLOR_ATTACHMENT10, GL_COLOR_ATTACHMENT11, GL_COLOR_ATTACHMENT12, GL_COLOR_ATTACHMENT13, GL_COLOR_ATTACHMENT14, GL_COLOR_ATTACHMENT15};
            glNamedFramebufferDrawBuffers(fbo, fbuff->getColorAttachmentCount(), bufs);
            //enable for general drawing
            glBindFramebuffer(GL_FRAMEBUFFER, fbo);
        }

        //shader + resources are bound allready
        GLenum indexType = (lod.index.size == 1) ? GL_UNSIGNED_BYTE : ((lod.index.size == 2) ? GL_UNSIGNED_SHORT : GL_UNSIGNED_INT);
        glDrawElementsInstanced(GL_TRIANGLES, lod.index.count, indexType, reinterpret_cast<const void*>(lod.index.offset), instances);
    };

    //extract all arguments
    const auto& [mesh, material, target, LOD, instances] = handle.getArguments<GLGE::Graphic::Mesh*, GLGE::Graphic::Material*, GLGE::Graphic::RenderTarget, GLGE::u8, GLGE::u32>();

    //store the mesh pool
    auto pool = reinterpret_cast<GLGE::Graphic::Backend::Graphic::OpenGL::MeshPool*>(mesh->getPool().get());
    //setup the offset
    GLGE::Graphic::Backend::Graphic::MeshPool::LODInfo lod = pool->getLODInfo(mesh->getID(), LOD);

    //bind the material
    material->getBackend()->bind(&cmdBuff, lod.vertex.offset);
    //draw
    cmdBuff.addCommand(helper, mesh->getID(), GLGE::u8(LOD), GLGE::u32(instances), GLGE::Graphic::RenderTarget(target), static_cast<GLGE::Graphic::Backend::Graphic::MeshPool::LODInfo>(lod));

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

}
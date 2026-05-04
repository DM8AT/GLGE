/**
 * @file Description.cpp
 * @author DM8AT
 * @brief implement the description for the OpenGL backend
 * @version 0.1
 * @date 2025-12-23
 * 
 * @copyright Copyright (c) 2025
 * 
 */
//add the OpenGL description
#include "Graphic/Backend/Builtin/Graphics/OpenGL/Description.h"

//add instances
#include "Graphic/Backend/Builtin/Graphics/OpenGL/Instance.h"
//add windows
#include "Graphic/Backend/Builtin/Graphics/OpenGL/Window.h"

//add normal command buffers (OpenGL just replays CPU commands)
#include "Graphic/Backend/Graphics/CommandBuffer.h"

//add command handles
#include "Graphic/Backend/Graphics/CommandHandle.h"

//add the translation functions
#include "Translators.h"

//add the sampler
#include "Graphic/Backend/Builtin/Graphics/OpenGL/Sampler.h"
//add the texture
#include "Graphic/Backend/Builtin/Graphics/OpenGL/Texture.h"
//add images
#include "Graphic/Backend/Builtin/Graphics/OpenGL/Image.h"
//add framebuffer
#include "Graphic/Backend/Builtin/Graphics/OpenGL/Framebuffer.h"
//add resources
#include "Graphic/Backend/Builtin/Graphics/OpenGL/ResourceSet.h"
//add shader
#include "Graphic/Backend/Builtin/Graphics/OpenGL/Shader.h"
//add buffers
#include "Graphic/Backend/Builtin/Graphics/OpenGL/Buffer.h"
//add sampled textures
#include "Graphic/Backend/Builtin/Graphics/OpenGL/SampledTexture.h"
//add mesh pools
#include "Graphic/Backend/Builtin/Graphics/OpenGL/MeshPool.h"
//add vertex layouts
#include "Graphic/Backend/Builtin/Graphics/OpenGL/VertexLayout.h"
//add materials
#include "Graphic/Backend/Builtin/Graphics/OpenGL/Material.h"
//add renderers
#include "Graphic/Backend/Builtin/Graphics/OpenGL/Renderer.h"

//add GLAD
#include "glad/glad.h"

//use the builtin namespace
using namespace GLGE::Graphic::Builtin::Graphics;

OpenGL::OpenGL() 
 : Description(GLGE::Graphic::Backend::Graphic::CommandTable({
        std::pair{GLGE::Graphic::COMMAND_SWAP_WINDOW, GLGE::Graphic::Backend::Graphic::CommandTable::TableEntry::create<GLGE::Graphic::Window*>(Impl::swapWindow)},
        std::pair{GLGE::Graphic::COMMAND_CLEAR, GLGE::Graphic::Backend::Graphic::CommandTable::TableEntry::create<GLGE::Graphic::RenderTarget, GLGE::u8, GLGE::vec4, GLGE::f32, GLGE::u32>(Impl::clear)},
        std::pair{GLGE::Graphic::COMMAND_COPY, GLGE::Graphic::Backend::Graphic::CommandTable::TableEntry::create<GLGE::Graphic::Framebuffer*, GLGE::Graphic::Window*, GLGE::u8>(Impl::copy)},
        std::pair{GLGE::Graphic::COMMAND_DISPATCH_COMPUTE, GLGE::Graphic::Backend::Graphic::CommandTable::TableEntry::create<GLGE::Graphic::Shader*, GLGE::uvec3>(Impl::dispatchCompute)},
        std::pair{GLGE::Graphic::COMMAND_DRAW_MESH, GLGE::Graphic::Backend::Graphic::CommandTable::TableEntry::create<GLGE::Graphic::Mesh*, GLGE::Graphic::Material*, GLGE::Graphic::RenderTarget, GLGE::u8, GLGE::u32>(Impl::drawSimpleMesh)},
        std::pair{GLGE::Graphic::COMMAND_DRAW_WORLD, GLGE::Graphic::Backend::Graphic::CommandTable::TableEntry::create<GLGE::Graphic::Renderer*>(Impl::drawWorld)}
    }))
{}

const char* OpenGL::getName() const noexcept
{return "GLGE_GRAPHIC_OPENGL_4_6_BUILTIN";}

GLGE::Version OpenGL::getVersion() const noexcept
{return GLGE::Version(0,1,0);}

GLGE::Reference<GLGE::Graphic::Backend::Graphic::Instance> OpenGL::createInstance( GLGE::Graphic::Instance* instance)
{return GLGE::Reference<GLGE::Graphic::Backend::Graphic::Instance>(new GLGE::Graphic::Backend::Graphic::OpenGL::Instance(instance), false);}

GLGE::Reference<GLGE::Graphic::Backend::Graphic::Window> OpenGL::createWindow(GLGE::Graphic::Window* window)
{return GLGE::Reference<GLGE::Graphic::Backend::Graphic::Window>(new GLGE::Graphic::Backend::Graphic::OpenGL::Window(window), false);}

GLGE::Reference<GLGE::Graphic::Backend::Graphic::CommandBuffer> OpenGL::createCommandBuffer([[maybe_unused]]GLGE::Graphic::RenderPipeline* renderPipeline)
{return GLGE::Reference<GLGE::Graphic::Backend::Graphic::CommandBuffer>(new GLGE::Graphic::Backend::Graphic::CommandBuffer(), false);}

GLGE::Reference<GLGE::Graphic::Backend::Graphic::Sampler> OpenGL::createSampler([[maybe_unused]] const GLGE::Graphic::SamplerCPU& sampler, [[maybe_unused]] GLGE::Graphic::Backend::Graphic::Instance* instance)
{return GLGE::Reference<GLGE::Graphic::Backend::Graphic::Sampler>(new GLGE::Graphic::Backend::Graphic::OpenGL::Sampler(sampler, instance), false);}

GLGE::Reference<GLGE::Graphic::Backend::Graphic::Texture> OpenGL::createTexture([[maybe_unused]] const GLGE::uvec2& size, [[maybe_unused]] GLGE::u16 mipCount, [[maybe_unused]] GLGE::Graphic::PixelFormat format)
{return GLGE::Reference<GLGE::Graphic::Backend::Graphic::Texture>(new GLGE::Graphic::Backend::Graphic::OpenGL::Texture(size, mipCount, format), false);}

GLGE::Reference<GLGE::Graphic::Backend::Graphic::Image> OpenGL::createImage([[maybe_unused]] const GLGE::uvec2& size, [[maybe_unused]] GLGE::Graphic::PixelFormat format, [[maybe_unused]] GLGE::u8 samples)
{return GLGE::Reference<GLGE::Graphic::Backend::Graphic::Image>(new GLGE::Graphic::Backend::Graphic::OpenGL::Image(size, format, samples), false);}

GLGE::Reference<GLGE::Graphic::Backend::Graphic::Framebuffer> OpenGL::createFramebuffer([[maybe_unused]] u8 colorAttachmentCount, [[maybe_unused]] Reference<GLGE::Graphic::Backend::Graphic::Image> const* colorAttachments, 
                                                                                        [[maybe_unused]] u8 depthAttachmentCount, [[maybe_unused]] Reference<GLGE::Graphic::Backend::Graphic::Image> const* depthAttachments, 
                                                                                        [[maybe_unused]] u8 stencilAttachmentCount, [[maybe_unused]] Reference<GLGE::Graphic::Backend::Graphic::Image> const* stencilAttachments, 
                                                                                        [[maybe_unused]] bool combinedDepthStencil)
{return GLGE::Reference<GLGE::Graphic::Backend::Graphic::Framebuffer>(new GLGE::Graphic::Backend::Graphic::OpenGL::Framebuffer(colorAttachmentCount, colorAttachments, depthAttachmentCount, depthAttachments, stencilAttachmentCount, stencilAttachments, combinedDepthStencil), false);}

GLGE::Reference<GLGE::Graphic::Backend::Graphic::ResourceSet> OpenGL::createResourceSet([[maybe_unused]]GLGE::Graphic::ResourceSet* set)
{return GLGE::Reference<GLGE::Graphic::Backend::Graphic::ResourceSet>(new GLGE::Graphic::Backend::Graphic::OpenGL::ResourceSet(set), false);}

GLGE::Reference<GLGE::Graphic::Backend::Graphic::Shader> OpenGL::createShader([[maybe_unused]] GLGE::Graphic::Shader* frontend)
{return GLGE::Reference<GLGE::Graphic::Backend::Graphic::Shader>(new GLGE::Graphic::Backend::Graphic::OpenGL::Shader(frontend), false);}

GLGE::Reference<GLGE::Graphic::Backend::Graphic::Buffer> OpenGL::createBuffer([[maybe_unused]] GLGE::Graphic::Backend::Graphic::Buffer::Type type, [[maybe_unused]] const void* initial, [[maybe_unused]] size_t size, [[maybe_unused]] GLGE::Graphic::Backend::Graphic::Buffer::Usage usage)
{return GLGE::Reference<GLGE::Graphic::Backend::Graphic::Buffer>(new GLGE::Graphic::Backend::Graphic::OpenGL::Buffer(type, initial, size, usage), false);}

GLGE::Reference<GLGE::Graphic::Backend::Graphic::SampledTexture> OpenGL::createSampledTexture([[maybe_unused]] const Reference<GLGE::Graphic::Backend::Graphic::Texture>& texture, [[maybe_unused]] const Reference<GLGE::Graphic::Backend::Graphic::Sampler>& sampler)
{return GLGE::Reference<GLGE::Graphic::Backend::Graphic::SampledTexture>(new GLGE::Graphic::Backend::Graphic::OpenGL::SampledTexture(texture, sampler), false);}

GLGE::Reference<GLGE::Graphic::Backend::Graphic::SampledTexture> OpenGL::createSampledTexture([[maybe_unused]] const Reference<GLGE::Graphic::Backend::Graphic::Image>& image, [[maybe_unused]] const Reference<GLGE::Graphic::Backend::Graphic::Sampler>& sampler)
{return GLGE::Reference<GLGE::Graphic::Backend::Graphic::SampledTexture>(new GLGE::Graphic::Backend::Graphic::OpenGL::SampledTexture(image, sampler), false);}

GLGE::Reference<GLGE::Graphic::Backend::Graphic::MeshPool> OpenGL::createMeshPool() 
{return GLGE::Reference<GLGE::Graphic::Backend::Graphic::MeshPool>(new GLGE::Graphic::Backend::Graphic::OpenGL::MeshPool(), false);}

GLGE::Reference<GLGE::Graphic::Backend::Graphic::VertexLayout> OpenGL::createVertexLayout(const VertexAttribute* ptr, size_t size, size_t stride, Reference<GLGE::Graphic::Backend::Graphic::MeshPool> pool)
{return GLGE::Reference<GLGE::Graphic::Backend::Graphic::VertexLayout>(new GLGE::Graphic::Backend::Graphic::OpenGL::VertexLayout(ptr, size, stride, pool), false);}

GLGE::Reference<GLGE::Graphic::Backend::Graphic::Material> OpenGL::createMaterial(Reference<GLGE::Graphic::Backend::Graphic::Shader> shader, Reference<GLGE::Graphic::Backend::Graphic::VertexLayout> layout, GLGE::Graphic::Backend::Graphic::Material::CullMode cullMode, GLGE::Graphic::Backend::Graphic::Material::DepthMode depthMode, bool depthWrite)
{return GLGE::Reference<GLGE::Graphic::Backend::Graphic::Material>(new GLGE::Graphic::Backend::Graphic::OpenGL::Material(shader, layout, cullMode, depthMode, depthWrite), false);}

GLGE::Reference<GLGE::Graphic::Backend::Graphic::Renderer> OpenGL::createRenderer(World& world, Object* camera, RenderTarget target)
{return GLGE::Reference<GLGE::Graphic::Backend::Graphic::Renderer>(new GLGE::Graphic::Backend::Graphic::OpenGL::Renderer(world, camera, target), false);}

GLGE::Graphic::GraphicAPI OpenGL::getAPI() const noexcept
{return GLGE::Graphic::GraphicAPI::OPEN_GL;}

GLGE::Version OpenGL::getAPIVersion() const noexcept
{return GLGE::Version(4, 6, 0);}

void OpenGL::initialize() {
    //get general GPU information
    const GLubyte* vendor   = glGetString(GL_VENDOR);
    const GLubyte* renderer = glGetString(GL_RENDERER);
    const GLubyte* version  = glGetString(GL_VERSION);
    //store the information
    m_gpuName = reinterpret_cast<const char*>(renderer);
    m_vendorName = reinterpret_cast<const char*>(vendor);
    m_driverVersion = reinterpret_cast<const char*>(version);
}
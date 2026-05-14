/**
 * @file Description.h
 * @author DM8AT
 * @brief define the backend description for the builtin vulkan backend
 * @version 0.1
 * @date 2026-04-14
 * 
 * @copyright Copyright (c) 2026
 * 
 */
//header guard
#ifndef _GLGE_GRAPHIC_BACKEND_BUILTIN_GRAPHICS_VK_DESCRIPTION_
#define _GLGE_GRAPHIC_BACKEND_BUILTIN_GRAPHICS_VK_DESCRIPTION_

//include the backend description
#include "Graphic/Backend/Graphics/BackendDescription.h"

//use the graphic builtin namespace
namespace GLGE::Graphic {
/**
 * @brief a namespace that stores all build-in descriptor types
 */
namespace Builtin {
/**
 * @brief a namespace that stores all graphic API related build-in descriptor types
 */
namespace Graphics {

    /**
     * @brief the descriptor class for the vulkan backend
     */
    class Vulkan : public GLGE::Graphic::Backend::Graphic::Description {
    public:

        /**
         * @brief Construct a new Open GL descriptor instance
         */
        Vulkan();

        /**
         * @brief Destroy the Open GL descriptor instance
         */
        virtual ~Vulkan() {}
        
        /**
         * @brief Get the Name of the backend
         * 
         * @return const char* the constant backend name
         */
        virtual const char* getName() const noexcept override;

        /**
         * @brief Get the version of the backend implementation
         * 
         * @return `GLGE::Version` the version of the backend in use
         */
        virtual GLGE::Version getVersion() const noexcept override;

        /**
         * @brief Create a Instance
         * 
         * @param instance a pointer to the instance to create the backend instance for
         * @return `Reference<GLGE::Graphic::Backend::Instance>` a pointer to the new backend instance
         */
        virtual Reference<GLGE::Graphic::Backend::Graphic::Instance> createInstance( GLGE::Graphic::Instance* instance) override;
        /**
         * @brief Create a Window
         * 
         * @param window a pointer to the window to create the backend window for
         * @return `Reference<GLGE::Graphic::Backend::Window>` a pointer to the new backend window
         */
        virtual Reference<GLGE::Graphic::Backend::Graphic::Window> createWindow(GLGE::Graphic::Window* window) override;

        /**
         * @brief Create a Command Buffer
         * 
         * @param renderPipeline the render pipeline that owns the command buffer
         * @return `Reference<GLGE::Graphic::Backend::Graphic::CommandBuffer>` a pointer to the new command buffer
         */
        virtual Reference<GLGE::Graphic::Backend::Graphic::CommandBuffer> createCommandBuffer(GLGE::Graphic::RenderPipeline* renderPipeline) override;

        /**
         * @brief Create a Sampler
         * 
         * @param sampler the initial state of the sampler
         * @param instance a pointer to the instance the sampler belongs to
         * @return `Reference<GLGE::Graphic::Backend::Graphic::Sampler>` a pointer to the new sampler
         */
        virtual Reference<GLGE::Graphic::Backend::Graphic::Sampler> createSampler([[maybe_unused]] const GLGE::Graphic::SamplerCPU& sampler, [[maybe_unused]] GLGE::Graphic::Backend::Graphic::Instance* instance) override;

        /**
         * @brief Create a Texture
         * 
         * @param size the size of the texture in pixels
         * @param mipCount the amount of mip levels the texture has
         * @param format the pixel format of the texture
         * @param instance a pointer to the instance the texture belongs to
         * @return `Reference<GLGE::Graphic::Backend::Graphic::Texture>` a pointer to the new texture
         */
        virtual Reference<GLGE::Graphic::Backend::Graphic::Texture> createTexture([[maybe_unused]] const uvec2& size, [[maybe_unused]] u16 mipCount, [[maybe_unused]] PixelFormat format, GLGE::Graphic::Backend::Graphic::Instance* instance) override;

        /**
         * @brief Create an Image
         * 
         * @param size the size of the image in pixels
         * @param format the pixel format of the image
         * @param samples the amount of samples per pixel
         * @param instance a pointer to the graphic instance the image will belong to
         * @return `Reference<GLGE::Graphic::Backend::Graphic::Image>` a pointer to the new image
         */
        virtual Reference<GLGE::Graphic::Backend::Graphic::Image> createImage([[maybe_unused]] const uvec2& size, [[maybe_unused]] PixelFormat format, [[maybe_unused]] u8 samples, GLGE::Graphic::Backend::Graphic::Instance* instance) override;

        /**
         * @brief Construct a new Framebuffer
         * 
         * @warning if a hard cap of the attachment count limit is surpassed this function will throw an exception in debug or clip in release
         * 
         * @param colorAttachmentCount the amount of color attachments to add
         * @param colorAttachments the color attachments to add
         * @param depthAttachmentCount the amount of depth / depth stencil attachments to add
         * @param depthAttachments the depth / depth stencil attachments to add
         * @param stencilAttachmentCount the amount of stencil attachments to add
         * @param stencilAttachments the stencil attachments to add
         * @param combinedDepthStencil `true` if depth and stencil buffer are combined, `false` if they are different
         * @param instance a pointer to the owning backend instance
         * @return `Reference<GLGE::Graphic::Backend::Graphic::Framebuffer>` a reference to the new framebuffer
         */
        virtual Reference<GLGE::Graphic::Backend::Graphic::Framebuffer> createFramebuffer([[maybe_unused]] u8 colorAttachmentCount, [[maybe_unused]] Reference<GLGE::Graphic::Backend::Graphic::Image> const* colorAttachments, 
                                                                                          [[maybe_unused]] u8 depthAttachmentCount, [[maybe_unused]] Reference<GLGE::Graphic::Backend::Graphic::Image> const* depthAttachments, 
                                                                                          [[maybe_unused]] u8 stencilAttachmentCount, [[maybe_unused]] Reference<GLGE::Graphic::Backend::Graphic::Image> const* stencilAttachments, 
                                                                                          [[maybe_unused]] bool combinedDepthStencil, GLGE::Graphic::Backend::Graphic::Instance* instance) override;

        /**
         * @brief Create a new Resource Set
         * 
         * @param set a pointer to the frontend resource set to construct from
         * @return `Reference<GLGE::Graphic::Backend::Graphic::ResourceSet>` a reference to the new resource set
         */
        virtual Reference<GLGE::Graphic::Backend::Graphic::ResourceSet> createResourceSet([[maybe_unused]]GLGE::Graphic::ResourceSet* set) override;

        /**
         * @brief Create a new Shader
         * 
         * @param frontend a pointer to the frontend shader
         * @return `Reference<GLGE::Graphic::Backend::Graphic::Shader>` a reference to the new shader
         */
        virtual Reference<GLGE::Graphic::Backend::Graphic::Shader> createShader([[maybe_unused]] GLGE::Graphic::Shader* frontend) override;

        /**
         * @brief Create a new Buffer
         * 
         * @param type the type of the buffer to create
         * @param initial the initial data for the buffer, may be `nullptr`
         * @param size the initial size of the buffer
         * @param usage the intended usage of the buffer
         * @param instance a pointer to the backend instance that will own the buffer
         * @return `Reference<GLGE::Graphic::Backend::Graphic::Buffer>` a reference to the new buffer backend
         */
        virtual Reference<GLGE::Graphic::Backend::Graphic::Buffer> createBuffer([[maybe_unused]] GLGE::Graphic::Backend::Graphic::Buffer::Type type, [[maybe_unused]] const void* initial, [[maybe_unused]] size_t size, [[maybe_unused]] GLGE::Graphic::Backend::Graphic::Buffer::Usage usage, GLGE::Graphic::Backend::Graphic::Instance* instance) override;

        /**
         * @brief Create a Sampled Texture
         * 
         * @param texture a reference to the texture backend to sample
         * @param sampler a reference to the sampler to use
         * @return `Reference<GLGE::Graphic::Backend::Graphic::SampledTexture>` a reference to the sampled texture
         */
        virtual Reference<GLGE::Graphic::Backend::Graphic::SampledTexture> createSampledTexture([[maybe_unused]] const Reference<GLGE::Graphic::Backend::Graphic::Texture>& texture, [[maybe_unused]] const Reference<GLGE::Graphic::Backend::Graphic::Sampler>& sampler) override;

        /**
         * @brief Create a Sampled Texture
         * 
         * @param image a reference to the image backend to sample
         * @param sampler a reference to the sampler to use
         * @return `Reference<GLGE::Graphic::Backend::Graphic::SampledTexture>` a reference to the sampled texture
         */
        virtual Reference<GLGE::Graphic::Backend::Graphic::SampledTexture> createSampledTexture([[maybe_unused]] const Reference<GLGE::Graphic::Backend::Graphic::Image>& image, [[maybe_unused]] const Reference<GLGE::Graphic::Backend::Graphic::Sampler>& sampler) override;

        /**
         * @brief Create a Mesh Pool
         * 
         * @param instance a pointer to the instance that will own the mesh pool
         * 
         * @return `Reference<GLGE::Graphic::Backend::Graphic::MeshPool>` a reference to the new mesh pool
         */
        virtual Reference<GLGE::Graphic::Backend::Graphic::MeshPool> createMeshPool(GLGE::Graphic::Instance* instance) override;

        /**
         * @brief Create a Vertex Layout
         * 
         * @param ptr the pointer to the beginning of a continues array of attributes
         * @param size the amount of attributes in the array
         * @param stride the byte size of a single vertex
         * @param pool a reference to the mesh pool the layout attaches to
         * @return `Reference<GLGE::Graphic::Backend::Graphic::VertexLayout>` a reference to the new vertex layout
         */
        virtual Reference<GLGE::Graphic::Backend::Graphic::VertexLayout> createVertexLayout(const VertexAttribute* ptr, size_t size, size_t stride, Reference<GLGE::Graphic::Backend::Graphic::MeshPool> pool) override;

        /**
         * @brief Create a Material
         * 
         * @param shader a reference to the used shader backend
         * @param layout a reference to the used vertex layout
         * @param fbuff a reference to the framebuffer the material will use
         * @param cullMode the mode for the backface culling
         * @param depthMode the mode to use for depth compares
         * @param depthWrite `true` to enable depth writing, `false` to disable depth writing
         * @return `Reference<GLGE::Graphic::Backend::Graphic::Material>` a reference to the new material
         */
        virtual Reference<GLGE::Graphic::Backend::Graphic::Material> createMaterial(Reference<GLGE::Graphic::Backend::Graphic::Shader> shader, Reference<GLGE::Graphic::Backend::Graphic::VertexLayout> layout, Reference<GLGE::Graphic::Backend::Graphic::Framebuffer> fbuff, GLGE::Graphic::Backend::Graphic::Material::CullMode cullMode, GLGE::Graphic::Backend::Graphic::Material::DepthMode depthMode, bool depthWrite) override;

        /**
         * @brief Create a Material
         * 
         * @param world a reference to the world to render (reference -> cannot be null)
         * @param camera store a pointer to the object to use as a camera, this can be null
         * @param target the target to adapt to
         * @return `Reference<GLGE::Graphic::Backend::Graphic::Renderer>` a reference to the new renderer
         */
        virtual Reference<GLGE::Graphic::Backend::Graphic::Renderer> createRenderer(World& world, Object* camera, RenderTarget target) override;

        /**
         * @brief get the API used by the backend
         * 
         * @return `GLGE::Graphic::GraphicAPI` the graphic API that is used by the backend
         */
        virtual GLGE::Graphic::GraphicAPI getAPI() const noexcept override;
        
        /**
         * @brief get the version of the API used by the backend
         * 
         * @return `GLGE::Version` a GLGE version type that holds the API version
         */
        virtual GLGE::Version getAPIVersion() const noexcept override;

        /**
         * @brief initialize the description
         * 
         * This function is automatically called by the instance after the graphic instance is created
         */
        virtual void initialize() override;
        
    };

}
}
}

#endif
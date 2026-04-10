/**
 * @file Graphic.h
 * @author DM8AT
 * @brief include all graphics related stuff
 * @version 0.1
 * @date 2025-12-21
 * 
 * @copyright Copyright (c) 2025
 * 
 */
//header guard
#ifndef _GLGE_GRAPHIC_
#define _GLGE_GRAPHIC_

//include the instance
#include "Instance.h"
//include windows
#include "Window.h"
//include displays
#include "Display.h"
//include render pipelines
#include "RenderPipeline.h"

//add CPU images
#include "ImageCPU.h"
//add CPU textures
#include "TextureCPU.h"

//add samplers
#include "Sampler.h"
//add images
#include "Image.h"
//add textures
#include "Texture.h"
//add framebuffers
#include "Framebuffer.h"
//add resource sets
#include "ResourceSet.h"
//add shader
#include "Shader.h"
//add sampled images
#include "SampledTexture.h"
//add vertex layouts
#include "VertexLayout.h"
//add meshes
#include "Mesh.h"
//add render targets
#include "RenderTarget.h"

//add raw buffers
#include "Buffer.h"
//add structured buffers
#include "StructuredBuffer.h"

//include the assets
#include "Assets/ImageCPUAsset.h"
#include "Assets/TextureCPUAsset.h"
#include "Assets/MeshAsset.h"

//include the backend stuff
#include "Backend/Graphics/BackendDescription.h"

//include the builtin descriptions
#include "Backend/Builtin/Graphics/OpenGL/Description.h"
#include "Backend/Builtin/Video/SDL3/Description.h"

//add materials
#include "Material.h"

//add the graphic related systems
#include "Renderer.h"

//add the graphic related components
#include "Components.h"

#endif
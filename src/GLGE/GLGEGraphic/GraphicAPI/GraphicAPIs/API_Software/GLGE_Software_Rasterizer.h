/**
 * @file GLGE_Software_Rasterizer.h
 * @author JuNi4
 * @brief Rasterizes triangles
 * @version 0.1
 * @date 2025-07-13
 * 
 * @copyright Copyright (c) 2025
 * 
 */

#include "../../../GLGERenderMesh.h"
#include "../../GLGEGraphicAPI_MemoryArena.h"
#include "GLGE_Software_Framebuffer.h"

#ifndef GLGE_SOFTWARE_RASTERIZER
#define GLGE_SOFTWARE_RASTERIZER

/**
 * @brief Renders any given RenderMesh
 * 
 * @param fbuff The frame buffer that will be rendered into
 * @param mesh The mesh that will be rendered. hopefully if everything works but it probably wont
 */
void renderMesh(Software_Framebuffer* fbuff, RenderMesh* mesh);

#endif //end of include guard
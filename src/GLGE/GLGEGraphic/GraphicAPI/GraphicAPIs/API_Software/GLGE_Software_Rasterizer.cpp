/**
 * @file GLGE_Software_Rasterizer.cpp
 * @author JuNi4
 * @brief Rasterizes triangles
 * @version 0.1
 * @date 2025-07-13
 * 
 * @copyright Copyright (c) 2025
 * 
 */

#include "GLGE_Software_Rasterizer.h"

#include <iostream>
#include <SDL2/SDL.h>

#include "GLGE_Software_Texture.h"
#include "../../../GLGEFramebuffer.h"

bool insideTriangle(vec2 p, vec2 a, vec2 b, vec2 c, vec3* weights, bool backfaceCulling)
{
    // compute the edges of the triangle and rotate them by 90 degrees
    vec2 ab = b - a;
    ab = vec2(ab.y, -ab.x);
    vec2 bc = c - b;
    bc = vec2(bc.y, -bc.x);
    vec2 ca = a - c;
    ca = vec2(ca.y, -ca.x);

    // project the point to test on the axis perpandicular to the triangle's edge
    float pab = dot(p - a, ab);
    float pbc = dot(p - b, bc);
    float pca = dot(p - c, ca);

    // for each axis, check if the point is inside the triangle
    bool inTri;
    if (backfaceCulling)
    {
        inTri = (pab >= 0.) && (pbc >= 0.) && (pca >= 0.);
    }
    else
    {
        inTri = ((pab >= 0.) == (pbc >= 0.)) && ((pbc >= 0.) == (pca >= 0.));
    }

    float totalArea = pab + pbc + pca;
    float invAreaSum = 1 / totalArea;
    weights->x = pbc * invAreaSum;
    weights->y = pca * invAreaSum;
    weights->z = pab * invAreaSum;

    return inTri && totalArea > 0;
}

typedef struct s_SoftTriangle
{
    SimpleVertex a;
    SimpleVertex b;
    SimpleVertex c;
} SoftTriangle;

void renderTriangle(Software_Framebuffer* fbuff, SoftTriangle tri)
{
    #define VEC322(V) vec2(V.x, V.y)

    //extract surface from frame buffer
    SDL_Surface* surface = ((SDL_Surface*)(((Software_Texture*)(fbuff->getFrameBuffer()->getColorAttatchment(0)->getGraphicTexture()))->getData()));
    if (SDL_LockSurface(surface)) { return; }

    uvec2 target_size = fbuff->getFrameBuffer()->getSize();

    //world space to screen space transformation
    vec3 transform = vec3(target_size.x / 2.f, target_size.y / 2.f, 1);
    vec3 center(target_size.x / 2.f, target_size.y / 2.f, 0);

    vec3 p0 = (tri.a.pos * transform) + center;
    vec3 p1 = (tri.b.pos * transform) + center;
    vec3 p2 = (tri.c.pos * transform) + center;

    int minX = center.x;
    int maxX = -center.x;
    int minY = center.y;
    int maxY = -center.y;

    // check for lowest values P0
    if (p0.x < minX) { minX = p0.x; }
    if (p0.x > maxX) { maxX = p0.x; }
    if (p0.y < minY) { minY = p0.y; }
    if (p0.y > maxY) { maxY = p0.y; }
    // check for lowest values P1
    if (p1.x < minX) { minX = p1.x; }
    if (p1.x > maxX) { maxX = p1.x; }
    if (p1.y < minY) { minY = p1.y; }
    if (p1.y > maxY) { maxY = p1.y; }
    // check for lowest values P2
    if (p2.x < minX) { minX = p2.x; }
    if (p2.x > maxX) { maxX = p2.x; }
    if (p2.y < minY) { minY = p2.y; }
    if (p2.y > maxY) { maxY = p2.y; }

    vec3 barycentric;
    
    //iterate over the area of the triangle
    for (int x = minX; x < (maxX +1); ++x)
    {
        for (int y = minY; y < (maxY +1); ++y)
        {
            //check if pixel is inside triangle
            if (!insideTriangle(vec2(x,y), VEC322(p0), VEC322(p1), VEC322(p2), &barycentric, false))
            { continue; }
            
            vec2 uv = tri.a.tex * barycentric.x + tri.b.tex * barycentric.y + tri.c.tex * barycentric.z;

            //set the pixel
            *((uint32_t*)surface->pixels + x + (target_size.y - y) * target_size.x) = SDL_MapRGBA(surface->format, uv.x * 255, uv.y * 255, 0, 255);
        }
    }

    SDL_UnlockSurface(surface);
}

void renderMesh(Software_Framebuffer* fbuff, RenderMesh* mesh)
{
    //spaghetti, i hope you like it, cuz there is way more where this came from... (i don't)
    GraphicMemoryArena* vert = ((GraphicMemoryArena*)mesh->getMaterial()->getVertexBuffer().getMemoryArena());
    GraphicMemoryArena* indi = ((GraphicMemoryArena*)mesh->getMaterial()->getIndexBuffer().getMemoryArena()); //(not indianer jones but, sadly, indices)

    //sanity check, if the vertex layout is the same size as the one used here
    if (mesh->getMesh()->getVertexLayout().getVertexSize() != sizeof(SimpleVertex))
    {
        mesh->getMaterial()->getInstance()->log("A mesh contained an unsupported vertex layout. Good luck.", MESSAGE_TYPE_CRITICAL_WARNING);
    }

    //get the actual data of the vertecies and indices
    SimpleVertex* verteces = (SimpleVertex*)vert->getRaw();
    idx_Triangle* triangles = (idx_Triangle*)indi->getRaw();

    size_t triangle_count = mesh->getIndexPointer().size / sizeof(idx_Triangle);

    //iterate over all triangles
    for (size_t i = 0; i < triangle_count; ++i)
    {
        idx_Triangle idx_tri = triangles[i];
        //fetch all vertecies
        SoftTriangle tri = {verteces[idx_tri.a], verteces[idx_tri.b], verteces[idx_tri.c]};

        renderTriangle(fbuff, tri);
    }
}

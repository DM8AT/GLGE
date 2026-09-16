/**
 * @file TextureCPU.cpp
 * @author DM8AT
 * @brief implement the CPU texture
 * @version 0.1
 * @date 2026-01-04
 * 
 * @copyright Copyright (c) 2026
 * 
 */
//add CPU textures
#include "Graphic/TextureCPU.h"

//add exceptions
#include "Core/Exception.h"

//use the library namespace
using namespace GLGE::Graphic;

TextureCPU::TextureCPU(const ImageCPU& image, u8 mipCount) {
    GLGE_PROFILER_SCOPE_NAMED("GLGE::Graphic::TextureCPU::TextureCPU")

    //store the valid max mip count
    u8 maxMips = std::floor(std::min<f32>(glm::log2(f32(image.getSize().x)), glm::log2(f32(image.getSize().y)))) + 1;
    //check for the automatic key
    if (mipCount == 0) {
        //automatically compute
        mipCount = maxMips;
    }
    //sanity check the mip count
    if (mipCount > maxMips) {
        //in debug throw an error
        #if GLGE_DEBUG
        throw GLGE::Exception("Tried to create to many mips", "GLGE::Graphic::TextureCPU::TextureCPU");
        #endif
        //set the mip count to the maximum mip count
        mipCount = maxMips;
    }

    //store the mip count
    m_mipCount = mipCount;

    //just copy over mip 0
    m_imgs[0] = image;
    //store the current to size
    uvec2 size = m_imgs[0].getSize() >> 1u;
    //iterate over all lower mips
    for (u8 i = 0; i < (m_mipCount-1); ++i) {
        //store from which image to which image to copy
        u8 from = i;
        u8 to = from+1;

        //create the image
        m_imgs[to] = ImageCPU(nullptr, m_imgs[0].getFormat(), size);

        //iterate over all pixel in the to image
        for (u32 y = 0; y < m_imgs[to].getSize().y; ++y) {
            for (u32 x = 0; x < m_imgs[to].getSize().x; ++x) {
                //compute the texels to take taps for on the bigger image
                uvec2 p_t0 = {x*2u + 0, y*2u + 0};
                uvec2 p_t1 = {x*2u + 0, y*2u + 1};
                uvec2 p_t2 = {x*2u + 1, y*2u + 0};
                uvec2 p_t3 = {x*2u + 1, y*2u + 1};
                //take 4 taps on the bigger image
                vec4 t0 = m_imgs[from].readTexelRGBA(p_t0);
                vec4 t1 = m_imgs[from].readTexelRGBA(p_t1);
                vec4 t2 = m_imgs[from].readTexelRGBA(p_t2);
                vec4 t3 = m_imgs[from].readTexelRGBA(p_t3);
                //average the 4 taps
                vec4 col = (t0 + t1 + t2 + t3) * 0.25f;
                //store the color in the mip image
                m_imgs[to].writeTexelRGBA(uvec2(x,y), col);
            }
        }

        //half the resolution
        size >>= 1u;
    }
}

TextureCPU::TextureCPU(const ImageCPU* mips, u8 mipCount) 
 : m_mipCount(mipCount)
{
    GLGE_PROFILER_SCOPE_NAMED("GLGE::Graphic::TextureCPU::TextureCPU")

    //iterate over all mip levels and copy the mips over
    for (size_t i = 0; i < m_mipCount; ++i) 
    {m_imgs[i] = mips[i];}
}
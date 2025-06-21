/**
 * @file GLGEFramebuffer.cpp
 * @author DM8AT
 * @brief implement the high level framebuffer for GLGE
 * @version 0.1
 * @date 2025-04-22
 * 
 * @copyright Copyright (c) 2025
 * 
 */

//include the framebuffer
#include "GLGEFramebuffer.h"
//include the APIs
#include "GraphicAPI/GraphicAPIs/GLGE_AllAPIs.h"

void Framebuffer::create(Texture** colorAttatchments, uint64_t colorAttatchmentCount, Texture* depthAttatchment)
{
    //store the size of the first texture
    const uvec2& size = (colorAttatchments && (colorAttatchmentCount > 0)) ? colorAttatchments[0]->getSize() : depthAttatchment->getSize();
    //check for color attatchments
    if ((colorAttatchments) && (colorAttatchmentCount > 0))
    {
        //iterate over all color attatchments
        for (uint64_t i = 0; i < colorAttatchmentCount; ++i)
        {
            //validate the texture
            if (colorAttatchments[i]->getPurpose() != TEXTURE_PURPOSE_RENDER)
            {
                //print an error
                m_instance->log("Failed to validate a color attatchment for a framebuffer", MESSAGE_TYPE_ERROR);
                return;
            }
            //check if the size is correct
            if ((colorAttatchments[i]->getSize().x != size.x) || (colorAttatchments[i]->getSize().y != size.y))
            {
                //print an error
                m_instance->log("Failed to validate the size of a color attatchment for a framebuffer: size missmatch. All attatchments must be the same size. ", MESSAGE_TYPE_ERROR);
                return;
            }
        }

        //store the color attatchments
        m_colorAttatchments.resize(colorAttatchmentCount);
        memcpy(m_colorAttatchments.data(), colorAttatchments, sizeof(*colorAttatchments)*colorAttatchmentCount);
    }
    else
    {
        //clear all color attatchments
        m_colorAttatchments.clear();
    }

    //validate the depth attatchment
    if (depthAttatchment)
    {
        //check the purpose
        if (depthAttatchment->getPurpose() != TEXTURE_PURPOSE_DEPTH)
        {
            //print an error
            m_instance->log("Failed to validate the depth attatchment for a framebuffer", MESSAGE_TYPE_ERROR);
            return;
        }

        //check the size
        if ((depthAttatchment->getSize().x != size.x) || (depthAttatchment->getSize().y != size.y))
        {
            //print an error
            m_instance->log("Failed to validate the depth attatchment for a framebuffer: size missmatch", MESSAGE_TYPE_ERROR);
            return;
        }
    }

    //store the depth attatchment
    m_depthAttatchment = depthAttatchment;

    //switch over the instance's API to create the correct low-level framebuffer
    switch (m_instance->getAPI())
    {
    case API_OPENGL_4_6:
        m_fbuff = new OGL4_6_Framebuffer(this, m_instance);
        break;
    
    default:
        break;
    }
    //create the whole framebuffer
    m_fbuff->onCreate();
}

void Framebuffer::create(uint64_t colorAttatchmentCount, bool alpha, bool hdr, bool depthAttatchment, const uvec2& size)
{
    //allocate the amount of color attatchments
    m_colorAttatchments.reserve(colorAttatchmentCount);
    //iterate over all color attatchments
    for (uint64_t i = 0; i < colorAttatchmentCount; ++i)
    {
        //create the color attatchment
        m_colorAttatchments.emplace_back(new Texture(TEXTURE_PURPOSE_RENDER, size, hdr, alpha, *m_instance));
    }

    //check if a depth attatchment should be created
    if (depthAttatchment)
    {
        //create the depth attatchment
        m_depthAttatchment = new Texture(TEXTURE_PURPOSE_DEPTH, size, 0, 0, *m_instance);
    }

    //switch over the instance's API to create the correct low-level framebuffer
    switch (m_instance->getAPI())
    {
    case API_OPENGL_4_6:
        m_fbuff = new OGL4_6_Framebuffer(this, m_instance);
        break;
    
    default:
        break;
    }
    //create the whole framebuffer
    m_fbuff->onCreate();
}

Framebuffer::~Framebuffer()
{
    //check if the framebuffer still exists
    if (!m_fbuff) {return;}

    //destroy the framebuffer
    m_fbuff->destroy();

    //if it still exists, delete the graphic framebuffer
    delete m_fbuff;
    m_fbuff = 0;
}


std::ostream& operator<<(std::ostream& os, const Framebuffer& fbuff) noexcept
{
    //add the main part
    os << "framebuffer{";
    //check for color attatchments
    if (fbuff.m_colorAttatchments.size() > 0)
    {
        //add the prefix
        os << "colorAttatchments: {\n";
        //iterate over all textures
        for (uint32_t i = 0; i < fbuff.m_colorAttatchments.size(); ++i)
        {
            //print the attatchment
            os << "    " << i << " : " << *fbuff.m_colorAttatchments[i] << ",\n";
        }
        //add the suffix
        os << "}";
        //check for a depth attatchment
        if (fbuff.m_depthAttatchment)
        {
            //add a comma
            os << ",\n";
        }
    }
    //check for a depth buffer
    if (fbuff.m_depthAttatchment)
    {
        //add a depth attatchment
        os << "depthAttatchment: " << *fbuff.m_depthAttatchment;
    }
    //add the ending and return
    return os << "}";
}

void Framebuffer::resize(const uvec2& newSize) noexcept
{
    //check if the framebuffer exists
    if (!m_fbuff) {return;}
    //update the size of the graphic framebuffer
    m_fbuff->resize(newSize);
}
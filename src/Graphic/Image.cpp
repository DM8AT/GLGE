/**
 * @file Image.cpp
 * @author DM8AT
 * @brief implement the image
 * @version 0.1
 * @date 2026-08-30
 * 
 * @copyright Copyright (c) 2026
 * 
 */
//add the header
#include "Graphic/Image.h"
//add sampled textures
#include "Graphic/SampledTexture.h"
//add resource sets
#include "Graphic/ResourceSet.h"

void GLGE::Graphic::Image::registerSampler(SampledTexture* st)
{attachInvalidator(*static_cast<CommandInvalidator*>(st));}

void GLGE::Graphic::Image::removeSampler(SampledTexture* st)
{detachInvalidator(*static_cast<CommandInvalidator*>(st));}

void GLGE::Graphic::Image::onBuildBinding(GLGE::Graphic::ResourceSet* set, u32 unit) 
{m_image->onBuildBinding(set, unit); set->attachTo(*m_image);}

void GLGE::Graphic::Image::onRemoveBinding(ResourceSet* set)
{m_image->onDropBinding(set); set->removeFrom(*m_image);}
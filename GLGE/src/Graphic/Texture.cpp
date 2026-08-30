/**
 * @file Texture.cpp
 * @author DM8AT
 * @brief implement the texture
 * @version 0.1
 * @date 2026-08-30
 * 
 * @copyright Copyright (c) 2026
 * 
 */
//add the header
#include "Graphic/Texture.h"
//add sampled textures
#include "Graphic/SampledTexture.h"

void GLGE::Graphic::Texture::registerSampler(SampledTexture* st)
{attachInvalidator(*static_cast<CommandInvalidator*>(st));}

void GLGE::Graphic::Texture::removeSampler(SampledTexture* st)
{detachInvalidator(*static_cast<CommandInvalidator*>(st));}
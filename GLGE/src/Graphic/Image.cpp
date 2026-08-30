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

void GLGE::Graphic::Image::registerSampler(SampledTexture* st)
{attachInvalidator(*static_cast<CommandInvalidator*>(st));}

void GLGE::Graphic::Image::removeSampler(SampledTexture* st)
{detachInvalidator(*static_cast<CommandInvalidator*>(st));}
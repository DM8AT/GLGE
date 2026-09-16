/**
 * @file Sampler.cpp
 * @author DM8AT
 * @brief implement the sampler
 * @version 0.1
 * @date 2026-08-30
 * 
 * @copyright Copyright (c) 2026
 * 
 */
//add the header
#include "Graphic/Sampler.h"
//add sampled textures
#include "Graphic/SampledTexture.h"

void GLGE::Graphic::Sampler::registerSampler(SampledTexture* st)
{attachInvalidator(*static_cast<CommandInvalidator*>(st));}

void GLGE::Graphic::Sampler::removeSampler(SampledTexture* st)
{detachInvalidator(*static_cast<CommandInvalidator*>(st));}
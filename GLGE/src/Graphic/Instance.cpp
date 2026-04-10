/**
 * @file Instance.cpp
 * @author DM8AT
 * @brief implement the functionality of a graphics instance
 * @version 0.1
 * @date 2025-12-27
 * 
 * @copyright Copyright (c) 2025
 * 
 */
//include the instance
#include "Graphic/Instance.h"

//use the libraries namespace
using namespace GLGE::Graphic;

void Instance::onInstanceSetting() {
    //create the internal video instance
    m_vInst = m_vDesc->createInstance(this, m_gDesc->getAPI(), m_gDesc->getAPIVersion());
    //create the internal graphic instance
    m_gInst = m_gDesc->createInstance(this);
}

Instance::Instance(GLGE::Graphic::Backend::Graphic::Description* graphicDescription, 
                   GLGE::Graphic::Backend::Video::Description*   videoDescription)
 : m_gDesc(graphicDescription), m_vDesc(videoDescription)
{
    //sanity check if the video and graphic API are compatable
    #if GLGE_DEBUG
    bool found = false;
    GLGE::Graphic::GraphicAPI gAPI = m_gDesc->getAPI();
    const auto& apis = m_vDesc->getSupportedAPIs();
    for (auto& api : apis)
    {if (api == gAPI) {found = true;}}
    //if they are not compatable throw an exception
    if (!found)
    {throw GLGE::Exception("Tried to create a graphic instance with an incompatible set of graphic and video API", "GLGE::Graphic::Instance::Instance");}
    #endif
}

Instance::~Instance() {
    //clean up the video instance
    delete m_vInst;
    m_vInst = nullptr;
}

void Instance::onUpdate() {
    //update the video instance
    m_vInst->onUpdate();
}

const GLGE::Graphic::DisplaySetup& Instance::getDisplaySetup() const noexcept
{return m_vInst->getDisplaySetup();}

void GLGE::Graphic::Instance::onGraphicBackendInit() {
    //create the mesh pool
    m_meshPool = m_gDesc->createMeshPool();
}

void GLGE::Graphic::Instance::onGraphicBackendDestroy() {
    //delete the mesh pool
    m_meshPool.~Reference();
}
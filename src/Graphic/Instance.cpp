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
#include "GLGE/Graphic/Instance.h"
//add windows
#include "GLGE/Graphic/Window.h"

//use the libraries namespace
using namespace GLGE::Graphic;

void Instance::onInstanceSetting() {
    //create the internal video instance
    m_vInst = m_vDesc->createInstance(this, m_gDesc->getAPI(), m_gDesc->getAPIVersion());
    //create the internal graphic instance
    m_gInst = m_gDesc->createInstance(this);

    //bind the graphic instance
    m_gInst->onBind();
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

void Instance::onMainUpdate() {
    //update all windows
    for (const auto& win : m_windows)
    {win->update();}
    //update the video instance
    m_vInst->onUpdate();
}

const GLGE::Graphic::DisplaySetup& Instance::getDisplaySetup() const noexcept
{return m_vInst->getDisplaySetup();}

void GLGE::Graphic::Instance::onGraphicBackendInit() {
    //create the mesh manager
    m_meshManager.emplace(this);
}

void GLGE::Graphic::Instance::onGraphicBackendDestroy() {
    //delete the mesh manager
    m_meshManager.reset();
}

void GLGE::Graphic::Instance::openURL(const std::string& url)
{m_vInst->openURL(url);}

std::vector<std::string> GLGE::Graphic::Instance::getClipboardTypes()
{return m_vInst->getClipboardTypes();}

std::vector<GLGE::u8> GLGE::Graphic::Instance::getClipboardData(const std::string& typeName) 
{return m_vInst->getClipboardData(typeName);}

void GLGE::Graphic::Instance::setClipboardData(const void* data, size_t size, const std::string& typeName) 
{m_vInst->setClipboardData(data, size, typeName);}

std::string GLGE::Graphic::Instance::getClipboardText() 
{return m_vInst->getClipboardText();}

void GLGE::Graphic::Instance::setClipboardText(const std::string& text) 
{m_vInst->setClipboardText(text);}

void GLGE::Graphic::Instance::hideCursor() 
{m_vInst->hideCursor();}

void GLGE::Graphic::Instance::showCursor() 
{m_vInst->showCursor();}

bool GLGE::Graphic::Instance::isCursorHidden() 
{return m_vInst->isCursorHidden();}

void GLGE::Graphic::Instance::openFileSelector(Pfn_SelectorCallback callback, bool allowMultiSelect, const std::vector<std::pair<std::string, std::string>>& filter, const std::filesystem::path& defaultLocation, GLGE::Graphic::Window* parent)
{m_vInst->openFileSelector(callback, allowMultiSelect, filter, defaultLocation, parent);}
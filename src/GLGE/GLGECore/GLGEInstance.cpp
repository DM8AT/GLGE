/**
 * @file GLGEInstance.cpp
 * @author DM8AT
 * @brief implement the GLGE Instance and related classes and functions
 * @version 0.1
 * @date 2025-04-12
 * 
 * @copyright Copyright (c) 2025
 * 
 */

//include the instance
#include "GLGEInstance.h"

//check if graphics is enabled
#if GLGE_INCLUDE_GRAPHICS
//include all API overloads
#include "../GLGEGraphic/GraphicAPI/GraphicAPIs/GLGE_AllAPIs.h"
//include windows
#include "../GLGEGraphic/GLGEWindow.h"
//include SDL2 and shared graphics
#include "../GLGEGraphic/GLGEGraphicShared.h"
#endif //Graphic section

//store the names for the enum values
const char* INST_ATTACHABLE_TYPE_NAMES[] = {
    "ATTACHMENT_TYPE_ASSET",
    "ATTACHMENT_TYPE_OBJECT",
    "ATTACHMENT_TYPE_WORLD",
    "ATTACHMENT_TYPE_BUFFER",
    "ATTACHMENT_TYPE_FRAMEBUFFER",
    "ATTACHMENT_TYPE_RENDER_MATERIAL",
    "ATTACHMENT_TYPE_RENDER_PIPELINE",
    "ATTACHMENT_TYPE_RENDER_VERTEX_LAYOUT",
    "ATTACHMENT_TYPE_TEXTURE",
    "ATTACHMENT_TYPE_WINDOW",
    "ATTACHMENT_TYPE_GRAPHIC_INSTANCE",
    "ATTACHMENT_TYPE_SHADER"
};

std::ostream& operator<<(std::ostream& os, const InstAttachableType& type) noexcept
{
    //print the requested element to the output
    return os << INST_ATTACHABLE_TYPE_NAMES[(uint32_t)type];
}

//only add the SDL limiter if graphics are included
//define the SDL2 limiter for the instance
GLGE_USE_IF_GRAPHICS(Limiter Instance::m_sdlLimiter = 60;)

/**
 * @brief this is a function for the update thread of an instance
 * 
 * @param instance a reference to the instance
 */
void instance_update_thread(Instance* instance)
{
    //start the limiter
    instance->updateLimiter().start();
    //loop while the instance is active
    while (instance->isActive())
    {
        //iterate over all elements attatched to the instance
        for (size_t i = 0; i < instance->getAttatchedElements().size();)
        {
            //update the element
            if (instance->getAttatchedElements()[i]->onUpdate())
            {
                //if the element still exists, increment the count
                ++i;
            }
        }
        //end the limiter
        instance->updateLimiter().endTick();
    }
}

Instance::Instance(std::string_view name, APIs api) : m_name(name), m_api(api)
{
    //create the instance update thread
    m_updateThread = std::thread(instance_update_thread, this);

    //check for graphic APIs
    #if GLGE_INCLUDE_GRAPHICS

    //add the keyboard layer
    m_globalLayers.addLayer(new KeyboardEventLayer(this));
    //add the mouse layer
    m_globalLayers.addLayer(new MouseEventLayer(this));

    #endif //end oc graphic specific section
}

Instance::~Instance() noexcept
{
    //say that the instance is no longer active
    m_active = false;
    //join the update thread
    m_updateThread.join();
    //iterate over all attatched elements
    while (m_elements.size() > 0)
    {
        //cash the element pointer
        InstAttachableClass* ptr = m_elements[0];
        //destroy the element
        delete m_elements[0];
        //erase the element, if it didn't do it itself
        if (m_elements.size() > 0) { if (m_elements[0] == ptr) { m_elements.erase(m_elements.begin()); } }
    }
    //check if a logger exists
    if (m_logger)
    {
        //print the contents of the logger
        m_logger->printAll();
        //delete the logger
        delete m_logger;
        //set the logger to 0
        m_logger = 0;
    }
}


void Instance::addElement(InstAttachableClass* element) noexcept
{
    //print a debug info
    GLGE_DEBUG_WRAPPER(
        std::stringstream stream;
        stream << "Adding Instance attatchable of type " << INST_ATTACHABLE_TYPE_NAMES[(uint32_t)element->getType()] << 
                  " named \"" + element->getName() << "\" to instance named \"" << m_name << "\": " << *element;
        logDebug(stream, MESSAGE_TYPE_DEBUG);
    )
    //add the instance
    m_elements.emplace_back(element);
}

void Instance::removeElement(InstAttachableClass* element) noexcept
{
    //print a debug info
    GLGE_DEBUG_WRAPPER(
        std::stringstream stream;
        stream << "Removing Instance attatchable of type " << INST_ATTACHABLE_TYPE_NAMES[(uint32_t)element->getType()] << 
                  " named \"" << element->getName() << "\" from instance named \"" << m_name << "\"";
        logDebug(stream, MESSAGE_TYPE_DEBUG);
    )
    //search the pointer
    auto pos = std::find(m_elements.begin(), m_elements.end(), element);
    //check if the element does not exist
    if (pos == m_elements.end())
    {
        //print an error and return
        log("Failed to find the requested element", MESSAGE_TYPE_ERROR);
        return;
    }
    //erase the element
    m_elements.erase(pos);
}

//check if graphics is enabled
#if GLGE_INCLUDE_GRAPHICS

void Instance::initGraphicAPI(Window* window)
{
    //check if the graphic api is initalized
    if (m_gInstance) {return;}

    //store a temporary pointer
    void* tmp = 0;
    //switch over the graphic api
    if (m_api == API_OPENGL_4_6)
    {
        //set the OpenGL attributes
        SDL_GL_SetAttribute(SDL_GL_CONTEXT_MAJOR_VERSION, 3);
        SDL_GL_SetAttribute(SDL_GL_CONTEXT_MINOR_VERSION, 3);
        SDL_GL_SetAttribute(SDL_GL_DOUBLEBUFFER, SDL_TRUE);
        SDL_GL_SetSwapInterval(0);
        //create a OpenGL context
        tmp = SDL_GL_CreateContext((SDL_Window*)window->getSDL2Window());
        //bind no context
        SDL_GL_MakeCurrent((SDL_Window*)window->getSDL2Window(), 0);
        //create a new OpenGL 4.6 instance
        m_gInstance = new OGL4_6_Instance(tmp, this);

        //stop the function
        return;
    }
    
    //check if the API is marked as supported
    if (GLGE_C_FUNC(getAPISupport(m_api)))
    {
        //if it is supported, log that the API is supported but no instance was implemented (WHAT?)
        m_logger->log("Graphic API is marked as supported, but no instance could be created. Are you working on an implementation?", MESSAGE_TYPE_FATAL_ERROR);
    }
    //log that the API is simply not supported
    m_logger->log("Using a not supported graphic API", MESSAGE_TYPE_FATAL_ERROR);
}

void Instance::closeGraphiAPI()
{
    //check if the graphic api is not initalized
    if (!m_gInstance) {return;}

    //delete the graphic instance
    m_gInstance->destroy();
    delete m_gInstance;
    //set the graphic instance to 0
    m_gInstance = 0;
}

void Instance::syncGraphicSetup() const noexcept
{
    //if no graphic instance exists, stop
    if (!m_gInstance) {return;}
    //synchronize with the graphic instance
    while (!m_gInstance->isSetupDone()) {std::this_thread::sleep_for(std::chrono::microseconds(10));}
}



Buffer* Instance::getTextureBuffer() const noexcept
{
    //return the texture buffer
    return m_gInstance->getTextureBuffer();
}

Buffer* Instance::getImageBuffer() const noexcept
{
    //return the image buffer
    return m_gInstance->getImageBuffer();
}

Buffer* Instance::getCameraBuffer() const noexcept
{
    //return the camera buffer
    return m_gInstance->getCameraBuffer();
}

Buffer* Instance::getObjectTransformBuffer() const noexcept
{
    //return the object transform buffer
    return m_gInstance->getObjectTransfBuffer();
}

void Instance::setMouseCursorShown(bool shown) noexcept
{
    //set if the cursor is shown
    SDL_ShowCursor(shown ? SDL_ENABLE : SDL_DISABLE);
}

void Instance::captureMouseByWindow(const Window* window) noexcept
{
    //store the window that now owns the mouse
    m_hasMouseCaptured = (Window*)window;

    //check if the window is 0 (releases the mouse)
    if (window)
    {
        //raise the requested window to initalize the mouse capture
        SDL_RaiseWindow((SDL_Window*)m_hasMouseCaptured->getSDL2Window());
        //if not, capture the mouse with the window
        SDL_SetRelativeMouseMode(SDL_TRUE);
    }
    else
    {
        //release the mouse
        SDL_SetRelativeMouseMode(SDL_FALSE);
    }
}

void Instance::setCapturedCursorVisible(bool visible) const noexcept
{
    //set if the cursor is visible
    SDL_SetHint("SDL_HINT_MOUSE_RELATIVE_CURSOR_VISIBLE", visible ? "1" : "0");
}

void Instance::setCapturedCursorCenter(bool center) const noexcept
{
    //set if the window gets centerd
    SDL_SetHint("SDL_HINT_MOUSE_RELATIVE_MODE_CENTER", center ? "1" : "0");
}

#endif //Graphic section

InstAttachableClass* Instance::getNamed(const std::string& name) const noexcept
{
    //iterate over all instance attatchables and check if they have the name
    for (size_t i = 0; i < m_elements.size(); ++i)
    {
        //return the current element if it has the requested name
        if (m_elements[i]->getName() == name)
        {return m_elements[i];}
    }
    //if the element was not found, return 0
    return 0;
}
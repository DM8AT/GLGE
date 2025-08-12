/**
 * @file GLGEWindow.cpp
 * @author DM8AT
 * @brief implement the window class
 * @version 0.1
 * @date 2025-04-12
 * 
 * @copyright Copyright (c) 2025
 * 
 */

//include the window class
#include "GLGEWindow.h"
//include the shared stuff
#include "GLGEGraphicShared.h"

//include SLD2
#include "GLGEGraphicSDL.cpp"
//incldue all grpahic APIs
#include "GraphicAPI/GraphicAPIs/GLGE_AllAPIs.h"

//include the icon
#include "GLGEWindowIcon.h"

void Window::open(std::string_view name, const uvec2& size, const uvec2& pos, const WindowSettings& settings, Instance& instance) noexcept
{
    //check if the window is not open
    if (isOpen())
    {
        //if it is, print an error
        instance.log("Failed to create a new window because the window was allready open", MESSAGE_TYPE_ERROR);
        return;
    }

    //store the inputs
    m_name = name;
    m_size = size;
    m_pos = pos;
    m_instance = &instance;
    m_settings = settings;
    //add the default layer stack to the window
    m_layerStack.addLayer(new WindowEventLayer(this));

    //print some debug info
    GLGE_DEBUG_WRAPPER(
        //store the stream to print
        std::stringstream stream;
        //fill the stream
        stream << "Started creation of a new window with name \"" << m_name << "\" and size " << m_size;
        //log that a new window was created
        m_instance->logDebug(Message(stream, MESSAGE_TYPE_DEBUG));
    )

    //store if this was the first window
    bool first = false;
    //check if SDL2 is initalized
    if (!__glge_sdl_2_thread)
    {
        //initalize SDL2
        if (SDL_InitSubSystem(SDL_INIT_EVERYTHING) < 0)
        {
            //print an error
            m_instance->log("Failed to initalize SDL2", MESSAGE_TYPE_FATAL_ERROR);
            //stop the function
            return;
        }

        //print a debug message
        GLGE_DEBUG_WRAPPER(
            //log a debug message
            m_instance->logDebug("Initalized SLD2", MESSAGE_TYPE_DEBUG);
        )

        //say that the SDL2 thread should be open
        __glge_sdl_thread_open = true;
        //create a new thread. Bind it to the logger of the instance
        __glge_sdl_2_thread = new std::thread(SDL_Main_Thread, m_instance->getLogger());
        //create the counter to count how many windows are open
        __glge_all_window_count = new uint32_t(0);
        //create a vector for all instances
        __glge_all_instances = new std::vector<Instance*>;
        //store that this is the first window
        first = true;
    }

    //store the window parameters
    SDL_WindowFlags flags = (SDL_WindowFlags)0;
    //check if the winow should be borderless
    flags = (SDL_WindowFlags)(flags | (m_settings.borderless ? SDL_WINDOW_BORDERLESS : (SDL_WindowFlags)0));
    //check if the winow should be in fullscreen
    flags = (SDL_WindowFlags)(flags | (m_settings.fullscreen ? SDL_WINDOW_FULLSCREEN : (SDL_WindowFlags)0));
    //check if the winow should be resizable
    flags = (SDL_WindowFlags)(flags | (m_settings.resizable ? SDL_WINDOW_RESIZABLE : (SDL_WindowFlags)0));
    //check if the winow should be minimized
    flags = (SDL_WindowFlags)(flags | (m_settings.minimized ? SDL_WINDOW_MINIMIZED : (SDL_WindowFlags)0));
    //check if the winow should be maximized
    flags = (SDL_WindowFlags)(flags | (m_settings.maximized ? SDL_WINDOW_MAXIMIZED : (SDL_WindowFlags)0));
    //check if the winow should have the mouse grabbed
    flags = (SDL_WindowFlags)(flags | (m_settings.mouseGrabbed ? SDL_WINDOW_MOUSE_GRABBED : (SDL_WindowFlags)0));
    //check if the winow should have the keyboard grabbed
    flags = (SDL_WindowFlags)(flags | (m_settings.inputGrabbed ? SDL_WINDOW_INPUT_GRABBED : (SDL_WindowFlags)0));
    //check if the winow should have the mouse focused
    flags = (SDL_WindowFlags)(flags | (m_settings.mouseFocused ? SDL_WINDOW_MOUSE_FOCUS : (SDL_WindowFlags)0));
    //check if the winow should have the keyboard focused
    flags = (SDL_WindowFlags)(flags | (m_settings.inputFocused ? SDL_WINDOW_INPUT_FOCUS : (SDL_WindowFlags)0));
    //check if the winow should be always on top
    flags = (SDL_WindowFlags)(flags | (m_settings.alwaysOnTop ? SDL_WINDOW_ALWAYS_ON_TOP : (SDL_WindowFlags)0));

    //add the correct flag for the Graphic API the instance uses
    switch (m_instance->getAPI())
    {
    //add flags for OpenGL
    case API_OPENGL_4_6:
    case API_OPENGL_4_6_RT:
        flags = (SDL_WindowFlags)(flags | SDL_WINDOW_OPENGL);

        //setup OpenGL parameters
        //disable all context flags
        SDL_GL_SetAttribute(SDL_GL_CONTEXT_FLAGS, 0);
        //set the OpenGL version to 4.6
        SDL_GL_SetAttribute(SDL_GL_CONTEXT_MAJOR_VERSION, 4);
        SDL_GL_SetAttribute(SDL_GL_CONTEXT_MINOR_VERSION, 6);
        //specify that the context is a core context
        SDL_GL_SetAttribute(SDL_GL_CONTEXT_PROFILE_MASK, SDL_GL_CONTEXT_PROFILE_CORE);
        //specify some default parameters for the window's context
        SDL_GL_SetAttribute(SDL_GL_DOUBLEBUFFER, 1);
        SDL_GL_SetAttribute(SDL_GL_DEPTH_SIZE, 24);
        SDL_GL_SetAttribute(SDL_GL_STENCIL_SIZE, 8);
        break;
    
    //add flags for Vulkan
    case API_VULKAN_1_2_DEFAULT:
    case API_VULKAN_1_2_INDIRECT:
    case API_VULKAN_1_2_RT:
        flags = (SDL_WindowFlags)(flags | SDL_WINDOW_VULKAN);
        break;
        
    //for software, add no flags
    default:
        break;
    }

    //create the window
    m_window = SDL_CreateWindow(m_name.data(), m_pos.x, m_pos.y, m_size.x, m_size.y, flags);
    //check if the window was created successfully
    if (!m_window)
    {
        //print an error
        m_instance->log("Failed to create a window", MESSAGE_TYPE_FATAL_ERROR);
        //stop the function
        return;
    }
    //check if this is the first window
    if (first)
    {
        //initalize the graphic API
        m_instance->initGraphicAPI(this);
    }

    //store the window mapping
    __glge_all_windows_sdl[SDL_GetWindowID((SDL_Window*)m_window)] = this;
    //increment the amount of existing windows
    ++(*__glge_all_window_count);

    //switch over the graphic APIs
    switch (m_instance->getAPI())
    {
    case API_OPENGL_4_6:
        m_gWindow = new OGL4_6_Window(this, m_instance->getGraphicInstance());
        break;
    
    default:
    {
        //log the missing window creation as a fatal error
        std::stringstream stream;
        stream << "No overload for API " << m_instance->getAPI() << " for a window was implemented";
        m_instance->log(stream, MESSAGE_TYPE_FATAL_ERROR);
        break;
    }

    }

    //set the default window icon

    //create the window icon
    setWindowIcon(new Texture((void*)gimp_image.pixel_data, TEXTURE_PURPOSE_CPU_ONLY, uvec2(gimp_image.width, gimp_image.height), 
                              false, (gimp_image.bytes_per_pixel == 4), *m_instance));
    //say that the icon was not external
    m_customIcon = false;
}

void Window::close() noexcept
{
    //if the window is not open, stop
    if (!isOpen()) {return;}

    //check for a graphic window
    if (m_gWindow)
    {
        //close the graphic window
        delete m_gWindow;
        m_gWindow = 0;
    }

    //get the window id
    uint64_t winId = SDL_GetWindowID((SDL_Window*)m_window);
    //set the SDL listing to NULL
    __glge_all_windows_sdl[winId] = 0;
    //close the window
    SDL_DestroyWindow((SDL_Window*)m_window);
    //remove the window from the SDL listing
    __glge_all_windows_sdl.erase(winId);

    //check if this is the last window
    if ((__glge_all_windows_sdl.size() == 0) && (__glge_sdl_2_thread))
    {
        //close the graphic API
        m_instance->closeGraphiAPI();

        //say that the SDL2 thread should be closed
        __glge_sdl_thread_open = false;
        //wait for the thread to close
        if (__glge_sdl_2_thread->joinable())
        {
            __glge_sdl_2_thread->join();
        }
        //destroy the thread
        delete __glge_sdl_2_thread;
        //set the thread to 0
        __glge_sdl_2_thread = 0;
        //delete the window counter
        delete __glge_all_window_count;
        __glge_all_window_count = 0;
        //delete the instance vector
        delete __glge_all_instances;
        __glge_all_instances = 0;
    }

    //reset all window parameters
    m_size = 0;
    m_pos = 0;
    m_settings = WINDOW_SETTINGS_DEFAULT;
    m_window = 0;
    //check if the counter exists
    if (__glge_all_window_count)
    {
        //decrement the window counter
        --(*__glge_all_window_count);
    }
}

bool Window::onUpdate() noexcept
{
    //store the return value
    bool ret = true;

    //check if the window should close
    if (m_shoudlClose)
    {
        //close the window
        close();
        //say that the window did close
        m_shoudlClose = false;
        //set the return value to false
        ret = false;
    }

    //return if the window exists
    return ret;
}

uint64_t Window::openWindowCount() noexcept
{
    //return the amount of open windows
    return ((__glge_all_window_count) ? *__glge_all_window_count : 0);
}

void Window::setWindowIcon(Texture* icon)
{
    //check if the texture type is cpu only
    if (icon->getPurpose() != TEXTURE_PURPOSE_CPU_ONLY)
    {
        //log an error
        m_instance->log("Can not create a window icon from a texture that is on the GPU", MESSAGE_TYPE_ERROR);
        return;
    }
    //if the texture is not custom, delete it
    if (!m_customIcon) {delete m_icon;}
    //store that the icon is custom
    m_customIcon = true;
    //store the icon texture
    m_icon = icon;

    //set the icon
    SDL_SetWindowIcon((SDL_Window*)m_window, (SDL_Surface*)m_icon->getGraphicTexture());
}

void Window::warpMouseCursor(uvec2 pixel) const noexcept
{
    //warp the mouse in the window
    SDL_WarpMouseInWindow((SDL_Window*)m_window, pixel.x, pixel.y);
}













bool WindowEventLayer::onEvent(Event* event) noexcept
{
    //don't handle the event if it is not a window event
    if (event->getIdentifyer().library != GLGE_EVENT_IDENTIFYER_WINDOW) {return false;}

    //check for SDL events
    if (event->getIdentifyer().event == GLGE_WINDOW_EVENT_SDL_EVENT)
    {
        //convert the event to a window event
        WindowEvent* wEv = (WindowEvent*)event;
        
        //switch over the window event types
        switch (wEv->event.window.event)
        {
        //check for closing
        case SDL_WINDOWEVENT_CLOSE:
            m_window->setQueuedClosing(true);
            break;

        //check for resizing
        case SDL_WINDOWEVENT_RESIZED:
            m_window->sdl_HandleResize(uvec2(wEv->event.window.data1, wEv->event.window.data2));
            break;

        //check for window moving
        case SDL_WINDOWEVENT_MOVED:
            m_window->m_pos = uvec2(wEv->event.window.data1, wEv->event.window.data2);
            break;
        
        default:
            break;
        }

        //event was handled successfully
        return true;
    }

    //the event was not handled
    return false;
}
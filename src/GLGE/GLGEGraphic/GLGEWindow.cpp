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
    //add this window to the instance
    m_instance->addElement(this);

    //print some debug info
    GLGE_DEBUG_WRAPPER(
        //store the stream to print
        std::stringstream stream;
        //fill the stream
        stream << "Started creation of a new window with name \"" << m_name << "\" and size " << m_size;
        //log that a new window was created
        m_instance->logDebug(Message(stream, MESSAGE_TYPE_DEBUG));
    )

    //check if SDL2 is initalized
    if (!__glge_sdl_2_thread)
    {
        //create a new thread. Bind it to the logger of the instance
        __glge_sdl_2_thread = new std::thread(SDL_Main_Thread, m_instance->getLogger());
        //create the counter to count how many windows are open
        __glge_all_window_count = new uint32_t(0);
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

    //store the window mapping
    __glge_all_windows_sdl[SDL_GetWindowID((SDL_Window*)m_window)] = this;
    //increment the amount of existing windows
    ++(*__glge_all_window_count);
}

void Window::close() noexcept
{
    //if the window is not open, stop
    if (!isOpen()) {return;}

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
        //check if the thread is joinable
        if (__glge_sdl_2_thread->joinable())
        {
            //join the thread
            __glge_sdl_2_thread->join();
        }
        //destroy the thread
        delete __glge_sdl_2_thread;
        //set the thread to 0
        __glge_sdl_2_thread = 0;
        //delete the window counter
        delete __glge_all_window_count;
        __glge_all_window_count = 0;
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
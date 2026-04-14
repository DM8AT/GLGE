/**
 * @file Window.cpp
 * @author DM8AT
 * @brief implement the window backend class for SDL3
 * @version 0.1
 * @date 2025-12-30
 * 
 * @copyright Copyright (c) 2025
 * 
 */
//include the SDL3 backend
#include "Graphic/Backend/Builtin/Video/SDL3/Window.h"
//include window frontends
#include "Graphic/Window.h"

//include graphic windows
#include "Graphic/Backend/Graphics/Window.h"

//add profiling
#include "Core/Profiler.h"

//add SDL3
#include <SDL3/SDL.h>

//use the namespace
using namespace GLGE::Graphic::Backend::Video::SDL3;

void Window::createWindow(GLGE::Graphic::Backend::Video::SDL3::Window* caller) {
    GLGE_PROFILER_SCOPE();

    //extract the required information for window setup
    const char* name = caller->getWindow()->getName().c_str();
    uvec2 size = caller->getWindow()->getSize();
    GLGE::Graphic::GraphicAPI api = caller->getBackendInstance()->getAPI();
    GLGE::Version apiVersion = caller->getBackendInstance()->getAPIVersion();
    GLGE::Graphic::WindowSettings& settings = caller->windowSettings();

    //prepare the window flags correctly
    SDL_WindowFlags flags = 0;
    //set the flag for the API
    flags |= (api == GLGE::Graphic::GraphicAPI::OPEN_GL) ? SDL_WINDOW_OPENGL : 0;
    flags |= (api == GLGE::Graphic::GraphicAPI::VULKAN)  ? SDL_WINDOW_VULKAN : 0;
    flags |= (api == GLGE::Graphic::GraphicAPI::METAL)   ? SDL_WINDOW_METAL  : 0;
    //set some flags depending on the states of the settings
    flags |= settings.fullscreen()       ? SDL_WINDOW_FULLSCREEN          : 0;
    flags |= settings.hidden()           ? SDL_WINDOW_HIDDEN              : 0;
    flags |= settings.borderless()       ? SDL_WINDOW_BORDERLESS          : 0;
    flags |= settings.resizable()        ? SDL_WINDOW_RESIZABLE           : 0;
    flags |= settings.minimized()        ? SDL_WINDOW_MINIMIZED           : 0;
    flags |= settings.maximized()        ? SDL_WINDOW_MAXIMIZED           : 0;
    flags |= settings.mouse_grabbed()    ? SDL_WINDOW_MOUSE_GRABBED       : 0;
    flags |= settings.input_focus()      ? SDL_WINDOW_INPUT_FOCUS         : 0;
    flags |= settings.mouse_focus()      ? SDL_WINDOW_MOUSE_FOCUS         : 0;
    flags |= settings.modal()            ? SDL_WINDOW_MODAL               : 0;
    flags |= settings.high_DPI()         ? SDL_WINDOW_HIGH_PIXEL_DENSITY  : 0;
    flags |= settings.mouse_capture()    ? SDL_WINDOW_MOUSE_CAPTURE       : 0;
    flags |= settings.always_on_top()    ? SDL_WINDOW_ALWAYS_ON_TOP       : 0;
    flags |= settings.utility()          ? SDL_WINDOW_UTILITY             : 0;
    flags |= settings.tooltip()          ? SDL_WINDOW_TOOLTIP             : 0;
    flags |= settings.popup_menu()       ? SDL_WINDOW_POPUP_MENU          : 0;
    flags |= settings.keyboard_grabbed() ? SDL_WINDOW_KEYBOARD_GRABBED    : 0;
    flags |= settings.transparent()      ? SDL_WINDOW_TRANSPARENT         : 0;
    flags |= settings.can_not_focus()    ? SDL_WINDOW_NOT_FOCUSABLE       : 0;
    flags |= settings.relative()         ? SDL_WINDOW_MOUSE_RELATIVE_MODE : 0;

    //create the SDL3 window
    SDL_Window* window = SDL_CreateWindow(name, size.x, size.y, flags);

    //read back the actual window settings

    //read the flags
    flags = SDL_GetWindowFlags(window);
    //set all the flags
    settings.fullscreen((flags & SDL_WINDOW_FULLSCREEN) != 0);
    settings.hidden((flags & SDL_WINDOW_HIDDEN) != 0);
    settings.borderless((flags & SDL_WINDOW_BORDERLESS) != 0);
    settings.resizable((flags & SDL_WINDOW_RESIZABLE) != 0);
    settings.minimized((flags & SDL_WINDOW_MINIMIZED) != 0);
    settings.maximized((flags & SDL_WINDOW_MAXIMIZED) != 0);
    settings.mouse_grabbed((flags & SDL_WINDOW_MOUSE_GRABBED) != 0);
    settings.input_focus((flags & SDL_WINDOW_INPUT_FOCUS) != 0);
    settings.mouse_focus((flags & SDL_WINDOW_MOUSE_FOCUS) != 0);
    settings.modal((flags & SDL_WINDOW_MODAL) != 0);
    settings.high_DPI((flags & SDL_WINDOW_HIGH_PIXEL_DENSITY) != 0);
    settings.mouse_capture((flags & SDL_WINDOW_MOUSE_CAPTURE) != 0);
    settings.always_on_top((flags & SDL_WINDOW_ALWAYS_ON_TOP) != 0);
    settings.utility((flags & SDL_WINDOW_UTILITY) != 0);
    settings.tooltip((flags & SDL_WINDOW_TOOLTIP) != 0);
    settings.popup_menu((flags & SDL_WINDOW_POPUP_MENU) != 0);
    settings.keyboard_grabbed((flags & SDL_WINDOW_KEYBOARD_GRABBED) != 0);
    settings.transparent((flags & SDL_WINDOW_TRANSPARENT) != 0);
    settings.can_not_focus((flags & SDL_WINDOW_NOT_FOCUSABLE) != 0);

    //set the relative mouse mode
    settings.relative(SDL_GetWindowRelativeMouseMode(window));

    //set the HDR state
    SDL_PropertiesID props = SDL_GetWindowProperties(window);
    settings.hdr(SDL_GetBooleanProperty(props, SDL_PROP_WINDOW_HDR_ENABLED_BOOLEAN, false));

    //get the window display ID
    caller->onDisplayChange(SDL_GetDisplayForWindow(window));
    //re-compute the whole size stuff
    //get the actual window size
    SDL_GetWindowSize(window, (int*)&size.x, (int*)&size.y);
    //get the actual usable resolution
    SDL_Rect safe;
    SDL_GetWindowSafeArea(window, &safe);
    //get the actual target resolution
    ivec2 res;
    SDL_GetWindowSizeInPixels(window, &res.x, &res.y);
    //get the display and then quarry the correct DPI
    const SDL_DisplayMode* mode = SDL_GetDesktopDisplayMode(SDL_GetDisplayForWindow(window));
    float dpi = mode->pixel_density * SDL_GetWindowPixelDensity(window);
    //notify of the actual size
    caller->onResize(uvec2(size.x, size.y), uvec2(safe.w, safe.h), dpi);
    //notify of the actual position
    int x, y = 0;
    SDL_GetWindowPosition(window, &x, &y);
    caller->onMoved(ivec2(x, y), ivec2(safe.x, safe.y));

    //store the final window in the caller
    caller->m_win = (void*)window;
    //register the window
    caller->getBackendInstance()->onWindowRegister(caller);
}

void Window::destroyWindow(GLGE::Graphic::Backend::Video::SDL3::Window* caller) {
    GLGE_PROFILER_SCOPE();

    //remove the window
    caller->getBackendInstance()->onWindowRemove(caller);
    //just destroy the window and set it to NULL
    SDL_DestroyWindow((SDL_Window*)caller->m_win);
    caller->m_win = nullptr;
}

Window::Window(GLGE::Graphic::Window* window)
 : GLGE::Graphic::Backend::Video::Window(window)
{
    GLGE_PROFILER_SCOPE();

    //create the window on the main thread
    GLGE::TaskHandle<void> handle;
    getWindow()->getInstance()->pushMainThreadTask(Task::create(&createWindow, &handle, this));
    handle.wait();
}

Window::~Window() {
    GLGE_PROFILER_SCOPE();

    //destroy the window from the main thread
    GLGE::TaskHandle<void> handle;
    getWindow()->getInstance()->pushMainThreadTask(Task::create(&destroyWindow, &handle, this));
    handle.wait();
}

void Window::onResize(const uvec2& size, const uvec2& safeSize, float pixelScale) {
    GLGE_PROFILER_SCOPE();

    //get the resolution
    ivec2 res;
    if (m_win) {
        SDL_GetWindowSizeInPixels(reinterpret_cast<SDL_Window*>(m_win), &res.x, &res.y);
    } else {
        res = {size.x*pixelScale, size.y*pixelScale};
    }
    //notify the graphic window of the resize
    if (getWindow()->getGraphicWindow().get())
    {getWindow()->getGraphicWindow()->onResolutionChange(size, safeSize, uvec2(res.x, res.y));}
    //notify the actual frontend window
    notifyResolutionChange(size, safeSize, pixelScale, uvec2(res.x, res.y));
}

void Window::onDisplayChange(u32 displayID) {
    GLGE_PROFILER_SCOPE();

    //store the new display id
    notifyDisplayChanged(displayID);
}

void Window::onMinimize() {
    GLGE_PROFILER_SCOPE();

    //set the minimize and maximize flag correctly
    windowSettings().minimized(true);
    windowSettings().maximized(false);
}

void Window::onMaximize() {
    GLGE_PROFILER_SCOPE();

    //set the minimize and maximize flag correctly
    windowSettings().minimized(false);
    windowSettings().maximized(true);
}

void Window::onRestored() {
    GLGE_PROFILER_SCOPE();

    //set the minimize and maximize flag correctly
    windowSettings().minimized(false);
    windowSettings().maximized(false);
}

void Window::onShown() {
    GLGE_PROFILER_SCOPE();

    //set the window to not hidden
    windowSettings().hidden(false);
    notifyVisibleChanged(true);
}

void Window::onHidden() {
    GLGE_PROFILER_SCOPE();

    //set the window to be hidden
    windowSettings().hidden(true);
    notifyDisplayChanged(false);
}

void Window::onExposed() {
    //do nothing
}

void Window::onOccluded() {
    //do nothing
}

void Window::onMoved(const ivec2& pos, const ivec2& safePos) {
    GLGE_PROFILER_SCOPE();

    //notify about the movement
    notifyMove(pos);
}

void Window::onMouseFocusChange(bool focus) {
    GLGE_PROFILER_SCOPE();

    //store the new mouse focus state
    windowSettings().mouse_focus(focus);
}

void Window::onInputFocusChange(bool focus) {
    GLGE_PROFILER_SCOPE();

    //store the new input state
    windowSettings().input_focus(focus);
}

void Window::onFullscreenUpdate(bool fullscreen) {
    GLGE_PROFILER_SCOPE();

    //store the new fullscreen state
    windowSettings().fullscreen(fullscreen);
}

void Window::onHDRStateChanged(bool hdr) {
    GLGE_PROFILER_SCOPE();

    //store the new HDR capable state
    windowSettings().hdr(hdr);
}

void Window::onMakeCurrent(void* context) {
    GLGE_PROFILER_SCOPE();

    //in debug check if this is the main thread
    #if GLGE_DEBUG
    if (!GLGE::Instance::isMainThread())
    {throw GLGE::Exception("Tried to bind a window to a context on a different thread than the main thread", "GLGE::Graphic::Backend::Video::SDL3::Window::onMakeCurrent");}
    #endif

    //get the used API
    GLGE::Graphic::GraphicAPI api = getWindow()->getGraphicInstance()->getVideoBackendInstance()->getAPI();

    //switch over the API and call the correct function
    //unsupported -> just skip it
    switch (api) {
        case GLGE::Graphic::GraphicAPI::OPEN_GL:
        case GLGE::Graphic::GraphicAPI::OPEN_GL_ES: {
            //just bind the context to the window
            SDL_GL_MakeCurrent((SDL_Window*)m_win, (SDL_GLContext)context);
        }
        break;

        default:
            break;
    }
}

/**
 * @brief convert the VSync enum to an integer value SDL 3 can understand
 * 
 * @param v the vsync enum value
 * @return `GLGE::i32` the integer that SDL 3 can understand
 */
static GLGE::i32 __gl_VSyncToSDL(GLGE::Graphic::VSync v) {
    //switch over the known vsync modes
    switch (v)
    {
    //check for disabled mode
    case GLGE::Graphic::VSYNC_DISABLED:
        return 0;
        break;
    //check for enabled mode
    case GLGE::Graphic::VSYNC_ENABLED:
        return 1;
        break;
    //check for adaptive mode
    case GLGE::Graphic::VSYNC_ADAPTIVE:
        return -1;
        break;
    
    default:
        //in debug throw an error
        #if GLGE_DEBUG
        throw GLGE::Exception("Tried to select a not supported VSync mode", "GLGE::Graphic::Backend::Builtin::Video::SDL3::__gl_VSyncToSDL");
        #endif
        //just return 0 (disabled)
        return 0;
        break;
    }
}

bool Window::onSetVSync(VSync vsync, void* context) {
    GLGE_PROFILER_SCOPE();

    //define a helper lambda to call from the main thread
    bool (*helper)(GLGE::Graphic::Backend::Video::Window*, VSync, void*) = [](GLGE::Graphic::Backend::Video::Window* window, VSync vsync, void* context){
        //make this the current window
        //this already does all sanity checks
        window->onMakeCurrent(context);

        //get the used API
        GLGE::Graphic::GraphicAPI api = window->getWindow()->getGraphicInstance()->getVideoBackendInstance()->getAPI();

        //switch over the API and call the correct function
        //unsupported -> just skip it
        switch (api) {
            case GLGE::Graphic::GraphicAPI::OPEN_GL:
            case GLGE::Graphic::GraphicAPI::OPEN_GL_ES: {
                //use OpenGL and return if it succeeded
                bool s = SDL_GL_SetSwapInterval(__gl_VSyncToSDL(vsync));
                if (!s) {SDL_GL_SetSwapInterval(1);}
                return s;
            }
            break;

            default:
                //return a failure
                return false;
                break;
        }
    };

    //call the helper from the main thread
    TaskHandle<bool> handle;
    getWindow()->getInstance()->pushMainThreadTask(Task::create(helper, &handle, this, vsync, context));
    handle.wait();
    //return the result from the helper
    return handle.get();
}

void Window::onSwapWindow() {
    GLGE_PROFILER_SCOPE();

    //in debug check if this is the main thread
    #if GLGE_DEBUG
    if (!GLGE::Instance::isMainThread())
    {throw GLGE::Exception("Tried to swap a window from a different thread than the main thread", "GLGE::Graphic::Backend::Video::SDL3::Window::onSwapWindow");}
    #endif

    //get the used API
    GLGE::Graphic::GraphicAPI api = getWindow()->getGraphicInstance()->getVideoBackendInstance()->getAPI();

    //switch over the API and call the correct function
    //unsupported -> just skip it
    switch (api) {
        case GLGE::Graphic::GraphicAPI::OPEN_GL:
        case GLGE::Graphic::GraphicAPI::OPEN_GL_ES: {
            //swap the window
            SDL_GL_SwapWindow((SDL_Window*)m_win);
        }
        break;

        default:
            break;
    }
}

void Window::onSetMinimumSize(const uvec2& size) {
    GLGE_PROFILER_SCOPE();

    //set the minimum size
    SDL_SetWindowMinimumSize(reinterpret_cast<SDL_Window*>(m_win), size.x, size.y);
}

void Window::onSetMaximumSize(const uvec2& size) {
    GLGE_PROFILER_SCOPE();

    //set the maximum size
    SDL_SetWindowMaximumSize(reinterpret_cast<SDL_Window*>(m_win), size.x, size.y);
}
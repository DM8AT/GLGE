/**
 * @file Instance.cpp
 * @author DM8AT
 * @brief implement the functionality for an instance of the SDL video backend
 * @version 0.1
 * @date 2025-12-29
 * 
 * @copyright Copyright (c) 2025
 * 
 */
//access the backend
#define __GLGE_DISPLAY_BACKEND
//include the SDL video backend instance
#include "Graphic/Backend/Builtin/Video/SDL3/Instance.h"

//include SDL windows
#include "Graphic/Backend/Builtin/Video/SDL3/Window.h"

//add SDL3
#include "SDL3/SDL.h"

//add profiling
#include "Core/Profiler.h"

//use the normal GLGE namespace
using namespace GLGE::Graphic;

//use the namespace
using namespace GLGE::Graphic::Backend::Video::SDL3;
using namespace GLGE;

static Key SDLKeycodeToKey(SDL_Keycode kc) {
    //Letters
    if (kc >= SDLK_A && kc <= SDLK_Z) {
        return static_cast<Key>('a' + (kc - SDLK_A));
    }

    //Numbers
    if (kc >= SDLK_0 && kc <= SDLK_9) {
        return static_cast<Key>('0' + (kc - SDLK_0));
    }

    //Printable symbols
    switch (kc) {
        case SDLK_SPACE:         return Key::Space;
        case SDLK_EXCLAIM:       return Key::Exclamation;
        case SDLK_DBLAPOSTROPHE: return Key::Quote;
        case SDLK_HASH:          return Key::Hash;
        case SDLK_DOLLAR:        return Key::Dollar;
        case SDLK_PERCENT:       return Key::Percent;
        case SDLK_AMPERSAND:     return Key::Ampersand;
        case SDLK_APOSTROPHE:    return Key::Apostrophe;
        case SDLK_LEFTPAREN:     return Key::LeftPrentices;
        case SDLK_RIGHTPAREN:    return Key::RightPrentices;
        case SDLK_ASTERISK:      return Key::Asterisk;
        case SDLK_PLUS:          return Key::Plus;
        case SDLK_COMMA:         return Key::Comma;
        case SDLK_MINUS:         return Key::Minus;
        case SDLK_PERIOD:        return Key::Period;
        case SDLK_SLASH:         return Key::Slash;
        case SDLK_COLON:         return Key::Colon;
        case SDLK_SEMICOLON:     return Key::Semicolon;
        case SDLK_LESS:          return Key::Less;
        case SDLK_EQUALS:        return Key::Equal;
        case SDLK_GREATER:       return Key::Greater;
        case SDLK_QUESTION:      return Key::Question;
        case SDLK_AT:            return Key::At;
        case SDLK_LEFTBRACKET:   return Key::LeftBracket;
        case SDLK_BACKSLASH:     return Key::Backslash;
        case SDLK_RIGHTBRACKET:  return Key::RightBracket;
        case SDLK_CARET:         return Key::Caret;
        case SDLK_UNDERSCORE:    return Key::Underscore;
        case SDLK_GRAVE:         return Key::Grave;
        case SDLK_LEFTBRACE:     return Key::LeftBrace;
        case SDLK_PIPE:          return Key::Pipe;
        case SDLK_RIGHTBRACE:    return Key::RightBrace;
        case SDLK_TILDE:         return Key::Tilde;
    }

    //Control keys
    switch (kc) {
        case SDLK_RETURN:     return Key::Enter;
        case SDLK_ESCAPE:     return Key::Escape;
        case SDLK_BACKSPACE:  return Key::Backspace;
        case SDLK_TAB:        return Key::Tab;
    }

    //Navigation keys
    switch (kc) {
        case SDLK_INSERT:     return Key::Insert;
        case SDLK_DELETE:     return Key::Delete;
        case SDLK_HOME:       return Key::Home;
        case SDLK_END:        return Key::End;
        case SDLK_PAGEUP:     return Key::PageUp;
        case SDLK_PAGEDOWN:   return Key::PageDown;
        case SDLK_UP:         return Key::ArrowUp;
        case SDLK_DOWN:       return Key::ArrowDown;
        case SDLK_LEFT:       return Key::ArrowLeft;
        case SDLK_RIGHT:      return Key::ArrowRight;
    }

    //Function keys
    if (kc >= SDLK_F1 && kc <= SDLK_F24) {
        return static_cast<Key>(static_cast<u8>(Key::F1) + (kc - SDLK_F1));
    }

    //Modifiers
    switch (kc) {
        case SDLK_LSHIFT:     return Key::LeftShift;
        case SDLK_RSHIFT:     return Key::RightShift;
        case SDLK_LCTRL:      return Key::LeftCtrl;
        case SDLK_RCTRL:      return Key::RightCtrl;
        case SDLK_LALT:       return Key::LeftAlt;
        case SDLK_RALT:       return Key::RightAlt;
        case SDLK_LGUI:       return Key::LeftMeta;
        case SDLK_RGUI:       return Key::RightMeta;
        case SDLK_CAPSLOCK:   return Key::CapsLock;
        case SDLK_NUMLOCKCLEAR:return Key::NumLock;
        case SDLK_SCROLLLOCK: return Key::ScrollLock;
    }

    //System keys
    switch (kc) {
        case SDLK_PRINTSCREEN: return Key::PrintScreen;
        case SDLK_PAUSE:       return Key::Pause;
        case SDLK_MENU:        return Key::Menu;
    }

    //Media keys
    switch (kc) {
        case SDLK_VOLUMEUP:             return Key::VolumeUp;
        case SDLK_VOLUMEDOWN:           return Key::VolumeDown;
        case SDLK_MUTE:                 return Key::VolumeMute;
        case SDLK_MEDIA_NEXT_TRACK:     return Key::MediaNext;
        case SDLK_MEDIA_PREVIOUS_TRACK: return Key::MediaPrev;
        case SDLK_MEDIA_STOP:           return Key::MediaStop;
        case SDLK_MEDIA_PLAY_PAUSE:     return Key::MediaPlayPause;
    }

    //undefined key
    return Key::UNDEFINED;
}

/**
 * @brief count the amount of bits in a mask
 * 
 * @param mask the mask to count
 * @return `GLGE::u8` the amount of bits
 */
static GLGE::u8 countBits(GLGE::u32 mask) {
    //store the count
    GLGE::u8 count = 0;
    //while the mask contains data increase the count and shift the mask down
    while(mask) {
        count += (mask & 1);
        mask >>= 1;
    }
    //return the final count
    return count;
}

/**
 * @brief map an SDL orientation to GLGEs orientation
 * 
 * @param orientation the orientation in SDLs format
 * @return `GLGE::Graphic::Display::Orientation` the orientation in a format that GLGE understands
 */
static GLGE::Graphic::Display::Orientation __sdlToCustomOrientation(SDL_DisplayOrientation orientation) {
    //map the enum values
    switch (orientation)
    {
    case SDL_ORIENTATION_UNKNOWN:
        return GLGE::Graphic::Display::Orientation::UNKNOWN;
        break;
    case SDL_ORIENTATION_LANDSCAPE:
        return GLGE::Graphic::Display::Orientation::LANDSCAPE;
        break;
    case SDL_ORIENTATION_LANDSCAPE_FLIPPED:
        return GLGE::Graphic::Display::Orientation::LANDSCAPE_FLIPPED;
        break;
    case SDL_ORIENTATION_PORTRAIT:
        return GLGE::Graphic::Display::Orientation::PORTRAIT;
        break;
    case SDL_ORIENTATION_PORTRAIT_FLIPPED:
        return GLGE::Graphic::Display::Orientation::PORTRAIT_FLIPPED;
        break;
    
    //the default is the unknown value
    default:
        return GLGE::Graphic::Display::Orientation::UNKNOWN;
        break;
    }
}

/**
 * @brief convert any SDL pixel format to a custom format
 * 
 * @param sdlFormat the SDL format to convert
 * @return `GLGE::Graphic::PixelFormat` the GLGE pixel format
 */
static GLGE::Graphic::PixelFormat __sdlToCustomPixelFormat(SDL_PixelFormat sdlFormat) {
    //first, create an empty format
    GLGE::Graphic::PixelFormat out;

    //check if this is a indexed format
    if (SDL_ISPIXELFORMAT_INDEXED(sdlFormat)) {
        //only index bit count matters
        int bpp = SDL_BITSPERPIXEL(sdlFormat);
        //the bit count for the index is stored in the red channel
        out.r_Bitcount = static_cast<GLGE::u8>(bpp);
        //set the other values to the correct defaults
        out.order = GLGE::Graphic::PixelFormat::Order::INDEXED;
        out.type  = GLGE::Graphic::PixelFormat::Type::UINT;
        //early return
        return out;
    }

    //get the masks for the channels
    int bpp;
    Uint32 rmask = 0, gmask = 0, bmask = 0, amask = 0;
    bool ok = SDL_GetMasksForPixelFormat(sdlFormat, &bpp, &rmask, &gmask, &bmask, &amask);

    //sanity check to prevent wired formats
    if (!ok) {
        //wired formats are not supported
        return out;
    }

    //count the amount of bits for all channels
    //they may be different for the channels, so be sure to encode that too
    out.r_Bitcount = countBits(rmask);
    out.g_Bitcount = countBits(gmask);
    out.b_Bitcount = countBits(bmask);
    out.a_Bitcount = countBits(amask);

    
    //determine the order of the bits

    //built a weighted map for all the formats
    if (rmask && !gmask && !bmask && !amask) {
        //something went terribly wrong
        out.order = GLGE::Graphic::PixelFormat::Order::UNDEFINED;
    //check for 4 component formats by checking if an alpha mask exists
    } else if (amask) {
        //4‑component formats
        if (rmask > gmask && gmask > bmask && bmask > amask) {
            out.order = GLGE::Graphic::PixelFormat::Order::RGBA;
        } else if (amask > rmask && rmask > gmask && gmask > bmask) {
            out.order = GLGE::Graphic::PixelFormat::Order::ARGB;
        } else if (bmask > gmask && gmask > rmask && rmask > amask) {
            out.order = GLGE::Graphic::PixelFormat::Order::BGRA;
        } else if (amask > bmask && bmask > gmask && gmask > rmask) {
            out.order = GLGE::Graphic::PixelFormat::Order::ABGR;
        } else {
            //default to undefined
            out.order = GLGE::Graphic::PixelFormat::Order::UNDEFINED;
        }
    } else {
        //3‑component formats
        if (rmask > gmask && gmask > bmask) {
            out.order = GLGE::Graphic::PixelFormat::Order::RGB;
        } else if (bmask > gmask && gmask > rmask) {
            out.order = GLGE::Graphic::PixelFormat::Order::BGR;
        } else {
            //default to undefined
            out.order = GLGE::Graphic::PixelFormat::Order::UNDEFINED;
        }
    }

    //check if the stored data is a float or unorm format
    if (SDL_ISPIXELFORMAT_FLOAT(sdlFormat)) 
    {out.type = GLGE::Graphic::PixelFormat::Type::FLOAT;}
    else
    {out.type = GLGE::Graphic::PixelFormat::Type::UNORM;}
    
    //return the final format
    return out;
}

/**
 * @brief a function to map SDL color spaces to GLGE color spaces
 * 
 * @param sdlColorSpace the SDL color space to map
 * @return `GLGE::Graphic::ColorSpace` the finalized GLGE color space
 */
static GLGE::Graphic::ColorSpace __sdlToCustomColorSpace(SDL_Colorspace sdlColorSpace) {
    //collapse the whole name together to simplify coding
    using CS = GLGE::Graphic::ColorSpace;

    //just switch over all the enum values and map them
    switch (sdlColorSpace) {
        case SDL_COLORSPACE_UNKNOWN:
            return CS::UNDEFINED;

        case SDL_COLORSPACE_SRGB:
            return CS::SRGB;
        case SDL_COLORSPACE_SRGB_LINEAR:
            return CS::SRGB_LINEAR;
        case SDL_COLORSPACE_HDR10:
            return CS::HDR10;
        case SDL_COLORSPACE_JPEG:
            return CS::JPEG;

        case SDL_COLORSPACE_BT601_LIMITED:
            return CS::BT601_LIMITED;
        case SDL_COLORSPACE_BT601_FULL:
            return CS::BT601_FULL;
        case SDL_COLORSPACE_BT709_LIMITED:
            return CS::BT709_LIMITED;
        case SDL_COLORSPACE_BT709_FULL:
            return CS::BT709_FULL;
        case SDL_COLORSPACE_BT2020_LIMITED:
            return CS::BT2020_LIMITED;
        case SDL_COLORSPACE_BT2020_FULL:
            return CS::BT2020_FULL;

        //default to undefined
        default:
            return CS::UNDEFINED;
    }
}

/**
 * @brief describe a display that is just identified by the ID
 * 
 * @param id the ID of the display to describe
 * @return Display the finalized display
 */
static Display __describeSDLDisplay(GLGE::u32 id) {
    //store the information about the display
    Display display;
    //fill in ID and name
    display.displayID = id;
    display.name = SDL_GetDisplayName(display.displayID);
    //fill in the bound
    SDL_Rect bound;
    SDL_GetDisplayBounds(display.displayID, &bound);
    display.pos = GLGE::uvec2(bound.x, bound.y);
    display.logicalSize = GLGE::uvec2(bound.w, bound.h);
    //fill in the usable bounds
    SDL_Rect usableBound;
    SDL_GetDisplayUsableBounds(display.displayID, &usableBound);
    display.usablePosition = GLGE::uvec2(usableBound.x, usableBound.y);
    display.usableSize = GLGE::uvec2(usableBound.w, usableBound.h);
    //fill in the orientation
    display.orientation = __sdlToCustomOrientation(SDL_GetCurrentDisplayOrientation(display.displayID));
    //fill in the display content scale
    display.scaleFactor = SDL_GetDisplayContentScale(display.displayID);
    //fill in everything related to the display mode
    const SDL_DisplayMode* mode = SDL_GetDesktopDisplayMode(display.displayID);
    //sanity check if the window is on a display
    if (mode) {
        display.refreshRate = mode->refresh_rate;
        display.pixelDensity = mode->pixel_density;
        display.resolution = GLGE::uvec2(mode->w * display.pixelDensity * display.scaleFactor, mode->h * display.pixelDensity * display.scaleFactor);
        //make sure to map the formats to GLGE formats
        display.format = __sdlToCustomPixelFormat(mode->format);
    }

    //return the finalized display
    return display;
}

GLGE::Graphic::Backend::Video::SDL3::Instance::Instance(GLGE::Graphic::Instance* instance, GLGE::Graphic::GraphicAPI api, GLGE::Version version)
 : GLGE::Graphic::Backend::Video::Instance(instance, api, version)
{
    GLGE_PROFILER_SCOPE();

    //atomically increment the instance counter
    //if it was 0 prepare SDL3 for initialization
    if (msl_instanceCount.fetch_add(1, std::memory_order_acq_rel) == 0) {
        //first, initialize the video backend
        SDL_Init(SDL_INIT_VIDEO | SDL_INIT_EVENTS);

        //get the SDL displays
        i32 displayCount = 0;
        SDL_DisplayID* displayIDs = SDL_GetDisplays(&displayCount);

        //iterate over all displays
        for (i32 i = 0; i < displayCount; ++i) {
            //store the new display
            msl_displaySetup.__backendRegisterDisplay(__describeSDLDisplay(displayIDs[i]));
        }

        //set the primary display ID
        msl_displaySetup.__backendSetPrimaryDisplayID(SDL_GetPrimaryDisplay());

        /**
         * @brief Don't forget to clean up the IDs
         */
        SDL_free(displayIDs);

        //register the main update function
        GLGE::Instance::staticUpdateSubscribe(&mainUpdate);
    }

    //load all the keyboards
    int keyboardCount = 0;
    SDL_KeyboardID* keyboards = SDL_GetKeyboards(&keyboardCount);
    for (int i = 0; i < keyboardCount; ++i) 
    {registerKeyboard(keyboards[i]);}

    //load all the mice
    int miceCount = 0;
    SDL_MouseID* mice = SDL_GetMice(&miceCount);
    for (int i = 0; i < miceCount; ++i) {
        //register the keyboard to the instance
        getInstance()->getInstance()->registerMouse(GLGE::Mouse(SDL_GetMouseNameForID(mice[i])));
        //register the mapping
        m_miceMap.insert_or_assign(mice[i], getInstance()->getInstance()->getMice().size()-1);
    }

    //get writing access to the instance list
    std::unique_lock lock(msl_instanceLock);
    //add the instance to the instance list
    msl_instances.push_back(instance);
}

GLGE::Graphic::Backend::Video::SDL3::Instance::~Instance() {
    GLGE_PROFILER_SCOPE();

    //atomically decrement the instance counter
    //if it was 1 (now 0) clean up
    if (msl_instanceCount.fetch_sub(1, std::memory_order_acq_rel) == 1) {
        //unsubscribe from the main update
        GLGE::Instance::staticUpdateUnsubscribe(&mainUpdate);
        //stop SDL3
        SDL_Quit();
    }

    //get writing access to the instance list
    std::unique_lock lock(msl_instanceLock);
    //search the instance in the list. If it is found, remove it
    for (size_t i = 0; i < msl_instances.size(); ++i)
    {msl_instances.erase(msl_instances.begin() + i);}
}

void GLGE::Graphic::Backend::Video::SDL3::Instance::onUpdate() {
    //just do nothing, the main update function is used for updates
    //this is because most SDL stuff must happen from the main thread
}

//define a helper macro to get the window
#define __getWindow \
    /*get the actual window*/ \
    auto pos = msl_windows.find(e.window.windowID);

//define a helper macro to resize the window correctly
#define __resizeWindow \
    /*handle and get the window*/ \
    __getWindow \
    SDL_Window* win = SDL_GetWindowFromEvent(&e); \
    /*get the window size*/ \
    ivec2 size; \
    SDL_GetWindowSize(win, &size.x, &size.y); \
    /*get the actual usable resolution*/ \
    SDL_Rect safe; \
    SDL_GetWindowSafeArea(win, &safe); \
    /*get the actual target resolution*/ \
    ivec2 res; \
    SDL_GetWindowSizeInPixels(win, &res.x, &res.y); \
    /*get the display and then quarry the correct DPI*/ \
    const SDL_DisplayMode* mode = SDL_GetDesktopDisplayMode(SDL_GetDisplayForWindow(win)); \
    float dpi = mode->pixel_density * SDL_GetWindowPixelDensity(win); \
    /*call the resize function*/ \
    pos->second->onResize(uvec2(size.x, size.y), uvec2(safe.w, safe.h), dpi); \

void GLGE::Graphic::Backend::Video::SDL3::Instance::mainUpdate() {
    GLGE_PROFILER_SCOPE();

    //lock the window mutex
    std::shared_lock lock(msl_windowMutex);
    //lock the instance mutex
    std::shared_lock instanceLock(msl_instanceLock);

    //store the current SDL event
    SDL_Event e;
    //poll all SDL events
    while (SDL_PollEvent(&e)) {
        GLGE_PROFILER_SCOPE_NAMED("GLGE::Graphic::Backend::Video::SDL3::Instance::mainUpdate::singleEvent");

        //switch over the event type to handle all events correctly
        switch (e.type) {
            //WINDOWS

            //check if a window should be closed
            case SDL_EVENT_WINDOW_CLOSE_REQUESTED:{
                //get the window
                __getWindow
                //if the window is valid, request closing
                pos->second->getWindow()->requestClosing();
            }
            break;
            //check if a window should be resized
            case SDL_EVENT_WINDOW_PIXEL_SIZE_CHANGED: {
                //handle the window resizing
                __resizeWindow
            }
            //check if the window was shown
            case SDL_EVENT_WINDOW_SHOWN: {
                //handle and get the window
                __getWindow
                //notify of the showing
                pos->second->onShown();
            }
            break;
            //check if the window was hidden
            case SDL_EVENT_WINDOW_HIDDEN: {
                //handle and get the window
                __getWindow
                //notify of the hiding
                pos->second->onHidden();
            }
            break;
            //check if the window was moved
            case SDL_EVENT_WINDOW_MOVED: {
                //handle and get the window
                __getWindow
                //get the new safe area
                SDL_Rect safe;
                SDL_GetWindowSafeArea(SDL_GetWindowFromEvent(&e), &safe);
                //notify of the movement
                pos->second->onMoved(ivec2(e.window.data1, e.window.data2), ivec2(safe.x, safe.y));
            }
            break;
            //check if the window was minimized
            case SDL_EVENT_WINDOW_MINIMIZED: {
                //get the window
                __getWindow
                //notify of the minimizing
                pos->second->onMinimize();
            }
            break;
            //check if the window was maximized
            case SDL_EVENT_WINDOW_MAXIMIZED: {
                //get the window
                __getWindow
                //notify of the maximizing
                pos->second->onMaximize();
            }
            break;
            //check if the window was restored
            case SDL_EVENT_WINDOW_RESTORED: {
                //get the window
                __getWindow
                //notify of the restoring
                pos->second->onRestored();
            }
            break;
            //check if the mouse entered the window
            case SDL_EVENT_WINDOW_MOUSE_ENTER: {
                //get the window
                __getWindow
                //notify of the mouse entering
                pos->second->onMouseFocusChange(true);
            }
            break;
            //check if the mouse left the window
            case SDL_EVENT_WINDOW_MOUSE_LEAVE: {
                //get the window
                __getWindow
                //notify of the mouse leaving
                pos->second->onMouseFocusChange(false);
            }
            break;
            //check if the window gained focus
            case SDL_EVENT_WINDOW_FOCUS_GAINED: {
                //get the window
                __getWindow
                //notify about the input focus gaining
                pos->second->onInputFocusChange(true);
            }
            break;
            //check if the window lost focus
            case SDL_EVENT_WINDOW_FOCUS_LOST: {
                //get the window
                __getWindow
                //notify about the input focus loosing
                pos->second->onInputFocusChange(false);
            }
            break;
            //check if the window was moved to another display
            case SDL_EVENT_WINDOW_DISPLAY_CHANGED: {
                //handle the window resizing
                __resizeWindow
                //store the new ID of the display the window is on
                pos->second->onDisplayChange(e.window.data1);
            }
            break;
            //check if the scale of the display the window is on changed
            case SDL_EVENT_WINDOW_DISPLAY_SCALE_CHANGED: {
                //handle the window resizing
                __resizeWindow
            }
            break;
            //check if the was occluded
            case SDL_EVENT_WINDOW_OCCLUDED: {
                //handle the window resizing
                __resizeWindow
                //store that the window is not visible
                pos->second->onOccluded();
            }
            break;
            //check if the window was exposed
            case SDL_EVENT_WINDOW_EXPOSED: {
                //handle the window resizing
                __resizeWindow
                //store that the window is visible
                pos->second->onExposed();
            }
            break;
            //check if the window entered fullscreen
            case SDL_EVENT_WINDOW_ENTER_FULLSCREEN: {
                //notify the size change
                __resizeWindow
                //notify about entering fullscreen
                pos->second->onFullscreenUpdate(true);
            }
            break;
            //check if the window left fullscreen
            case SDL_EVENT_WINDOW_LEAVE_FULLSCREEN: {
                //notify the size change
                __resizeWindow
                //notify about leaving fullscreen
                pos->second->onFullscreenUpdate(false);
            }
            break;
            //check if the HDR state of the window changed
            case SDL_EVENT_WINDOW_HDR_STATE_CHANGED: {
                //quarry the window
                __getWindow
                //notify that the HDR state change
                pos->second->onHDRStateChanged(e.window.data1);
            }
            break;
            
            //DISPLAY STUFF

            //check for plugging in a new display
            case SDL_EVENT_DISPLAY_ADDED: {
                //add the new display
                msl_displaySetup.__backendRegisterDisplay(__describeSDLDisplay(e.display.displayID));
            }
            break;
            //check if a display was removed
            case SDL_EVENT_DISPLAY_REMOVED: {
                //remove the display
                msl_displaySetup.__backendRemoveDisplay(e.display.displayID);
            }
            break;
            //check if the content scale of the display changed
            case SDL_EVENT_DISPLAY_CONTENT_SCALE_CHANGED: {
                //set the new content scale
                Display& display = msl_displaySetup.__backendAccessForEditing(e.display.displayID);
                display.scaleFactor = e.display.data1;
                display.resolution = uvec2(display.logicalSize.x * display.pixelDensity * display.scaleFactor, display.logicalSize.y * display.pixelDensity * display.scaleFactor);
            }
            break;
            //check if the current display mode or desktop mode changed
            case SDL_EVENT_DISPLAY_CURRENT_MODE_CHANGED:
            case SDL_EVENT_DISPLAY_DESKTOP_MODE_CHANGED: {
                //get the new mode
                const SDL_DisplayMode* mode = SDL_GetDesktopDisplayMode(e.display.displayID);
                //get the display
                Display& display = msl_displaySetup.__backendAccessForEditing(e.display.displayID);
                //update everything
                display.refreshRate = mode->refresh_rate;
                display.pixelDensity = mode->pixel_density;
                display.resolution = GLGE::uvec2(mode->w * display.pixelDensity * display.scaleFactor, mode->h * display.pixelDensity * display.scaleFactor);
                //make sure to map the formats to GLGE formats
                display.format = __sdlToCustomPixelFormat(mode->format);
            }
            break;
            //check if the display was moved
            case SDL_EVENT_DISPLAY_MOVED: {
                //get the display to edit
                Display& display = msl_displaySetup.__backendAccessForEditing(e.display.displayID);
                //store the new position
                display.pos = uvec2(e.display.data1, e.display.data2);
            }
            break;
            //check if the 
            case SDL_EVENT_DISPLAY_ORIENTATION: {
                //get the display to edit
                Display& display = msl_displaySetup.__backendAccessForEditing(e.display.displayID);
                //store the new orientation
                display.orientation = __sdlToCustomOrientation(SDL_GetCurrentDisplayOrientation(e.display.displayID));

                //also re-get all resolution dependent stuff as it is now rotated

                //fill in the bound
                SDL_Rect bound;
                SDL_GetDisplayBounds(display.displayID, &bound);
                display.pos = GLGE::uvec2(bound.x, bound.y);
                display.logicalSize = GLGE::uvec2(bound.w, bound.h);

                //fill in the usable bounds
                SDL_Rect usableBound;
                SDL_GetDisplayUsableBounds(display.displayID, &usableBound);
                display.usablePosition = GLGE::uvec2(usableBound.x, usableBound.y);
                display.usableSize = GLGE::uvec2(usableBound.w, usableBound.h);

                //get the new mode
                const SDL_DisplayMode* mode = SDL_GetDesktopDisplayMode(e.display.displayID);
                //update the new resolution
                display.resolution = GLGE::uvec2(mode->w * display.pixelDensity * display.scaleFactor, mode->h * display.pixelDensity * display.scaleFactor);
            }
            break;

            //KEYBOARD STUFF

            //check for key down events
            case SDL_EVENT_KEY_DOWN: {
                    //iterate over all instances and notify them
                    for (auto& instance : msl_instances)
                    {reinterpret_cast<GLGE::Graphic::Backend::Video::SDL3::Instance*>(instance->getVideoBackendInstance())->keyUpdate(true, static_cast<u64>(e.key.key), static_cast<u32>(e.key.which));}
                } break;
            //check for key up events
            case SDL_EVENT_KEY_UP: {
                    //iterate over all instances and notify them
                    for (auto& instance : msl_instances)
                    {reinterpret_cast<GLGE::Graphic::Backend::Video::SDL3::Instance*>(instance->getVideoBackendInstance())->keyUpdate(false, static_cast<u64>(e.key.key), static_cast<u32>(e.key.which));}
                } break;
            //check for a keyboard registration
            case SDL_EVENT_KEYBOARD_ADDED: {
                    //iterate over all instances and notify them
                    for (auto& instance : msl_instances)
                    {reinterpret_cast<GLGE::Graphic::Backend::Video::SDL3::Instance*>(instance->getVideoBackendInstance())->registerKeyboard(static_cast<u32>(e.kdevice.which));}
                } break;
            //check for a keyboard remove event
            case SDL_EVENT_KEYBOARD_REMOVED: {
                    //iterate over all instances and notify them
                    for (auto& instance : msl_instances)
                    {reinterpret_cast<GLGE::Graphic::Backend::Video::SDL3::Instance*>(instance->getVideoBackendInstance())->removeKeyboard(static_cast<u32>(e.kdevice.which));}
                } break;
            
            default:
                break;
        }
    }
}

void GLGE::Graphic::Backend::Video::SDL3::Instance::keyUpdate(bool state, u64 keycode, u32 keyboardId) {
    //map the keycode to the key enum
    Key key = SDLKeycodeToKey(static_cast<SDL_Keycode>(keycode));
    //get the keyboard ID
    auto keyboard = m_keyboardMap.find(static_cast<SDL_KeyboardID>(keyboardId));
    if (keyboard == m_keyboardMap.end()) {return;}
    //update the key
    if (state)
    {getInstance()->getInstance()->getKeyboard(keyboard->second).press(key);}
    else
    {getInstance()->getInstance()->getKeyboard(keyboard->second).release(key);}
}

void GLGE::Graphic::Backend::Video::SDL3::Instance::registerKeyboard(u32 keyboardId) {
    //if the keyboard is known, stop
    if (m_keyboardMap.contains(static_cast<SDL_KeyboardID>(keyboardId))) {return;}

    //register the keyboard to the instance
    getInstance()->getInstance()->registerKeyboard(GLGE::Keyboard(SDL_GetKeyboardNameForID(static_cast<SDL_KeyboardID>(keyboardId))));
    //register the mapping
    m_keyboardMap.insert_or_assign(static_cast<SDL_KeyboardID>(keyboardId), getInstance()->getInstance()->getKeyboards().size()-1);
}

void GLGE::Graphic::Backend::Video::SDL3::Instance::removeKeyboard(u32 keyboardId) {
    //get the keyboard
    auto pos = m_keyboardMap.find(static_cast<SDL_KeyboardID>(keyboardId));
    //if the keyboard is not known, stop
    if (pos == m_keyboardMap.end()) {return;}

    //remove the keyboard to the instance
    getInstance()->getInstance()->removeKeyboard(pos->second);
}

void GLGE::Graphic::Backend::Video::SDL3::Instance::onWindowRegister(GLGE::Graphic::Backend::Video::Window* window) {
    GLGE_PROFILER_SCOPE();

    //get write ownership of the window list
    std::unique_lock lock (msl_windowMutex);

    //get the SDL window id
    //safe to cast to SDL window as this must be an SDL window, else something went terribly wrong
    SDL_Window* win = (SDL_Window*)((GLGE::Graphic::Backend::Video::SDL3::Window*)window)->getSDLWindow();
    //get the window ID
    u32 id = SDL_GetWindowID(win);
    //store the window entry
    msl_windows.insert_or_assign(id, window);
}

void GLGE::Graphic::Backend::Video::SDL3::Instance::onWindowRemove(GLGE::Graphic::Backend::Video::Window* window) {
    GLGE_PROFILER_SCOPE();

    //get write ownership of the window list
    std::unique_lock lock (msl_windowMutex);

    //get the SDL window id
    //safe to cast to SDL window as this must be an SDL window, else something went terribly wrong
    SDL_Window* win = (SDL_Window*)((GLGE::Graphic::Backend::Video::SDL3::Window*)window)->getSDLWindow();
    //get the window ID
    u32 id = SDL_GetWindowID(win);
    //remove the window entry
    msl_windows.erase(id);
}

void* GLGE::Graphic::Backend::Video::SDL3::Instance::onContextCreate(GLGE::Graphic::Backend::Video::Window* window) {
    GLGE_PROFILER_SCOPE();

    //switch over the API
    switch (getAPI()) {
    case GLGE::Graphic::GraphicAPI::OPEN_GL: {
        //set the version correctly
        SDL_GL_SetAttribute(SDL_GL_CONTEXT_MAJOR_VERSION, getAPIVersion().getMajor());
        SDL_GL_SetAttribute(SDL_GL_CONTEXT_MINOR_VERSION, getAPIVersion().getMinor());
        SDL_GL_SetAttribute(SDL_GL_CONTEXT_PROFILE_MASK, (getAPIVersion().getPatch() == 1) ? SDL_GL_CONTEXT_PROFILE_COMPATIBILITY : SDL_GL_CONTEXT_PROFILE_CORE);

        //request acceleration
        SDL_GL_SetAttribute(SDL_GL_ACCELERATED_VISUAL, 1);

        //create and return the context
        SDL_GLContext context = SDL_GL_CreateContext((SDL_Window*)((GLGE::Graphic::Backend::Video::SDL3::Window*)window)->getSDLWindow());

        return context;
    }
    break;
    case GLGE::Graphic::GraphicAPI::OPEN_GL_ES: {
        //set the version correctly
        SDL_GL_SetAttribute(SDL_GL_CONTEXT_MAJOR_VERSION, getAPIVersion().getMajor());
        SDL_GL_SetAttribute(SDL_GL_CONTEXT_MINOR_VERSION, getAPIVersion().getMinor());
        SDL_GL_SetAttribute(SDL_GL_CONTEXT_PROFILE_MASK,  SDL_GL_CONTEXT_PROFILE_ES);
        //create and return the context
        return SDL_GL_CreateContext((SDL_Window*)((GLGE::Graphic::Backend::Video::SDL3::Window*)window)->getSDLWindow());
    }
    break;
    case GLGE::Graphic::GraphicAPI::VULKAN: {
        //just do nothing, but it is supported but does not require an instance
        return nullptr;
    }
    break;
    
    default: {
        //throw an exception
        throw GLGE::Exception("Found an unsupported API while trying to create a new context", "GLGE::Graphic::Backend::Video::SDL3::Instance::onContextCreate");
        //and return null
        return nullptr;
    }
    break;
    }
}

void GLGE::Graphic::Backend::Video::SDL3::Instance::onContextDestroy(void* context) {
    GLGE_PROFILER_SCOPE();

    //switch over the API to correctly destroy the context
    switch (getAPI()) {
    case GLGE::Graphic::GraphicAPI::OPEN_GL:
    case GLGE::Graphic::GraphicAPI::OPEN_GL_ES: {
        //destroy the OpenGL context
        SDL_GL_MakeCurrent(SDL_GL_GetCurrentWindow(), nullptr);
        SDL_GL_DestroyContext((SDL_GLContext)context);
    }
    break;
    
    default:
        //some APIs never required a context
        break;
    }
}
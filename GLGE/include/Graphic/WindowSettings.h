/**
 * @file WindowSettings.h
 * @author DM8AT
 * @brief define a structure that packs settings for a window into a bitmap
 * 
 * This structure is used to request window settings on startup and to report on dynamic setting changes
 * 
 * @version 0.1
 * @date 2026-01-02
 * 
 * @copyright Copyright (c) 2026
 * 
 */
//header guard
#ifndef _GLGE_GRAPHIC_WINDOW_SETTINGS_
#define _GLGE_GRAPHIC_WINDOW_SETTINGS_

//add common stuff
#include "Core/Common.h"

//use the library namespace
namespace GLGE::Graphic {

    /**
     * @brief store the settings for a window
     * 
     * All settings are represented as bits inside a 32-bit bitmask.
     * This allows compact storage while providing fast accessors.
     */
    class WindowSettings {
    public:

        /**
         * @brief Construct default window settings
         * 
         * This are pre-defined settings that are the same for all video backends
         * 
         * By default a window is resizable, shown and focussed. 
         */
        constexpr WindowSettings() noexcept
            : m_flags(
                  (false ? FULLSCREEN             : 0) |
                  (false ? HIDDEN                 : 0) |
                  (false ? BORDERLESS             : 0) |
                  (true  ? RESIZABLE              : 0) |
                  (false ? MINIMIZED              : 0) |
                  (false ? MAXIMIZED              : 0) |
                  (false ? MOUSE_GRABBED          : 0) |
                  (true  ? INPUT_FOCUS            : 0) |
                  (true  ? MOUSE_FOCUS            : 0) |
                  (false ? MODAL                  : 0) |
                  (true  ? HIGH_DPI               : 0) |
                  (false ? MOUSE_CAPTURE          : 0) |
                  (false ? ALWAYS_ON_TOP          : 0) |
                  (false ? UTILITY                : 0) |
                  (false ? TOOLTIP                : 0) |
                  (false ? POPUP_MENU             : 0) |
                  (false ? KEYBOARD_GRABBED       : 0) |
                  (false ? TRANSPARENT_BACKGROUND : 0) |
                  (false ? CAN_NOT_FOCUS          : 0) |
                  (false ? RELATIVE_MOUSE_MODE    : 0) |
                  (false ? HDR                    : 0))
        {}

        /**
         * @brief get if the window is in fullscreen mode
         * 
         * @return `true` the window is in fullscreen mode
         */
        constexpr inline bool fullscreen() const noexcept 
        {return get(FULLSCREEN);}
        /**
         * @brief set if the window is (or should be) in fullscreen mode
         * 
         * @param v `true` to request fullscreen mode, `false` otherwise
         */
        constexpr inline void fullscreen(bool v) noexcept 
        {set(FULLSCREEN, v);}

        /**
         * @brief get if the window is hidden
         * 
         * @return `true` the window is hidden, `false` otherwise
         */
        constexpr inline bool hidden() const noexcept
        {return get(HIDDEN);}
        /**
         * @brief set if the window is hidden
         * 
         * @param v `true` to hide the window, `false` otherwise
         */
        constexpr inline void hidden(bool v) noexcept
        {set(HIDDEN, v);}

        /**
         * @brief get if the window is borderless
         * 
         * @return `true` the window is borderless, `false` otherwise
         */
        constexpr inline bool borderless() const noexcept
        {return get(BORDERLESS);}
        /**
         * @brief set if the window should be borderless
         * 
         * @param v `true` to request borderless mode, `false` otherwise
         */
        constexpr inline void borderless(bool v) noexcept
        {set(BORDERLESS, v);}

        /**
         * @brief get if the window is resizable
         * 
         * @return `true` the window can be resized, `false` otherwise
         */
        constexpr inline bool resizable() const noexcept
        {return get(RESIZABLE);}
        /**
         * @brief set if the window can be resized
         * 
         * @param v `true` to allow resizing, `false` otherwise
         */
        constexpr inline void resizable(bool v) noexcept
        {set(RESIZABLE, v);}

        /**
         * @brief get if the window is minimized
         * 
         * @return `true` the window is minimized, `false` otherwise
         */
        constexpr inline bool minimized() const noexcept
        {return get(MINIMIZED);}
        /**
         * @brief set if the window should be minimized
         * 
         * @param v `true` to minimize the window, `false` otherwise
         */
        constexpr inline void minimized(bool v) noexcept
        {set(MINIMIZED, v);}

        /**
         * @brief get if the window is maximized
         * 
         * @return `true` the window is maximized, `false` otherwise
         */
        constexpr inline bool maximized() const noexcept
        {return get(MAXIMIZED);}
        /**
         * @brief set if the window should be maximized
         * 
         * @param v `true` to maximize the window, `false` otherwise
         */
        constexpr inline void maximized(bool v) noexcept
        {set(MAXIMIZED, v);}

        /**
         * @brief get if the mouse is grabbed by the window
         * 
         * @return `true` the mouse cannot leave the window, `false` if it can
         */
        constexpr inline bool mouse_grabbed() const noexcept
        {return get(MOUSE_GRABBED);}
        /**
         * @brief set if the mouse should be grabbed
         * 
         * @param v `true` to enable grabbing, `false` otherwise
         */
        constexpr inline void mouse_grabbed(bool v) noexcept
        {set(MOUSE_GRABBED, v);}

        /**
         * @brief get if the window has keyboard focus
         * 
         * @return `true` the window has input focus, `false` otherwise
         */
        constexpr inline bool input_focus() const noexcept
        {return get(INPUT_FOCUS);}
        /**
         * @brief set if the window has keyboard focus
         * 
         * @param v `true` to give keyboard focus, `false` otherwise
         */
        constexpr inline void input_focus(bool v) noexcept
        {set(INPUT_FOCUS, v);}

        /**
         * @brief get if the window has mouse focus
         * 
         * @return `true` the window has mouse focus, `false` otherwise
         */
        constexpr inline bool mouse_focus() const noexcept
        {return get(MOUSE_FOCUS);}
        /**
         * @brief set if the window has mouse focus
         * 
         * @param v `true` to give mouse focus, `false` otherwise
         */
        constexpr inline void mouse_focus(bool v) noexcept
        {set(MOUSE_FOCUS, v);}

        /**
         * @brief get if the window is modal
         * 
         * @return `true` the window is modal, `false` otherwise
         */
        constexpr inline bool modal() const noexcept
        {return get(MODAL);}
        /**
         * @brief set if the window is modal
         * 
         * @param v `true` for modal behavior, `false` otherwise
         */
        constexpr inline void modal(bool v) noexcept
        {set(MODAL, v);}

        /**
         * @brief get if the window has high DPI mode enabled
         * 
         * @return `true` the window uses high DPI, `false` otherwise
         */
        constexpr inline bool high_DPI() const noexcept
        {return get(HIGH_DPI);}
        /**
         * @brief set if the window should use high DPI
         * 
         * @param v `true` to request high DPI mode, `false` otherwise
         */
        constexpr inline void high_DPI(bool v) noexcept
        {set(HIGH_DPI, v);}

        /**
         * @brief get if the mouse is captured
         * 
         * @return `true` the mouse is captured, `false` otherwise
         */
        constexpr inline bool mouse_capture() const noexcept
        {return get(MOUSE_CAPTURE);}
        /**
         * @brief set if the mouse should be captured
         * 
         * @param v `true` to capture mouse, `false` otherwise
         */
        constexpr inline void mouse_capture(bool v) noexcept
        {set(MOUSE_CAPTURE, v);}

        /**
         * @brief get if the window is always on top
         * 
         * @return `true` window stays above all others
         */
        constexpr inline bool always_on_top() const noexcept
        {return get(ALWAYS_ON_TOP);}
        /**
         * @brief set if the window should be on top
         * 
         * @param v `true` to keep it on top, `false` otherwise
         */
        constexpr inline void always_on_top(bool v) noexcept
        {set(ALWAYS_ON_TOP, v);}

        /**
         * @brief get if the window is a utility window
         * 
         * @return `true` the window is a utility window
         */
        constexpr inline bool utility() const noexcept
        {return get(UTILITY);}
        /**
         * @brief set if the window is a utility window
         * 
         * @param v `true` for utility window type, `false` otherwise
         */
        constexpr inline void utility(bool v) noexcept
        {set(UTILITY, v);}

        /**
         * @brief get if the window is a tooltip
         * 
         * @return `true` the window is a tooltip
         */
        constexpr inline bool tooltip() const noexcept
        {return get(TOOLTIP);}
        /**
         * @brief set if the window is a tooltip
         * 
         * @param v `true` for tooltip type, `false` otherwise
         */
        constexpr inline void tooltip(bool v) noexcept
        {set(TOOLTIP, v);}

        /**
         * @brief get if the window is a popup menu
         * 
         * @return `true` the window is a popup menu
         */
        constexpr inline bool popup_menu() const noexcept
        {return get(POPUP_MENU);}
        /**
         * @brief set if the window is a popup menu
         * 
         * @param v `true` for popup menu type, `false` otherwise
         */
        constexpr inline void popup_menu(bool v) noexcept
        {set(POPUP_MENU, v);}

        /**
         * @brief get if keyboard input is grabbed
         * 
         * @return `true` the keyboard is grabbed
         */
        constexpr inline bool keyboard_grabbed() const noexcept
        {return get(KEYBOARD_GRABBED);}
        /**
         * @brief set if keyboard input should be grabbed
         * 
         * @param v `true` to grab keyboard input, `false` otherwise
         */
        constexpr inline void keyboard_grabbed(bool v) noexcept
        {set(KEYBOARD_GRABBED, v);}

        /**
         * @brief get if the window uses a transparent framebuffer
         * 
         * @return `true` the window has transparent framebuffer
         */
        constexpr inline bool transparent() const noexcept
        {return get(TRANSPARENT_BACKGROUND);}
        /**
         * @brief set if the window should use a transparent framebuffer
         * 
         * @param v `true` to enable transparency, `false` otherwise
         */
        constexpr inline void transparent(bool v) noexcept
        {set(TRANSPARENT_BACKGROUND, v);}

        /**
         * @brief get if the window cannot be focused
         * 
         * @return `true` the window cannot receive focus
         */
        constexpr inline bool can_not_focus() const noexcept
        {return get(CAN_NOT_FOCUS);}
        /**
         * @brief set if the window cannot be focused
         * 
         * @param v `true` to disable focus, `false` otherwise
         */
        constexpr inline void can_not_focus(bool v) noexcept
        {set(CAN_NOT_FOCUS, v);}

        /**
         * @brief get if the window runs in relative mouse mode
         * 
         * @return `true` the window uses relative mouse mode
         */
        constexpr inline bool relative() const noexcept
        {return get(RELATIVE_MOUSE_MODE);}
        /**
         * @brief set if the window uses relative mouse mode
         * 
         * @param v `true` to enable relative mode, `false` otherwise
         */
        constexpr inline void relative(bool v) noexcept
        {set(RELATIVE_MOUSE_MODE, v);}

        /**
         * @brief get if the window uses HDR mode
         * 
         * @return `true` HDR mode is active
         */
        constexpr inline bool hdr() const noexcept
        {return get(HDR);}
        /**
         * @brief set if the window uses HDR mode
         * 
         * @param v `true` to request HDR mode, `false` otherwise
         */
        constexpr inline void hdr(bool v) noexcept
        {set(HDR, v);}

        /**
         * @brief get the raw flag bitmask
         * 
         * @return `u32` the mask value
         */
        constexpr inline u32 mask() const noexcept { return m_flags; }

        /**
         * @brief set the raw flag bitmask
         * 
         * @param m the mask to set
         */
        constexpr inline void mask(u32 m) noexcept { m_flags = m; }

    private:

        /**
         * @brief get a bit inside the bitmask
         * 
         * @param bit the bitmask to query
         * @return `true` if the bit is set, `false` otherwise
         */
        constexpr inline bool get(u32 bit) const noexcept {
            return (m_flags & bit) ? true : false;
        }

        /**
         * @brief set or clear a bit inside the bitmask
         * 
         * @param bit the bitmask to modify
         * @param value `true` to set the bit, `false` to clear
         */
        constexpr inline void set(u32 bit, bool value) noexcept {
            m_flags = value ? (m_flags | bit) : (m_flags & ~bit);
        }

        /**
         * @brief flag for fullscreen mode 
         */
        inline constexpr static u32 FULLSCREEN             = 1u << 0;
        /**
         * @brief flag for window hidden state 
         */
        inline constexpr static u32 HIDDEN                 = 1u << 1;
        /**
         * @brief flag for borderless window mode 
         */
        inline constexpr static u32 BORDERLESS             = 1u << 2;
        /**
         * @brief flag for resizable window 
         */
        inline constexpr static u32 RESIZABLE              = 1u << 3;
        /**
         * @brief flag for minimized window 
         */
        inline constexpr static u32 MINIMIZED              = 1u << 4;
        /**
         * @brief flag for maximized window 
         */
        inline constexpr static u32 MAXIMIZED              = 1u << 5;
        /**
         * @brief flag for mouse being grabbed 
         */
        inline constexpr static u32 MOUSE_GRABBED          = 1u << 6;
        /**
         * @brief flag for keyboard input focus 
         */
        inline constexpr static u32 INPUT_FOCUS            = 1u << 7;
        /**
         * @brief flag for mouse input focus 
         */
        inline constexpr static u32 MOUSE_FOCUS            = 1u << 8;
        /**
         * @brief flag for modal window state 
         */
        inline constexpr static u32 MODAL                  = 1u << 9;
        /**
         * @brief flag for high DPI mode 
         */
        inline constexpr static u32 HIGH_DPI               = 1u << 10;
        /**
         * @brief flag for mouse capture 
         */
        inline constexpr static u32 MOUSE_CAPTURE          = 1u << 11;
        /**
         * @brief flag for always-on-top state 
         */
        inline constexpr static u32 ALWAYS_ON_TOP          = 1u << 12;
        /**
         * @brief flag for utility window type 
         */
        inline constexpr static u32 UTILITY                = 1u << 13;
        /**
         * @brief flag for tooltip window type 
         */
        inline constexpr static u32 TOOLTIP                = 1u << 14;
        /**
         * @brief flag for popup menu window type 
         */
        inline constexpr static u32 POPUP_MENU             = 1u << 15;
        /**
         * @brief flag for keyboard input capture 
         */
        inline constexpr static u32 KEYBOARD_GRABBED       = 1u << 16;
        /**
         * @brief flag for transparent framebuffer 
         */
        inline constexpr static u32 TRANSPARENT_BACKGROUND = 1u << 17;
        /**
         * @brief flag for non-focusable window 
         */
        inline constexpr static u32 CAN_NOT_FOCUS          = 1u << 18;
        /**
         * @brief flag for relative mouse mode 
         */
        inline constexpr static u32 RELATIVE_MOUSE_MODE    = 1u << 19;
        /**
         * @brief flag for HDR mode 
         */
        inline constexpr static u32 HDR                    = 1u << 20;

        /**
         * @brief store all settings in a single bitmask
         */
        u32 m_flags = 0;

    };

}

#endif
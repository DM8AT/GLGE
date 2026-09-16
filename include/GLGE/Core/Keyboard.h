/**
 * @file Keyboard.h
 * @author DM8AT
 * @brief define all keyboard related structures
 * @version 0.1
 * @date 2026-04-06
 * 
 * @copyright Copyright (c) 2026
 * 
 */
//header guard
#ifndef _GLGE_KEYS_
#define _GLGE_KEYS_

//add common stuff
#include "Common.h"

//use the library namespace
namespace GLGE {
    
    /**
     * @brief store a enum that identifies all keys
     */
    enum class Key : u8 {
        /**
         * @brief a undefined / start value
         * 
         * Use this if a key is not set or as a NULL key
         */
        UNDEFINED = 0,

        //------------------------------------------------------------
        //Latin letter keys
        //------------------------------------------------------------

        /**
         * @brief a value that represents the A key
         */
        A = 'a',
        /**
         * @brief a value that represents the B key
         */
        B = 'b',
        /**
         * @brief a value that represents the C key
         */
        C = 'c',
        /**
         * @brief a value that represents the D key
         */
        D = 'd',
        /**
         * @brief a value that represents the E key
         */
        E = 'e',
        /**
         * @brief a value that represents the F key
         */
        F = 'f',
        /**
         * @brief a value that represents the G key
         */
        G = 'g',
        /**
         * @brief a value that represents the H key
         */
        H = 'h',
        /**
         * @brief a value that represents the I key
         */
        I = 'i',
        /**
         * @brief a value that represents the J key
         */
        J = 'j',
        /**
         * @brief a value that represents the K key
         */
        K = 'k',
        /**
         * @brief a value that represents the L key
         */
        L = 'l',
        /**
         * @brief a value that represents the M key
         */
        M = 'm',
        /**
         * @brief a value that represents the N key
         */
        N = 'n',
        /**
         * @brief a value that represents the O key
         */
        O = 'o',
        /**
         * @brief a value that represents the P key
         */
        P = 'p',
        /**
         * @brief a value that represents the Q key
         */
        Q = 'q',
        /**
         * @brief a value that represents the R key
         */
        R = 'r',
        /**
         * @brief a value that represents the S key
         */
        S = 's',
        /**
         * @brief a value that represents the T key
         */
        T = 't',
        /**
         * @brief a value that represents the U key
         */
        U = 'u',
        /**
         * @brief a value that represents the V key
         */
        V = 'v',
        /**
         * @brief a value that represents the W key
         */
        W = 'w',
        /**
         * @brief a value that represents the X key
         */
        X = 'x',
        /**
         * @brief a value that represents the Y key
         */
        Y = 'y',
        /**
         * @brief a value that represents the Z key
         */
        Z = 'z',

        //------------------------------------------------------------
        //Digits
        //------------------------------------------------------------

        /**
         * @brief a value that represents the number 0 key
         */
        Num0 = '0',
        /**
         * @brief a value that represents the number 1 key
         */
        Num1 = '1',
        /**
         * @brief a value that represents the number 2 key
         */
        Num2 = '2',
        /**
         * @brief a value that represents the number 3 key
         */
        Num3 = '3',
        /**
         * @brief a value that represents the number 4 key
         */
        Num4 = '4',
        /**
         * @brief a value that represents the number 5 key
         */
        Num5 = '5',
        /**
         * @brief a value that represents the number 6 key
         */
        Num6 = '6',
        /**
         * @brief a value that represents the number 7 key
         */
        Num7 = '7',
        /**
         * @brief a value that represents the number 8 key
         */
        Num8 = '8',
        /**
         * @brief a value that represents the number 9 key
         */
        Num9 = '9',

        //------------------------------------------------------------
        //Printable symbols
        //------------------------------------------------------------
        
        /**
         * @brief a value that represents the space key
         */
        Space          = ' ',
        /**
         * @brief a value that represents the exclamation symbol key
         */
        Exclamation    = '!',
        /**
         * @brief a value that represents the quote key
         */
        Quote          = '"',
        /**
         * @brief a value that represents the hash key
         */
        Hash           = '#',
        /**
         * @brief a value that represents the dollar key
         */
        Dollar         = '$',
        /**
         * @brief a value that represents the percent key
         */
        Percent        = '%',
        /**
         * @brief a value that represents the ampersand key
         */
        Ampersand      = '&',
        /**
         * @brief a value that represents the apostrophe key
         */
        Apostrophe     = '\'',
        /**
         * @brief a value that represents the left prentices key
         */
        LeftPrentices  = '(',
        /**
         * @brief a value that represents the right prentices key
         */
        RightPrentices = ')',
        /**
         * @brief a value that represents the asterisk key
         */
        Asterisk       = '*',
        /**
         * @brief a value that represents the plus key
         */
        Plus           = '+',
        /**
         * @brief a value that represents the comma key
         */
        Comma          = ',',
        /**
         * @brief a value that represents the minus key
         */
        Minus          = '-',
        /**
         * @brief a value that represents the period key
         */
        Period         = '.',
        /**
         * @brief a value that represents the slash key
         */
        Slash          = '/',
        /**
         * @brief a value that represents the colon key
         */
        Colon          = ':',
        /**
         * @brief a value that represents the semicolon key
         */
        Semicolon      = ';',
        /**
         * @brief a value that represents the less key
         */
        Less           = '<',
        /**
         * @brief a value that represents the equal key
         */
        Equal          = '=',
        /**
         * @brief a value that represents the greater key
         */
        Greater        = '>',
        /**
         * @brief a value that represents the question mark key
         */
        Question       = '?',
        /**
         * @brief a value that represents the at key
         */
        At             = '@',
        /**
         * @brief a value that represents the left  key
         */
        LeftBracket    = '[',
        /**
         * @brief a value that represents the backslash key
         */
        Backslash      = '\\',
        /**
         * @brief a value that represents the right bracket key
         */
        RightBracket   = ']',
        /**
         * @brief a value that represents the left bracket key
         */
        Caret          = '^',
        /**
         * @brief a value that represents the underscore key
         */
        Underscore     = '_',
        /**
         * @brief a value that represents the grave key
         */
        Grave          = '`',
        /**
         * @brief a value that represents the left brace key
         */
        LeftBrace      = '{',
        /**
         * @brief a value that represents the pipe key
         */
        Pipe           = '|',
        /**
         * @brief a value that represents the right brace key
         */
        RightBrace     = '}',
        /**
         * @brief a value that represents the tilde key
         */
        Tilde          = '~',

        //------------------------------------------------------------
        //Start of non-printable key range
        //Value 127 is the last ASCII character used above.
        //------------------------------------------------------------

        /**
         * @brief a value that represents the maximum UTF-8 value and the start of the non-pritable enum values
         */
        _StartNonPrintable = 127,

        //------------------------------------------------------------
        //Control keys
        //------------------------------------------------------------

        /**
         * @brief a value that represents the enter key
         */
        Enter = _StartNonPrintable,
        /**
         * @brief a value that represents the escape key
         */
        Escape,
        /**
         * @brief a value that represents the backspace key
         */
        Backspace,
        /**
         * @brief a value that represents the tab key
         */
        Tab,

        //------------------------------------------------------------
        //Navigation
        //------------------------------------------------------------

        /**
         * @brief a value that represents the insert key
         */
        Insert,
        /**
         * @brief a value that represents the delete key
         */
        Delete,
        /**
         * @brief a value that represents the home key
         */
        Home,
        /**
         * @brief a value that represents the end key
         */
        End,
        /**
         * @brief a value that represents the page up key
         */
        PageUp,
        /**
         * @brief a value that represents the page down key
         */
        PageDown,
        /**
         * @brief a value that represents the arrow up key
         */
        ArrowUp,
        /**
         * @brief a value that represents the arrow down key
         */
        ArrowDown,
        /**
         * @brief a value that represents the arrow left key
         */
        ArrowLeft,
        /**
         * @brief a value that represents the arrow right key
         */
        ArrowRight,

        //------------------------------------------------------------
        //Function keys
        //------------------------------------------------------------

        /**
         * @brief a value that represents the F1 key
         */
        F1, 
        /**
         * @brief a value that represents the F2 key
         */
        F2,
        /**
         * @brief a value that represents the F3 key
         */
        F3, 
        /**
         * @brief a value that represents the F4 key
         */
        F4, 
        /**
         * @brief a value that represents the F5 key
         */
        F5, 
        /**
         * @brief a value that represents the F6 key
         */
        F6,
        /**
         * @brief a value that represents the F7 key
         */
        F7, 
        /**
         * @brief a value that represents the F8 key
         */
        F8,
        /**
         * @brief a value that represents the F9 key
         */
        F9, 
        /**
         * @brief a value that represents the F10 key
         */
        F10,
        /**
         * @brief a value that represents the F11 key
         */
        F11, 
        /**
         * @brief a value that represents the F12 key
         */
        F12, 
        /**
         * @brief a value that represents the F13 key
         */
        F13, 
        /**
         * @brief a value that represents the F14 key
         */
        F14, 
        /**
         * @brief a value that represents the F15 key
         */
        F15,
        /**
         * @brief a value that represents the F16 key
         */
        F16, 
        /**
         * @brief a value that represents the F17 key
         */
        F17, 
        /**
         * @brief a value that represents the F18 key
         */
        F18, 
        /**
         * @brief a value that represents the F19 key
         */
        F19, 
        /**
         * @brief a value that represents the F20 key
         */
        F20,
        /**
         * @brief a value that represents the F21 key
         */
        F21, 
        /**
         * @brief a value that represents the F22 key
         */
        F22, 
        /**
         * @brief a value that represents the F23 key
         */
        F23, 
        /**
         * @brief a value that represents the F24 key
         */
        F24, 
        /**
         * @brief a value that represents the F25 key
         */
        F25,

        //------------------------------------------------------------
        //Modifiers
        //------------------------------------------------------------

        /**
         * @brief a value that represents the left shift key
         */
        LeftShift,
        /**
         * @brief a value that represents the right shift key
         */
        RightShift,
        /**
         * @brief a value that represents the left control key
         */
        LeftCtrl,
        /**
         * @brief a value that represents the right control key
         */
        RightCtrl,
        /**
         * @brief a value that represents the left alt key
         */
        LeftAlt,
        /**
         * @brief a value that represents the right alt key
         */
        RightAlt,
        /**
         * @brief a value that represents the left meta key
         */
        LeftMeta,
        /**
         * @brief a value that represents the right meta key
         */
        RightMeta,
        /**
         * @brief a value that represents the caps lock key
         */
        CapsLock,
        /**
         * @brief a value that represents the num lock key
         */
        NumLock,
        /**
         * @brief a value that represents the scroll lock key
         */
        ScrollLock,

        //------------------------------------------------------------
        //System keys
        //------------------------------------------------------------

        /**
         * @brief a value that represents the print screen key
         */
        PrintScreen,
        /**
         * @brief a value that represents the pause key
         */
        Pause,
        /**
         * @brief a value that represents the menu key
         */
        Menu,

        //------------------------------------------------------------
        //Media keys
        //------------------------------------------------------------

        /**
         * @brief a value that represents the volume up key
         */
        VolumeUp,
        /**
         * @brief a value that represents the volume down key
         */
        VolumeDown,
        /**
         * @brief a value that represents the volume mute key
         */
        VolumeMute,
        /**
         * @brief a value that represents the media next key
         */
        MediaNext,
        /**
         * @brief a value that represents the media previous key
         */
        MediaPrev,
        /**
         * @brief a value that represents the media stop key
         */
        MediaStop,
        /**
         * @brief a value that represents the media play / pause key
         */
        MediaPlayPause,
        /**
         * @brief a value that represents the brightness up key
         */
        BrightnessUp,
        /**
         * @brief a value that represents the brightness down key
         */
        BrightnessDown,

        /**
         * @brief a value that represents the maximum value of the key enum
         */
        ENUM_MAX = 0xff
    };

    /**
     * @brief store a keyboard
     */
    class Keyboard {
    public:

        /**
         * @brief define a type that stores the state of a keyboard
         */
        struct KeyState {
        public:

            /**
             * @brief Construct a new Key State
             */
            KeyState() = default;

            /**
             * @brief Construct a new Key State
             * 
             * Copy constructor
             */
            KeyState(const KeyState&) = default;

            /**
             * @brief Construct a new Key State
             * 
             * Move constructor
             */
            KeyState(KeyState&&) = default;

            /**
             * @brief Copy operator
             * 
             * @return `KeyState&` a reference to the keystate after copying
             */
            KeyState& operator=(const KeyState&) = default;

            /**
             * @brief Move operator
             * 
             * @return `KeyState&` a reference to the keystate after moving
             */
            KeyState& operator=(KeyState&&) = default;

            /**
             * @brief set a key of the keyboard
             * 
             * @param key the key to update the state for
             * @param state `true` if the key is pressed, `false` if the key is not pressed
             */
            inline void set(Key key, bool state) noexcept {
                //extract the byte and sub-byte index of the key
                u8 byte = static_cast<u8>(key)>>3;
                u8 idx = static_cast<u8>(key)-(byte<<3);
                //depending on the new state either set or erase the bit
                if (state) {m_state[byte] |= (1<<idx);}
                else       {m_state[byte] &= ~(1<<idx);}
            }

            /**
             * @brief set a key of the keyboard
             * 
             * @param key the key to update the state for
             * @param state `true` if the key is pressed, `false` if the key is not pressed
             */
            inline void set(u8 key, bool state) noexcept
            {set(static_cast<Key>(key), state);}

            /**
             * @brief get the state of a specific key
             * 
             * @param key the key to get
             * @return `true` if the key is pressed, `false` if the key is not pressed
             */
            inline bool get(Key key) const noexcept {
                //extract the byte and sub-byte index of the key
                u8 byte = static_cast<u8>(key)>>3;
                u8 idx = static_cast<u8>(key)-(byte<<3);
                //return if the bit is set
                return static_cast<bool>(m_state[byte] & (1<<idx));
            }

            /**
             * @brief get the state of a specific key
             * 
             * @param key the key to get
             * @return `true` if the key is pressed, `false` if the key is not pressed
             */
            inline bool get(u8 key) const noexcept
            {return get(static_cast<Key>(key));}

            /**
             * @brief get the state of a specific key
             * 
             * @param key the key to get
             * @return `true` if the key is pressed, `false` if the key is not pressed
             */
            inline bool operator[](Key key) const noexcept
            {return get(key);}

            /**
             * @brief get the state of a specific key
             * 
             * @param key the key to get
             * @return `true` if the key is pressed, `false` if the key is not pressed
             */
            inline bool operator[](u8 key) const noexcept
            {return get(static_cast<Key>(key));}

            /**
             * @brief fill the key state with a solid 0 or 1
             * 
             * @param state the state to fill the key state with
             */
            inline void fill(bool state) noexcept {
                //get a byte that is full or empty, depending on state
                u8 byte = state ? 0xff : 0;
                //set all bytes of the state to the full or empty byte
                memset(m_state, byte, sizeof(m_state));
            }

        protected:

            /**
             * @brief store a bitmask to store the state
             */
            u8 m_state[(static_cast<u8>(Key::ENUM_MAX) + 8)>>3]{};

        };

            /**
             * @brief Construct a new Keyboard
             * 
             * @param name the name of the keyboard
             */
            Keyboard(const std::string& name = "UNNAMED KEYBOARD")
             : m_name(name)
            {}

            /**
             * @brief Construct a new Keyboard
             * 
             * Copy constructor
             */
            Keyboard(const Keyboard&) = default;

            /**
             * @brief Construct a new Keyboard
             * 
             * Move constructor
             */
            Keyboard(Keyboard&&) = default;

            /**
             * @brief Copy operator
             * 
             * @return `Keyboard&` a reference to the keyboard after copying
             */
            Keyboard& operator=(const Keyboard&) = default;

            /**
             * @brief Move operator
             * 
             * @return `Keyboard&` a reference to the keyboard after moving
             */
            Keyboard& operator=(Keyboard&&) = default;

            /**
             * @brief register the press of a specific key
             * 
             * @param key the key to press
             */
            inline void press(Key key) noexcept {
                //register that the key is down
                m_thisTickDown.set(key, !m_pressed[key]);
                //the key is down, so it cannot be up
                //this may happen in the following order:
                //  Key update -> key released -> key pressed again -> key update
                //The update may be slower than the key registration
                m_thisTickUp.set(key, false);
            }

            /**
             * @brief register the press of a specific key
             * 
             * @param key the key to press
             */
            inline void press(char key) noexcept 
            {press(static_cast<Key>(key));}

            /**
             * @brief register the releasing of a specific key
             * 
             * @param key the key to release
             */
            inline void release(Key key) noexcept {
                //register that the key is up
                m_thisTickUp.set(key, m_pressed[key]);
                //the down must be set to false, details are explained in the press method
                m_thisTickDown.set(key, false);
            }

            /**
             * @brief register the releasing of a specific key
             * 
             * @param key the key to release
             */
            inline void release(char key) noexcept 
            {release(static_cast<Key>(key));}

            /**
             * @brief update the pressed key state
             */
            void update() noexcept;

            /**
             * @brief get the key state of the currently pressed keys
             * 
             * @return `const KeyState&` a constant reference to the key state of the pressed keys
             */
            inline const KeyState& pressed() const noexcept
            {return m_pressed;}

            /**
             * @brief get the key state of the keys that where pressed down this tick
             * 
             * @return `const KeyState&` a constant reference to the key state of the pressed down keys
             */
            inline const KeyState& down() const noexcept
            {return m_down;}

            /**
             * @brief get the key state of the keys that where released this tick
             * 
             * @return `const KeyState&` a constant reference to the key state of the released keys
             */
            inline const KeyState& up() const noexcept
            {return m_up;}

            /**
             * @brief fill the keyboard with a specific state
             * 
             * @param state `true` to press all keys, `false` to release all keys
             */
            inline void fill(bool state) noexcept {
                m_pressed.fill(state);
                m_down.fill(false);
                m_up.fill(false);
            }

            /**
             * @brief Get the name of the keyboard
             * 
             * @return `const std::string&` the name of the keyboard
             */
            inline const std::string& getName() const noexcept
            {return m_name;}

    protected:

        /**
         * @brief store the name of the keyboard
         */
        std::string m_name = "UNNAMED KEYBOARD";

        /**
         * @brief store the keys pressed down this tick
         */
        KeyState m_thisTickDown;
        /**
         * @brief store the keys lifted up this tick
         */
        KeyState m_thisTickUp;

        /**
         * @brief store a bitmask of all currently pressed keys
         */
        KeyState m_pressed;
        /**
         * @brief store all keys that were released in the LAST tick
         */
        KeyState m_up;
        /**
         * @brief store all keys that were pressed down in the LAST tick
         */
        KeyState m_down;

    };

}


#endif
/**
 * @file Mouse.h
 * @author DM8AT
 * @brief define all mouse related classes
 * @version 0.1
 * @date 2026-04-07
 * 
 * @copyright Copyright (c) 2026
 * 
 */
//header guard
#ifndef _GLGE_MOUSE_
#define _GLGE_MOUSE_

//add common stuff
#include "Common.h"

//use the library namespace
namespace GLGE {

    /**
     * @brief define an enum for all mouse buttons
     */
    enum class MouseButton : u8 {
        /**
         * @brief define an enum for the left mouse button
         */
        LEFT = 0,
        /**
         * @brief define an enum for the middle mouse button (the mouse wheel button)
         */
        MIDDLE = 1,
        /**
         * @brief define an enum for the right mouse button
         */
        RIGHT = 2,

        /**
         * @brief define the maximum legal mouse button
         */
        ENUM_MAX = 15
    };

    /**
     * @brief store the state of a mouse
     */
    class Mouse {
    public:

        /**
         * @brief define a type that stores the state of a mouse's buttons
         */
        struct ButtonState {
        public:

            /**
             * @brief Construct a new button state
             */
            ButtonState() = default;

            /**
             * @brief Construct a new button State
             * 
             * Copy constructor
             */
            ButtonState(const ButtonState&) = default;

            /**
             * @brief Construct a new button State
             * 
             * Move constructor
             */
            ButtonState(ButtonState&&) = default;

            /**
             * @brief Copy operator
             * 
             * @return `ButtonState&` a reference to the button state after copying
             */
            ButtonState& operator=(const ButtonState&) = default;

            /**
             * @brief Move operator
             * 
             * @return `ButtonState&` a reference to the button state after moving
             */
            ButtonState& operator=(ButtonState&&) = default;

            /**
             * @brief set a buttons of the mouse
             * 
             * @param button the buttons to update the state for
             * @param state `true` if the button is pressed, `false` if the button is not pressed
             */
            inline void set(MouseButton button, bool state) noexcept {
                //extract the byte and sub-byte index of the button
                u8 byte = static_cast<u8>(button)>>3;
                u8 idx = static_cast<u8>(button)-(byte<<3);
                //depending on the new state either set or erase the bit
                if (state) {m_state[byte] |= (1<<idx);}
                else       {m_state[byte] &= ~(1<<idx);}
            }

            /**
             * @brief set a button of the mouse
             * 
             * @param button the button to update the state for
             * @param state `true` if the button is pressed, `false` if the button is not pressed
             */
            inline void set(u8 button, bool state) noexcept
            {set(static_cast<MouseButton>(button), state);}

            /**
             * @brief get the state of a specific button
             * 
             * @param button the button to get
             * @return `true` if the button is pressed, `false` if the button is not pressed
             */
            inline bool get(MouseButton button) const noexcept {
                //extract the byte and sub-byte index of the button
                u8 byte = static_cast<u8>(button)>>3;
                u8 idx = static_cast<u8>(button)-(byte<<3);
                //return if the bit is set
                return static_cast<bool>(m_state[byte] & (1<<idx));
            }

            /**
             * @brief get the state of a specific button
             * 
             * @param button the button to get
             * @return `true` if the button is pressed, `false` if the button is not pressed
             */
            inline bool get(u8 button) const noexcept
            {return get(static_cast<MouseButton>(button));}

            /**
             * @brief get the state of a specific button
             * 
             * @param button the button to get
             * @return `true` if the button is pressed, `false` if the button is not pressed
             */
            inline bool operator[](MouseButton button) const noexcept
            {return get(button);}

            /**
             * @brief get the state of a specific button
             * 
             * @param button the button to get
             * @return `true` if the button is pressed, `false` if the button is not pressed
             */
            inline bool operator[](u8 button) const noexcept
            {return get(static_cast<MouseButton>(button));}

            /**
             * @brief fill the button state with a solid 0 or 1
             * 
             * @param state the state to fill the button state with
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
            u8 m_state[(static_cast<u8>(MouseButton::ENUM_MAX) + 8)>>3]{};

        };

        /**
         * @brief Construct a new Mouse
         * 
         * @param name the name of the mouse
         */
        Mouse(const std::string& name = "UNNAMED MOUSE")
         : m_name(name)
        {}

        /**
         * @brief Construct a new mouse
         * 
         * Copy constructor
         */
        Mouse(const Mouse&) = default;

        /**
         * @brief Construct a new mouse
         * 
         * Move constructor
         */
        Mouse(Mouse&&) = default;

        /**
         * @brief Copy operator
         * 
         * @return `Mouse&` a reference to the mouse after copying
         */
        Mouse& operator=(const Mouse&) = default;

        /**
         * @brief Move operator
         * 
         * @return `Mouse&` a reference to the mouse after moving
         */
        Mouse& operator=(Mouse&&) = default;

        /**
         * @brief register the press of a specific key
         * 
         * @param button the button to press
         */
        inline void press(MouseButton button) noexcept {
            //register that the button is down
            m_thisTickDown.set(button, !m_pressed[button]);
            //the button is down, so it cannot be up
            //this may happen in the following order:
            //  Button update -> button released -> button pressed again -> button update
            //The update may be slower than the button registration
            m_thisTickUp.set(button, false);
        }

        /**
         * @brief register the releasing of a specific button
         * 
         * @param button the button to release
         */
        inline void release(MouseButton button) noexcept {
            //register that the button is up
            m_thisTickUp.set(button, m_pressed[button]);
            //the down must be set to false, details are explained in the press method
            m_thisTickDown.set(button, false);
        }

        /**
         * @brief fill the mouse with a specific state
         * 
         * @param state `true` to press all buttons, `false` to release all buttons
         */
        inline void fill(bool state) noexcept {
            m_pressed.fill(state);
            m_down.fill(false);
            m_up.fill(false);
        }

        /**
         * @brief move the mouse
         * 
         * The delta input is manual to adapt correctly to non-delta movement (just parse 0,0 as the delta vector)
         * 
         * @param pos the position to move to
         * @param delta the delta vector done by the mouse
         */
        void move(const vec2& pos, const vec2& delta) 
        {m_pos = pos; m_delta = delta;}

        /**
         * @brief scroll the mouse wheel
         * 
         * @param wheel how much the mouse wheel is scrolled
         */
        void scroll(f32 wheel)
        {m_wheel += wheel;}

        /**
         * @brief update the pressed button state
         */
        void update() noexcept;

        /**
         * @brief get the button state of the currently pressed keys
         * 
         * @return `const ButtonState&` a constant reference to the button state of the pressed keys
         */
        inline const ButtonState& pressed() const noexcept
        {return m_pressed;}

        /**
         * @brief get the button state of the keys that where pressed down this tick
         * 
         * @return `const ButtonState&` a constant reference to the button state of the pressed down keys
         */
        inline const ButtonState& down() const noexcept
        {return m_down;}

        /**
         * @brief get the button state of the keys that where released this tick
         * 
         * @return `const ButtonState&` a constant reference to the button state of the released keys
         */
        inline const ButtonState& up() const noexcept
        {return m_up;}

        /**
         * @brief Get the position of the mouse
         * 
         * @return `const vec2&` the global position of the mouse
         */
        inline const vec2& getPos() const noexcept
        {return m_pos;}

        /**
         * @brief Get the movement delta of the mouse
         * 
         * @return `const vec2&` the movement delta of the mouse
         */
        inline const vec2& getDelta() const noexcept
        {return m_delta;}

        /**
         * @brief Get the wheel scrolling this tick
         * 
         * @return `f32` the wheel delta this tick
         */
        inline f32 getWheel() const noexcept
        {return m_wheel;}

        /**
         * @brief Get the name of the keyboard
         * 
         * @return `const std::string&` the name of the keyboard
         */
        inline const std::string& getName() const noexcept
        {return m_name;}

    protected:

        /**
         * @brief store the name of the mouse
         */
        std::string m_name = "UNNAMED MOUSE";

        /**
         * @brief store the global position of the mouse
         */
        vec2 m_pos;
        /**
         * @brief store how much the mouse is moved this tick
         * 
         * This is not effected by using the mouse warp function, just the move function
         */
        vec2 m_delta;
        /**
         * @brief store how much the mouse wheel is moved
         * 
         * Reset during the update method
         */
        f32 m_wheel;

        /**
         * @brief store the buttons that where pressed down this tick
         */
        ButtonState m_thisTickDown;
        /**
         * @brief store the buttons that where lifted up this tick
         */
        ButtonState m_thisTickUp;
        /**
         * @brief store the currently pressed buttons
         */
        ButtonState m_pressed;
        /**
         * @brief store the buttons that where pressed down in the LAST tick
         */
        ButtonState m_down;
        /**
         * @brief store the buttons that where lifted up in the LAST tick
         */
        ButtonState m_up;

    };

}

#endif
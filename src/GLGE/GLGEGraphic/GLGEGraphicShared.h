/**
 * @file GLGEGraphicShared.h
 * @author DM8AT
 * @brief store all the shared data for the graphic module
 * @version 0.1
 * @date 2025-04-13
 * 
 * @copyright Copyright (c) 2025
 * 
 */

//header guard
#ifndef _GLGE_GRAPHIC_SHARED_
#define _GLGE_GRAPHIC_SHARED_

//include the settings
#include "../GLGECore/GLGESettings.h"

//include windows
#include "GLGEWindow.h"
//include SDL2
#include <SDL2/SDL.h>

//check for C++
#ifdef GLGE_CPP

//store all the windows
extern std::unordered_map<uint32_t, Window*> __glge_all_windows_sdl;

//store the SDL2 thread
extern std::thread* __glge_sdl_2_thread;

//store the amount of open windows
extern uint32_t* __glge_all_window_count;

//store all instances
extern std::vector<Instance*>* __glge_all_instances;

//include events
#include "../GLGECore/GLGEEvent.h"

//define the identifyer used by window events for SDL events
#define GLGE_WINDOW_EVENT_SDL_EVENT 0

/**
 * @brief this is an event that is handled by windows for example to close it
 * 
 * @warning only available internaly
 */
class WindowEvent : public Event
{
public:

    /**
     * @brief Construct a new Window Event
     * 
     * @param _event a constant reference to the SDL event for the window to handle
     */
    WindowEvent(const SDL_Event& _event) : Event("Window Event", {GLGE_EVENT_IDENTIFYER_WINDOW, GLGE_WINDOW_EVENT_SDL_EVENT}),  event(_event) {}

    //only internaly available, no need to use protected. It makes access only harder

    /**
     * @brief store the SDL event to be handled by the window
     */
    SDL_Event event;
};


//define the event identifyers

#define GLGE_KEYBOARD_EVENT_KEY_STATE_CHANGE 0
#define GLGE_MOUSE_EVENT_MOUSE_BUTTON_STATE_CHANGE 1
#define GLGE_MOUSE_EVENT_MOUSE_MOVEMENT 2

//say that instances will exist
class Instance;

/**
 * @brief store an event for a window key update
 */
class KeyboardKeyEvent : public Event
{
public:

    /**
     * @brief Construct a new Keyboard Key Event
     * @warning explicetly deleted
     */
    KeyboardKeyEvent() = delete;

    /**
     * @brief Construct a new Keyboard Key Event
     * 
     * @param released determines if the key was pressed or released
     * @param scancode the scancode of the key to handle
     */
    KeyboardKeyEvent(bool released, uint32_t scancode)
     : Event("Key Event", {GLGE_EVENT_IDENTIFYER_INPUT, GLGE_KEYBOARD_EVENT_KEY_STATE_CHANGE}), m_released(released), m_key(scancode)
    {}

    /**
     * @brief check if the key was pressed or released
     * 
     * @return true : the key was pressed
     * @return false : the key was released
     */
    inline bool wasReleased() const noexcept {return m_released;}

    /**
     * @brief Get the key the event is about
     * 
     * @return const SDL_Keysym& a constant reference to the effected key
     */
    inline const uint32_t& getKey() const noexcept {return m_key;}

protected:

    /**
     * @brief store if the key was pressed or released
     */
    bool m_released;
    /**
     * @brief store the symbol of the pressed key
     */
    uint32_t m_key;

};

/**
 * @brief a layer that manages keyboard input for an instance
 */
class KeyboardEventLayer : public Layer
{
public:

    /**
     * @brief Construct a new Keyboard Event Layer
     * @warning explicetly deleted
     */
    KeyboardEventLayer() = delete;

    /**
     * @brief Construct a new Keyboard Event Layer
     * 
     * @param instance a pointer to the instance the keyboard event layer belongs to
     */
    KeyboardEventLayer(Instance* instance)
     : m_instance(instance)
    {}

    /**
     * @brief the function is called when an event is passed to the layer
     * 
     * @param event a pointer to the event that is being passed through the layer stack
     * 
     * @return true : the event was handled. This means the event will not being passed down further the layer stack
     * @return false : the event was not handled by this layer
     */
    virtual bool onEvent(Event* event) noexcept override;

protected:

    /**
     * @brief store the instance the keyboard event layer works with
     */
    Instance* m_instance;
};


/**
 * @brief store an event for a mouse
 */
class MouseEvent : public Event
{
public:

    /**
     * @brief Construct a new Mouse Event
     * @warning explicetly deleted
     */
    MouseEvent() = delete;

    /**
     * @brief Construct a new Mouse Button state change event
     * 
     * @param identifyer the identifyer of the mouse button
     * @param state the new state of the mouse button
     */
    MouseEvent(uint8_t identifyer, bool state)
     : Event("Mouse Button Event", {GLGE_EVENT_IDENTIFYER_INPUT, GLGE_MOUSE_EVENT_MOUSE_BUTTON_STATE_CHANGE}), data(ButtonPress(identifyer, state))
    {}

    /**
     * @brief Construct a new Mouse Motion event
     * 
     * @param pixel the pixel the mouse moved to
     */
    MouseEvent(uvec2 pixel)
     : Event("Mouse Motion Event", {GLGE_EVENT_IDENTIFYER_INPUT, GLGE_MOUSE_EVENT_MOUSE_MOVEMENT}), data(pixel)
    {}

//this stuff is used internaly, no need for protextion, it makes it harder
    
    /**
     * @brief store information about a button press
     */
    struct ButtonPress
    {
        /**
         * @brief store the identifyer of the mouse button
         */
        uint8_t identifyer;
        /**
         * @brief store the new state for the mouse button
         */
        bool state;

        /**
         * @brief Construct a new Button Press data instance
         * 
         * @param _identifyer the identifyer of the button that changed
         * @param _state the new state of the button
         */
        ButtonPress(uint8_t _identifyer, bool _state)
         : identifyer(_identifyer), state(_state)
        {}
    };

    /**
     * @brief store the data attatched to this event
     */
    union Data
    {
        /**
         * @brief store information for the button press
         */
        ButtonPress button;
        /**
         * @brief store the pixel focosed by the mouse in the SDL coordinate space
         */
        uvec2 pixel;

        /**
         * @brief Construct some new data
         * 
         * @param _button the button press state change data
         */
        Data(const ButtonPress& _button) : button(_button) {}

        /**
         * @brief Construct some new data
         * 
         * @param _pixel the pixel the mouse moved to
         */
        Data(const uvec2& _pixel) : pixel(_pixel) {}
    } data;

};

/**
 * @brief store the event layer that manages the mouse input
 */
class MouseEventLayer : public Layer
{
public:

    /**
     * @brief Construct a new Mouse Event Layer
     * @warning explicetly deleted
     */
    MouseEventLayer() = delete;

    /**
     * @brief Construct a new Mouse Event Layer
     * 
     * @param instance a pointer to the instance the mouse event layer is bound to
     */
    MouseEventLayer(Instance* instance)
     : m_instance(instance)
    {}

    /**
     * @brief the function is called when an event is passed to the layer
     * 
     * @param event a pointer to the event that is being passed through the layer stack
     * 
     * @return true : the event was handled. This means the event will not being passed down further the layer stack
     * @return false : the event was not handled by this layer
     */
    virtual bool onEvent(Event* event) noexcept override;

protected:

    /**
     * @brief store a pointer to the instance the layer is bound to
     */
    Instance* m_instance;
};

#endif //end of C++ section

#endif
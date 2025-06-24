/**
 * @file GLGEKeyboard.h
 * @author DM8AT
 * @brief store informatoin about keyboard states
 * @version 0.1
 * @date 2025-06-21
 * 
 * @copyright Copyright (c) 2025
 * 
 */

//header guard
#ifndef _GLGE_KEYBOARD_
#define _GLGE_KEYBOARD_

//include the settings
#include "../../GLGECore/GLGESettings.h"
//include layers, layer stacks and events
#include "../../GLGECore/GLGELayers.h"

/**
 * @brief store information about the pressed keys
 */
typedef struct s_Keys
{
    //create a bool for all letters in the alphabet
    bool a,b,c,d,e,f,g,h,i,j,k,l,m,n,o,
         p,q,r,s,t,u,v,w,x,y,z;
    //create a bool for all special keys
    bool f1,f2,f3,f4,f5,f6,f7,f8,f9,f10,f11,f12, space, 
         leftShift, rightShift, leftAlt, rightAlt, shift, alt, arrowRight, arrowLeft, 
         arrowUp, arrowDown, 
         key1, key2, key3, key4, key5, key6, key7, key8, key9, key0, 
         backspace, enter, tab, capslockKey, dot, comma, dash, plus, hash;
        
    //store if capslock is active
    bool capslock;

    //check for C++
    #if GLGE_CPP

    /**
     * @brief Construct a new Keys 
     * default constructor
     */
    s_Keys() : a(false),b(false),c(false),d(false),e(false),f(false),g(false),h(false),i(false),j(false),k(false),l(false),m(false),n(false),o(false),
               p(false),q(false),r(false),s(false),t(false),u(false),v(false),w(false),x(false),y(false),z(false),
               //create a bool for all special keys
               f1(false),f2(false),f3(false),f4(false),f5(false),f6(false),f7(false),f8(false),f9(false),f10(false),f11(false),f12(false), space(false), 
               leftShift(false), rightShift(false), leftAlt(false), rightAlt(false), shift(false), alt(false), arrowRight(false), arrowLeft(false), 
               arrowUp(false), arrowDown(false), 
               key1(false), key2(false), key3(false), key4(false), key5(false), key6(false), key7(false), key8(false), key9(false), key0(false), 
               backspace(false), enter(false), tab(false), capslockKey(false), dot(false), comma(false), dash(false), plus(false), hash(false),
                
               //store if capslock is active
               capslock(false) {}

    /**
     * @brief update a special key
     * 
     * @param key the key to update
     * @param status the new status for the key
     */
    void keyUpdate(int key, bool status);

    /**
     * @brief set all keys to false
     */
    void clear();

    /**
     * @brief Get the status of one key
     * 
     * @param key the key to check
     * @return true : the key is pressed | 
     * @return false : the key is not pressed
     */
    bool getKey(unsigned char key);

    /**
     * @brief Get the status of a key
     * 
     * @param scancode the scancode for the key
     * @return true : the key is pressed |
     * @return false : the key is not pressed
     */
    bool getKey(uint32_t scancode);

    #endif //end of C++ section
} Keys;

/**
 * @brief store information about toggable keys like numpad and capslock
 */
typedef struct s_Keystates
{
    //store the numpad and capslock state
    bool numpad, capslock;

    //check for C++ to create constructors
    #if GLGE_CPP

    /**
     * @brief Construct new storage for the states of a keyboard's toggable keys
     */
    s_Keystates() : numpad(false), capslock(false) {}

    #endif //end of C++ section

} Keystates;

/**
 * @brief store a combination of a key state and the state of all keys
 */
typedef struct s_Keyboard 
{
    /**
     * @brief store the information about all keys
     */
    Keys keys;
    /**
     * @brief store the state of toggable keys
     */
    Keystates states;
} Keyboard;

#endif
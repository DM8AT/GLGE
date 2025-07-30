/**
 * @file GLGEKeyboard.cpp
 * @author DM8AT
 * @brief implment the functionality for the Keyboard and Keys
 * @version 0.1
 * @date 2025-06-21
 * 
 * @copyright Copyright (c) 2025
 * 
 */

//include the keyboard
#include "GLGEKeyboard.h"

//include SDL2 for the scancodes
#include <SDL2/SDL.h>

//update a key
void s_Keys::keyUpdate(int key, bool status)
{
    //update the key according to the key
    switch (key)
    {
        //check if the key is a
        case SDL_SCANCODE_A:
            //update a
            this->a = status;
            //exit
            break;
        //check if the key is b
        case SDL_SCANCODE_B:
            //update b
            this->b = status;
            //exit
            break;
        //check if the key is c
        case SDL_SCANCODE_C:
            //update c
            this->c = status;
            //exit
            break;
        //check if the key is d
        case SDL_SCANCODE_D:
            //update d
            this->d = status;
            //exit
            break;
        //check if the key is e
        case SDL_SCANCODE_E:
            //update e
            this->e = status;
            //exit
            break;
        //check if the key is f
        case SDL_SCANCODE_F:
            //update f
            this->f = status;
            //exit
            break;
        //check if the key is g
        case SDL_SCANCODE_G:
            //update g
            this->g = status;
            //exit
            break;
        //check if the key is h
        case SDL_SCANCODE_H:
            //update h
            this->h = status;
            //exit
            break;
        //check if the key is i
        case SDL_SCANCODE_I:
            //update i
            this->i = status;
            //exit
            break;
        //check if the key is j
        case SDL_SCANCODE_J:
            //update j
            this->j = status;
            //exit
            break;
        //check if the key is k
        case SDL_SCANCODE_K:
            //update k
            this->k = status;
            //exit
            break;
        //check if the key is l
        case SDL_SCANCODE_L:
            //update l
            this->l = status;
            //exit
            break;
        //check if the key is m
        case SDL_SCANCODE_M:
            //update m
            this->m = status;
            //exit
            break;
        //check if the key is n
        case SDL_SCANCODE_N:
            //update n
            this->n = status;
            //exit
            break;
        //check if the key is o
        case SDL_SCANCODE_O:
            //update o
            this->o = status;
            //exit
            break;
        //check if the key is p
        case SDL_SCANCODE_P:
            //update p
            this->p = status;
            //exit
            break;
        //check if the key is q
        case SDL_SCANCODE_Q:
            //update q
            this->q = status;
            //exit
            break;
        //check if the key is r
        case SDL_SCANCODE_R:
            //update r
            this->r = status;
            //exit
            break;
        //check if the key is s
        case SDL_SCANCODE_S:
            //update s
            this->s = status;
            //exit
            break;
        //check if the key is t
        case SDL_SCANCODE_T:
            //update t
            this->t = status;
            //exit
            break;
        //check if the key is u
        case SDL_SCANCODE_U:
            //update u
            this->u = status;
            //exit
            break;
        //check if the key is v
        case SDL_SCANCODE_V:
            //update v
            this->v = status;
            //exit
            break;
        //check if the key is w
        case SDL_SCANCODE_W:
            //update w
            this->w = status;
            //exit
            break;
        //check if the key is x
        case SDL_SCANCODE_X:
            //update x
            this->x = status;
            //exit
            break;
        //check if the key is y
        case SDL_SCANCODE_Y:
            //update y
            this->y = status;
            //exit
            break;
        //check if the key is z
        case SDL_SCANCODE_Z:
            //update z
            this->z = status;
            //exit
            break;
        //check if the key is space
        case SDL_SCANCODE_SPACE:
            //update space
            this->space = status;
            //exit
            break;
        //check if the key is f1
        case SDL_SCANCODE_F1:
            //update f1
            this->f1 = status;
            //exit
            break;
        //check if the key is f2
        case SDL_SCANCODE_F2:
            //update f2
            this->f2 = status;
            //exit
            break;
        //check if the key is f3
        case SDL_SCANCODE_F3:
            //update f3
            this->f3 = status;
            //exit
            break;
        //check if the key is f4
        case SDL_SCANCODE_F4:
            //update f4
            this->f4 = status;
            //exit
            break;
        //check if the key is f5
        case SDL_SCANCODE_F5:
            //update f5
            this->f5 = status;
            //exit
            break;
        //check if the key is f6
        case SDL_SCANCODE_F6:
            //update f6
            this->f6 = status;
            //exit
            break;
        //check if the key is f7
        case SDL_SCANCODE_F7:
            //update f7
            this->f7 = status;
            //exit
            break;
        //check if the key is f8
        case SDL_SCANCODE_F8:
            //update f8
            this->f8 = status;
            //exit
            break;
        //check if the key is f9
        case SDL_SCANCODE_F9:
            //update f9
            this->f9 = status;
            //exit
            break;
        //check if the key is f10
        case SDL_SCANCODE_F10:
            //update f10
            this->f10 = status;
            //exit
            break;
        //check if the key is f11
        case SDL_SCANCODE_F11:
            //update f11
            this->f11 = status;
            //exit
            break;
        //check if the key is f12
        case SDL_SCANCODE_F12:
            //update f12
            this->f12 = status;
            //exit
            break;
        //check if the key is r_shift
        case SDL_SCANCODE_RSHIFT:
            //update r_shift
            this->rightShift = status;
            //update the shift variable
            this->shift = (this->rightShift || this->leftShift);
            //exit
            break;
        //check if the key is l_shift
        case SDL_SCANCODE_LSHIFT:
            //update l_shift
            this->leftShift = status;
            //update the shift variable
            this->shift = (this->rightShift || this->leftShift);
            //exit
            break;
        //check if the right arrow key is pressed
        case SDL_SCANCODE_RIGHT:
            //update the right arrow
            this->arrowRight = status;
            //exit
            break;
        //check if the left arrow key is pressed
        case SDL_SCANCODE_LEFT:
            //update the left arrow
            this->arrowLeft = status;
            //exit
            break;
        //check if the up arrow key is pressed
        case SDL_SCANCODE_UP:
            //update the up arrow
            this->arrowUp = status;
            //exit
            break;
        //check if the down arrow key is pressed
        case SDL_SCANCODE_DOWN:
            //update the down arrow
            this->arrowDown = status;
            //exit
            break;
        //check if the right alt key is pressed
        case SDL_SCANCODE_RALT:
            //update the right arrow
            this->rightAlt = status;
            //update the alt variable
            this->alt = (this->rightAlt || this->leftAlt);
            //exit
            break;
        //check if the left alt key is pressed
        case SDL_SCANCODE_LALT:
            //update the left arrow
            this->leftAlt = status;
            //update the alt variable
            this->alt = (this->rightAlt || this->leftAlt);
            //exit
            break;
        //check if the number 0 is pressed
        case SDL_SCANCODE_0:
            //update the down arrow
            this->key0 = status;
            //exit
            break;
        //check if the number 1 is pressed
        case SDL_SCANCODE_1:
            //update the down arrow
            this->key1 = status;
            //exit
            break;
        //check if the number 2 is pressed
        case SDL_SCANCODE_2:
            //update the down arrow
            this->key2 = status;
            //exit
            break;
        //check if the number 3 is pressed
        case SDL_SCANCODE_3:
            //update the down arrow
            this->key3 = status;
            //exit
            break;
        //check if the number 4 is pressed
        case SDL_SCANCODE_4:
            //update the down arrow
            this->key4 = status;
            //exit
            break;
        //check if the number 5 is pressed
        case SDL_SCANCODE_5:
            //update the down arrow
            this->key5 = status;
            //exit
            break;
        //check if the number 6 is pressed
        case SDL_SCANCODE_6:
            //update the down arrow
            this->key6 = status;
            //exit
            break;
        //check if the number 7 is pressed
        case SDL_SCANCODE_7:
            //update the down arrow
            this->key7 = status;
            //exit
            break;
        //check if the number 8 is pressed
        case SDL_SCANCODE_8:
            //update the down arrow
            this->key8 = status;
            //exit
            break;
        //check if the number 9 is pressed
        case SDL_SCANCODE_9:
            //update the down arrow
            this->key9 = status;
            //exit
            break;
        //check for backspace
        case SDL_SCANCODE_BACKSPACE:
            //update the down arrow
            this->backspace = status;
            //exit
            break;
        //check for enter
        case SDL_SCANCODE_RETURN:
            //update the down arrow
            this->enter = status;
            //exit
            break;
    }
}

bool s_Keys::getKey(unsigned char key)
{
    //check if a should be updated
    if (key == (unsigned char)'a' || key == (unsigned char)'A')
    {
        return this->a;
    }
    //check if b should be updated
    else if (key == (unsigned char)'b' || key == (unsigned char)'B')
    {
        return this->b;
    }
    //check if c should be updated
    else if (key == (unsigned char)'c' || key == (unsigned char)'C')
    {
        return this->c;
    }
    //check if d should be updated
    else if (key == (unsigned char)'d' || key == (unsigned char)'D')
    {
        return this->d;
    }
    //check if e should be updated
    else if (key == (unsigned char)'e' || key == (unsigned char)'E')
    {
        return this->e;
    }
    //check if f should be updated
    else if (key == (unsigned char)'f' || key == (unsigned char)'F')
    {
        return this->f;
    }
    //check if g should be updated
    else if (key == (unsigned char)'g' || key == (unsigned char)'G')
    {
        return this->g;
    }
    //check if h should be updated
    else if (key == (unsigned char)'h' || key == (unsigned char)'H')
    {
        return this->h;
    }
    //check if i should be updated
    else if (key == (unsigned char)'i' || key == (unsigned char)'I')
    {
        return this->i;
    }
    //check if j should be updated
    else if (key == (unsigned char)'j' || key == (unsigned char)'J')
    {
        return this->j;
    }
    //check if k should be updated
    else if (key == (unsigned char)'k' || key == (unsigned char)'K')
    {
        return this->k;
    }
    //check if l should be updated
    else if (key == (unsigned char)'l' || key == (unsigned char)'L')
    {
        return this->l;
    }
    //check if m should be updated
    else if (key == (unsigned char)'m' || key == (unsigned char)'M')
    {
        return this->m;
    }
    //check if n should be updated
    else if (key == (unsigned char)'n' || key == (unsigned char)'N')
    {
        return this->n;
    }
    //check if o should be updated
    else if (key == (unsigned char)'o' || key == (unsigned char)'O')
    {
        return this->o;
    }
    //check if p should be updated
    else if (key == (unsigned char)'p' || key == (unsigned char)'P')
    {
        return this->p;
    }
    //check if q should be updated
    else if (key == (unsigned char)'q' || key == (unsigned char)'Q')
    {
        return this->q;
    }
    //check if r should be updated
    else if (key == (unsigned char)'r' || key == (unsigned char)'R')
    {
        return this->r;
    }
    //check if s should be updated
    else if (key == (unsigned char)'s' || key == (unsigned char)'S')
    {
        return this->s;
    }
    //check if t should be updated
    else if (key == (unsigned char)'t' || key == (unsigned char)'T')
    {
        return this->t;
    }
    //check if u should be updated
    else if (key == (unsigned char)'u' || key == (unsigned char)'U')
    {
        return this->u;
    }
    //check if v should be updated
    else if (key == (unsigned char)'v' || key == (unsigned char)'V')
    {
        return this->v;
    }
    //check if w should be updated
    else if (key == (unsigned char)'w' || key == (unsigned char)'W')
    {
        return this->w;
    }
    //check if x should be updated
    else if (key == (unsigned char)'x' || key == (unsigned char)'X')
    {
        return this->x;
    }
    //check if y should be updated
    else if (key == (unsigned char)'y' || key == (unsigned char)'Y')
    {
        return this->y;
    }
    //check if z should be updated
    else if (key == (unsigned char)'z' || key == (unsigned char)'Z')
    {
        return this->z;
    }
    //check if space should be updated
    else if (key == (unsigned char)' ')
    {
        return this->space;
    }

    switch (key)
    {
    case '.':
        return this->dot;
        break;
    case ',':
        return this->comma;
        break;
    case '-':
        return this->dash;
        break;
    case '+':
        return this->plus;
        break;
    case '#':
        return this->hash;
        break;
    case '1':
        return this->key1;
        break;
    case '2':
        return this->key2;
        break;
    case '3':
        return this->key3;
        break;
    case '4':
        return this->key4;
        break;
    case '5':
        return this->key5;
        break;
    case '6':
        return this->key6;
        break;
    case '7':
        return this->key7;
        break;
    case '8':
        return this->key8;
        break;
    case '9':
        return this->key9;
        break;
    case '0':
        return this->key0;
        break;
    case '\n':
        return this->enter;
        break;
    
    default:
        //if the key is not valide, return false
        return false;
    }
}

bool s_Keys::getKey(uint32_t scancode)
{
    //update the key according to the key
    switch (scancode)
    {
        //check if the key is a
        case SDL_SCANCODE_A:
            return this->a;
            //exit
            break;
        //check if the key is b
        case SDL_SCANCODE_B:
            return this->b;
            //exit
            break;
        //check if the key is c
        case SDL_SCANCODE_C:
            return this->c;
            //exit
            break;
        //check if the key is d
        case SDL_SCANCODE_D:
            return this->d;
            //exit
            break;
        //check if the key is e
        case SDL_SCANCODE_E:
            return this->e;
            //exit
            break;
        //check if the key is f
        case SDL_SCANCODE_F:
            return this->f;
            //exit
            break;
        //check if the key is g
        case SDL_SCANCODE_G:
            return this->g;
            //exit
            break;
        //check if the key is h
        case SDL_SCANCODE_H:
            return this->h;
            //exit
            break;
        //check if the key is i
        case SDL_SCANCODE_I:
            return this->i;
            //exit
            break;
        //check if the key is j
        case SDL_SCANCODE_J:
            return this->j;
            //exit
            break;
        //check if the key is k
        case SDL_SCANCODE_K:
            return this->k;
            //exit
            break;
        //check if the key is l
        case SDL_SCANCODE_L:
            return this->l;
            //exit
            break;
        //check if the key is m
        case SDL_SCANCODE_M:
            return this->m;
            //exit
            break;
        //check if the key is n
        case SDL_SCANCODE_N:
            return this->n;
            //exit
            break;
        //check if the key is o
        case SDL_SCANCODE_O:
            return this->o;
            //exit
            break;
        //check if the key is p
        case SDL_SCANCODE_P:
            return this->p;
            //exit
            break;
        //check if the key is q
        case SDL_SCANCODE_Q:
            return this->q;
            //exit
            break;
        //check if the key is r
        case SDL_SCANCODE_R:
            return this->r;
            //exit
            break;
        //check if the key is s
        case SDL_SCANCODE_S:
            return this->s;
            //exit
            break;
        //check if the key is t
        case SDL_SCANCODE_T:
            return this->t;
            //exit
            break;
        //check if the key is u
        case SDL_SCANCODE_U:
            return this->u;
            //exit
            break;
        //check if the key is v
        case SDL_SCANCODE_V:
            return this->v;
            //exit
            break;
        //check if the key is w
        case SDL_SCANCODE_W:
            return this->w;
            //exit
            break;
        //check if the key is x
        case SDL_SCANCODE_X:
            return this->x;
            //exit
            break;
        //check if the key is y
        case SDL_SCANCODE_Y:
            return this->y;
            //exit
            break;
        //check if the key is z
        case SDL_SCANCODE_Z:
            return this->z;
            //exit
            break;
        //check if the key is space
        case SDL_SCANCODE_SPACE:
            return this->space;
            //exit
            break;
        //check if the key is f1
        case SDL_SCANCODE_F1:
            return this->f1;
            //exit
            break;
        //check if the key is f2
        case SDL_SCANCODE_F2:
            return this->f2;
            //exit
            break;
        //check if the key is f3
        case SDL_SCANCODE_F3:
            return this->f3;
            //exit
            break;
        //check if the key is f4
        case SDL_SCANCODE_F4:
            return this->f4;
            //exit
            break;
        //check if the key is f5
        case SDL_SCANCODE_F5:
            return this->f5;
            //exit
            break;
        //check if the key is f6
        case SDL_SCANCODE_F6:
            return this->f6;
            //exit
            break;
        //check if the key is f7
        case SDL_SCANCODE_F7:
            return this->f7;
            //exit
            break;
        //check if the key is f8
        case SDL_SCANCODE_F8:
            return this->f8;
            //exit
            break;
        //check if the key is f9
        case SDL_SCANCODE_F9:
            return this->f9;
            //exit
            break;
        //check if the key is f10
        case SDL_SCANCODE_F10:
            return this->f10;
            //exit
            break;
        //check if the key is f11
        case SDL_SCANCODE_F11:
            return this->f11;
            //exit
            break;
        //check if the key is f12
        case SDL_SCANCODE_F12:
            return this->f12;
            //exit
            break;
        //check if the key is r_shift
        case SDL_SCANCODE_RSHIFT:
            return this->rightShift;
            //exit
            break;
        //check if the key is l_shift
        case SDL_SCANCODE_LSHIFT:
            return this->leftShift;
            //exit
            break;
        //check if the right arrow key is pressed
        case SDL_SCANCODE_RIGHT:
            return this->arrowRight;
            //exit
            break;
        //check if the left arrow key is pressed
        case SDL_SCANCODE_LEFT:
            return this->arrowLeft;
            //exit
            break;
        //check if the up arrow key is pressed
        case SDL_SCANCODE_UP:
            return this->arrowUp;
            //exit
            break;
        //check if the down arrow key is pressed
        case SDL_SCANCODE_DOWN:
            return this->arrowDown;
            //exit
            break;
        //check if the right alt key is pressed
        case SDL_SCANCODE_RALT:
            return this->rightAlt;
            //exit
            break;
        //check if the left alt key is pressed
        case SDL_SCANCODE_LALT:
            return this->leftAlt;
            //exit
            break;
        //check if the number 0 is pressed
        case SDL_SCANCODE_0:
            return this->key0;
            //exit
            break;
        //check if the number 1 is pressed
        case SDL_SCANCODE_1:
            return this->key1;
            //exit
            break;
        //check if the number 2 is pressed
        case SDL_SCANCODE_2:
            return this->key2;
            //exit
            break;
        //check if the number 3 is pressed
        case SDL_SCANCODE_3:
            return this->key3;
            //exit
            break;
        //check if the number 4 is pressed
        case SDL_SCANCODE_4:
            return this->key4;
            //exit
            break;
        //check if the number 5 is pressed
        case SDL_SCANCODE_5:
            return this->key5;
            //exit
            break;
        //check if the number 6 is pressed
        case SDL_SCANCODE_6:
            return this->key6;
            //exit
            break;
        //check if the number 7 is pressed
        case SDL_SCANCODE_7:
            return this->key7;
            //exit
            break;
        //check if the number 8 is pressed
        case SDL_SCANCODE_8:
            return this->key8;
            //exit
            break;
        //check if the number 9 is pressed
        case SDL_SCANCODE_9:
            return this->key9;
            //exit
            break;
        //check for backspace
        case SDL_SCANCODE_BACKSPACE:
            return this->backspace;
            //exit
            break;
        //check for enter
        case SDL_SCANCODE_RETURN:
            return this->enter;
            //exit
            break;
    }

    //key not found
    return false;
}

void s_Keys::clear()
{
    //zero out the object
    bzero(this, sizeof(*this));
}
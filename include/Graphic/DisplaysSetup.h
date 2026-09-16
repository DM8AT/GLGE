/**
 * @file DisplaysSetup.h
 * @author DM8AT
 * @brief define a structure to store information about the whole display setup
 * @version 0.1
 * @date 2025-12-28
 * 
 * @copyright Copyright (c) 2025
 * 
 */
//header guard
#ifndef _GLGE_GRAPHIC_DISPLAY_SETUP_
#define _GLGE_GRAPHIC_DISPLAY_SETUP_

//include display information
#include "Display.h"
//include ordered maps to store displays
#include "Core/utils/OrderedMap.h"

//use the librarys namespace
namespace GLGE::Graphic {

    /**
     * @brief store information about the display setup
     * 
     * This class encapsulates all information about all the known displays
     */
    class DisplaySetup {
    public:
    
        /**
         * @brief Construct a new Display Setup
         */
        DisplaySetup() = default;

        /**
         * @brief Get a specific display by its ID
         * 
         * @param id the ID of the display to get
         * @return `const Display` a constant pointer to the display or NULL if the ID is invalid
         */
        const Display* getDisplay(u32 id) const;

        /**
         * @brief Get the amount of known displays
         * 
         * @return u32 the amount of known displays
         */
        inline u32 getDisplayCount() const noexcept
        {return m_displays.size();}

        /**
         * @brief Get the Primary Display
         * 
         * @return const Display& a constant reference to the primary display
         */
        inline const Display& getPrimaryDisplay() const noexcept
        {return *getDisplay(m_primaryID);}

        /**
         * @brief Get the Displays
         * 
         * @return `const GLGE::OrderedMap<u32, Display>&` a constant reference to the ordered map 
         * that contains all the displays
         */
        inline const GLGE::OrderedMap<u32, Display>& getDisplays() const noexcept
        {return m_displays;}

        //only expose the display backend functions if the display backend access is defined
        //this is done to not pollute the users API
        #ifdef __GLGE_DISPLAY_BACKEND

        /**
         * @brief register a new display to the display setup
         * 
         * @param display the fully filled out display structure to register
         * 
         * @warning this is a backend function. It should only be used in a backend. 
         */
        void __backendRegisterDisplay(const Display& display);

        /**
         * @brief remove a specific display from the display setup
         * 
         * @param id the stable display ID of the display to remove
         * 
         * @warning this is a backend function. It should only be used in a backend. 
         */
        void __backendRemoveDisplay(u32 id);

        /**
         * @brief access a specific display to edit the capabilities of it
         * 
         * @param id the stable display ID of the display to edit
         * @return Display& a reference to the display. This is an editable reference. 
         * 
         * @warning this is a backend function. It should only be used in a backend. 
         */
        Display& __backendAccessForEditing(u32 id);

        /**
         * @brief set the ID of the primary display
         * 
         * @param id the stable display ID of the primary display
         */
        void __backendSetPrimaryDisplayID(u32 id);

        #endif

    protected:

        /**
         * @brief store the ID of the primary display
         */
        u32 m_primaryID = 0;
        /**
         * @brief store all the displays in an ordered map
         */
        GLGE::OrderedMap<u32, Display> m_displays;

    };

}

#endif
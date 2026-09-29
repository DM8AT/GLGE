/**
 * @file GUIProvider.h
 * @author DM8AT
 * @brief define an interface structure that is responsible for providing draw data for a GUI
 * @version 0.1
 * @date 2026-09-27
 * 
 * @copyright Copyright (c) 2026
 * 
 */
//header guard
#ifndef _GLGE_GRAPHIC_GUI_PROVIDER_
#define _GLGE_GRAPHIC_GUI_PROVIDER_

//add render targets
#include "RenderTarget.h"

//use the library namespace
namespace GLGE::Graphic {

    /**
     * @brief a structure to provide draw data into a GUI context
     */
    class GUIProvider {
    public:

        /**
         * @brief define an enum used to define blend factors
         * 
         * They are used to set the factors in the blend equation
         * 
         * Used blend equation: $col_{fin} = col_{in} * factor_a + col_{curr} * factor_b$
         */
        enum class BlendFactor : u8 {
            /**
             * @brief sets the blend factor to a constant 0
             */
            ZERO,
            /**
             * @brief sets the blend factor to a constant 1
             */
            ONE,
            /**
             * @brief sets the blend factor equal to the inputted colors alpha
             */
            SRC_ALPHA,
            /**
             * @brief sets the blend factor equal to one minus the inputted colors alpha
             */
            ONE_MINUS_SRC_ALPHA
        };

        /**
         * @brief a structure to store GUI commands
         */
        struct Command {
            /**
             * @brief a command to update the current target state
             */
            struct SetTarget {
                /**
                 * @brief the new target to use
                 * 
                 * If regardless of type a nullptr is parsed, this resets to the context's default target
                 */
                RenderTarget target;
            };
            /**
             * @brief a structure to update the blending state
             * 
             * Used blend equation: $col_{fin} = col_{in} * factor_a + col_{curr} * factor_b$
             */
            struct SetBlending {
                /**
                 * @brief the factor for the inputted color
                 */
                BlendFactor inFactor;
                /**
                 * @brief the factor for the current color
                 */
                BlendFactor currFactor;
            };
        };

    protected:

        

    };

}

#endif
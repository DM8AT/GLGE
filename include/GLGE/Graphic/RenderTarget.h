/**
 * @file RenderTarget.h
 * @author your name (you@domain.com)
 * @brief 
 * @version 0.1
 * @date 2026-03-27
 * 
 * @copyright Copyright (c) 2026
 * 
 */

//header guard
#ifndef _GLGE_GRAPHIC_RENDER_TARGET_
#define _GLGE_GRAPHIC_RENDER_TARGET_

//add common stuff
#include "GLGE/Core/Common.h"

//use the library namespace
namespace GLGE::Graphic {

    //windows and framebuffers are defined somewhere else
    class Window;
    class Framebuffer;

    /**
     * @brief define a resource that can be rendered to
     */
    class RenderTarget {
    public:

        /**
         * @brief define the type of render target 
         */
        enum Type : u8 {
            /**
             * @brief the target is a window
             */
            WINDOW = 0,
            /**
             * @brief the target is a framebuffer
             */
            FRAMEBUFFER
        };

        /**
         * @brief Construct a new Render Target
         * 
         * @param target a pointer to the framebuffer to use for storage
         */
        RenderTarget(Framebuffer* target)
         : m_type(FRAMEBUFFER), m_target(target)
        {}

        /**
         * @brief Construct a new Render Target
         * 
         * @param window a pointer to the window to use for storage
         */
        RenderTarget(Window* window)
         : m_type(WINDOW), m_target(window)
        {}

        /**
         * @brief Get the Type
         * 
         * @return `Type` the type of the render target
         */
        inline Type getType() const noexcept
        {return m_type;}

        /**
         * @brief Get the Target
         * 
         * @return `void*` a pointer to the target
         */
        inline void* getTarget() const noexcept
        {return m_target;}

        /**
         * @brief a structure used to hash a render target
         */
        struct Hasher {
            /**
             * @brief functor to hash a render target
             * 
             * @param target the target to hash
             * @return `std::size_t` the hash for the target
             */
            std::size_t operator()(const RenderTarget& target) {
                //store the current hash
                std::size_t seed = std::hash<void*>{}(target.m_target);

                //similar to boost::hash_combine
                auto hashCombine = [&seed](std::size_t value) 
                                    {seed ^= value + static_cast<std::size_t>(0x9e3779b9) + (seed << 6) + (seed >> 2);};
                //special function to hash enums
                auto hashEnum = [&hashCombine](auto value) {
                    using T = std::remove_cv_t<decltype(value)>;
                    using U = std::underlying_type_t<T>;
                    hashCombine(std::hash<U>{}(static_cast<U>(value)));
                };

                //add the type to the hash
                hashEnum(target.m_type);

                //return the final hash
                return seed;
            };
        };

        /**
         * @brief check if two render targets are identical
         * 
         * @return `true` if they are identical, `false` when they are different
         */
        constexpr bool operator==(const RenderTarget&) const noexcept = default;

    protected:

        /**
         * @brief store the type of render target
         */
        Type m_type = WINDOW;

        /**
         * @brief store a raw pointer to the target
         */
        void* m_target = nullptr;

    };

}

namespace std {
    
    /**
     * @brief specialization of std::hash for a render target
     * 
     * @tparam  [UNUSED]
     */
    template <>
    struct hash<GLGE::Graphic::RenderTarget> {
        /**
         * @brief functor operator for the hasher
         * 
         * @param target the target to hash
         * @return `std::size_t` the corresponding hash
         */
        std::size_t operator()(const GLGE::Graphic::RenderTarget& target) const noexcept 
        {return GLGE::Graphic::RenderTarget::Hasher{}(target);}
    };

};

#endif
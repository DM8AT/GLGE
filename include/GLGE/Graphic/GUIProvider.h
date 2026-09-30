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
//add shader
#include "Shader.h"
//add GPU images
#include "Image.h"

//add vectors
#include <vector>
//add variants
#include <variant>

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
         * @brief define how vertices are connected
         * 
         * This implicitly defines if indexing is used or not
         */
        enum class RenderMode : u8 {
            /**
             * @brief 3 consecutive indices are used to index the vertex buffer, the resulting vertices are connected to a triangle
             */
            TRIANGLES = 0,
            /**
             * @brief 2 consecutive indices are used to index the vertex buffer, the resulting vertices are connected to a line
             */
            LINES = 1,
            /**
             * @brief the vertex buffer is indexed directly. A single point is drawn at the vertex position. The vertex size is controlled by the shader
             * 
             * @warning The index part of the command may be poisoned
             */
            POINTS
        };

        /**
         * @brief define the vertex structure of the GUI
         */
        struct Vertex {
            /**
             * @brief store the position of the vertex
             */
            vec2 pos;
            /**
             * @brief store the UV value of the vertex
             */
            vec2 uv;
            /**
             * @brief store the color of the vertex as rgba-8 Unorm
             */
            u32 color_RGBA8;
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
            /**
             * @brief a command used to define the region of the current target to render to
             * 
             * The region is inputted in two-dimensional NDC space and is clamped to 0 to 1 per axis
             */
            struct SetRenderRegion {
                /**
                 * @brief the bottom left corner of the rectangle to start rendering from
                 */
                vec2 from;
                /**
                 * @brief the top right corner of the region to render in
                 */
                vec2 to;
            };
            /**
             * @brief set a specific shader
             */
            struct SetShader {
                /**
                 * @brief store a pointer to the shader to use
                 * 
                 * @note in contrast to other shaders no resource sets are required for the shader. 
                 * @warning All bound resource sets are ignored. No per-draw data exists. Textures are bound using the `SetTexture` command, not via resource sets. 
                 */
                Shader* shader;
            };
            /**
             * @brief a command that is used to set a texture
             */
            struct SetTexture {
                /**
                 * @brief store a pointer to the image to bind
                 * 
                 * `nullptr` is used to clear a slot
                 */
                Image* image;
                /**
                 * @brief store the slot to bind to
                 * 
                 * Valid indices are 0 to 15. Everything else should be rejected by the backend. 
                 */
                u8 slot;
            };
            /**
             * @brief set the projection matrix
             * 
             * This is NOT ment for 3D-Projection, but for 2D transformation. But it allows all forms of projection. 
             */
            struct SetProjection {
                /**
                 * @brief stored as 4x4 column-major matrix
                 * 
                 *  0  1  2  3 <- column 0
                 *  4  5  6  7 <- column 1
                 *  8  9 10 11 <- column 2
                 * 12 13 14 15 <- column 3
                 */
                float matrix[16];
            };
            /**
             * @brief a command to draw vertices
             */
            struct Draw {
                /**
                 * @brief the render mode to use for this draw command
                 */
                RenderMode renderMode;
                /**
                 * @brief the amount of elements to draw
                 * 
                 * This is defined in either vertices or indices. The pseudo-code `drawElements % renderMode.ElementsPerPrimitive == 0` must evaluate to true. 
                 */
                u32 drawElements;
                /**
                 * @brief the first index of the draw
                 * 
                 * @warning This may be poisoned
                 */
                u32 firstIndex;
                /**
                 * @brief the first vertex of the draw
                 */
                u32 firstVertex;
            };

            /**
             * @brief store the recorded command
             */
            std::variant<
                SetTarget,
                SetBlending,
                SetRenderRegion,
                SetShader,
                SetProjection,
                SetTexture,
                Draw
            > command;
        };

        /**
         * @brief Get the recorded indices
         * 
         * @return `const std::vector<u32>&` the full recorded index buffer
         */
        inline const std::vector<u32>& getIndices() const noexcept
        {return m_indices;}

        /**
         * @brief Get the recorded vertices
         * 
         * @return `const std::vector<Vertex>&` the full recorded vertex buffer
         */
        inline const std::vector<Vertex>& getVertices() const noexcept
        {return m_vertices;}

        /**
         * @brief Get the recorded commands
         * 
         * @return `const std::vector<Command>&` the full recorded command buffer
         */
        inline const std::vector<Command>& getCommands() const noexcept
        {return m_cmds;}

    protected:

        /**
         * @brief store the index buffer of the provider
         */
        std::vector<u32> m_indices;
        /**
         * @brief store the vertex buffer of the provider
         */
        std::vector<Vertex> m_vertices;
        /**
         * @brief store the commands
         * 
         * @note order is important
         */
        std::vector<Command> m_cmds;

    };

}

#endif
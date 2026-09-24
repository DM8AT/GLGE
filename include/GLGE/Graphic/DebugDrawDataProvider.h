/**
 * @file DebugDrawDataProvider.h
 * @author DM8AT
 * @brief define an structure that defines the interface between a structure that generates debug draw commands and the debug context
 * @version 0.1
 * @date 2026-09-23
 * 
 * @copyright Copyright (c) 2026
 * 
 */
//header guard
#ifndef _GLGE_GRAPHIC_DEBUG_DRAW_DATA_PROVIDER_
#define _GLGE_GRAPHIC_DEBUG_DRAW_DATA_PROVIDER_

//add types
#include "GLGE/Core/Common.h"
//add materials
//this lets me re-use the material enums
#include "Material.h"

//use the library namespace
namespace GLGE::Graphic {

    /**
     * @brief this class defines the abstract interface for all classes that can generate debug draw data commands. 
     * 
     * This class does no GPU work. All operations are fully CPU-Side. 
     */
    class DebugDrawDataProvider {
    public:
        
        /**
         * @brief define a rendering style
         */
        struct Style {
            /**
             * @brief define how to render the element
             * 
             * The render mode may be ignored by some commands
             */
            enum class RenderMode : u8 {
                /**
                 * @brief The object is rendered filled out with a solid color
                 * 
                 * This ignores the line width argument
                 */
                SOLID = 0,
                /**
                 * @brief The object is rendered as the outlines of the triangles
                 */
                WIREFRAME = 1,
                /**
                 * @brief The object is rendered as a single point at every vertex
                 * 
                 * The line width argument is used as the point radius
                 */
                VERTICES = 2
            };

            /**
             * @brief Define how the blending is done
             * 
             * This effects the blending equation directly. 
             */
            enum class BlendMode {
                /**
                 * @brief Used for opaque rendering
                 * 
                 * Alpha channel is ignored. This sets the blend equation to $c_{res} = c_{in} * 1 + c_{old} * 0$. 
                 */
                OPAQUE = 0,
                /**
                 * @brief Used for some order independent transparency systems
                 * 
                 * Alpha channel effects strength of addition. 
                 * This sets the blend equation to $c_{res} = c_{in} * a_{in} + c_{old} * 1$. 
                 */
                ADDITIVE = 1,
                /**
                 * @brief This sets up a "normal" blending equation
                 * 
                 * This sets the blend equation to $c_{res} = c_{in} * a_{in} + c_{old} * (1 - a_{in})$
                 */
                NORMAL = 2
            };

            /**
             * @brief Re-Use the material depth testing options
             */
            using DepthTest = GLGE::Graphic::Material::DepthMode;
            /**
             * @brief Re-Use the material cull mode options
             */
            using CullMode = GLGE::Graphic::Material::CullMode;

            /**
             * @brief store the color for the object to render
             * 
             * Format: RGBA32F, non-normalized
             * 
             * @note The valid range of the alpha channel is 0 to 1. The values are clamped upon usage. The R,G and B channels are unbound, values in range [-inf, inf] are valid. 
             * 
             * Default value: Bright, fully-opaque red (red: 100%, green: 0%, blue: 0%, alpha: 1)
             */
            vec4 color = {1,0,0,1};
            /**
             * @brief define the size of points
             * 
             * Only used when the render mode is set to vertex
             */
            float pointSize = 1.f;
            /**
             * @brief define how the object is rendered
             * 
             * Default value: Solid
             */
            RenderMode renderMode = RenderMode::SOLID;
            /**
             * @brief define how the blending is set up
             * 
             * Default value: Normal
             */
            BlendMode blendMode = BlendMode::NORMAL;
            /**
             * @brief This is used to decide if the depth of the objects is written to a depth buffer. 
             * 
             * Use `true` to enable the depth writing and preserve the object's depth, use `false` to discard the depth. 
             * If no depth buffer is valid, all values will be interpreted as `false`. 
             * 
             * Default value: `true`
             */
            bool depthWrite = true;
            /**
             * @brief define how the depth of the rendered fragment is tested against existing depth buffer values
             * 
             * If no depth buffer is present, the equation will always evaluate to `true`, unless `NEVER` is specified. 
             * 
             * Default value: Less
             */
            DepthTest depthTest = DepthTest::DEPTH_COMPARE_LESS;
            /**
             * @brief Define how the reversed faces of objects are rendered. 
             * 
             * The winding order of all rendered elements is clockwise as front by default. 
             * 
             * Default value: Back
             */
            CullMode cullMode = CullMode::BACK;
        };

        /**
         * @brief define a constant preset for a solid style
         * 
         * This results in a solid, opaque-red render. 
         * The blend mode is set to normal, allowing for alpha modification to show in correct transparent behaviour. 
         * 
         * @note By default this also modifies the depth buffer and respects depth testing. 
         */
        inline static constexpr Style SOLID = Style {
            .color = {1.f, 0.f, 0.f, 1.f},
            .pointSize = 0.f,
            .renderMode = Style::RenderMode::SOLID,
            .blendMode = Style::BlendMode::NORMAL,
            .depthWrite = true,
            .depthTest = Style::DepthTest::DEPTH_COMPARE_LESS,
            .cullMode = Style::CullMode::BACK
        };

        /**
         * @brief define a constant preset for a wireframe style
         * 
         * This results in an opaque, red wireframe of the object being rendered. 
         * The blend mode is set to normal, allowing for alpha modification to show in correct transparent behaviour. 
         * 
         * @note by default this also modifies the depth buffer and respects depth testing. 
         */
        inline static constexpr Style WIREFRAME = Style {
            .color = {1.f, 0.f, 0.f, 1.f},
            .pointSize = 0.f,
            .renderMode = Style::RenderMode::WIREFRAME,
            .blendMode = Style::BlendMode::NORMAL,
            .depthWrite = true,
            .depthTest = Style::DepthTest::DEPTH_COMPARE_LESS,
            .cullMode = Style::CullMode::OFF
        };

        /**
         * @brief define a constant preset for a vertices style
         * 
         * This results in an opaque, red dots are rendered at the vertices of the object
         * The blend mode is set to normal, allowing for alpha modification to show in correct transparent behaviour. 
         * 
         * @note by default this also modifies the depth buffer and respects depth testing. 
         */
        inline static constexpr Style VERTICES = Style {
            .color = {1.f, 0.f, 0.f, 1.f},
            .pointSize = 5.f,
            .renderMode = Style::RenderMode::VERTICES,
            .blendMode = Style::BlendMode::NORMAL,
            .depthWrite = true,
            .depthTest = Style::DepthTest::DEPTH_COMPARE_LESS,
            .cullMode = Style::CullMode::OFF
        };

        /**
         * @brief define how a command is stored
         */
        struct CommandRecord {
            /**
             * @brief store the style to render the command in
             * 
             * The style is used to infer the amount of indirections used. 
             * - Solid -> Triangles (3 indices per element)
             * - Wireframe -> Lines (2 indices per element)
             * - Vertices -> Points (this forces the index start to be UINT32_MAX as a deliberate poison and the index count to be 0 (which should be used as detection), points should not be indexed)
             */
            Style style;
            /**
             * @brief store the index into the point buffer where the point section for this command starts
             */
            u32 pointStart;
            /**
             * @brief the amount of points that belong to this command
             */
            u32 pointCount;
            /**
             * @brief the index offset where the index section of this command starts
             */
            u32 indexStart;
            /**
             * @brief the amount of indices that belong to the command
             * 
             * @note these indices are stored in command-local space, meaning that index 0 references to the first point belonging to this command
             */
            u32 indexCount;
        };

        /**
         * @brief Construct a new Debug Draw Data Provider
         */
        DebugDrawDataProvider() = default;

        /**
         * @brief Destroy the Debug Draw Data Provider
         * 
         * @note this class does not provide any virtual interface
         */
        ~DebugDrawDataProvider() = default;

        /**
         * @brief Get the Records
         * 
         * @return `const std::vector<CommandRecord>&` all recorded commands
         */
        inline const std::vector<CommandRecord>& getRecords() const noexcept
        {return m_records;}

        /**
         * @brief Get the Point Buffer
         * 
         * @return `const std::vector<vec3>&` the command buffer
         */
        inline const std::vector<vec3>& getPointBuffer() const noexcept
        {return m_points;}

        /**
         * @brief Get the Index Buffer
         * 
         * @return `const std::vector<u32>&` the index buffer
         */
        inline const std::vector<u32>& getIndexBuffer() const noexcept
        {return m_connections;}

    protected: //ensure that sub-classes have unrestricted access

        /**
         * @brief store a list of recorded points
         */
        std::vector<vec3> m_points;
        /**
         * @brief store a list of point indices to connect
         * 
         * The buffer does not define how many elements form a connection (e.g. triangles or lines). 
         * This must be inferred from the command record. 
         */
        std::vector<u32> m_connections;
        /**
         * @brief store a list of all recorded commands
         */
        std::vector<CommandRecord> m_records;

    };

}

#endif
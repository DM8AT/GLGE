/**
 * @file DebugVisualizer.h
 * @author DM8AT
 * @brief 
 * @version 0.1
 * @date 2026-09-19
 * 
 * @copyright Copyright (c) 2026
 * 
 */
//header guard
#ifndef _GLGE_GRAPHIC_DEBUG_VISUALIZER_
#define _GLGE_GRAPHIC_DEBUG_VISUALIZER_

//add types
#include "GLGE/Core/Common.h"

//add the command system
#include "Command.h"
//add the camera component
#include "Components.h"
//add targets
#include "RenderTarget.h"
//add transforms
#include "Transform.h"
//add materials
//this lets me re-use the material enums
#include "Material.h"

//add core meshes
#include "GLGE/Core/Mesh.h"

//add core AABBs
#include "GLGE/Core/AABB.h"

//use the library namespace
namespace GLGE::Graphic {

    /**
     * @brief Define a structure that is responsible to record debug render commands
     * 
     * This makes multi-submission to different targets easy. 
     * 
     * @note This does NO GPU-Work
     */
    class DebugRenderer {
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
             * @brief define the line width in pixels
             * 
             * The exact usage is defined by the render mode. 
             * 
             * Default value: 1 pixel
             */
            float lineWidth = 1.f;
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
            .lineWidth = 1.f,
            .renderMode = Style::RenderMode::SOLID,
            .blendMode = Style::BlendMode::NORMAL,
            .depthWrite = true,
            .depthTest = Style::DepthTest::DEPTH_COMPARE_LESS,
            .cullMode = CullMode::BACK
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
            .lineWidth = 5.f,
            .renderMode = Style::RenderMode::WIREFRAME,
            .blendMode = Style::BlendMode::NORMAL,
            .depthWrite = true,
            .depthTest = Style::DepthTest::DEPTH_COMPARE_LESS,
            .cullMode = CullMode::OFF
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
            .lineWidth = 5.f,
            .renderMode = Style::RenderMode::VERTICES,
            .blendMode = Style::BlendMode::NORMAL,
            .depthWrite = true,
            .depthTest = Style::DepthTest::DEPTH_COMPARE_LESS,
            .cullMode = CullMode::OFF
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
             * - Vertices -> Points (this forces the index start to be UINT32_MAX and the index count to be 0, points are not indexed)
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
             */
            u32 indexCount;
        };

        /**
         * @brief Construct a new Debug Renderer
         */
        DebugRenderer() = default;

        /**
         * @brief Destroy the Debug Renderer
         */
        ~DebugRenderer() = default;

        /**
         * @brief clear the internal draw command list
         * 
         * This does NOT effect any submitted work to debug renderers, even if they did not finish recording
         */
        void reset();

        /**
         * @brief render an axis-aligned bounding box (AABB)
         * 
         * @param aabb the axis aligned bounding box to draw
         * @param style the style to draw it in
         */
        void drawAABB(const AABB& aabb, const Style& style);

        /**
         * @brief draw a box
         * 
         * In box-local space 0,0,0 is the front bottom left corner of the box. 
         * In box-local space the box will span from 0,0,0 to `extent`. 
         * 
         * @param transform the transformation of the 
         * @param extent the extent of the box. This can be understood as the back top right corner of the box. 
         * @param style the style to draw it in
         */
        void drawBox(const Transform& transform, const vec3& extent, const Style& style);

        /**
         * @brief draw a single dot
         * 
         * This implicitly sets the draw style to `VERTICES`. 
         * 
         * @param position the position to draw the dot at
         * @param style the style to draw in
         */
        void drawDot(const vec3& position, const Style& style);

        /**
         * @brief draw a sphere
         * 
         * This creates a sphere mesh on the fly. The created sphere mesh is a UV-Sphere. 
         * 
         * @param position the position to draw the sphere at
         * @param radius the radius of the sphere
         * @param style the style to draw in
         */
        void drawSphere(const vec3& position, float radius, const Style& style);

        /**
         * @brief draw a line
         * 
         * This implicitly sets the draw style to `WIREFRAME`
         * 
         * @param a the first end point of the line
         * @param b the second end point of the line
         * @param style the style to draw in
         */
        void drawLine(const vec3& a, const vec3& b, const Style& style);

        /**
         * @brief draw a mesh
         * 
         * @note in contrast to the main renderer render pipeline, this does NO persistend GPU VBO / VAO upload. 
         * @note this only uses the position argument of the vertex layout. 
         * @warning this requires the mesh vertex layout to contain a position argument using the default `GLGE::VertexAttribute::Position` type. 
         * 
         * @param mesh the CPU mesh to use
         * @param lod the level of detail index of the mesh
         * @param transform the transformation to apply to the mesh
         * @param style the style to draw in
         */
        void drawMesh(const GLGE::Mesh& mesh, u8 lod, const Transform& transform, const Style& style);

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

    protected:

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

    /**
     * @brief Define the context for debug visualization
     * 
     * This is used to actually render debug visualization data
     * 
     * @note this does GPU-Operations. 
     */
    class DebugContext : public CommandInvalidator {
    public:

        /**
         * @brief define the current state of the context
         */
        enum class State : u8 {
            /**
             * @brief no recording was started nor finished
             */
            UNINITIALIZED = 0,
            /**
             * @brief a recording was started but not yet finished
             */
            RECORDING = 1,
            /**
             * @brief a recording was started and finished
             */
            RECORDED = 2
        };

        /**
         * @brief Construct a new Debug Context
         */
        DebugContext();

        /**
         * @brief Destroy the Debug Context
         */
        ~DebugContext();

        /**
         * @brief start the recording of the debug context
         * 
         * This readies the context for debug recording. This sets the state to `RECORDING` and prepares the backend. 
         * 
         * @warning this call is only valid if the context is NOT in a recording state
         * @note this does not invalidate old recordings. The backend should create a new recording target while leaving a potential old one valid. 
         */
        void beginRecording();

        /**
         * @brief Set the Camera
         * 
         * @warning this call is only valid if the context is in a recording state
         * @note this needs to be submitted before calling any submit renderer commands
         * 
         * @param camera the camera component to use
         */
        void setCamera(const Component::Camera& camera);

        /**
         * @brief Set the Target
         * 
         * @warning this call is only valid if the context is in a recording state
         * @note this needs to be submitted before calling any submit renderer commands
         * 
         * @param target the target to render to
         */
        void setTarget(const RenderTarget& target);

        /**
         * @brief submit a list of objects to render
         * 
         * @warning before this call is recorded the `setCamera` and `setTarget` commands must be called at least once. 
         * 
         * @param renderer the renderer to render from
         */
        void submitRenderer(const DebugRenderer& renderer);

        /**
         * @brief finish the recording
         * 
         * This sets the state to `RECORDED` and finalizes the backend. 
         * 
         * @warning this call is only valid if the context is in a recording state
         * @note this call invalidates the old recording. 
         */
        void endRecording();

        /**
         * @brief Get the State of the context
         * 
         * @return `State` the current state
         */
        inline State getState() const noexcept
        {return m_state;}

    protected:

        /**
         * @brief store the current state of the context
         */
        State m_state = State::UNINITIALIZED;

    };

}

#endif
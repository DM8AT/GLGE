/**
 * @file Components.h
 * @author DM8AT
 * @brief define all components of the graphic system
 * @version 0.1
 * @date 2026-04-05
 * 
 * @copyright Copyright (c) 2026
 * 
 */
//header guard
#ifndef _GLGE_GRAPHIC_COMPONENTS_
#define _GLGE_GRAPHIC_COMPONENTS_

//add default types
#include "Core/Common.h"

//use the library namespace
namespace GLGE::Graphic {

    //meshes and materials are defined somewhere else
    class Mesh;
    class Material;

/**
 * @brief a namespace for all graphic components
 */
namespace Component {

    /**
     * @brief a component that identifies an object as a camera
     */
    struct Camera {
        /**
         * @brief store the field of view of the camera in degrees, the valid range is (0; 180)
         */
        float FOV;
        /**
         * @brief store the near clipping plane of the camera in world units, the valid range is (0; far)
         */
        float clip_near;
        /**
         * @brief store the far clipping plane of the camera in world units, the valid range is (near; inf)
         */
        float clip_far;

        /**
         * @brief store the euler angles of the camera rotation
         * 
         * This angle is stored in radians
         */
        vec3 eulerAngles;
    };

    /**
     * @brief a component that identifies an object as a renderable
     */
    struct Renderable {
        /**
         * @brief store a pointer to the mesh to use for rendering
         */
        Mesh* mesh;
        /**
         * @brief store a pointer to the material to use for rendering
         */
        Material* material;
        /**
         * @brief a boolean to enable or disable the renderable component
         * 
         * `true` to mark the object for rendering, `false` to disallow rendering
         */
        bool enabled;
    };

    /**
     * @brief store all data needed for a single point light
     */
    struct PointLight {
        //the position is quarried from the transform

        /**
         * @brief store the radius of the point light source
         */
        float radius = 0.f;
        /**
         * @brief store the RGB color of the light
         */
        vec3 color{1,1,1};
        /**
         * @brief store the intensity of the light
         */
        float intensity = 1;
        /**
         * @brief store the linear fallof coefficient
         */
        float fallof_linear = 1.f;
        /**
         * @brief store the quadratic fallof coefficient
         */
        float fallof_quadratic = 0.f;
        /**
         * @brief store the flags
         */
        u32 flags = 0;
    };

    /**
     * @brief store all data needed for a single spot light
     */
    struct SpotLight {
        //Position and direction are quarried from the transform

        /**
         * @brief store the color of the spot light
         */
        vec3 color;
        /**
         * @brief store the intensity of the light
         */
        float intensity;
        /**
         * @brief store the linear fallof coefficient
         */
        float fallof_linear;
        /**
         * @brief store the quadratic fallof coefficient
         */
        float fallof_quadratic;
        /**
         * @brief store the inner cone angle (angle of the cone with maximal intensity)
         */
        float cone_inner;
        /**
         * @brief store the outer cone angle (angle of the cone where the intensity hits zero)
         */
        float cone_outer;
        /**
         * @brief store the flags of the spot light
         */
        u32 flags;
    };

    /**
     * @brief store all data needed for a single directional light
     */
    struct DirectionalLight {
        //the direction is quarried from a transform

        /**
         * @brief store the color of the spot light
         */
        vec3 color;
        /**
         * @brief store the intensity of the light
         */
        float intensity;
        /**
         * @brief store the flags of the directional light
         */
        u32 flags;
    };

}

}

#endif
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
        float near;
        /**
         * @brief store the far clipping plane of the camera in world units, the valid range is (near; inf)
         */
        float far;

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

}

}

#endif
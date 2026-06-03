/**
 * @file Transform.h
 * @author DM8AT
 * @brief define what a transform component is
 * @version 0.1
 * @date 2026-03-23
 * 
 * @copyright Copyright (c) 2026
 * 
 */
//header guard
#ifndef _GLGE_CORE_TRANSFORM_
#define _GLGE_CORE_TRANSFORM_

//add common stuff
#include "Common.h"
//add objects
#include "Object.h"

/**
 * @brief use the libraries namespace
 */
namespace GLGE {

    /**
     * @brief a transform stores the position, rotation and scaling
     */
    struct Transform : public SerializableComponent {

        /**
         * @brief Construct a new Transform
         */
        Transform() = default;

        /**
         * @brief Construct a new Transform
         * 
         * @param _pos the position in 3D
         * @param _rot the rotation quaternion
         * @param _scale the scale
         */
        Transform(const vec3& _pos, const Quaternion& _rot = Quaternion(1,0,0,0), const vec3& _scale = vec3(1,1,1))
         : pos(_pos), rot(_rot), scale(_scale)
        {}

        /**
         * @brief Destroy the Transform
         */
        virtual ~Transform() {}

        /**
         * @brief store the position
         */
        vec3 pos;
        /**
         * @brief store the rotation
         */
        Quaternion rot;
        /**
         * @brief store the scale
         */
        vec3 scale;

        /**
         * @brief load the serializable component
         * 
         * Span is used to not copy the data
         * 
         * @param buffer the buffer to load from
         */
        virtual void load(const std::span<const u8>& buffer) override;

        /**
         * @brief store the serializable asset
         * 
         * @param buffer the buffer to write the serializable data to
         */
        virtual void store(std::vector<u8>& buffer) override;

    };

    /**
     * @brief store a 2 dimensional transform
     */
    struct Transform2D : public SerializableComponent {

        /**
         * @brief Construct a new Transform 2D
         */
        Transform2D() = default;

        /**
         * @brief Construct a new Transform
         * 
         * @param _pos the position in 3D
         * @param _angle the rotation angle
         * @param _scale the scale
         */
        Transform2D(const vec2& _pos, const float& _angle = 0.f, const vec2& _scale = vec2(1,1))
         : pos(_pos), angle(_angle), scale(_scale)
        {}

        /**
         * @brief Destroy the Transform
         */
        virtual ~Transform2D() {}

        /**
         * @brief the position in 2D space
         */
        vec2 pos;
        /**
         * @brief the angle of the object
         */
        float angle;
        /**
         * @brief the scale of the object
         */
        vec2 scale;

        /**
         * @brief load the serializable component
         * 
         * Span is used to not copy the data
         * 
         * @param buffer the buffer to load from
         */
        virtual void load(const std::span<const u8>& buffer) override;

        /**
         * @brief store the serializable asset
         * 
         * @param buffer the buffer to write the serializable data to
         */
        virtual void store(std::vector<u8>& buffer) override;

    };

    /**
     * @brief store a transform that holds 3D transformation relative to the world's root
     * 
     * This is the version only used for 3D transforms
     */
    struct WorldTransform {
        /**
         * @brief store the position relative to the world's root
         */
        vec3 pos;
        /**
         * @brief store the rotation relative to the world's root
         */
        Quaternion rot;
        /**
         * @brief store the scale relative to the world's root
         */
        vec3 scale;

        /**
         * @brief cast the world transform to a normal transform
         * 
         * @return `Transform` the resulting transform
         */
        inline operator Transform() const noexcept
        {return Transform(pos, rot, scale);}

        /**
         * @brief a function to get the matrix representing this transformation
         * 
         * @return `glm::mat4` the matrix representing the transformation
         */
        inline glm::mat4 toMatrix() const noexcept {
            //create matrices for the elements
            glm::mat4 T = glm::translate(glm::mat4(1.0f), pos);
            glm::mat4 R = glm::toMat4(rot);
            glm::mat4 S = glm::scale(glm::mat4(1.0f), scale);
            //return multiplied together
            return T * R * S;
        }

        /**
         * @brief helper operator to get as a 4x4 matrix
         * 
         * @return `glm::mat4` a matrix representing the transform
         */
        inline operator glm::mat4() const noexcept
        {return toMatrix();}
    };

    /**
     * @brief a namespace that contains all system functions
     */
    namespace System {

        /**
         * @brief a system that bakes all transforms to the world transforms
         * 
         * @param world a reference to the world to operate on
         */
        void BakeTransforms(World& world);

    };

}

#endif
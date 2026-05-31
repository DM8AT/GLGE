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
        virtual void load(const std::span<const u8>& buffer) override {
            //copy the data
            memcpy(&pos, buffer.data(), sizeof(pos));
            memcpy(&rot, buffer.data() + sizeof(pos), sizeof(rot));
            memcpy(&scale, buffer.data() + sizeof(pos) + sizeof(rot), sizeof(scale));
        }

        /**
         * @brief store the serializable asset
         * 
         * @param buffer the buffer to write the serializable data to
         */
        virtual void store(std::vector<u8>& buffer) override {
            //add the data
            buffer.insert(buffer.end(), reinterpret_cast<u8*>(&pos), reinterpret_cast<u8*>(&pos) + sizeof(pos));
            buffer.insert(buffer.end(), reinterpret_cast<u8*>(&rot), reinterpret_cast<u8*>(&rot) + sizeof(rot));
            buffer.insert(buffer.end(), reinterpret_cast<u8*>(&scale), reinterpret_cast<u8*>(&scale) + sizeof(scale));
        }

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
        virtual void load(const std::span<const u8>& buffer) override {
            //copy the data
            memcpy(&pos, buffer.data(), sizeof(pos));
            memcpy(&angle, buffer.data() + sizeof(angle), sizeof(angle));
            memcpy(&scale, buffer.data() + sizeof(pos) + sizeof(angle), sizeof(scale));
        }

        /**
         * @brief store the serializable asset
         * 
         * @param buffer the buffer to write the serializable data to
         */
        virtual void store(std::vector<u8>& buffer) override {
            //add the data
            buffer.insert(buffer.end(), reinterpret_cast<u8*>(&pos), reinterpret_cast<u8*>(&pos) + sizeof(pos));
            buffer.insert(buffer.end(), reinterpret_cast<u8*>(&angle), reinterpret_cast<u8*>(&angle) + sizeof(angle));
            buffer.insert(buffer.end(), reinterpret_cast<u8*>(&scale), reinterpret_cast<u8*>(&scale) + sizeof(scale));
        }

    };

}

#endif
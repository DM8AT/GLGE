/**
 * @file Jolt_Internal_LayerSetup.h
 * @author DM8AT
 * @brief 
 * @version 0.1
 * @date 2026-06-01
 * 
 * @copyright Copyright (c) 2026
 * 
 */
//header guard
#ifndef _GLGE_PHYSIC_BACKEND_JOLT_INTERNAL_LAYER_SETUP_
#define _GLGE_PHYSIC_BACKEND_JOLT_INTERNAL_LAYER_SETUP_

//add jolt
#include <Jolt/Jolt.h>
#include <Jolt/RegisterTypes.h>
#include <Jolt/Core/Factory.h>
#include <Jolt/Core/TempAllocator.h>
#include <Jolt/Core/JobSystemThreadPool.h>
#include <Jolt/Physics/PhysicsSystem.h>
#include <Jolt/Physics/Collision/Shape/BoxShape.h>

//use the backend namespace
namespace GLGE::Physic::Backend::Jolt {

    /**
     * @brief a namespace used for jolt boilerplate
     */
    namespace Internal {
        /**
         * @brief a namespace that contains layer constants
         */
        namespace Layers {
            /**
             * @brief the layer for non-moving (static) stuff
             */
            static constexpr JPH::ObjectLayer NON_MOVING = 0;
            /**
             * @brief the layer for moving stuff
             */
            static constexpr JPH::ObjectLayer MOVING = 1;
            /**
             * @brief the amount of layers
             */
            static constexpr JPH::ObjectLayer NUM_LAYERS = 2;
        }

        /**
         * @brief a class used for layer pair filtering
         */
        class ObjectLayerPairFilterImpl : public JPH::ObjectLayerPairFilter {
        public:
            /**
             * @brief check if two object layers should collide
             * 
             * @param a the first layer
             * @param b the second layer
             * @return `true` if they should, `false` if not
             */
            bool ShouldCollide(JPH::ObjectLayer a, JPH::ObjectLayer b) const override {
                switch (a) {
                    case Layers::NON_MOVING: return b == Layers::MOVING;
                    case Layers::MOVING: return true;
                    default: return false;
                }
            }
        };

        /**
         * @brief define the layers for broad-phase collision detection
         */
        namespace BroadPhaseLayers {
            /**
             * @brief the layer for non-moving (static) stuff
             */
            static constexpr JPH::BroadPhaseLayer NON_MOVING(0);
            /**
             * @brief the layer for moving stuff
             */
            static constexpr JPH::BroadPhaseLayer MOVING(1);
            /**
             * @brief the amount of layers
             */
            static constexpr uint NUM_LAYERS(2);
        }

        /**
         * @brief define a function used for broad phase layers
         */
        class BPLayerInterfaceImpl final : public JPH::BroadPhaseLayerInterface {
        public:

#if defined(JPH_EXTERNAL_PROFILE) || defined(JPH_PROFILE_ENABLED)

            /**
             * @brief Get the Broad Phase Layer Name
             * 
             * @param inLayer the layer to get the name for
             * @return `const char*` the name for the layer
             */
            virtual const char* GetBroadPhaseLayerName(JPH::BroadPhaseLayer inLayer) const override {
                switch (inLayer.GetValue()) {
                    case 0:  return "NON MOVING";
                    case 1:  return "MOVING";
                    default: return "UNNAMED";
                }
            }
#endif

            /**
             * @brief Get the number of Broad Phase Layers
             * 
             * @return `uint` the amount of broad phase layers
             */
            uint GetNumBroadPhaseLayers() const override
            {return BroadPhaseLayers::NUM_LAYERS;}

            /**
             * @brief Get the Broad Phase Layer
             * 
             * @param layer the ojbect layer
             * @return `JPH::BroadPhaseLayer` the corresponding broad phase layer
             */
            JPH::BroadPhaseLayer GetBroadPhaseLayer(JPH::ObjectLayer layer) const override {
                switch (layer) {
                    case Layers::NON_MOVING: return BroadPhaseLayers::NON_MOVING;
                    case Layers::MOVING: return BroadPhaseLayers::MOVING;
                    default: return BroadPhaseLayers::NON_MOVING;
                }
            }
        };

        /**
         * @brief a class used to determine if an object layer and a broad phase layer should collide
         */
        class ObjectVsBroadPhaseLayerFilterImpl : public JPH::ObjectVsBroadPhaseLayerFilter {
        public:
            /**
             * @brief a function to check if a object layer and a broad phase layer should collide
             * 
             * @param layer the layer to check
             * @param broadPhaseLayer the broad phase layer to check
             * @return `true` if the objects should collide, `false` if not
             */
            bool ShouldCollide(JPH::ObjectLayer layer, JPH::BroadPhaseLayer broadPhaseLayer) const override {
                switch (layer) {
                    case Layers::NON_MOVING: return broadPhaseLayer == BroadPhaseLayers::MOVING;
                    case Layers::MOVING: return true;
                    default: return false;
                }
            }
        };
    }

}

#endif
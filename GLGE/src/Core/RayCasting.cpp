/**
 * @file RayCasting.cpp
 * @author DM8AT
 * @brief implement the ray casting system
 * @version 0.1
 * @date 2026-08-25
 * 
 * @copyright Copyright (c) 2026
 * 
 */
//add the header
#include "Core/RayCasting.h"
//add transforms
#include "Core/Transform.h"

//add embree
#include <embree4/rtcore.h>

std::optional<GLGE::Hit> GLGE::System::CastRay(GLGE::World& world, const Ray& ray) {
    //store the hit structure to return
    std::optional<Hit> ret;

    world.each<MeshComponent>(
        [&](Tiny::ECS::Entity entity, const MeshComponent& meshComp) {
            //skip if the mesh is nullptr
            if (meshComp.mesh == nullptr) {return;}

            //get an optional transform
            auto* transf = world.get<GLGE::WorldTransform>(entity);
            //store the world-space position and direction of the ray
            vec3 o = ray.origin;
            vec3 d = glm::normalize(ray.direction); //make sure that the direction is normalized
            //if a transform was found, transform according to it
            if (transf) {
                //compute the rotation matrix from the quaternion
                glm::mat3 m {transf->rot};

                //apply the inverse transform
                o -= transf->pos;
                o /= transf->scale;
                o = o * m; //invert the matrix multiply order to transpose the matrix (v * m == transpose(m) * v)

                //also update the direction
                d = d * m;
            }

            //setup the embree ray
            RTCRay embreeRay {};
            embreeRay.org_x = o.x;
            embreeRay.org_y = o.y;
            embreeRay.org_z = o.z;
            embreeRay.dir_x = d.x;
            embreeRay.dir_y = d.y;
            embreeRay.dir_z = d.z;
            embreeRay.tnear = ray.tMin;
            embreeRay.tfar = ret.has_value() ? ret->distance : ray.tMax;
            embreeRay.mask = -1;
            embreeRay.flags = 0;
            embreeRay.time = 0.f;

            RTCRayHit embreeRayHit {};
            embreeRayHit.ray = embreeRay;
            embreeRayHit.hit.geomID = RTC_INVALID_GEOMETRY_ID;
            embreeRayHit.hit.instID[0] = RTC_INVALID_GEOMETRY_ID;

            //fetch the embree scene
            RTCScene scene = reinterpret_cast<RTCScene>(meshComp.mesh->getLOD(0).getAccelerationStructure());

            //perform the raycast
            rtcIntersect1(scene, &embreeRayHit);

            //if a hit is valid, it's the closest
            //that is ensured by the clipping
            if (embreeRayHit.hit.geomID != RTC_INVALID_GEOMETRY_ID) {
                //update the hit structure
                Hit h {};
                //store the hit distance
                h.distance = embreeRayHit.ray.tfar;
                //store the hit position
                h.position.x = o.x + (d.x * h.distance);
                h.position.y = o.y + (d.y * h.distance);
                h.position.z = o.z + (d.z * h.distance);
                //if transformation exists, apply the transformation to the hit
                if (transf) {
                    glm::mat3 m {transf->rot};
                    h.position = m * h.position;
                    h.position *= transf->scale;
                    h.position += transf->pos;
                }
                //update the distance
                h.distance = glm::distance(h.position, ray.origin);
                //store the mesh and object
                h.obj = GLGE::Object(entity);
                h.mesh = meshComp.mesh;
                h.lod = 0; //lod is always 0

                //obtain the hit triangle
                h.triangle = meshComp.mesh->getLOD(0).indices().get(embreeRayHit.hit.primID);
                //store the barycentric coordinates
                f32 u = embreeRayHit.hit.u;
                f32 v = embreeRayHit.hit.v;
                f32 w = 1.f - (u + v);
                h.barycentric = vec3(u, v, w);
                
                //update the best hit
                ret = h;
            }
        }
    );

    //return the hit
    return ret;
}
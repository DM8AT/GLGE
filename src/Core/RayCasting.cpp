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
#include "GLGE/Core/RayCasting.h"
//add transforms
#include "GLGE/Core/Transform.h"

//add mesh assets
#include "GLGE/Core/MeshAsset.h"

//add embree
#include <embree4/rtcore.h>

/**
 * @brief load the serializable component
 * 
 * Span is used to not copy the data
 * 
 * @param buffer the buffer to load from
 */
void GLGE::MeshComponent::load(const std::span<const u8>& buffer) {
    //size check
    if (buffer.size() < 5) 
    {throw GLGE::Exception("Failed to load a mesh component: Invalid size. Size must be greater than 5.", "GLGE::MeshComponent::load");}

    //check magic value
    if (!((buffer[0] == 'M') && (buffer[1] == 'e') && (buffer[2] == 's') && (buffer[3] == 'h'))) 
    {throw GLGE::Exception("Failed to load a mesh component: Invalid magic value.", "GLGE::MeshComponent::load");}

    //valid magic value. Read what is stored. 
    if ((buffer[4] == 0) || (buffer[4] == 1)) {
        mesh.reset();
        return;
    }

    //check the asset
    if (buffer[4] == 2) {
        //extract the path to the asset

        //scan the length
        size_t len = 0;
        for (size_t i = 0; i < (buffer.size() - 5); ++i) {
            if (buffer[4 + i] != 0)
            {++len;}
            else
            {break;}
        }
        //store the path
        std::string str(len, '0');
        memcpy(str.data(), buffer.data() + 4, len);

        //load the asset from the pointed-to file
        auto ass = GLGE::Instance::getCurrentInstance()->assets().load<GLGE::MeshAsset>(str);
        //store the loaded mesh
        mesh = ass.reference()->getMesh();

        //finished
        return;
    }

    //check for embedded data
    if (buffer[4] == 3) {
        //use the mesh asset to load it
        GLGE::MeshAsset ass;
        std::vector<u8> data(buffer.subspan(4).begin(), buffer.subspan(4).end());
        ass.load(&GLGE::Instance::getCurrentInstance()->assets(), data);
        //store the mesh
        mesh = ass.getMesh();

        //finished
        return;
    }

    //invalid
    std::unreachable();
}

/**
 * @brief store the serializable asset
 * 
 * @param buffer the buffer to write the serializable data to
 */
void GLGE::MeshComponent::store(std::vector<u8>& buffer) {
    //write some magic bytes to identify and guard against corruption
    buffer.push_back('M');
    buffer.push_back('e');
    buffer.push_back('s');
    buffer.push_back('h');
    //first, store where the asset came from
    switch (mesh->getDataSource()) {
    case GLGE::Mesh::Source::Unknown:  buffer.emplace_back(0); break;
    case GLGE::Mesh::Source::Asset:    buffer.emplace_back(1); break;
    case GLGE::Mesh::Source::Embedded: buffer.emplace_back(2); break;
    case GLGE::Mesh::Source::Dynamic:  buffer.emplace_back(3); break;
    
    default: throw GLGE::Exception("Tried to save a mesh component with a mesh that came from an unknown source (and source != `Source::Unknown`).", "GLGE::MeshComponent::store"); break;
    }

    //case: Unknown and Dynamic
    //just do nothing
    if (mesh->getDataSource() == GLGE::Mesh::Source::Unknown || mesh->getDataSource() == GLGE::Mesh::Source::Unknown)
    {return;}

    //Case: Asset
    //just embed the path to the asset
    if (mesh->getDataSource() == GLGE::Mesh::Source::Asset) {
        //write the path as a null-terminated string
        std::string str = mesh->getSourcePath().string();
        //pre-reserve, then write
        buffer.reserve(buffer.size() + str.size() + 1);
        for (const auto& c : str) 
        {buffer.push_back(static_cast<uint8_t>(c));}
        //add null terminator
        buffer.push_back(0);
        
        //finished
        return;
    }

    //Case: Embedded
    //the data is written RIGHT NEXT to the object
    if (mesh->getDataSource() == GLGE::Mesh::Source::Embedded) {
        //create a temporary asset
        GLGE::MeshAsset a;
        a.setMesh(mesh);

        //store the asset into the buffer
        a.store(buffer);

        //finished
        return;
    }

    //invalid
    std::unreachable();
}

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
                h.mesh = meshComp.mesh.get();
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
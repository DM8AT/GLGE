/**
 * @file Mesh.h
 * @author DM8AT
 * @brief implement the geometry mesh frontend
 * @version 0.1
 * @date 2026-06-14
 * 
 * @copyright Copyright (c) 2026
 * 
 */
//header guard
#ifndef _GLGE_GRAPHIC_MESH_
#define _GLGE_GRAPHIC_MESH_

//add meshes
#include "Core/Mesh.h"
//add the mesh backend
#include "Backend/Graphics/MeshManager.h"
//add graphic instances
#include "Instance.h"

//add exceptions
#include "Core/Exception.h"

//use the libraries namespace
namespace GLGE::Graphic {

    /**
     * @brief a mesh wrapper for graphic
     */
    class Mesh {
    public:

        /**
         * @brief Construct a new Mesh
         * 
         * @param mesh the mesh to upload
         * @param layout the layout to use for the mesh
         */
        Mesh(const GLGE::Mesh& mesh, const GLGE::Graphic::VertexLayout& layout)
         : m_inst(GLGE::Instance::getCurrentInstance()->getExtension<GLGE::Graphic::Instance>())
        {
            //in debug: sanity check that the instance is valid
            #if GLGE_DEBUG
            if (m_inst == nullptr) {throw GLGE::Exception("Cannot create a mesh without a graphic instance on the current instance", "GLGE::Graphic::Mesh::Mesh");}
            #endif
            //create the handle
            m_handle = m_inst->meshManager().createMesh(mesh, layout);
        }

        /**
         * @brief Construct a new Mesh
         * 
         * Move constructor
         */
        Mesh(Mesh&&) = default;
        /**
         * @brief Move assign operator
         * 
         * @return `Mesh&` a reference to the 
         */
        Mesh& operator=(Mesh&&) = default;

        /**
         * @brief Construct a new Mesh
         * 
         * Copy constructor
         * 
         * @param other the mesh to copy
         */
        Mesh(const Mesh& other) 
         : m_inst(other.m_inst),
           m_handle(other.m_handle)
        {
            //register the handle copy
            m_inst->meshManager().copyHandle(m_handle);
        }

        /**
         * @brief Copy assignment operator
         * 
         * @param other the mesh to copy
         * @return `Mesh&` a reference to the mesh after copying
         */
        Mesh& operator=(const Mesh& other) {
            //prevent copy to self
            if (this == &other) {return *this;}

            //copy the data over
            m_inst = other.m_inst;
            m_handle = other.m_handle;

            //copy the handle
            m_inst->meshManager().copyHandle(m_handle);
        }

        /**
         * @brief Destroy the Mesh
         */
        ~Mesh() {
            //clean up the handle
            m_inst->meshManager().freeMesh(m_handle);
        }

        /**
         * @brief Get the Instance
         * 
         * @return `GLGE::Graphic::Instance*` a pointer to the instance that the mesh belongs to
         */
        inline GLGE::Graphic::Instance* getInstance() const noexcept
        {return m_inst;}

        /**
         * @brief Get the mesh handle to the stored mesh
         * 
         * @return `GLGE::Graphic::Backend::Graphic::MeshHandle` the wrapped handle
         */
        inline GLGE::Graphic::Backend::Graphic::MeshHandle getHandle() const noexcept
        {return m_handle;}

    protected:

        /**
         * @brief store a pointer to the instance the mesh belongs to
         */
        GLGE::Graphic::Instance* m_inst;
        /**
         * @brief store a handle for the mesh
         */
        GLGE::Graphic::Backend::Graphic::MeshHandle m_handle;

    };

}

#endif
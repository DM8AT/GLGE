/**
 * @file GLGEAssetObject.cpp
 * @author DM8AT
 * @brief implement the object asset loading
 * @version 0.1
 * @date 2025-08-12
 * 
 * @copyright Copyright (c) 2025
 * 
 */

//include the object asset to implement the functions of it
#include "GLGEAssetObject.h"

//include pugixml for XML stuff
#include "../../GLGE3rdParty/pugixml/src/pugixml.hpp"

AssetObject::AssetObject(std::filesystem::path path, Object** children, uint64_t childCount, AssetStorage* storage)
   //initalize the asset
 : Asset(path, storage),
   //store the additional children
   m_additionalChildren(children),
   m_additionalChildCount(childCount)
{
    //load the object's contents
    loadObject();
}

AssetObject::~AssetObject() noexcept
{
    //if the object exists, delete it
    if (m_object)
    {
        //zero the object
        delete m_object;
        m_object = 0;
    }
}

void AssetObject::reload() noexcept
{
    //if the file is empty, stop
    if (m_file == "") {return;}

    //check if the file exists
    if (std::filesystem::is_regular_file(m_file))
    {
        //check if the file was changed
        if (m_lastWrite != std::filesystem::last_write_time(m_file))
        {
            //debug info
            GLGE_DEBUG_WRAPPER(
                //print some info
                std::stringstream stream;
                stream << "Re-Loading an existsing texture asset from file " << m_file << " because the file was updated.";
                m_storage->instance->logDebug(stream, MESSAGE_TYPE_DEBUG);
            )

            //re-load the contents
            loadObject();

            //store the new last write time
            m_lastWrite = std::filesystem::last_write_time(m_file);
        }
    }
}

void AssetObject::loadObject() noexcept
{
    //check if the file exists
    if (!std::filesystem::is_regular_file(m_file))
    {
        //error - file not found
        std::stringstream stream;
        stream << "Failed to load the asset shader file " << m_file << " because the file was not found";
        //log the error
        m_storage->instance->log(stream, MESSAGE_TYPE_ERROR);
        //stop the functoin 
        return;
    }
    //mark that the file is now loaded
    m_lastWrite = std::filesystem::last_write_time(m_file);

    //load the file with the xml parser
    pugi::xml_document doc;
    pugi::xml_parse_result res = doc.load_file(m_file.c_str());
    
    //check if the parsing is successfully
    if (!res)
    {
        //parsing was not successfull - print an error
        std::stringstream stream;
        stream << "Failed to load the asset shader file " << m_file << " because a parsing error occoured. \nError: \n    "
               << res.description() << "\nAt:\n" << getCharacterInfo(m_file, res.offset, "    ");
        //log the error
        m_storage->instance->log(stream, MESSAGE_TYPE_ERROR);
        //stop the functoin 
        return;
    }
    //check if the version is OK
    if (!hasAcceptableGLGEVersion(&doc, m_file, m_storage->instance))
    {
        //print a warning
        m_storage->instance->log("Loading a shader with newer GLGE version than the library. Some elements may not load correctly.", MESSAGE_TYPE_WARNING);
    }

    //get the object node
    pugi::xml_node obj = doc.child("object");
    if (!obj)
    {
        //the object element does not exist - print an error
        std::stringstream stream;
        stream << "Failed to load the asset object file " << m_file << " because no element named \"object\" is child of the root node";
        //log the error
        m_storage->instance->log(stream, MESSAGE_TYPE_ERROR);
        //stop the functoin 
        return;
    }

    //get the name attribute
    pugi::xml_attribute name = obj.attribute("name");
    //check if a name attribute exists
    if (!name)
    {
        //the name attribute was not found -> error
        std::stringstream stream;
        stream << "Failed to load the object asset file " << m_file << " because no attrbute named \"name\" belonging to top-level attribute named \"object\"";
        m_storage->instance->log(stream, MESSAGE_TYPE_ERROR);
        return;
    }

    //get the asset name
    std::string nameStr = name.value();
    //replace all occourences of a slash with a % (percent) to allow access to compound assets
    for (auto pos = nameStr.find_first_of(GLGE_ASSET_SUB_ASSET_SEPERATOR); pos != std::string::npos; pos = nameStr.find_first_of(GLGE_ASSET_SUB_ASSET_SEPERATOR)) {nameStr.replace(pos, 1, GLGE_REPLACE_SUB_ASSET_SEPERATOR_WITH);}
    //check if the name matches
    if ((m_name == nameStr) || !m_object)
    {
        //store the name
        m_name = nameStr;
    }
    else
    {
        //name changed, but it is not allowed to -> print a warning
        std::stringstream stream;
        stream << "Failed to load the object asset file " << m_file << " because the assets and object's name was changed, but it can't be updated at run-time. Please re-start for the name change to take effect.";
        m_storage->instance->log(stream, MESSAGE_TYPE_WARNING);
    }

    //store the transofrmation of the object
    Transform transf;
    //get the transform of the object if it exists
    pugi::xml_node transform = obj.child("transform");
    if (transform)
    {
        //load the transform

        //get the position
        pugi::xml_node pos = transform.child("pos");
        if (pos)
        {
            //get the x attribute
            pugi::xml_attribute x = pos.attribute("x");
            if (x) {transf.pos.x = x.as_float();}
            //get the y attribute
            pugi::xml_attribute y = pos.attribute("y");
            if (y) {transf.pos.y = y.as_float();}
            //get the z attribute
            pugi::xml_attribute z = pos.attribute("z");
            if (z) {transf.pos.z = z.as_float();}
        }
        //get the rotation
        pugi::xml_node rot = transform.child("rotation");
        if (rot)
        {
            //store the rotation
            vec3 rot_vec = 0;
            //get the x attribute
            pugi::xml_attribute x = rot.attribute("x");
            if (x) {rot_vec.x = x.as_float();}
            //get the y attribute
            pugi::xml_attribute y = rot.attribute("y");
            if (y) {rot_vec.y = y.as_float();}
            //get the z attribute
            pugi::xml_attribute z = rot.attribute("z");
            if (z) {rot_vec.z = z.as_float();}

            //store the final rotatoin
            transf.rot = Euler(rot_vec);
        }
        //get the scale
        pugi::xml_node scale = transform.child("scale");
        if (scale)
        {
            //get a uniform scale attribute
            pugi::xml_attribute scale_attr = scale.attribute("scale");
            if (scale_attr)
            {
                //set the scale uniform
                transf.scale = scale_attr.as_float();
            }
            else
            {
                //else, load the vector elements

                //get the x attribute
                pugi::xml_attribute x = scale.attribute("x");
                if (x) {transf.scale.x = x.as_float();}
                //get the y attribute
                pugi::xml_attribute y = scale.attribute("y");
                if (y) {transf.scale.y = y.as_float();}
                //get the z attribute
                pugi::xml_attribute z = scale.attribute("z");
                if (z) {transf.scale.z = z.as_float();}
            }
        }
    }

    //check if the object exists
    if (m_object)
    {
        //just update the transform
        m_object->setTransform(transf);
    }
    else
    {
        //create a new object
        m_object = new Object(m_name, transf, m_additionalChildren, m_additionalChildCount, *m_storage->instance);
    }
}
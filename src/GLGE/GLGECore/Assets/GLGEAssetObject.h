/**
 * @file GLGEAssetObject.h
 * @author DM8AT
 * @brief define the asset to load an object
 * @version 0.1
 * @date 2025-08-12
 * 
 * @copyright Copyright (c) 2025
 * 
 */

//header guard
#ifndef _GLGE_ASSET_OBJECT_
#define _GLGE_ASSET_OBJECT_

//include the base asset class to overload it
#include "GLGEAsset.h"

//include objects to encapsulate in the asset class
#include "../Geometry/GLGEObject.h"

//check for C++ to create a class
#if GLGE_CPP

/**
 * @brief encapsulate an object and add load / store functionality
 */
class AssetObject final : public Asset
{
public:

    /**
     * @brief Construct a new Asset Object
     * Explicetly deleted to prevent empty objects
     */
    AssetObject() = delete;

    /**
     * @brief Construct a new Asset Object
     * 
     * @param path the path to load the asset from
     * @param children additional children to add to the asset (NULL = no children)
     * @param childCount the amount of additional children to add (if children is NULL, this must be 0)
     * @param storage a pointer to the asset storage the asset will belong to
     */
    AssetObject(std::filesystem::path path, Object** children, uint64_t childCount, AssetStorage* storage);

    /**
     * @brief re-load the asset from the disk
     */
    virtual void reload() noexcept override;

    /**
     * @brief Get the Type Name of the asset
     * @warning Even if this function is not virtual, sub-classes MUST have a function with this name
     * 
     * @return const char* the constant name of the asset
     */
    static const char* getTypeNameStatic() noexcept {return "GLGE_OBJECT_ASSET";}

    /**
     * @brief Get the Type Name of the asset (but overloadable)
     * 
     * @return const char* the type name of the asset
     */
    virtual const char* getTypeName() const noexcept override {return getTypeNameStatic();}

    /**
     * @brief Get a pointer to the encapsulated object
     * 
     * @return Object* a pointer to the loaded object or 0 if loading failed
     */
    inline Object* getObject() const noexcept {return m_object;}

    /**
     * @brief convert the object asset to an object pointer
     * 
     * @return Object* a pointer to the contained object
     */
    inline operator Object*() const noexcept {return m_object;}

    /**
     * @brief convert the object asset to an object reference
     * 
     * @return Object& a reference to the contained object
     */
    inline operator Object&() noexcept {return *m_object;}

    /**
     * @brief convert the object asset to a constant object reference
     * 
     * @return const Object& a constant object reference to the stored object
     */
    inline operator const Object&() const noexcept {return *m_object;}

protected:

    /**
     * @brief load or re-load the content of the object
     */
    void loadObject() noexcept;

    /**
     * @brief a pointer to the loaded object or 0 if it is not loaded
     */
    Object* m_object = 0;
    /**
     * @brief a list of children to add to the loaded object
     */
    Object** m_additionalChildren = 0;
    /**
     * @brief the amount of additional children to add to the loaded object
     */
    uint64_t m_additionalChildCount = 0;

};

#endif //end of C++ section

#endif
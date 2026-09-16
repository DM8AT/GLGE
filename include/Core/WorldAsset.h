/**
 * @file WorldAsset.h
 * @author DM8AT
 * @brief define the world asset
 * @version 0.1
 * @date 2026-05-30
 * 
 * @copyright Copyright (c) 2026
 * 
 */
//header guard
#ifndef _GLGE_CORE_WORLD_ASSET_
#define _GLGE_CORE_WORLD_ASSET_

//add the object system
#include "Object.h"
//add assets
#include "Asset.h"

//add transforms
#include "Transform.h"

//use the library namespace
namespace GLGE {

    /**
     * @brief a helper to check if a component is serializable
     * 
     * @tparam `T` the type of the component to check
     */
    template <typename T>
    constexpr bool is_serializable_component_v = std::is_base_of_v<SerializableComponent, T>;

    /**
     * @brief an asset that can store a world and all loaded objects with all attached serializable components
     */
    class WorldAsset : public Asset {
    public:

        /**
         * @brief a registry class used to keep track of all registerable components the world asset knows
         */
        class ComponentRegistry {
        public:

            /**
             * @brief Destroy the Component Registry
             */
            ~ComponentRegistry() = default;

            /**
             * @brief add a specific component type to the type registry
             * 
             * @tparam T the type to add
             */
            template <typename T>
            requires is_serializable_component_v<T>
            void addType() {
                //get the entry
                TypeEntry& entry = m_typeEntries[getTypeHash64<T>()];
                //write the functions
                entry.gatherFn = static_cast<PFN_GetAllOf>([](World* world, std::vector<Object>* gathered) {
                    //gather all entities
                    world->each<T>([gathered](const Tiny::ECS::Entity& ent, const T&) {gathered->push_back(Object(ent));});
                });
                //for the selected type, invoke the serialize function
                entry.storeFn = static_cast<PFN_Serialize>([](Tiny::ECS::Entity entity, std::vector<u8>& buffer, World* world) {
                    //get the entity
                    T* el = world->get<T>(Object(entity));
                    //if it exists, store it
                    if (el) {static_cast<SerializableComponent*>(el)->store(buffer);}
                });
                //for the selected type, invoke the deserialize function
                entry.loadFn  = static_cast<PFN_Deserialize>([](Tiny::ECS::Entity entity, const std::span<const u8>& buffer, World* world) {
                    //add the type to the entity
                    world->add<T>(Object(entity));
                    //then get it and load
                    static_cast<SerializableComponent*>(world->get<T>(Object(entity)))->load(buffer);
                });
            }

        protected:

            /**
             * @brief Construct a new Component Registry
             */
            ComponentRegistry() {
                //add all default components
                addType<Transform>();
                addType<Transform2D>();
            }

            /**
             * @brief define the type used for the type-erased discovery functions
             */
            typedef void (*PFN_GetAllOf)(World*, std::vector<Object>*);

            /**
             * @brief define the type used for the type-erased load function
             */
            typedef void (*PFN_Deserialize)(Tiny::ECS::Entity entity, const std::span<const u8>& buffer, World* world);

            /**
             * @brief define the type used for the type-erased store function
             */
            typedef void (*PFN_Serialize)(Tiny::ECS::Entity entity, std::vector<u8>& buffer, World* world);

            /**
             * @brief store all functions for a single type
             */
            struct TypeEntry {
                /**
                 * @brief store the function to gather all entities of this type
                 */
                PFN_GetAllOf  gatherFn = nullptr;
                /**
                 * @brief store a function used to serialize a single entity
                 */
                PFN_Serialize  storeFn = nullptr;
                /**
                 * @brief store a function used to deserialize a single entity
                 */
                PFN_Deserialize loadFn = nullptr;
            };

            /**
             * @brief store functions to operate on all types
             */
            std::unordered_map<u64, TypeEntry> m_typeEntries;

            //world assets are friends
            friend class WorldAsset;

        };

        /**
         * @brief define an enum for the different used formats
         */
        enum Format : u32 {
            /**
             * @brief define that the GLGE 
             */
            GLGE = 0
        };  

        /**
         * @brief Construct a new World Asset
         */
        WorldAsset()
         : m_world("World")
        {}

        /**
         * @brief load the asset from raw safe data
         * 
         * @param manager a pointer to the asset manager used for loading
         * @param data the raw binary data to load the asset from
         * @return `u64` the amount of loaded bytes
         */
        virtual u64 load(AssetManager* manager, const std::vector<u8>& data) override;

        /**
         * @brief store the asset in binary
         * 
         * @warning do NOT assume that data is empty at the start. It should be APPENDED and not overridden. 
         * 
         * @param data a vector to append the raw binary data to
         */
        virtual void store(std::vector<u8>& data) override;

        /**
         * @brief import the asset from a stand-alone file
         * 
         * @param manager a pointer to the asset manager used for loading
         * @param file the file to import the asset from
         * @param format the format of the asset
         */
        virtual void import_from(AssetManager* manager, const std::filesystem::path& file, u32 format) noexcept(false) override;

        /**
         * @brief export the asset in a specific file format
         * 
         * @param file a path to the file to store the asset into
         * @param format the file format to use
         */
        virtual void export_as(const std::filesystem::path& file, u32 format) noexcept(false) override;

        /**
         * @brief Get the Component Registry
         * 
         * @return `ComponentRegistry&` a reference to the component registry
         */
        static inline ComponentRegistry& getComponentRegistry() noexcept
        {return ms_compReg;}

        /**
         * @brief access the world wrapped by the asset
         * 
         * @return `World&` a modifiable reference to the asset
         */
        inline World& world() noexcept
        {return m_world;}

    protected:

        /**
         * @brief store the data required to serialize an object
         */
        struct ObjSerializationData {
            /**
             * @brief store the name component
             */
            Component::Name name;
            /**
             * @brief store the hierarchy node
             */
            Component::HierarchyNode node;
            /**
             * @brief store a list of all serializers required for the object
             */
            std::vector<std::pair<u64, ComponentRegistry::PFN_Serialize>> compSerializers;
        };

        /**
         * @brief store the componenet registry of the world
         */
        inline static ComponentRegistry ms_compReg;

        /**
         * @brief store the own world
         */
        World m_world;

    };

}

#endif
/**
 * @file UUID.h
 * @author DM8AT
 * @brief a class to manage universally unique identifiers
 * @version 0.1
 * @date 2026-02-03
 * 
 * @copyright Copyright (c) 2026
 * 
 */
#ifndef _GLGE_CORE_UUID_
#define _GLGE_CORE_UUID_

//add common stuff
#include "Common.h"
//add a mutex for thread safety
#include <mutex>
//add unordered sets to store if a UUID already exists
#include <unordered_set>
//for random sequences
#include <random>

//use the library namespace
namespace GLGE {

    /**
     * @brief define the UUID as a 64 bit unsigned integer
     */
    typedef u64 UUID;

    /**
     * @brief a class to manage Universally unique identifiers
     * 
     * @warning the IDs are universally unique in the local context of this registry. If two registries are used, the same ID may be created by both registries. 
     */
    class UUIDRegistry {
    public:

        /**
         * @brief Construct a new uuid registry
         * 
         * @param seed the seed for the UUID generator
         */
        UUIDRegistry(u64 seed = std::random_device{}()) 
         : m_rng(seed), m_distribution(0, std::numeric_limits<UUID>::max())
        {}

        /**
         * @brief register a pre-existing UUID (e.g. from asset loading)
         * 
         * @param id the ID to register
         * @return `true` if the ID was registered, `false` otherwise
         */
        bool registerExisting(UUID id) {
            //safety
            std::unique_lock lock(m_mtx);

            //if the UUID is known, reject it
            if (m_uuids.contains(id)) {return false;}
            //else, store it
            m_uuids.insert(id);
        }

        /**
         * @brief generate a new, unique UUID
         * 
         * @return `UUID` the new UUID
         */
        UUID generate() {
            //thread safety
            std::unique_lock lock(m_mtx);

            //loop until a valid candidate was found
            while (true) {
                //poll the next candidate by generating a random number
                UUID candidate = m_distribution(m_rng);
                //check if the candidate is known. 
                //If not, store it and return the UUID. 
                //Else, continue the search
                if (!m_uuids.contains(candidate)) {
                    m_uuids.insert(candidate);
                    return candidate;
                }
            }
        }

        /**
         * @brief check if the UUID is known
         * 
         * @param id the UUID to check
         * @return `true` if the UUID is known, `false` if it is unknown
         */
        bool isKnown(UUID id) {
            //thread safety
            std::unique_lock lock(m_mtx);

            //check if the element is known
            return m_uuids.contains(id);
        }

    protected:

        /**
         * @brief store a mutex for thread safety
         */
        std::mutex m_mtx;
        /**
         * @brief store all known UUIDs
         */
        std::unordered_set<UUID> m_uuids;
        /**
         * @brief store the random number generator
         */
        std::mt19937_64 m_rng;
        /**
         * @brief store the integer distribution for the random number generator
         */
        std::uniform_int_distribution<UUID> m_distribution;

    };

}

#endif
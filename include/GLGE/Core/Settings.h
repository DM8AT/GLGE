/**
 * @file Settings.h
 * @author DM8AT
 * @brief define the settings for GLGE
 * 
 * @warning if any of these settings are changed the library must be re-built from source, else some parts may break
 * 
 * @version 0.1
 * @date 2025-12-12
 * 
 * @copyright Copyright (c) 2025
 * 
 */
//header guard
#ifndef _GLGE_CORE_SETTINGS_
#define _GLGE_CORE_SETTINGS_

//
// VERSION
//

/**
 * @brief define the size of the major version in bits
 */
#define GLGE_VERSION_MAJOR_BIT_SIZE 11
/**
 * @brief define the size of the minor version in bits
 */
#define GLGE_VERSION_MINOR_BIT_SIZE 11
/**
 * @brief define the size of the patch version in bits
 */
#define GLGE_VERSION_PATCH_BIT_SIZE 10

/**
 * @brief compute the whole size of the version
 */
#define GLGE_VERSION_SIZE (GLGE_VERSION_MAJOR_BIT_SIZE + GLGE_VERSION_MINOR_BIT_SIZE + GLGE_VERSION_PATCH_BIT_SIZE)

/**
 * @brief define a bit masks for the major version value
 */
#define GLGE_VERSION_MAJOR_MASK ((~0u) >> (GLGE_VERSION_SIZE - GLGE_VERSION_MAJOR_BIT_SIZE))
/**
 * @brief define a bit mask for the minor version value
 */
#define GLGE_VERSION_MINOR_MASK ((~0u) >> (GLGE_VERSION_SIZE - GLGE_VERSION_MINOR_BIT_SIZE))
/**
 * @brief define a bit mask for the patch version value
 */
#define GLGE_VERSION_PATCH_MASK ((~0u) >> (GLGE_VERSION_SIZE - GLGE_VERSION_PATCH_BIT_SIZE))

/**
 * @brief define the offset of the major version in the version structure
 */
#define GLGE_VERSION_MAJOR_OFFSET (GLGE_VERSION_MINOR_BIT_SIZE + GLGE_VERSION_PATCH_BIT_SIZE)
/**
 * @brief define the offset of the minor version in the version structure
 */
#define GLGE_VERSION_MINOR_OFFSET GLGE_VERSION_PATCH_BIT_SIZE
/**
 * @brief define the offset of the patch version in the version structure
 */
#define GLGE_VERSION_PATCH_OFFSET 0u

/**
 * @brief define an utility function that merges a version number together
 * 
 * @param major the value for the major version number
 * @param minor the value for the minor version number
 * @param patch the value for the patch version number
 */
#define GLGE_VERSION_COMBINE(major, minor, patch) (((major & GLGE_VERSION_MAJOR_MASK) << GLGE_VERSION_MAJOR_OFFSET) | \
                                                   ((minor & GLGE_VERSION_MINOR_MASK) << GLGE_VERSION_MINOR_OFFSET) | \
                                                   ((patch & GLGE_VERSION_PATCH_MASK) << GLGE_VERSION_PATCH_OFFSET)   \
                                                  )

/**
 * @brief define an utility function that extracts the major version from a version
 */
#define GLGE_VERSION_GET_MAJOR(version) ((version >> GLGE_VERSION_MAJOR_OFFSET) & GLGE_VERSION_MAJOR_MASK)
/**
 * @brief define an utility function that extracts the minor version from a version
 */
#define GLGE_VERSION_GET_MINOR(version) ((version >> GLGE_VERSION_MINOR_OFFSET) & GLGE_VERSION_MINOR_MASK)
/**
 * @brief define an utility function that extracts the patch version from a version
 */
#define GLGE_VERSION_GET_PATCH(version) ((version >> GLGE_VERSION_PATCH_OFFSET) & GLGE_VERSION_PATCH_MASK)

/**
 * @brief define the major library version
 */
#define GLGE_VERSION_MAJOR 0u
/**
 * @brief define the minor library version
 */
#define GLGE_VERSION_MINOR 1u
/**
 * @brief define the patch used
 */
#define GLGE_VERSION_PATCH 0u
/**
 * @brief define the library version string
 */
#define GLGE_VERSION_STR #GLGE_VERSION_MAJOR "." #GLGE_VERSION_MINOR "." #GLGE_VERSION_PATCH

/**
 * @brief define the current GLGE core version
 */
#define GLGE_VERSION GLGE_VERSION_COMBINE(GLGE_VERSION_MAJOR, GLGE_VERSION_MINOR, GLGE_VERSION_PATCH)

//
// Sanity checks
//

/**
 * @brief a quick sanity check to make sure that the size of a version number is at least 32 bits (the size of the version type defined in Common.h)
 */
static_assert(GLGE_VERSION_SIZE <= 32u,
              "Major, Minor and Patch version sizes do not sum up to more than 32 bits");

#endif
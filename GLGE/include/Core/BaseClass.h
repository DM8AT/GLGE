/**
 * @file BaseClass.h
 * @author DM8AT
 * @brief define a class that all advanced GLGE classes should inherit from
 * @version 0.1
 * @date 2025-12-21
 * 
 * @copyright Copyright (c) 2025
 * 
 */
//header guard
#ifndef _GLGE_CORE_BASE_CLASS_
#define _GLGE_CORE_BASE_CLASS_

//add instances
#include "Instance.h"

//use the library namespace
namespace GLGE {

    /**
     * @brief a class that is responsible to manage instance attachment
     */
    class BaseClass {
    public:

        /**
         * @brief Construct a new Base Class
         */
        BaseClass();

        /**
         * @brief Get the Instance the class is attached to
         * 
         * @return Instance* a pointer to the parent instance
         */
        inline Instance* getInstance() const noexcept
        {return m_instance;}

    private:

        /**
         * @brief store a pointer to the instance the class is attached to
         */
        Instance* m_instance = 0;

    };

}

#endif
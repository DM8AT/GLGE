/**
 * @file Contracts.h
 * @author DM8AT
 * @brief define the base for all API contracts
 * @version 0.1
 * @date 2026-05-04
 * 
 * @copyright Copyright (c) 2026
 * 
 */
#ifndef _GLGE_GRAPHIC_BACKEND_VIDEO_API_CONTRACTS_
#define _GLGE_GRAPHIC_BACKEND_VIDEO_API_CONTRACTS_

//use the backend video namespace
namespace GLGE::Graphic::Backend::Video {

    //say that instances exist somewhere
    class Instance;

    /**
     * @brief the namespace that stores all contracts
     * 
     * An API contract is the video backend declaring that it has an interface for a specific graphic API. The contract defines how the 
     * video backend and the graphic API may interface with each other to allow for clean and efficient implementations that keep the
     * concerns separated. 
     * 
     * Because different APIs require fundamentally different functionalities the base contract class does not define what a contract must implement, 
     * it just defines the interface used to identify the contract. The graphic backend may then interface with the functions presented by the contract. 
     * These calls may activate graphic-api specific calls, video api specific calls, both, or none. 
     * 
     * A contract is loaded by the video backend upon the creation of the video backend instance. During the creation the information provided by the graphic
     * backend description is used to load the correct contract. If the API is not recognized, the instance creation may throw, since a contract is always mandatory. 
     * 
     * The contract is owned by the video backend instance. Once it is destroyed, the contract is destroyed too. 
     * 
     */
    namespace Contracts {

        /**
         * @brief store the base class for all contracts
         */
        class BaseContract {
        public:

            /**
             * @brief Construct a new Base Contract
             * 
             * @param instance create a new base contract
             */
            BaseContract(GLGE::Graphic::Backend::Video::Instance* instance)
             : m_instance(instance)
            {}

            /**
             * @brief Destroy the Base Contract
             * 
             * Virtual because a VTable is required for child classes
             */
            virtual ~BaseContract() = default;

            /**
             * @brief Get the Instance
             * 
             * @return `GLGE::Graphic::Backend::Video::Instance*` a pointer to the stored instance
             */
            inline GLGE::Graphic::Backend::Video::Instance* getInstance() const noexcept
            {return m_instance;}

        protected:

            /**
             * @brief store a pointer to the video instance that owns the contract
             */
            GLGE::Graphic::Backend::Video::Instance* m_instance = nullptr;

        };

    }

}

#endif
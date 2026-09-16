/**
 * @file Application.cpp
 * @author DM8AT
 * @brief implement the application
 * @version 0.1
 * @date 2026-05-27
 * 
 * @copyright Copyright (c) 2026
 * 
 */
//add applications
#include "Core/Application.h"

void GLGE::Application::run() {
    //main thread check
    if (!GLGE::Instance::isMainThread()) {throw GLGE::Exception("An application can only be run from the main thread. If this is the main thread, please call \'GLGE::Instance::staticInit()\' before creating an application.", "GLGE::Application");}

    //start the instance
    m_instance.start();

    //loop while the application is active
    while (isActive()) {
        m_instance.startMainTick();

        //update
        onUpdate();

        m_instance.endMainTick();
    }

    //stop the instance
    m_instance.shutdown();
}
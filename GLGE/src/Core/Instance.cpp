/**
 * @file Instance.cpp
 * @author DM8AT
 * @brief implement the full functionality of instances
 * @version 0.1
 * @date 2025-12-12
 * 
 * @copyright Copyright (c) 2025
 * 
 */
//add instances
#include "Core/Instance.h"

//use the libraries namespace
using namespace GLGE;

/**
 * @brief store the version of GLGE with which the library was compiled
 */
static constexpr Version __GLGE_VERSION = GLGE_VERSION;

InstanceExtension::InstanceExtension(const OrderedMap<std::string_view, InstanceExtension*> extensions) 
 : m_extensions(extensions)
{
    //iterate over all extensions and set this as the parent extension
    for (auto& [name, ext] : m_extensions)
    {ext->m_parent = this;}
}

void InstanceExtension::start() {
    GLGE_PROFILER_SCOPE();

    {
        GLGE_PROFILER_SCOPE_NAMED("GLGE::InstanceExtension::start - onStart");

        //call the start binding
        onStart();
    }

    {
        GLGE_PROFILER_SCOPE_NAMED("GLGE::InstanceExtension::start - recursion");

        //iterate over all extensions and start them
        for (auto& [name, ext] : m_extensions)
        {ext->start();}
    }
}

void InstanceExtension::update() {
    GLGE_PROFILER_SCOPE();
    
    {
        GLGE_PROFILER_SCOPE_NAMED("GLGE::InstanceExtension::update - onUpdate");

        //call the update binding
        onUpdate();
    }

    {
        GLGE_PROFILER_SCOPE_NAMED("GLGE::InstanceExtension::update - recursion");

        //iterate over all extensions and update them
        for (auto& [name, ext] : m_extensions)
        {ext->update();}
    }
}

void InstanceExtension::mainUpdate() {
    GLGE_PROFILER_SCOPE();
    
    {
        GLGE_PROFILER_SCOPE_NAMED("GLGE::InstanceExtension::mainUpdate - onMainUpdate");

        //call the update binding
        onMainUpdate();
    }

    {
        GLGE_PROFILER_SCOPE_NAMED("GLGE::InstanceExtension::mainUpdate - recursion");

        //iterate over all extensions and update them
        for (auto& [name, ext] : m_extensions)
        {ext->mainUpdate();}
    }
}

void InstanceExtension::shutdown() {
    GLGE_PROFILER_SCOPE();
    
    {
        GLGE_PROFILER_SCOPE_NAMED("GLGE::InstanceExtension::shutdown - onShutdown");

        //call the shutdown binding
        onShutdown();
    }

    {
        GLGE_PROFILER_SCOPE_NAMED("GLGE::InstanceExtension::shutdown - recursion");

        //iterate over all extensions and shut them down
        for (auto& [name, ext] : m_extensions)
        {ext->shutdown();}
    }
}

void InstanceExtension::setInstance(Instance* inst) noexcept {
    GLGE_PROFILER_SCOPE();
    
    //store the instance pointer
    m_instance = inst;
    {
        GLGE_PROFILER_SCOPE_NAMED("GLGE::InstanceExtension::setInstance - onInstanceSetting");

        //call the on instance set function
        onInstanceSetting();
    }

    {
        GLGE_PROFILER_SCOPE_NAMED("GLGE::InstanceExtension::setInstance - recursion");

        //iterate and recurse over the children
        for (auto& [name, ext] : m_extensions)
        {ext->setInstance(inst);}
    }
}

void InstanceExtension::bind() noexcept {
    GLGE_PROFILER_SCOPE();
    
    {
        GLGE_PROFILER_SCOPE_NAMED("GLGE::InstanceExtension::bind - onBind");

        //call the function hook
        onBind();
    }

    {
        GLGE_PROFILER_SCOPE_NAMED("GLGE::InstanceExtension::bind - recursion");

        //iterate over all children and bind them
        for (auto& [name, ext] : m_extensions)
        {ext->bind();}
    }
}

void InstanceExtension::unbind() noexcept {
    {
        GLGE_PROFILER_SCOPE_NAMED("GLGE::InstanceExtension::unbind - onUnbind");

        //call the function hook
        onUnbind();
    }

    {
        GLGE_PROFILER_SCOPE_NAMED("GLGE::InstanceExtension::unbind - recursion");

        //iterate over all children and unbind them
        for (auto& [name, ext] : m_extensions)
        {ext->unbind();}
    }
}



void Instance::updateThread() {
    GLGE_PROFILER_SET_THREAD_NAME("GLGE::Instance::updateThread");

    //mark the update thread as alive
    m_aliveUpdateThread.store(true, std::memory_order_release);
    //only loop while the instance is running
    while (m_active.load(std::memory_order_acquire)) {
        GLGE_PROFILER_FRAME_MARKER_START("GLGE::Instance::updateThread");

        //start a tick for the update limiter
        m_updateLimiter.startTick();

        {
            GLGE_PROFILER_SCOPE_NAMED("GLGE::Instance::updateThread - update extension propagation");

            //update the instance
            update();
        }

        //finalize the tick
        m_updateLimiter.endTick();

        GLGE_PROFILER_FRAME_MARKER_END("GLGE::Instance::updateThread");
    }
    //the update thread is not alive
    m_aliveUpdateThread.store(false, std::memory_order_release);
}


Instance::~Instance() {
    GLGE_PROFILER_SCOPE();

    //shutdown the instance
    shutdown();
    //and unbind the instance
    unbind();
}

const Version& Instance::getGLGEVersion() noexcept 
{return __GLGE_VERSION;}

bool Instance::isDebug() noexcept {
    //in debug return true, else return false
    #if GLGE_DEBUG
    return true;
    #else
    return false;
    #endif
}

void Instance::start() {
    GLGE_PROFILER_SCOPE();

    //can only be called from the main thread
    #if GLGE_DEBUG
    if (!isMainThread())
    {throw Exception("The start function of an instance must be called from the main thread", "Instance::start");}
    #endif
    //if the instance is allready active, stop
    if (m_active) {return;}

    //start all the extensions
    for (auto& [name, ext] : m_extensions)
    {ext->start();}

    //the instance is now active
    m_active.store(true, std::memory_order_release);

    //start the update thread
    m_updateThread = std::thread(&Instance::updateThread, this);
}

void Instance::update() {
    GLGE_PROFILER_SCOPE();

    //updates are only allowed in an active state
    if (!m_active) {return;}

    //update all the extensions
    for (auto& [name, ext] : m_extensions)
    {ext->update();}

    //update the keyboards
    {
        GLGE_PROFILER_SCOPE_NAMED("GLGE::Instance::update - keyboard update");
        
        //update all registered keyboards
        for (auto& keyboard : m_keyboards) {keyboard.update();}

        //check if the special case of 0xff is set for the selected keyboard
        if (m_preferredKeyboard == 0xff) {
            //press all buttons that are pressed or release all that are not pressed on ANY keyboard
            for (u8 key = 0; key < static_cast<u8>(Key::ENUM_MAX); ++key) {
                bool press = false;
                for (auto& keyboard : m_keyboards) {press |= keyboard.pressed()[key];}
                if (press) {m_mainKeyboard.press(key);}
                else {m_mainKeyboard.release(key);}
            }
            //here it is required to update the main keyboard
            m_mainKeyboard.update();
        } else {
            //just copy over the selected keyboard state or reset if the selected keyboard is invalid
            if (m_preferredKeyboard < m_keyboards.size())
            {m_mainKeyboard = m_keyboards[m_preferredKeyboard];}
            else
            {m_mainKeyboard.fill(false); m_mainKeyboard.update();}

            //do NOT update the main keyboard: it was updated before the copy
        }
    }

    //update the mice
    {
        GLGE_PROFILER_SCOPE_NAMED("GLGE::Instance::update - mouse update");
        
        //update all registered mice
        for (auto& mouse : m_mice) {mouse.update();}

        //just copy over the selected keyboard state or reset if the selected keyboard is invalid
        if (m_preferredMouse < m_mice.size())
        {m_mainMouse = m_mice[m_preferredMouse];}
        else
        {m_mainMouse.move({0,0}, {0,0}); m_mainMouse.fill(false); m_mainMouse.update();}
    }
}

void Instance::shutdown() {
    GLGE_PROFILER_SCOPE();

    //can only be called from the main thread
    #if GLGE_DEBUG
    if (!isMainThread())
    {throw Exception("The shutdown function of an instance must be called from the main thread", "Instance::shutdown");}
    #endif
    //shutdown is only allowed if the instance is active
    if (!m_active) {return;}

    {
        GLGE_PROFILER_SCOPE_NAMED("GLGE::Instance::shutdown - extension shutdown");

        //shutdown all the extensions
        for (auto& [name, ext] : m_extensions)
        {ext->shutdown();}
    }

    {
        GLGE_PROFILER_SCOPE_NAMED("GLGE::Instance::shutdown - marking disabled");

        //mark the instance as not active
        m_active.store(false, std::memory_order_release);
    }

    {
        GLGE_PROFILER_SCOPE_NAMED("GLGE::Instance::shutdown - joining update thread");

        {
            GLGE_PROFILER_SCOPE_NAMED("GLGE::Instance::shutdown - waiting");

            //deactivate the update thread. Make sure that all updates continue during shutdown. 
            while (m_aliveUpdateThread.load(std::memory_order_acquire))
            {mainUpdate();staticUpdate();}
        }

        //join the update thread
        m_updateThread.join();
    }
}

void Instance::startMainTick() {
    //mark the start of a new frame
    GLGE_PROFILER_FRAME_MARKER_START(m_name.data());

    //notify the rate limiter
    m_mainLimiter.startTick();

    //run the main update
    mainUpdate();

    //run the static update
    GLGE::Instance::staticUpdate();
}

void Instance::endMainTick() {
    //notify the rate limiter
    m_mainLimiter.endTick();

    //mark the end of the tick
    GLGE_PROFILER_FRAME_MARKER_END(m_name.data());
}

void Instance::mainUpdate() {
    GLGE_PROFILER_SCOPE();

    //sanity check the main thread requirement in debug
    #if GLGE_DEBUG
    if (!isMainThread())
    {throw Exception("The main update function can only be called from the main thread", "Instance::mainUpdate");}
    #endif

    {
        GLGE_PROFILER_SCOPE_NAMED("GLGE::Instance::mainUpdate - extension main update");

        for (auto& [name, ext] : m_extensions)
        {ext->mainUpdate();}
    }

    {
        GLGE_PROFILER_SCOPE_NAMED("GLGE::Instance::mainUpdate - flushing local task queue");

        //flush the task queue
        m_taskQueue.execute();
    }

    {
        GLGE_PROFILER_SCOPE_NAMED("GLGE::Instance::mainUpdate - flushing local event queue");

        //flush the event system
        Event event;
        while (m_eventQueue.pop(event)) 
        {m_eventBus.emit(event);}
    }
}

bool Instance::pushMainThreadTask(const Task& task) noexcept {
    GLGE_PROFILER_SCOPE();

    //check if this is the main thread to not block the main thread endlessly
    if (isMainThread()) {
        //run the task directly to not give time to block
        ((Task*)&task)->call();
        //always successfully added, even thou it was never added
        return true;
    }

    //else, enqueue the task
    return m_taskQueue.push(task);
}

void Instance::bind() noexcept {
    GLGE_PROFILER_SCOPE();

    //if an instance is bound, unbind it
    if (mls_bound)
    {unbind();}
    //make this the current instance
    mls_bound = this;

    {
        GLGE_PROFILER_SCOPE_NAMED("GLGE::Instance::bind - binding propagation");

        //tell all extensions that this instance is bound
        for (auto& [name, ext] : m_extensions)
        {ext->bind();}
    }
}

void Instance::unbind() noexcept {
    GLGE_PROFILER_SCOPE();

    //if no instance is bound, stop
    if (!mls_bound) {return;}

    {
        GLGE_PROFILER_SCOPE_NAMED("GLGE::Instance::unbind - unbinding propagation");

        //else, tell all extensions of the bound instance that it is no longer bound
        for (auto& [name, ext] : mls_bound->m_extensions)
        {ext->unbind();}
    }

    //set the bound instance to a nullpointer
    mls_bound = nullptr;
}

void Instance::init() {
    GLGE_PROFILER_SCOPE();

    //if the initialization was called already, stop
    if (mls_isInitialized)
    {return;}

    //set a default profiling name
    GLGE_PROFILER_SET_APP_NAME("GLGE APP - UNNAMED");

    //store this as the main thread
    mls_mainThreadID = std::this_thread::get_id();
    //nothing can be bound
    mls_bound = nullptr;

    //initialization finished
    mls_isInitialized = true;
}

void Instance::staticUpdate() {
    GLGE_PROFILER_SCOPE();

    //in debug sanity check if this is the debug thread
    #if GLGE_DEBUG
    if (!isMainThread())
    {throw Exception("The static update can only be called from the main thread", "Instance::staticUpdate");}
    #endif

    //call the hooks
    for (auto& func : mls_staticUpdateHooks) {
        GLGE_PROFILER_SCOPE_NAMED("GLGE::Instance::staticUpdate - single update hook");
        func();
    }
}

void Instance::staticUpdateUnsubscribe(void (*func)()) {
    GLGE_PROFILER_SCOPE();

    for (size_t i = 0; i < mls_staticUpdateHooks.size(); ++i) {
        if (mls_staticUpdateHooks[i] == func) {
            mls_staticUpdateHooks.erase(mls_staticUpdateHooks.begin() + i); 
            break;
        }
    }
}
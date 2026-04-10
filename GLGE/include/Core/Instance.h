/**
 * @file Instance.h
 * @author DM8AT
 * @brief define what a core instance is
 * @version 0.1
 * @date 2025-12-12
 * 
 * @copyright Copyright (c) 2025
 * 
 */
//header guard
#ifndef _GLGE_CORE_INSTANCE_
#define _GLGE_CORE_INSTANCE_

//add common stuff
#include "Common.h"
//add type info
#include "TypeInfo.h"
//add exceptions
#include "Exception.h"
//add task queues and tasks
#include "utils/TaskQueue.h"
//include the event system (Event Bus + Event)
#include "EventBus.h"
//to stop the update thread from utilizing a core fully
#include "RateLimiter.h"
//add the profiler
#include "Core/Profiler.h"
//add the asset system
#include "AssetManager.h"
//add the keyboard and mouse system
#include "Keyboard.h"
#include "Mouse.h"

//use vectors to store lists of data
#include <vector>
//use unordered maps for storing named data
#include <unordered_map>
//this is like a hashable version of const char*
#include <string_view>
//for working with types
#include <type_traits>
//for working with threads
#include <thread>

//add ordered maps
#include "utils/OrderedMap.h"

//add tiny jobs for threading (TinyJobs is my custom job system)
#define TINY_JOBS_NO_FIBERS
#define TINY_JOBS_NO_LOCKFREE
#include "dependencies/TinyJobs.h"

/**
 * @brief use the libraries namespace
 */
namespace GLGE {

    /**
     * @brief instances will be defined later
     */
    class Instance;

    /**
     * @brief an instance extension extends the functionality of the underlying instance. 
     * 
     * It is a structure that directly attaches to the instance extension tree. It has a custom update, start and shutdown 
     * functions that are attached to the main instance start, update and shutdown sequence without major abstraction. 
     * 
     * An instance tree could look like this:
     * ```
     *        [INSTANCE]
     *         |      |
     * [EXTENSION 1]  [EXTENSION 2]
     *                 |         |
     *          [EXTENSION 3]  [EXTENSION 4]
     * ```
     * The functions are passed down, where the parent is called before the children. 
     */
    class InstanceExtension {
    public:

        /**
         * @brief Construct a new Instance Extension
         * 
         * @param extensions a list of extensions to initialize with
         */
        InstanceExtension(const OrderedMap<std::string_view, InstanceExtension*> extensions = {});

        /**
         * @brief Destroy the Instance Component
         * 
         * Make sure to release all imported assets
         */
        virtual ~InstanceExtension() {}

        /**
         * @brief a function that is called when the `start` function of the parent is called. 
         * 
         * The purpose of the function is to prepare for running. Here, stuff like asset loading should be finalizing. 
         * The threads used for async asset loading should rejoin here. 
         */
        virtual void onStart() = 0;

        /**
         * @brief the actual function that is called on start
         */
        virtual void start() final;

        /**
         * @brief a function that is called every tick. This happens when the `update` function of the instance is called. 
         */
        virtual void onUpdate() = 0;

        /**
         * @brief the actual function that is called on update
         */
        virtual void update() final;

        /**
         * @brief a function that is called when the `shutdown` function of the instance is called or when the instance is being deleted - 
         *        whatever happens first. 
         * 
         * The purpose of this is to return to a stable start state. 
         * 
         * @warning this function is not forced to be the end of lifetime of the component. The instance may get re-started. 
         */
        virtual void onShutdown() = 0;

        /**
         * @brief the actual function that is called to shutdown the instance component
         */
        virtual void shutdown() final;

        /**
         * @brief a function that is called when the instance is set
         */
        virtual void onInstanceSetting() = 0;

        /**
         * @brief a function that is called when the instance is bound
         */
        virtual void onBind() = 0;

        /**
         * @brief a function that is called when the instance is unbound
         */
        virtual void onUnbind() = 0;

        /**
         * @brief access the extensions attached to the instance extension
         * 
         * @return OrderedMap& a reference to the attached instances
         */
        inline OrderedMap<std::string_view, InstanceExtension*>& extensions() noexcept
        {return m_extensions;}

        /**
         * @brief Get the Instance the extension is attached to
         * 
         * @return Instance* a pointer to the parent instance
         */
        inline Instance* getInstance() const noexcept
        {return m_instance;}

        /**
         * @brief Get a pointer to the parent extension
         * 
         * @return InstanceExtension* a pointer to the parent extension or `nullptr` if this extension is directly attached to the instance
         */
        inline InstanceExtension* getParent() const noexcept
        {return m_parent;}

    protected:

        /**
         * @brief an instance extension can be extended with extensions too
         */
        OrderedMap<std::string_view, InstanceExtension*> m_extensions;

    private:

        /**
         * @brief add the instance as a friend class
         */
        friend class Instance;

        /**
         * @brief Set the Instance for this extension
         * 
         * @param inst a pointer to the instance to set to
         */
        virtual void setInstance(Instance* inst) noexcept final;

        /**
         * @brief a function that is called when the parent instance is bound
         */
        virtual void bind() noexcept final;

        /**
         * @brief a function that is called when the parent instance is unbound
         */
        virtual void unbind() noexcept final;

        /**
         * @brief store a pointer to the instance the extension is attached to
         */
        Instance* m_instance = nullptr;
        /**
         * @brief store a pointer to the parent extension
         */
        InstanceExtension* m_parent = nullptr;
    };

    /**
     * @brief an instance is the center of every GLGE projet. It is required for most advanced GLGE structures
     * 
     * To simplify creation of other objects an instance is bound - similar to OpenGL. All objects created while the instance is bound will work
     * with the instance that was bound at the time of creation. 
     */
    class Instance {
    public:

        /**
         * @brief create a new instance
         * 
         * @warning this binds this as the current active instance in the thread
         * 
         * @param name the name of the instance
         * @param version the version of the instance
         * @param extensions a named list of extensions to attach to the instance
         */
        template <typename... Args>
        requires (std::is_base_of_v<InstanceExtension, Args> && ...) //all arguments must be valid extensions
        Instance(const std::string_view& name, const Version& version, std::pair<const char*, Args*>... extensions)
         : m_name(name), m_version(version)
        {
            //must be called from the main thread
            if (!isMainThread())
            {throw Exception("Instances can only be created on the main thread. If you are sure that you are calling it from the main thread, check if the static initialization was called.", 
                             "Instance::Instance");}
            
            //first, bind the instance
            bind();

            //then, add all the attachment
            ((addExtension(extensions)), ...);
        }

        /**
         * @brief Construct a new Instance
         * 
         * Explicitly deleted
         */
        Instance(const Instance&) = delete;
        /**
         * @brief Construct a new Instance
         * 
         * Explicitly deleted
         */
        Instance(Instance&&) = delete;

        /**
         * @brief Copy assignment
         * 
         * Explicitly deleted
         */
        Instance& operator=(const Instance&) = delete;
        /**
         * @brief Move assignment
         * 
         * Explicitly deleted
         */
        Instance& operator=(Instance&&) = delete;

        /**
         * @brief Destroy the Instance
         */
        ~Instance();

        /**
         * @brief bind this as the currently active instance
         */
        void bind() noexcept;

        /**
         * @brief unbind the instance from the current thread
         */
        static void unbind() noexcept;

        /**
         * @brief Get the currently bound instance
         * 
         * @return Instance* a pointer to the currently bound instance or NULL if none is bound
         */
        inline static Instance* getCurrentInstance() noexcept
        {return mls_bound;}

        /**
         * @brief get the internal version of GLGE
         * 
         * @return const Version& a constant reference to the constant GLGE version
         */
        static const Version& getGLGEVersion() noexcept;

        /**
         * @brief get if this is a debug build
         * 
         * @return `true` if this is a debug build, else `false`
         */
        static bool isDebug() noexcept;

        /**
         * @brief Get the Version of the instance
         * 
         * @return `const Version&` a constant reference to the version of the instance
         */
        inline const Version& getVersion() const noexcept
        {return m_version;}

        /**
         * @brief Get the Name of the instance
         * 
         * @return `const std::string_view&` a constant reference to the name of the instance
         */
        inline const std::string_view& getName() const noexcept
        {return m_name;}

        /**
         * @brief get if the instance is active
         * 
         * @return `true` if the instance is active, `false` otherwise
         */
        inline bool isActive() const noexcept
        {return m_active.load(std::memory_order_acquire);}

        /**
         * @brief add a new extension to the instance
         * 
         * @tparam T the type of instance extension to add
         * @param extension the packed extension to add
         * @return `true` if the extension was added successfully, `false` otherwise
         */
        template <typename T>
        requires std::is_base_of_v<InstanceExtension, T> //T must be a valid extension
        bool addExtension(std::pair<const char*, T*> extension) 
        {return addExtension(extension.first, extension.second);}

        /**
         * @brief add a new extension to the instance
         * 
         * @tparam T the type of instance extension to add
         * @param name the name of the instance to add
         * @param extension a pointer to the extension to add
         * @return `true` if the extension was added successfully, `false` otherwise
         */
        template <typename T>
        requires std::is_base_of_v<InstanceExtension, T> //T must be a valid extension
        bool addExtension(std::string_view name, T* extension) {
            //check if the extension exists
            if (m_extTypeHash.contains(getTypeHash64<T>()))
            {return false;}
            //check if the name exists
            if (m_extensions.contains(name))
            {return false;}

            //store the type hash
            m_extTypeHash.insert_or_assign(getTypeHash64<T>(), m_extensions.size());
            //add the new entry
            m_extensions.push_back({name, extension});

            //notify the extension about the attachment
            extension->setInstance(this);

            //success
            return true;
        }

        /**
         * @brief check if the instance is extended by a specific extension
         * 
         * @tparam T the extension type to check
         * @return `true` if the extension exists, `false` otherwise
         */
        template <typename T>
        requires std::is_base_of_v<InstanceExtension, T> //T must be a valid extension
        inline bool hasExtension() const noexcept
        {return m_extTypeHash.contains(getTypeHash64<T>());}

        /**
         * @brief Get a specific instance extension
         * 
         * @tparam T the type of the extension to get
         * @return `T*` a pointer to the stored extension
         */
        template <typename T>
        requires std::is_base_of_v<InstanceExtension, T> //T must be a valid extension
        inline T* getExtension() const noexcept
        {return (hasExtension<T>()) ? ((T*)m_extensions[m_extTypeHash.find(getTypeHash64<T>())->second]) : nullptr;}

        /**
         * @brief start the instance and prepare to run the application
         */
        void start();

        /**
         * @brief tick the instance
         */
        void update();

        /**
         * @brief prepare the instance to stop the looping
         */
        void shutdown();

        /**
         * @brief an update function for the instance that must be called from the main thread
         */
        void mainUpdate();

        /**
         * @brief push a new task to the internal task queue
         * 
         * @param task the task to queue to be run on the main thread
         * @return `true` if the task was pushed successfully, `false` otherwise
         */
        bool pushMainThreadTask(const Task& task) noexcept;

        /**
         * @brief access the extensions of the instance
         * 
         * @return OrderedMap<std::string_view, InstanceExtension*> an ordered map of the extensions
         */
        inline OrderedMap<std::string_view, InstanceExtension*> extensions() noexcept
        {return m_extensions;}

        /**
         * @brief access the event bus of the instance
         * 
         * @return `EventBus&` a reference to the event bus
         */
        inline EventBus& eventBus() noexcept
        {return m_eventBus;}

        /**
         * @brief queue an event so that it executes on the main thread
         * 
         * @param event a constant reference to the event to enqueue
         * @return `true` when the event was pushed successfully, `false` otherwise
         */
        inline bool queueEvent(const Event& event)
        {return m_eventQueue.push(event);}

        /**
         * @brief queue a new event so that it executes on the main thread
         * 
         * @tparam Args the types of the arguments to enqueue
         * @param id the ID for the new event
         * @param args the arguments for the event
         * @return `true` when the event was pushed successfully, `false` otherwise
         */
        template <typename... Args>
        inline bool queueEvent(u64 id, Args&&... args)
        {return queueEvent(Event::create<Args...>(id, std::forward<Args>(args)...));}

        /**
         * @brief queue a new event so that it executes on the main thread
         * 
         * @tparam Args the types of the arguments to enqueue
         * @param name the name for the event. The ID is computed as the FNV 1A hash of the string. 
         * @param args the arguments for the event
         * @return `true` when the event was pushed successfully, `false` otherwise
         */
        template <typename... Args>
        inline bool queueEvent(const std::string_view& name, Args&&... args)
        {return queueEvent(Event::create<Args...>(name, std::forward<Args>(args)...));}

        /**
         * @brief emit an event directly to the event bus
         * 
         * @param event a constant reference to the event to emit
         */
        inline void emitEvent(const Event& event)
        {m_eventBus.emit(event);}

        /**
         * @brief emit an event directly to the event bus
         * 
         * @tparam Args the types of the arguments for the event
         * @param id the ID for the event
         * @param args the actual arguments for the event
         */
        template <typename... Args>
        inline void emitEvent(u64 id, Args&&... args)
        {emitEvent(Event::create<Args...>(id, std::forward<Args>(args)...));}

        /**
         * @brief emit an event directly to the event bus
         * 
         * @tparam Args the types of the arguments for the event
         * @param name the name for the event. The ID is computed as the FNV 1A 64 bit hash of the string. 
         * @param args the actual arguments for the event
         */
        template <typename... Args>
        inline void emitEvent(const std::string_view& name, Args&&... args)
        {emitEvent(Event::create<Args...>(name, args...));}

        /**
         * @brief access the update limiter
         * 
         * @return `RateLimit&` a reference to the rate limiter
         */
        inline RateLimit& updateLimiter() noexcept
        {return m_updateLimiter;}

        /**
         * @brief access all the assets of the instance
         * 
         * @return `AssetManager&` a reference to the asset manager
         */
        inline AssetManager& assets() noexcept
        {return m_assetManager;}

        /**
         * @brief access the employer of the instance
         * 
         * @return `Tiny::Jobs::Employer&` a reference to the employer of the instance
         */
        inline Tiny::Jobs::Employer& employer() noexcept
        {return m_employer;}

        /**
         * @brief register a new keyboard
         * 
         * @param keyboard the keyboard to register
         */
        inline void registerKeyboard(const Keyboard& keyboard) {
            //in debug sanity check the index
            #ifdef GLGE_DEBUG
            if (m_keyboards.size() == (UINT8_MAX-1)) 
            {throw Exception("Failed to register the keyboard: No free keyboard index is available", "GLGE::Instance::registerKeyboard");} 
            #endif
            //add the keyboard
            m_keyboards.push_back(keyboard);
        }

        /**
         * @brief Get the index of a keyboard using the name
         * 
         * @param name the name of the keyboard to get
         * @return `u8` the index of the keyboard, `UINT8_MAX` means that the keyboard was not found
         */
        inline u8 getIndexOfKeyboard(const std::string& name) noexcept {
            //just iterate over all keyboards and store the index if the name matches
            u8 idx = UINT8_MAX;
            for (u8 i = 0; i < static_cast<u8>(m_keyboards.size()); ++i) 
            {if (m_keyboards[i].getName() == name) {idx = i; break;}}
            return idx;
        }

        /**
         * @brief remove a keyboard 
         * 
         * @param idx the index of the keyboard to remove
         */
        inline void removeKeyboard(u8 idx) {
            //in debug sanity check the index
            #ifdef GLGE_DEBUG
            if (m_keyboards.size() <= idx)
            {throw Exception("Failed to remove keyboard - index out of range", "GLGE::Instance::removeKeyboard");}
            #endif
            //erase the keyboard
            m_keyboards.erase(m_keyboards.begin() + idx);
        }

        /**
         * @brief remove a keyboard
         * 
         * @param name the name of the keyboard to remove
         */
        inline void removeKeyboard(const std::string& name)
        {removeKeyboard(getIndexOfKeyboard(name));}

        /**
         * @brief Get all registered keyboards
         * 
         * @return `const std::vector<Keyboard>&` a constant reference to the stored list of keyboards
         */
        inline const std::vector<Keyboard>& getKeyboards() const noexcept
        {return m_keyboards;}

        /**
         * @brief Get a specific keyboard
         * 
         * @return `const Keyboard&` a constant reference to the main keyboard
         */
        inline Keyboard& getKeyboard() noexcept
        {return m_mainKeyboard;}

        /**
         * @brief Get a specific keyboard
         * 
         * @param idx the index of the keyboard to get
         * @return `const Keyboard&` a constant reference to the selected keyboard
         */
        inline Keyboard& getKeyboard(u8 idx) noexcept
        {return m_keyboards[idx];}

        /**
         * @brief set the selected preferred keyboard ID
         * 
         * @param id the ID of the preferred keyboard, use `UINT8_MAX` to or all registered keyboards together
         */
        inline void selectPreferredKeyboard(u8 id) noexcept
        {m_preferredKeyboard = id;}

        /**
         * @brief Get the Preferred Keyboard Id
         * 
         * @return `u8` the ID of the preferred keyboard, or `UINT8_MAX` for a combined keyboard
         */
        inline u8 getPreferredKeyboardId() const noexcept
        {return m_preferredKeyboard;}

        /**
         * @brief register a new mouse
         * 
         * @param mouse the mouse to register
         */
        inline void registerMouse(const Mouse& mouse) {
            //in debug sanity check the index
            #ifdef GLGE_DEBUG
            if (m_mice.size() == (UINT8_MAX-1)) 
            {throw Exception("Failed to register the mouse: No free mouse index is available", "GLGE::Instance::registerMouse");} 
            #endif
            //add the mouse
            m_mice.push_back(mouse);
        }

        /**
         * @brief Get the index of a mouse using the name
         * 
         * @param name the name of the mouse to get
         * @return `u8` the index of the mouse, `UINT8_MAX` means that the mouse was not found
         */
        inline u8 getIndexOfMouse(const std::string& name) noexcept {
            //just iterate over all mice and store the index if the name matches
            u8 idx = UINT8_MAX;
            for (u8 i = 0; i < static_cast<u8>(m_mice.size()); ++i) 
            {if (m_mice[i].getName() == name) {idx = i; break;}}
            return idx;
        }

        /**
         * @brief remove a mouse
         * 
         * @param idx the index of the mouse to remove
         */
        inline void removeMouse(u8 idx) {
            //in debug sanity check the index
            #ifdef GLGE_DEBUG
            if (m_mice.size() <= idx)
            {throw Exception("Failed to remove mouse - index out of range", "GLGE::Instance::removeMouse");}
            #endif
            //erase the mouse
            m_mice.erase(m_mice.begin() + idx);
        }

        /**
         * @brief remove a mouse
         * 
         * @param name the name of the mouse to remove
         */
        inline void removeMouse(const std::string& name)
        {removeKeyboard(getIndexOfMouse(name));}

        /**
         * @brief Get all registered mice
         * 
         * @return `const std::vector<Mouse>&` a constant reference to the stored list of mice
         */
        inline const std::vector<Mouse>& getMice() const noexcept
        {return m_mice;}

        /**
         * @brief Get a specific mouse
         * 
         * @return `const Mouse&` a constant reference to the main mouse
         */
        inline const Mouse& getMouse() const noexcept
        {return m_mainMouse;}

        /**
         * @brief Get a specific mouse
         * 
         * @param idx the index of the mouse to get
         * @return `const Mouse&` a constant reference to the selected mouse
         */
        inline const Mouse& getMouse(u8 idx) const noexcept
        {return m_mice[idx];}

        /**
         * @brief set the selected preferred mouse ID
         * 
         * @param id the ID of the preferred mouse
         */
        inline void selectPreferredMouse(u8 id) noexcept
        {m_preferredMouse = id;}

        /**
         * @brief Get the preferred mouse Id
         * 
         * @return `u8` the ID of the preferred mouse
         */
        inline u8 getPreferredMouseId() const noexcept
        {return m_preferredMouse;}






        /**
         * @brief statically initialize the library core
         * 
         * @warning this must be called from the main thread before anything else may execute
         */
        static void init();

        /**
         * @brief check if the instance is statically initialized
         * 
         * @return `true` if the instance was statically initialized, `false` otherwise
         */
        inline static bool isInitialized() noexcept
        {return mls_isInitialized;}

        /**
         * @brief check if a specific thread is the main thread of the application
         * 
         * @return `true` if the requested thread is the main thread, `false` otherwise. `false` is also outputted if the library was not statically initialized first. 
         */
        inline static bool isMainThread() noexcept
        {return mls_isInitialized && (std::this_thread::get_id() == mls_mainThreadID);}

        /**
         * @brief a static update function
         * 
         * This function must be called from the main tick, every tick
         */
        static void staticUpdate();

        /**
         * @brief subscribe to the static update function
         * 
         * @param func the function to call back to during static update
         */
        inline static void staticUpdateSubscribe(void (*func)())
        {mls_staticUpdateHooks.push_back(func);}

        /**
         * @brief unsubscribe a specific function from the static update function
         * 
         * @param func the function to call back to
         */
        static void staticUpdateUnsubscribe(void (*func)());

    private:

        /**
         * @brief the function that runs on the update thread
         */
        void updateThread();
    
        /**
         * @brief store the name of the instance
         */
        std::string_view m_name = "[GLGE] UNNAMED_INSTANCE";
        /**
         * @brief store the version of whatever the instance is running
         */
        Version m_version = 0;
        /**
         * @brief store if the instance is currently active
         */
        std::atomic_bool m_active{false};
        /**
         * @brief store a flag if an alive update thread exists
         */
        std::atomic_bool m_aliveUpdateThread{false};
        /**
         * @brief store all extensions attached to the instance
         */
        OrderedMap<std::string_view, InstanceExtension*> m_extensions;
        /**
         * @brief store type hashes for all the extensions
         */
        std::unordered_map<u64, size_t> m_extTypeHash;

        /**
         * @brief store the main update thread for the instance
         */
        std::thread m_updateThread;

        /**
         * @brief store the task queue for the instance
         */
        TaskQueue<512> m_taskQueue;

        /**
         * @brief store a queue for events that are handled from main update
         */
        RingBuffer_MP_SC<Event, 256> m_eventQueue;
        /**
         * @brief store the event bus
         * 
         * Events may be handled directly (from all threads) but also queued up and handled from main update
         */
        EventBus m_eventBus;
        /**
         * @brief store the rate limiter for the update thread
         */
        RateLimit m_updateLimiter;

        /**
         * @brief store all the assets used by this instance
         */
        AssetManager m_assetManager;

        /**
         * @brief store the employer for the job system
         * 
         * The employer works as a thread pool
         */
        Tiny::Jobs::Employer m_employer;

        /**
         * @brief store the main combined keyboard
         */
        Keyboard m_mainKeyboard;
        /**
         * @brief store the index of the preferred keyboard
         * 
         * 0xff is reserved for a logical keyboard that has all hardware keyboards or'd together
         */
        u8 m_preferredKeyboard = 0xff;
        /**
         * @brief store the main combined mouse
         */
        Mouse m_mainMouse;
        /**
         * @brief store the index of the preferred mouse
         * 
         * the default index is 0, no exceptions exist
         */
        u8 m_preferredMouse = 0;

        /**
         * @brief store all registered keyboards
         */
        std::vector<Keyboard> m_keyboards;
        /**
         * @brief store all registered mice
         */
        std::vector<Mouse> m_mice;

        /**
         * @brief store the instance that is currently bound to this thread
         */
        inline static thread_local Instance* mls_bound = nullptr;

        /**
         * @brief get the ID for the main thread of the application
         */
        inline static std::thread::id mls_mainThreadID{};
        /**
         * @brief store if the static initialization function of the library was called
         */
        inline static bool mls_isInitialized = false;

        /**
         * @brief store hooks that are called during static updates
         */
        inline static std::vector<void (*)()> mls_staticUpdateHooks;

    };

}

#endif
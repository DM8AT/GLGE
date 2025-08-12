/**
 * @file GLGEInstance.h
 * @author DM8AT
 * @brief define the layout of an GLGE instance
 * @version 0.1
 * @date 2025-04-12
 * 
 * @copyright Copyright (c) 2025
 * 
 */

//header guard
#ifndef _GLGE_INSTANCE_
#define _GLGE_INSTANCE_

//include the settings
#include "GLGESettings.h"
//include the APIs
#include "APIs/GLGEApi.h"
//include a logger
#include "Logging/GLGELogger.h"
//include the time
#include "Time/GLGETime.h"
//include limiter
#include "Time/GLGELimiter.h"
//include layers
#include "GLGELayers.h"

/**
 * @brief specify what type an instance attachable class is
 */
typedef enum e_InstAttachableType
{
    /**
     * @brief specify that the type of the instance attachable is an object
     */
    ATTACHMENT_TYPE_OBJECT = 0,
    /**
     * @brief specify that the type of the instance attachable is a world
     */
    ATTACHMENT_TYPE_WORLD,
    /**
     * @brief specify that the type of the instance attachable is a buffer
     */
    ATTACHMENT_TYPE_BUFFER,
    /**
     * @brief specify that the type of the instance attachable is a framebuffer
     */
    ATTACHMENT_TYPE_FRAMEBUFFER,
    /**
     * @brief specify that the type of the instance attachable is a render material
     */
    ATTACHMENT_TYPE_RENDER_MATERIAL,
    /**
     * @brief specify that the type of the instance attachable is a render pipeline
     */
    ATTACHMENT_TYPE_RENDER_PIPELINE,
    /**
     * @brief specify that the type of the instance attachable is a render vertex layout
     */
    ATTACHMENT_TYPE_RENDER_VERTEX_LAYOUT,
    /**
     * @brief specify that the type of the instance attachable is a texture
     */
    ATTACHMENT_TYPE_TEXTURE,
    /**
     * @brief specify that the type of the instance attachable is a window
     */
    ATTACHMENT_TYPE_WINDOW,
    /**
     * @brief specify that the type of the instance attachable is a graphic instance
     */
    ATTACHMENT_TYPE_GRAPHIC_INSTANCE,
    /**
     * @brief specify that the type of the instance attachable is a shader
     */
    ATTACHMENT_TYPE_SHADER,
    /**
     * @brief specify that the type of the instance attachable is an asset manager
     */
    ATTACHMENT_TYPE_ASSET_MANAGER
} InstAttachableType;

//check if this is C++
#if GLGE_CPP

/**
 * @brief print an instance of the instance attachable type enum into an output stream
 * 
 * @param os the output stream to print to
 * @param type the type to print
 * @return std::ostream& the filled output stream
 */
std::ostream& operator<<(std::ostream& os, const InstAttachableType& type) noexcept;

//check if graphics should be included
#if GLGE_INCLUDE_GRAPHICS
//say that graphic instances will be defined later
class GraphicInstance;
//say that windows will be defined later
class Window;
//say that buffers will be defined later
class Buffer;

//include keyboards
#include "../GLGEGraphic/GLGEKeyboard.h"
//include mouse
#include "../GLGEGraphic/GLGEMouse.h"

#endif //Graphic API include

/**
 * @brief attatchable classes will be defined later
 */
class InstAttachableClass;

/**
 * @brief a instance is a class that owns all other GLGE classes that need an instance in construction
 */
class Instance
{
public:

    /**
     * @brief Construct a new Instance
     * @warning forbidden, no name nor an API is defined
     */
    Instance() = delete;

    /**
     * @brief forbid the copy operator, only one instance of the instance may exist at any time
     */
    void operator=(Instance&) = delete;

    /**
     * @brief Construct a new Instance
     * 
     * @param name the name for the instance
     * @param api the API to use
     */
    Instance(std::string_view name, APIs api);

    /**
     * @brief Destroy the Instance
     */
    ~Instance() noexcept;

    /**
     * @brief Set the logger for the instance
     * 
     * @param logger a pointer to a logger or a class that publicly inherits from the logger base class
     */
    inline void setLogger(Logger* logger) noexcept {m_logger = logger;}

    /**
     * @brief Get the logger used by the instance
     * 
     * @return Logger* a pointer to the logger
     */
    inline Logger* getLogger() noexcept {return m_logger;}

    /**
     * @brief Get the name of the instance
     * 
     * @return const std::string_view& a constant reference to the name
     */
    inline const std::string_view& getName() const noexcept {return m_name;}

    /**
     * @brief get the API of the instance
     * 
     * @return APIs the API of the instance
     */
    inline APIs getAPI() const noexcept {return m_api;}

    /**
     * @brief log a message if a logger exists
     * 
     * @param args the arguments for the message creation
     */
    template<typename... Args> inline void log(Args&&... args) noexcept {if (m_logger) {m_logger->log((args)...);}}

    /**
     * @brief log a message if a logger exists and debug mode is enabled
     * 
     * @param args the arguments for the message creation
     */
    template<typename... Args> inline void logDebug(Args&&... args) noexcept {if (m_logger) {if (m_logger->isDebug()) {m_logger->log((args)...);}}}

    /**
     * @brief print the instance into the default output
     * 
     * @param os a refernece to the output stream to print into
     * @param i a constant reference to the instance to print
     * @return std::ostream& a reference to the filled output stream
     */
    inline friend std::ostream& operator<<(std::ostream& os, const Instance& i) {return os << "instance{name: \"" << i.m_name << "\", api: " << i.m_api << ", current elements: " << i.m_elements << "}";}

    /**
     * @brief add a new object to the class
     * 
     * @param element a pointer to the instance to add
     */
    void addElement(InstAttachableClass* element) noexcept;

    /**
     * @brief remove a class instance from the instance
     * 
     * @param element a pointer to the instance to remove
     */
    void removeElement(InstAttachableClass* element) noexcept;

    /**
     * @brief check if the instance is active
     * 
     * @return true : the instance is active
     * @return false : the instance is not active
     */
    inline bool isActive() const noexcept {return m_active;}

    /**
     * @brief Get all elements that are attatched to the instance
     * @warning DO ONLY USE IF YOU KNOW WHAT YOU ARE DOING
     * 
     * @return std::vector<InstAttachableClass*>& a reference to the vector containing all the elements
     */
    inline std::vector<InstAttachableClass*>& getAttatchedElements() {return m_elements;}

    /**
     * @brief get a reference to the update limiter
     * 
     * @return Limiter& a reference to the limiter for the update thread
     */
    inline Limiter& updateLimiter() noexcept {return m_updateLimiter;}

    /**
     * @brief add a layer to the layer stack that is shared between windows
     * 
     * @param layer a pointer to the layer to add to the stack
     */
    inline void addLayer(Layer* layer) noexcept {m_globalLayers.addLayer(layer);}

    /**
     * @brief send an event through the shared layer stack untill it is handled or reaches the bottom
     * 
     * @param event a pointer to the event to send
     * 
     * @return true : the event was handled
     * @return false : the event was not handled
     */
    inline bool handleEvent(Event* event) noexcept {return m_globalLayers.sendEvent(event);}

    //check if graphics should be included
    #if GLGE_INCLUDE_GRAPHICS

    /**
     * @brief Set the Graphic Instance
     * @warning ONLY USE IF YOU KNOW WHAT YOU ARE DOING
     * 
     * @param gInstance a pointer to the new graphic instance
     */
    inline void setGraphicInstance(GraphicInstance* gInstance) noexcept {m_gInstance = gInstance;}

    /**
     * @brief Get the Graphic Instance
     * 
     * @return GraphicInstance* a pointer to the graphic instance
     */
    inline GraphicInstance* getGraphicInstance() noexcept {return m_gInstance;}

    /**
     * @brief syncronize with the initalization of the instance
     */
    void syncGraphicSetup() const noexcept;

    /**
     * @brief initalize the graphic API
     * 
     * @param window a pointer to the window to create from
     */
    void initGraphicAPI(Window* window);
    /**
     * @brief close the graphic API
     */
    void closeGraphiAPI();

    /**
     * @brief get the SLD2 limiter
     * 
     * @return Limiter& the limiter for the SDL2 thread
     */
    static inline Limiter& getSDLLimiter() noexcept {return m_sdlLimiter;}

    /**
     * @brief Get the Texture Buffer
     * @warning the format of the textures is API-dependend. 
     * 
     * @return Buffer* a buffer containing references to all instances
     */
    Buffer* getTextureBuffer() const noexcept;

    /**
     * @brief Get the Image Buffer
     * @warning the format of the textures is API-dependend. 
     * 
     * @return Buffer* a buffer containing references to all instances
     */
    Buffer* getImageBuffer() const noexcept;

    /**
     * @brief Get the Camera Buffer
     * 
     * @return Buffer* a buffer containing references to all instances
     */
    Buffer* getCameraBuffer() const noexcept;

    /**
     * @brief Get the Object Transform Buffer
     * 
     * @return Buffer* a buffer containing the transforms of all objects. Indexd by the object's identifyer
     */
    Buffer* getObjectTransformBuffer() const noexcept;

    /**
     * @brief Get the keyboard of the instance
     * 
     * @return Keyboard& a reference to the keyboard that stores all keys pressed for the instance
     */
    inline Keyboard& getKeys() noexcept {return m_keyboard;}

    /**
     * @brief Get the keyboard with all keys that changed
     * 
     * @return Keyboard& a reference to the keyboard with all changed keys
     */
    inline Keyboard& getChangedKeys() noexcept {return m_toggled;}

    //add the keyboard layer as a friend class
    friend class KeyboardEventLayer;

    /**
     * @brief Get the mouse state of the instance
     * 
     * @return Mouse& a reference to the mouse state stored by the instance
     */
    inline Mouse& getMouse() noexcept {return m_mouse;}

    /**
     * @brief Get how much the mouse was changed
     * 
     * @return Mouse& a reference to a mouse that stores the change in the mouse data
     */
    inline Mouse& getMouseChange() noexcept {return m_deltaMouse;}

    /**
     * @brief Set if the mouse cursor is shown or hidden
     * 
     * @param shown true : the cursor is shown | false : the cursor is hidden
     */
    void setMouseCursorShown(bool shown) noexcept;

    /**
     * @brief set a window to capture the mouse motion
     * 
     * @param window the window to capture the mouse or 0 to release the mouse
     */
    void captureMouseByWindow(const Window* window) noexcept;

    /**
     * @brief get if some window has captured the mouse
     * 
     * @return true : the mouse is captured by some window
     * @return false : the mouse is not captured
     */
    inline bool isMouseCaptured() const noexcept {return m_hasMouseCaptured;}

    /**
     * @brief get wich window has the mouse captured
     * @warning may be 0
     * 
     * @return Window* a pointer to the window or 0 if the mouse is not captured
     */
    inline Window* hasMouseCaptured() const noexcept {return m_hasMouseCaptured;}

    /**
     * @brief Set if the mouse cursor is visible when it is captured
     * 
     * @param visible true : the cursor is visible | false : the cursor is hidden
     */
    void setCapturedCursorVisible(bool visible) const noexcept;

    /**
     * @brief Set if the cursor will be constantly centered if it is captured
     * 
     * @param center true : the cursor will be captured | false : the cursor is free to move inside the window
     */
    void setCapturedCursorCenter(bool center) const noexcept;

    //add the mouse layer as a friend class
    friend class MouseEventLayer;

    #endif //Graphic only section

    /**
     * @brief Get an instance attatchable class pointer to an element by it's name
     * 
     * @param name the name of the element to querry
     * @return InstAttachableClass* a pointer to the instance attatchable that has that name or 0 if the name dosn't exist
     */
    InstAttachableClass* getNamed(const std::string& name) const noexcept;

private:

    /**
     * @brief store the name of the instance
     */
    std::string_view m_name = "";
    /**
     * @brief store the API the instance uses
     */
    APIs m_api = API_FALLBACK_ERROR;

    /**
     * @brief store if the instance is active
     */
    bool m_active = true;

    /**
     * @brief store the own update thread
     */
    std::thread m_updateThread;

    /**
     * @brief store the own logger, 0 = no logging
     */
    Logger* m_logger = 0;

    /**
     * @brief store the limiter for the update thread
     */
    Limiter m_updateLimiter;

    /**
     * @brief store all elements that are attatched to the class
     */
    std::vector<InstAttachableClass*> m_elements;

    /**
     * @brief store a layer stack that shares events between windows
     */
    LayerStack m_globalLayers;

    //check if graphics should be included
    #if GLGE_INCLUDE_GRAPHICS
    /**
     * @brief store a pointer to the graphic instance
     */
    GraphicInstance* m_gInstance = 0;

    /**
     * @brief store a global keyboard that stores all key states for the instance
     */
    Keyboard m_keyboard;
    /**
     * @brief store a global keyboard that stores all toggled key states for the instance
     * @warning a value here is true if it CHANGED, NOT if it CHANGED TO TRUE
     */
    Keyboard m_toggled;

    /**
     * @brief store the current mouse state
     */
    Mouse m_mouse;
    /**
     * @brief store the difference in mouse state
     * 
     * Buttons : stores if the buttons where toggled
     * position : stores how much the mouse was moved
     */
    Mouse m_deltaMouse;

    /**
     * @brief store the SDL limiter
     */
    static Limiter m_sdlLimiter;

    /**
     * @brief store a pointer to the window that has captured the mouse
     */
    Window* m_hasMouseCaptured = 0;

    #endif //Graphic only section
};

/**
 * @brief a simple virtual class to add objects to a GLGE instance
 */
class InstAttachableClass
{
public:

    /**
     * @brief Construct a new instance
     * Explicetly deleted, can't be used uninitalized
     */
    InstAttachableClass() = delete;

    /**
     * @brief Construct a new instance
     * 
     * @param instance a pointer to the GLGE instance the new instance will belong to
     * @param type the enum identifyer of the type of the instance attachment
     * @param name the name of the instance attatchable
     */
    inline InstAttachableClass(Instance* instance, InstAttachableType type, const std::string& name) : m_instance(instance), m_type(type), m_name(name) {m_instance->addElement(this);}

    /**
     * @brief Destroy the instance
     */
    inline virtual ~InstAttachableClass() {if (!m_instance) {return;} m_instance->removeElement(this);m_instance = 0;}

    /**
     * @brief print an instance of this class into an output stream
     * 
     * @param os the output stream to print into
     * @param iac the instance to print
     * @return std::ostream& a reference to the output stream with the new content
     */
    inline friend std::ostream& operator<<(std::ostream& os, const InstAttachableClass& iac) noexcept {return os << "InstAttachableClass{this: " << &iac << ", type: " << iac.m_type << ", name: \"" << iac.m_name << "\", instance: " << iac.m_instance << ", instance name: \"" << iac.m_instance->getName() << "\"}";}

    /**
     * @brief update the attatched object
     * 
     * @return true : the element still exists
     * @return false : the element no longer exists
     */
    virtual bool onUpdate() {return true;}

    /**
     * @brief Get the Instance the attatchable is attatched to
     * 
     * @return Instance* a pointer to the parent instance
     */
    inline Instance* getInstance() noexcept {return m_instance;}

    /**
     * @brief Get the Name of the instance attatchable
     * 
     * @return const std::string& a constant reference to the name of the instance attatchable
     */
    inline const std::string& getName() const noexcept {return m_name;}

    /**
     * @brief Get the Type of the instance attachable class
     * 
     * @return InstAttachableType the type of the instance attachable class
     */
    inline InstAttachableType getType() const noexcept {return m_type;}

protected:

    /**
     * @brief store the instance the class belongs to
     */
    Instance* m_instance = 0;
    /**
     * @brief store the type of the instance attachable class
     */
    InstAttachableType m_type;
    /**
     * @brief store the name of the instance attatchable
     */
    std::string m_name = "";

};

#endif

#endif
#include "GLGE/GLGE.h"

class Attatchment : public ObjectAttatchable
{
public:

    Attatchment() = default;

    virtual void onAttatch() override
    {
        std::cout << m_message << *m_object << "\n";
    }

    virtual void onUpdate() override
    {
        //log the update
        m_object->getInstance()->log(m_tick.data(), MESSAGE_TYPE_INFO);
    }

    /**
     * @brief Get the Type Name
     * 
     * @return const char* always Attatchment
     */
    virtual const char* getTypeName() const noexcept override {return "Attatchment";}

    /**
     * @brief Set the tick message for the attatchment
     * 
     * @param m the tick message
     */
    inline void setMsg(const std::string_view& m) {m_tick = m;}
    
protected:

    std::string_view m_tick = "Tick";
    std::string_view m_message = "Hello World! Object: ";

};

class FileLogger : public Logger
{
public:
    /**
     * @brief Construct a new File Logger
     * 
     * @param path the path to the log file
     * @param debug say if debugging is enabled
     */
    FileLogger(Path path, bool debug) : Logger(debug), m_file(path, true)
    {
        //clear the file contents
        m_file.content() = "";
    }

    /**
     * @brief print one message into the file
     */
    virtual void print() override
    {
        //format the line and print it to the file
        std::stringstream stream;
        //get the string for the message type
        std::string_view typestr;
        getMessageTypeString(typestr, m_messages[0].getMessageType());
        //output the time, then the type string and finaly the message
        stream << "At " << m_messages[0].getTime() << " : " << typestr << " " << m_messages[0].getMessage() << "\n";

        //print the data to the file
        m_file.content() += stream.str();
        //save the file
        m_file.save();
    }

protected:
    //store the file to log to
    File m_file;
};

int main()
{
    std::vector<APIs> apis = {
        API_OPENGL_4_6
    };
    APIs best = getBestGraphicAPI(apis.data(), apis.size());
    if (best == API_FALLBACK_ERROR) {return 1;}
    Instance inst("Main instance", best);
    //set a logger for the instance
    inst.setLogger(new FileLogger(Path("TEST_CPP.log", PATH_LAYOUT_DEBIAN), true));

    WindowSettings settings = WINDOW_SETTINGS_DEFAULT;
    Window win("Hello World!", 600, 0, settings, inst);
    Texture tex("assets/textures/cubeTexture.png", true, inst);
    Texture depth(TEXTURE_PURPOSE_DEPTH, win.getSize(), 0, 0, inst);

    //store all elements for a vertex
    VertexLayoutElement elements[] = {
        {"Position", sizeof(vec3)},
        {"Normal", sizeof(vec3)},
        {"Texture", sizeof(vec2)}
    };
    //create the vertex layout
    VertexLayout layout(elements, sizeof(elements)/sizeof(*elements));
    std::cout << layout << "\n";

    Object obj1("Object", Transform(vec3(0)), 0, 0, inst);
    Object child("Child", Transform(vec3(0)), 0, 0, inst);
    obj1.addChild(child);
    child.addAttatchment(new Attatchment());
    child.getFirstOf<Attatchment>()->setMsg("Hello");

    //store all elements for the world 
    Object* objects[] = {&obj1};
    World world(objects, sizeof(objects)/sizeof(*objects), "Hello World!", inst);
    //print the world
    std::cout << "World: " << world << "\n";

    Framebuffer fbuff(3, false, true, true, uvec2(600), inst);
    std::cout << fbuff << "\n";
    Color clear = Color(0, 1, 1, 1, COLOR_SPACE_HSVA);

    ShaderProcessor processor;
    processor.loadPackage(Path("shader/test.gp", PATH_LAYOUT_DEBIAN), "test");
    std::cout << processor << "\n";

    Shader shader(Path("shader/test.gs", PATH_LAYOUT_DEBIAN), &processor, inst);

    Limiter lim = 60;
    lim.start();
    while (win.isOpen())
    {
        vec4 values = clear.getValues() + vec4(0.125/lim.getIPS(), 0, 0, 0);
        values.x = (values.x > 1.) ? 0 : values.x;
        clear.setValues(values);

        std::cout << "\rCurrent FPS: " << (uint32_t)win.getRenderLimiter().getIPS() << ", clear color: " << clear << "\x1b[0K" << std::flush;

        lim.endTick();
    }
    std::cout << "\n";
}
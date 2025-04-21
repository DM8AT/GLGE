#include "GLGE/GLGE.h"

class FileLogger : public Logger
{
public:
    /**
     * @brief Construct a new File Logger
     * 
     * @param path the path to the log file
     * @param debug say if debugging is enabled
     */
    FileLogger(std::string_view path, bool debug) : Logger(debug), m_file(path, true)
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
        API_OPENGL_3_3
    };
    APIs best = getBestGraphicAPI(apis.data(), apis.size());
    if (best == API_FALLBACK_ERROR) {return 1;}
    Instance inst("Main instance", best);

    WindowSettings settings = WINDOW_SETTINGS_DEFAULT;
    settings.borderless = true;
    settings.alwaysOnTop = true;
    Window win("Hello World!", 600, 0, settings, inst);
    Texture tex("assets/textures/cubeTexture.png", true, inst);
    Texture depth(TEXTURE_PURPOSE_DEPTH, win.getSize(), 0, 0, inst);

    Color clear = Color(0, 1, 1, 1, COLOR_SPACE_HSVA);

    Limiter lim = 60;
    lim.start();
    while (win.isOpen())
    {
        win.setClearColor(clear);
        vec4 values = clear.getValues() + vec4(0.125/lim.getIPS(), 0, 0, 0);
        values.x = (values.x > 1.) ? 0 : values.x;
        clear.setValues(values);

        std::cout << "\rCurrent FPS: " << (uint32_t)win.getRenderLimiter().getIPS() << ", clear color: " << clear << "\x1b[0K" << std::flush;
        
        lim.endTick();
    }
    std::cout << "\n";
}
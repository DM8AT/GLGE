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
    FileLogger(std::string_view path, bool debug) : Logger(debug), m_file(path, true) {m_file.content() = "";}

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
    //store all APIs that might work
    std::vector<APIs> apiList {
        API_VULKAN_1_2_INDIRECT,
        API_VULKAN_1_2_DEFAULT,
        API_OPENGL_4_6,
        API_OPENGL_3_3,
        API_SOFTWARE
    };
    //get the best API
    APIs best = getBestGraphicAPI(apiList.data(), apiList.size());
    //check if an API was found
    if (best == API_FALLBACK_ERROR)
    {
        //print an error
        std::cerr << "[ERROR] No good graphic API was found\n";
        //return an error
        return EXIT_FAILURE;
    }

    //print the selected API
    std::cout << "Selected Graphics API: " << best << "\n";
    //create an instance with the best API
    Instance instance("Main Instance", best);
    //create a logger
    instance.setLogger(new FileLogger("TEST_CPP.log", true));

    //create a new window
    Window window("Hello World!", uvec2(600), uvec2(0), WINDOW_SETTINGS_DEFAULT, instance);
    //create custom window settings
    WindowSettings settings = WINDOW_SETTINGS_DEFAULT;
    //make the window borderless
    settings.maximized = false;
    //create another window
    Window other("Second window", 300, 0, settings, instance);

    //create a framerate limiter
    Limiter lim = 60;

    while (Window::openWindowCount() > 0)
    {
        //start the tick
        lim.start();
        //end the tick
        lim.endTick();
    }
}
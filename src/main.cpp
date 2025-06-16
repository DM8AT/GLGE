#include "GLGE/GLGE.h"

/**
 * @brief overload the logger class so the output is written into a log file and not into the console
 */
class FileLogger : public Logger
{
public:

    /**
     * @brief Construct a new File Logger
     */
    FileLogger() = default;

    /**
     * @brief Construct a new File Logger
     * 
     * @param fileName the name of the file the data will be stored in
     * @param debug true : debug info will be printed | false : no debug info will be printed
     */
    FileLogger(std::string fileName, bool debug = false)
     : Logger(debug), m_file(Path(fileName), true) {m_file.content() = "";}

    /**
     * @brief Destroy the File Logger
     */
    ~FileLogger()
    {
        //print all the data into the file
        printAll();
        //save the file. The file will close once it runs out of scope
        m_file.save();
    }

    /**
     * @brief overload the print function to store to a file instead of printing to the console
     */
    virtual void print() override
    {
        //print the message into the file
        m_file.content() += format(m_messages[0]) + "\n";
    }

protected:

    /**
     * @brief store the file to store the data in
     */
    File m_file;

};

int main()
{
    //create a new instance using the OpenGL 4.6 API
    Instance inst = Instance("Base instance", API_OPENGL_4_6);
    //add a new instance of the file logger class to the instance
    //The log will be stored in TEST_CPP.log in the top level directory (the directory over the source directory)
    //debug printing is also enabled
    inst.setLogger(new FileLogger("TEST_CPP.log", true));
    
    //a new window with the name "Hello Triangle!" is constructued. 
    //the inital size of the window is 600 x 600 pixel and it is positioned at 0,0 (whereever that may be, usually the most top left pixel)
    //the window will also use only the default window settings (resizable, with decor, usw)
    //the window is also registerd to the instance created in the steps before
    Window win = Window("Hello Triangle!", 600, 0, WINDOW_SETTINGS_DEFAULT, inst);

    //create a new framebuffer. The Framebuffer will have 1 color attatchment in low dynamic range without an alpha channel
    //the framebuffer will also have no depth buffer. 
    //the inital size of the framebuffer is the inital size of the window
    //the framebuffer is also registerd to the instance
    Framebuffer fbuff = Framebuffer(1, false, false, false, win.getSize(), inst);
    //this stores the clear color of the framebuffer. Initaly, it is red
    Color clear = Color(0,1,0.5,1, COLOR_SPACE_HSLA);
    //set the clear color of the framebuffer to the color created in the line above. 
    //this UPDATES the clear color. When the clear variable is changed, the change won't 
    //take effect unless it is explicetly set again
    fbuff.setClearColor(clear);

    //store a list of all stages followed for rendering
    RenderStage stages[] = {
        //thie clear stage clears a specific framebuffer to the clear color the framebuffer is set to. 
        //in this case, no user data or user functions are passed
        RenderStage(RENDER_STAGE_CLEAR, RenderStage::Data(ClearStageData(&fbuff)), 0,0,0),
        //the blit to window stage copies the content from a framebuffer to a window. 
        //also here, there are no user define functionalities used
        RenderStage(RENDER_STAGE_BLIT_TO_WINDOW, RenderStage::Data(BlitToWindowStageData(&fbuff, &win)), 0,0,0),
        //the swap window stage is used to update the content of a window
        RenderStage(RENDER_STAGE_SWAP_WINDOW, RenderStage::Data(SwapWindowData(&win)), 0,0,0)
    };
    //create a new render pipeline using the stages specified above. 
    //the render pipeline will iterate over all stages in a different thread and record a command buffer
    //according to the stages. The stages are iterated over from index 0 to n, where n is the length of the stage
    //array minus 1. Here it is spezified that the render pipeline starts imediatly, but it may also be defferd to 
    //start at a user spezified time point. 
    //the render pipeline is also registerd to the instance. 
    RenderPipeline pipeline = RenderPipeline(stages, sizeof(stages)/sizeof(*stages), true, inst);

    ShaderProcessor shaderProc;
    shaderProc.loadPackage(Path("shader/test.gp"), "test");
    Shader shader(Path("shader/test file.gs"), &shaderProc, inst);

    SimpleVertex vertices[] = {
        {vec3(-0.5,0,0), vec2(0,0),   vec3(0,0,1)},
        {vec3(0,1,0),    vec2(0.5,1), vec3(0,0,1)},
        {vec3(0.5,0,0),  vec2(1,0),   vec3(0,0,1)}
    };
    idx_Triangle indices[] = {
        {0,1,2}
    };
    Mesh mesh = Mesh(vertices, sizeof(vertices)/sizeof(*vertices), indices, sizeof(indices) / sizeof(*indices));

    RenderMesh rMesh = RenderMesh(&mesh);

    //create a memory arena FOR TEST ONLY, NEVER USE THIS DIRECLTY! MEMORY LEAK WARNING!
    //(i know the memory is leaked here too, but for testing, i don't care since the program
    //terminates direclty afterwards. I can't insert the arena from here into the default
    //destruction chain)
    GraphicMemoryArena* arena = new OGL4_6_MemoryArena(64, true, MEMORY_USAGE_UNIFORM, inst);
    arena->onCreate();
    std::vector<GraphicMemoryArena::GraphicPointer> ptrs;
    for (uint64_t i = 0; i < 4; ++i)
    {
        ptrs.push_back(arena->allocate(8));
    }
    uint64_t dat = 0xff00ff00ff00ff00;
    uint64_t adat = ~dat;
    arena->update(ptrs[0], &dat);
    arena->update(ptrs[1], &adat);
    arena->update(ptrs[2], &dat);
    arena->update(ptrs[3], &adat);
    for (GraphicMemoryArena::GraphicPointer& ptr : ptrs)
    {
        arena->release(ptr);
    }

    //create a new limiter
    //a limiter is used to limit the iteration rate of a loop. They are mostly used to 
    //stop main loops like the render or update loop from using a CPU core fully if not needed
    //the amount per second a limiter ries to limit the loop to can be spezified by the user
    //below, the limiter will try to make the loop run 60 times per second. 
    Limiter lim = 60;
    //start the limiter. This initalizes the first starting time to stop it from being infinite
    lim.start();

    //main loop. 
    //this loop will run while the main window is open
    while (win.isOpen())
    {
        //rotate the clear color of the framebuffer a bit around the hue weel
        vec4 vals = clear.getValues();
        vals.x += 0.001;
        vals.x = fmodf(vals.x, 1.);
        clear.setValues(vals);
        //register the changes made to the clear color
        fbuff.setClearColor(clear);
        //print the current frames per second of the render pipeline to the console. Use flush to print
        //it imeadiatly
        std::cout << "\33[2K\rCurrent FPS: " << pipeline.getLimiter().getIPS() << std::flush;

        //end the tick of the limiter
        //at this time the limiter will evaluate the elapsed time and estimate how often the program 
        //will run per second and wait acordingly
        lim.endTick();
    }

    //print a new line character so the next input request from the terminal is in the next line
    //this has nothing to do with the application logic
    std::cout << "\n";
}
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

    /**
     * @brief overload the hook that says that all messages are printed
     */
    virtual void finishPrint() override
    {
        //when the printing finishes, save all changes
        m_file.save();
    }

protected:

    /**
     * @brief store the file to store the data in
     */
    File m_file;

};

//store a structure that mapps the data the compute shader can access
struct ComputeData
{
    //this stores the identifyer of the texture the compute shader will write to
    int32_t writeTo;
    //this stores the identifyer of the texture the compute shader will use for texturing
    int32_t texture;
};

/**
 * @brief this function creates a window to set the background color of the window
 * 
 * @param color a pointer to the color value used to set the framebuffer's clear color
 */
void imGuiBgColorPicker(void* color, uint64_t)
{
    //extract the color
    Color* col = (Color*)color;

    //start a ImGui window
    ImGui::Begin("Background");
    
    //store the color
    vec3 col_v = col->getColor(col->getInSpace(COLOR_SPACE_RGBA), COLOR_SPACE_RGBA);
    //draw a color picker
    ImGui::ColorPicker3("Color", (float*)&col_v);
    //update the color
    col->setTo(col_v.x, col_v.y, col_v.z, 1, COLOR_SPACE_RGBA);

    //end the window
    ImGui::End();
}

/**
 * @brief store some data that is used to resize the framebuffer
 */
struct FramebufferResizeData
{
    /**
     * @brief store a pointer to the render pipeline that resizes the framebuffer
     */
    RenderPipeline* pipeline;
    /**
     * @brief store the identifyer of the stage that changes the framebuffer's size
     */
    uint64_t stageID;
    /**
     * @brief store a pointer to the window the framebuffer should fit into
     */
    Window* window;
};

/**
 * @brief this function is used to calculate the new size for the main framebuffer to keep is as close as pissible to the actual window size
 * 
 * @param data a pointer to some data storing the data needed for the resizing of the framebuffer
 */
void calculateFramebufferSize(FramebufferResizeData* data)
{
    //set the new stage size
    data->pipeline->getStage(data->stageID).data.resizeFramebuffer.size = data->window->getSize();
}

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

    //wait for the setup to be done (setup may execute on another thread)
    //MAKE SURE TO CALL AFTER THE WINDOW IS CREATED
    //the graphic instance creation is started when the window is created
    inst.syncGraphicSetup();

    //create a new framebuffer. The Framebuffer will have 1 color attatchment in low dynamic range with an alpha channel
    //for some reason OpenGL images can only be created if the alpha channel is enabled
    //the framebuffer will also have no depth buffer. 
    //the inital size of the framebuffer is the inital size of the window
    //the framebuffer is also registerd to the instance
    Framebuffer fbuff = Framebuffer(1, true, false, false, win.getSize(), inst);
    //this stores the clear color of the framebuffer. Initaly, it is red
    Color clear = Color(0,1,0.5,1, COLOR_SPACE_HSLA);
    //set the clear color of the framebuffer to the color created in the line above. 
    //this UPDATES the clear color. When the clear variable is changed, the change won't 
    //take effect unless it is explicetly set again
    fbuff.setClearColor(clear);

    //a shader processor is a class to manage the packages a shader may include
    //it is usually good practice to only use what you need
    ShaderProcessor shaderProc;
    //load the package to handle textures with GLGE
    shaderProc.loadPackage(Path("shader/glgeTextures.gp"), "GLGETextures");

    //load the texture named "cube texture" (i know it is applied to a circle) from disk
    //the texture will be used to texture an image
    Texture tex = Texture("assets/textures/cubeTexture.png", false, TEXTURE_PURPOSE_IMAGE, inst);

    //store an instance of the compute data to ship to the GPU
    ComputeData computeData;
    //load the identifyers of the textures to the data to ship it to the compute shader
    computeData.writeTo = fbuff.getColorAttatchment(0)->getIdentifyer();
    computeData.texture = tex.getIdentifyer();
    //create the GPU buffer to ship to the compute shader
    StructuredBuffer<ComputeData> cmpData(&computeData, 1, MEMORY_USAGE_UNIFORM, inst);
    
    //load a compute shader and compile it with the packages added to the shader processor
    Shader compute(Path("shader/compute.gs"), &shaderProc, {
            //add the color attatchment of the framebuffer to the compute shader
            //the name has nothing to do with the access in the shader
            {"WriteTo", fbuff.getColorAttatchment(0)},
            //also add the loaded texture to the compute shader
            //this will be used to texture the circle
            {"Texture", &tex}
        }, {
            //give the shader access to all textures
            //this must be done manually since the wanted position of the buffer
            //responsible for passing texture data can't be guessed. 
            //here, it will be bound to unit 0
            {"GLGE_TextureBuffer", BufferShaderBinding(0, inst.getTextureBuffer())},
            //give the shader access to all images
            //this way that compute shader can write to the images, but can't write to textures
            //here, the texture buffer will bind to unit 1
            {"GLGE_ImageBuffer", BufferShaderBinding(1, inst.getImageBuffer())},
            //specify the buffer the shader can access
            //bind it to unit 0
            {"Textures", BufferShaderBinding(0, &cmpData)}
        }, inst);
    //store the amount of instances per execution for the compute shader
    uvec3 instance(32,32,1);

    //store the data that is used to change the framebuffer's size
    FramebufferResizeData fbuffResizeData;

    //store a list of all stages followed for rendering
    RenderStage stages[] = {
        //the ImGui render stage is only available if GLGE was compiled using ImGui. Else, it won't do anything
        RenderStage(RENDER_STAGE_DEAR_IMGUI_START_FRAME, RenderStage::Data(), 0,0,0),
        //this render stage is responsible for updating the framebuffer's size
        RenderStage(RENDER_STAGE_RESIZE_FRAMEBUFFER, RenderStage::Data(ResizeFramebufferData(&fbuff, win.getSize())), 
                    (void*)&fbuffResizeData, (void (*)(void*))calculateFramebufferSize, 0),
        //the clear stage fills a specific framebuffer with the clear color specified for it
        //the color can be changed on the framebuffer object to take effect
        RenderStage(RENDER_STAGE_CLEAR, RenderStage::Data(ClearStageData(&fbuff)), 0,0,0),
        //Add a render stage that invoces the compute shader
        RenderStage(RENDER_STAGE_COMPUTE, RenderStage::Data(ComputeStageData(compute, 
            //the amount of invocations is divided by 8 because each invocation has 8 threads per axis
            //but it is rounded up to still compute the whole image
            //this is used to not waste performance
            uvec3(ceil(fbuff.getColorAttatchment(0)->getSize().x / (float)instance.x), ceil(fbuff.getColorAttatchment(0)->getSize().y / (float)instance.y),
                1))), 0,0,0),

        //the blit to window stage copies the content from a framebuffer to a window. 
        //also here, there are no user define functionalities used
        RenderStage(RENDER_STAGE_BLIT_TO_WINDOW, RenderStage::Data(BlitToWindowStageData(&fbuff, &win)), 0,0,0),
        //the ImGui Function render stage executes a function that wrapps around a lot of ImGui functions
        //it will also do nothing when GLGE was compiled without ImGui
        RenderStage(RENDER_STAGE_DEAR_IMGUI_FUNCTION, RenderStage::Data(DearImGuiStageData(imGuiBgColorPicker, &clear, sizeof(clear))), 0,0,0),
        //the ImGui End function ends the frame for ImGui. After that all ImGui functions will raise errors
        //when GLGE was compiled without ImGui it won't do anything
        RenderStage(RENDER_STAGE_DEAR_IMGUI_END_FRAME, RenderStage::Data(), 0,0,0),
        //the swap window stage is used to update the content of a window
        RenderStage(RENDER_STAGE_SWAP_WINDOW, RenderStage::Data(SwapWindowData(&win)), 0,0,0)
    };
    //create a new render pipeline using the stages specified above. 
    //the render pipeline will iterate over all stages in a different thread and record a command buffer
    //according to the stages. The stages are iterated over from index 0 to n, where n is the length of the stage
    //array minus 1. Here it is spezified that the render pipeline does not start immediatly, instead
    //it should start execution at a user-specified time point
    //the render pipeline is also registerd to the instance. 
    RenderPipeline pipeline = RenderPipeline(stages, sizeof(stages)/sizeof(*stages), false, inst);

    //populate the data for the buffer resizing
    //because that relies on the render pipeline existing, it
    //can't be populated when it is created.
    fbuffResizeData.pipeline = &pipeline;
    fbuffResizeData.stageID = 1;
    fbuffResizeData.window = &win;

    //now, after all data is populated, the render pipeline can be started
    pipeline.start();

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
        //register the changes made to the clear color
        fbuff.setClearColor(clear);
        //change the amount of executions to still compute the image correctly 
        //even if the size changed
        pipeline.getStage(3).data.compute.executions = uvec3(
            ceil(win.getSize().x / (float)instance.x),
            ceil(win.getSize().y / (float)instance.y),
            1);

        //print all stages messages
        inst.getLogger()->printAll();

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
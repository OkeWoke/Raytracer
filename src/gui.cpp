#include "gui.h"
using namespace std;

static void glfw_error_callback(int error, const char* description)
{
    fprintf(stderr, "Glfw Error %d: %s\n", error, description);
}

void gui(ImageArray& img)
{
     glfwSetErrorCallback(glfw_error_callback);
    if (!glfwInit())
        return;


    const char* glsl_version = "#version 150";
    glfwWindowHint(GLFW_CONTEXT_VERSION_MAJOR, 3);
    glfwWindowHint(GLFW_CONTEXT_VERSION_MINOR, 2);
    glfwWindowHint(GLFW_OPENGL_PROFILE, GLFW_OPENGL_CORE_PROFILE);  // 3.2+ only
    glfwWindowHint(GLFW_OPENGL_FORWARD_COMPAT, GL_TRUE);            // Required on Mac

    // Create window with graphics context
    GLFWwindow* window = glfwCreateWindow(800, 800, "Path Tracer", NULL, NULL);
    if (window == NULL)
        return;
    glfwMakeContextCurrent(window);
    glfwSwapInterval(1); // Enable vsync

    glewExperimental = GL_TRUE;
    bool err = glewInit() != GLEW_OK;
    float vertices[] = {
    //  Position      Color             Texcoords
        -1.0f,  1.0f, 1.0f, 1.0f, 1.0f, 1.0f, 0.0f, // Top-left
         1.0f,  1.0f, 1.0f, 1.0f, 1.0f, 0.0f, 0.0f, // Top-right
         1.0f, -1.0f, 1.0f, 1.0f, 1.0f, 0.0f, 1.0f, // Bottom-right
        -1.0f, -1.0f, 1.0f, 1.0f, 1.0f, 1.0f, 1.0f  // Bottom-left
    };

    GLuint elements[] = {
    0, 1, 2,
    2, 3, 0
    };


    GLuint vao;
    glGenVertexArrays(1, &vao);
    glBindVertexArray(vao);

    GLuint tex;
    glGenTextures(1, &tex);
    glBindTexture(GL_TEXTURE_2D, tex);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_REPEAT);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_REPEAT);

    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_NEAREST);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_NEAREST);

    glTexImage2D(GL_TEXTURE_2D, 0, GL_RGB, 800, 800, 0, GL_RGB, GL_FLOAT, img.float_array); //2nd param is level of detail, 0 is base img., 3rd is format, 4th and 5th is width/height of the image. 6th should always be a 0.
    //the following parameters describe the format to be read in, i.e. r g b, and type.

    GLuint vbo;
    glGenBuffers(1, &vbo); // Generate 1 buffer

    glBindBuffer(GL_ARRAY_BUFFER, vbo); //select the buffer for data to be uploaded to

    glBufferData(GL_ARRAY_BUFFER, sizeof(vertices), vertices, GL_STATIC_DRAW); // upload vertex data to buffer


    GLuint ebo;
    glGenBuffers(1, &ebo);
    glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, ebo);
    glBufferData(GL_ELEMENT_ARRAY_BUFFER, sizeof(elements), elements, GL_STATIC_DRAW);
    ///Loading shaders
    /// Can compilse shaders from file or thorugh raw string literals

    const char* vertexSource = R"glsl(
        #version 150 core
        in vec2 texcoord;
        in vec3 color;
        in vec2 position;

        out vec3 Color;
        out vec2 Texcoord;


        void main()
        {
            Texcoord = texcoord;
            Color = color;
            gl_Position = vec4(position, 0.0, 1.0);
        }
    )glsl";

    const char* fragmentSource = R"glsl(
        /// fragment shader
        ///output from vertex shader is interpolated over all pixels covered, fragement shader operators on this.
        //this outputs the colour

        #version 150 core
        in vec3 Color;
        in vec2 Texcoord;
        out vec4 outColor;
        uniform sampler2D tex;
        void main()
        {
            outColor = texture(tex, Texcoord) * vec4(Color, 1.0);
        }

    )glsl";

    GLuint vertexShader = glCreateShader(GL_VERTEX_SHADER); // create shader object
    glShaderSource(vertexShader, 1, &vertexSource, NULL); //load glsl shader code into it
    glCompileShader(vertexShader);

    //Checking if shader compiles correctly
    GLint status;
    glGetShaderiv(vertexShader, GL_COMPILE_STATUS, &status);
    if (status == GL_TRUE){
        cout <<"success" << endl;
    }

    //fragment shader compilation
    GLuint fragmentShader = glCreateShader(GL_FRAGMENT_SHADER);
    glShaderSource(fragmentShader, 1, &fragmentSource, NULL);
    glCompileShader(fragmentShader);


    glGetShaderiv(fragmentShader, GL_COMPILE_STATUS, &status);
    if (status == GL_TRUE){
        cout <<"success" << endl;
    }
    //now to connect the two shaders by making a shader program.

    GLuint shaderProgram = glCreateProgram();
    glAttachShader(shaderProgram, vertexShader);
    glAttachShader(shaderProgram, fragmentShader);

    //the fragment shader can write to multiple framebuffers and typically you need to specify this, but in this case the default is 0.
    //glBindFragDataLocation(shaderProgram, 0, "outColor");


    //linking the program, can make changes to the shaderfs after being added to a program, but result does not change until program has been linked.
    glLinkProgram(shaderProgram);
    glUseProgram(shaderProgram);

    //color attribute
    GLint colAttrib = glGetAttribLocation(shaderProgram, "color");
    glEnableVertexAttribArray(colAttrib);
    glVertexAttribPointer(colAttrib, 3, GL_FLOAT, GL_FALSE,
                           7*sizeof(float), (void*)(2*sizeof(float)));

    //retrieve ref to posisition input of vertex shader
    GLint posAttrib = glGetAttribLocation(shaderProgram, "position");
    //the input, no. vals (dimensionality of vec), component type, normalise -1/1 or 0/1, stride (no. bytes between posisitin), offset (how many bytes from the start, does the attribute occur)
    glVertexAttribPointer(posAttrib, 2, GL_FLOAT, GL_FALSE,
                           7*sizeof(float), 0);
    glEnableVertexAttribArray(posAttrib);



    GLint texAttrib = glGetAttribLocation(shaderProgram, "texcoord");
    glEnableVertexAttribArray(texAttrib);
    glVertexAttribPointer(texAttrib, 2, GL_FLOAT, GL_FALSE,
                           7*sizeof(float), (void*)(5*sizeof(float)));

    //IMGUI stuff
    //GLFWwindow* window2 = glfwCreateWindow(1280, 720, "Dear ImGui GLFW+OpenGL3 example", NULL, NULL);
    //glfwMakeContextCurrent(window2);
    IMGUI_CHECKVERSION();
    ImGui::CreateContext();
    ImGuiIO& io = ImGui::GetIO(); (void)io;
    ImGui::StyleColorsDark();
    ImGui_ImplGlfw_InitForOpenGL(window, true);
    ImGui_ImplOpenGL3_Init(glsl_version);



    bool show_demo_window = true;
    bool show_another_window = false;
    ImVec4 clear_color = ImVec4(0.45f, 0.55f, 0.60f, 1.00f);
    // Main loop
    while (!glfwWindowShouldClose(window))
    {

        //glfwMakeContextCurrent(window);
        //glTexImage2D(GL_TEXTURE_2D, 0, GL_RGB, 800, 800, 0, GL_RGB, GL_FLOAT, img.flat_array);

        glfwPollEvents();
        glDrawElements(GL_TRIANGLES, 6, GL_UNSIGNED_INT, 0);
        glTexSubImage2D(GL_TEXTURE_2D, 0, 0, 0,800,800, GL_RGB, GL_FLOAT, img.float_array);

        ImGui_ImplOpenGL3_NewFrame();
        ImGui_ImplGlfw_NewFrame();
        ImGui::NewFrame();
         if (show_demo_window)
            ImGui::ShowDemoWindow(&show_demo_window);

        // 2. Show a simple window that we create ourselves. We use a Begin/End pair to created a named window.
        {
            static float f = 0.0f;
            static int counter = 0;

            ImGui::Begin("Hello, world!");                          // Create a window called "Hello, world!" and append into it.

            ImGui::Text("This is some useful text.");               // Display some text (you can use a format strings too)
            ImGui::Checkbox("Demo Window", &show_demo_window);      // Edit bools storing our window open/close state
            ImGui::Checkbox("Another Window", &show_another_window);

            ImGui::SliderFloat("float", &f, 0.0f, 1.0f);            // Edit 1 float using a slider from 0.0f to 1.0f
            ImGui::ColorEdit3("clear color", (float*)&clear_color); // Edit 3 floats representing a color

            if (ImGui::Button("Button"))                            // Buttons return true when clicked (most widgets return true when edited/activated)
                counter++;
            ImGui::SameLine();
            ImGui::Text("counter = %d", counter);

            ImGui::Text("Application average %.3f ms/frame (%.1f FPS)", 1000.0f / ImGui::GetIO().Framerate, ImGui::GetIO().Framerate);
            ImGui::End();
        }

        // 3. Show another simple window.
        if (show_another_window)
        {
            ImGui::Begin("Another Window", &show_another_window);   // Pass a pointer to our bool variable (the window will have a closing button that will clear the bool when clicked)
            ImGui::Text("Hello from another window!");
            if (ImGui::Button("Close Me"))
                show_another_window = false;
            ImGui::End();
        }

        ImGui::Render();
        int display_w, display_h;
        glfwGetFramebufferSize(window, &display_w, &display_h);
        glViewport(0, 0, display_w, display_h);
        //glClearColor(clear_color.x * clear_color.w, clear_color.y * clear_color.w, clear_color.z * clear_color.w, clear_color.w);
        //glClear(GL_COLOR_BUFFER_BIT);
        ImGui_ImplOpenGL3_RenderDrawData(ImGui::GetDrawData());

        glfwSwapBuffers(window);

    }

    // Cleanup
    ImGui_ImplOpenGL3_Shutdown();

    ImGui_ImplGlfw_Shutdown();
    ImGui::DestroyContext();

    glfwDestroyWindow(window);
    glfwTerminate();
}

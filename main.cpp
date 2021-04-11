#include <iostream>
#include <sstream>
#include <iomanip>
#include <ctime>
#include <math.h>
#include <string>
#include <png.hpp>
#include <conio.h>
#include <chrono>
#include <iomanip>
#include <ctime>
#include <thread>
#include <functional>
#include <fstream>
#include <future>
#include <stdlib.h>
#include <time.h>

#include "Camera.h"
#include "Light.h"
#include "Utility.h"
#include "Vector.h"
#include "Matrix.h"
#include "imageArray.h"
#include "BoundVolume.h"
#include "BoundVolumeHierarchy.h"
#include "HaltonSampler.h"
#include "RandomSampler.h"
#include "GObjects/Sphere.h"
#include "GObjects/GObject.h"
#include "GObjects/Plane.h"
#include "GObjects/Triangle.h"
#include "GObjects/Mesh.h"

#include "ext/Markup.h"
#include "ext/CImg.h"

//sorry linux, i need these to figure out when the scene.xml file was last written to.

#include <tchar.h>
//#include <strsafe.h>

#include <windows.h>

#include "imgui/imgui.h"
#include "imgui/imgui_impl_glfw.h"
#include "imgui/imgui_impl_opengl3.h"

#include <GL/glew.h>
#include <GLFW/glfw3.h>


#ifndef M_PI
    #define M_PI 3.14159265358979323846
#endif

using namespace std;
using namespace cimg_library;

struct Hit
{
    Vector src;
    Vector ray_dir;
    Vector n;
    double t;
    GObject* obj;
    Color color;


    ~Hit()
    {
        obj = nullptr;
    }
};

struct Config
{
    int threads_to_use;
    int max_reflections;
    int spp;
    string stretch;
};

// function prototypes
void draw(ImageArray& img, string filename);
Hit intersect(const Vector& src, const Vector& ray_dir, BoundVolumeHierarchy* bvh);
Color shade(const Hit& hit, int reflection_count, Sampler* ha1, Sampler* ha2, BoundVolumeHierarchy* bvh, const Config& config, const vector<GObject*>& objects, const vector<Light>& lights);
void cast_rays(const Camera& cam, const ImageArray& img, int row_start, int row_end);
void deserialize(string filename, vector<Light>& lights, vector<GObject*>& gLights, vector<GObject*>& objects, Camera& cam, Config& config);
void cast_rays_multithread(const Config& config, const Camera& cam, const ImageArray& img, Sampler* sampler1, Sampler* sampler2, BoundVolumeHierarchy* bvh, const vector<GObject*>& objects, const vector<Light>& lights, const vector<GObject*>& gLights);
double double_rand(const double & min, const double & max);
Color trace_rays_iterative(const Vector& origin, const Vector& ray_dir, BoundVolumeHierarchy* bvh, const Config& config, int depth, Sampler* ha1, Sampler* ha2, const vector<GObject*>& objects, const vector<GObject*>& gLights);
Vector uniform_hemisphere(double u1, double u2, Vector& n);
Vector cosine_weighted_hemisphere(double u1, double u2, Vector& n);
void create_orthonormal_basis(const Vector& v1, Vector& v2, Vector& v3);
Vector uniform_sphere(double u1, double u2);
Mesh* obj_reader(string filename);
void clear_globals();
bool is_light(GObject* obj);
void gui(ImageArray& img);
// global var declaration
uint64_t numPrimaryRays = 0;
uint64_t numRayTrianglesTests = 0;
uint64_t numRayTrianglesIsect = 0;

Vector snells_law(const Vector& incident_ray, const Vector& normal, double cos_angle, double n_1, double n_2)
//assume the two rays are normalised, thus dot product returns the cosine of them.
//takes cos_angle to avoid doing a redundant dot product.
{
    Vector ray_hor = incident_ray - cos_angle*normal;
    double sin_theta_2 = ray_hor.abs()*n_1/n_2;
    Vector refr_ray = normalise(-1*normal + normalise(ray_hor)*sin_theta_2);
    return refr_ray;
}

double schlick_fresnel(double cos_angle, double n_1, double n_2)
//returns probabibility of reflection based on angle relative to normal. (0 to 90 degrees)
{
    double R_0 = (n_1-n_2)/(n_1+ n_2);
    R_0 = R_0 * R_0;

    double R_theta = R_0 + (1 - R_0)*pow((1-cos_angle),5);
    return R_theta;
}

Vector uniform_sphere(double u1, double u2)
{
    const double r = sqrt(1.0 - u1*u1);
	const double phi = 2 * PI * u2;
	Vector ray = Vector(cos(phi)*r, sin(phi)*r, u1);

	return ray;
}
Vector uniform_hemisphere(double u1, double u2, Vector& n) {
	const double r = sqrt(1.0 - u1*u1);
	const double phi = 2 * PI * u2;
	Vector ray = Vector(cos(phi)*r, sin(phi)*r, u1);
	if (ray.dot(n)<0)
    {
        return -1*ray;
    }
	return ray;
}

Vector cosine_weighted_hemisphere(double u1, double u2, const Vector& n)
// taken from http://www.rorydriscoll.com/2009/01/07/better-sampling/
//modified to ensure on the correct hemisphere
{
    const double r = sqrt(u1);
    const double theta = 2 * PI * u2;

    const double x = r * cos(theta);
    const double y = r * sin(theta);
    const double z = 1 - x*x - y*y;

    Vector ray = Vector(x,y,z);
    if(ray.dot(n)<0)
    {
        return -1*ray;
    }
    return ray;
}

//below function is taken from smallpaint
// given v1, set v2 and v3 so they form an orthonormal system
// (we assume v1 is already normalized)
void create_orthonormal_basis(const Vector& v1, Vector& v2, Vector& v3) {
	if (std::abs(v1.x) > std::abs(v1.y)) {
		// project to the y = 0 plane and construct a normalized orthogonal vector in this plane
		double invLen = 1.f / sqrtf(v1.x * v1.x + v1.z * v1.z);
		v2 = Vector(-v1.z * invLen, 0.0f, v1.x * invLen);
	} else {
		// project to the x = 0 plane and construct a normalized orthogonal vector in this plane
		double invLen = 1.0f / sqrtf(v1.y * v1.y + v1.z * v1.z);
		v2 = Vector(0.0f, v1.z * invLen, -v1.y * invLen);
	}
	v3 = v1 % v2;
}

bool is_light(GObject* a)
{
    if(a->emission.r >0 || a->emission.g > 0 || a->emission.b > 0)
    {
        return true;
    }
    return false;
}

void clear_globals()
{

    numPrimaryRays = 0;
    numRayTrianglesTests = 0;
    numRayTrianglesIsect = 0;
}

string get_write_time(string filename)
//returns a string of the last write time of a file
{
    char filename_char[filename.length()+1];
    strcpy(filename_char, filename.c_str());

    HANDLE hFile;
    FILETIME ftCreate, ftAccess, ftWrite;
    SYSTEMTIME stUTC, stLocal;
    hFile = CreateFile(filename_char, GENERIC_READ, FILE_SHARE_READ, NULL, OPEN_EXISTING, 0, NULL);
    if(GetFileTime(hFile, &ftCreate, &ftAccess, &ftWrite)){}
    FileTimeToSystemTime(&ftWrite, &stUTC);
    SystemTimeToTzSpecificLocalTime(NULL, &stUTC, &stLocal);
    CloseHandle(hFile);
    ostringstream write_time_string;
    write_time_string << stLocal.wHour << stLocal.wMinute << stLocal.wSecond;
    return write_time_string.str();
}


static void glfw_error_callback(int error, const char* description)
{
    fprintf(stderr, "Glfw Error %d: %s\n", error, description);
}


static void ShowPlaceholderObject(const char* prefix, int uid)
{
    // Use object uid as identifier. Most commonly you could also use the object pointer as a base ID.
    ImGui::PushID(uid);

    // Text and Tree nodes are less high than framed widgets, using AlignTextToFramePadding() we add vertical spacing to make the tree lines equal high.
    ImGui::TableNextRow();
    ImGui::TableSetColumnIndex(0);
    ImGui::AlignTextToFramePadding();
    bool node_open = ImGui::TreeNode("Object", "%s_%u", prefix, uid);
    ImGui::TableSetColumnIndex(1);
    ImGui::Text("my sailor is rich");

    if (node_open)
    {
        static float placeholder_members[8] = { 0.0f, 0.0f, 1.0f, 3.1416f, 100.0f, 999.0f };
        for (int i = 0; i < 8; i++)
        {
            ImGui::PushID(i); // Use field index as identifier.
            if (i < 2)
            {
                ShowPlaceholderObject("Child", 424242);
            }
            else
            {
                // Here we use a TreeNode to highlight on hover (we could use e.g. Selectable as well)
                ImGui::TableNextRow();
                ImGui::TableSetColumnIndex(0);
                ImGui::AlignTextToFramePadding();
                ImGuiTreeNodeFlags flags = ImGuiTreeNodeFlags_Leaf | ImGuiTreeNodeFlags_NoTreePushOnOpen | ImGuiTreeNodeFlags_Bullet;
                ImGui::TreeNodeEx("Field", flags, "Field_%d", i);

                ImGui::TableSetColumnIndex(1);
                ImGui::SetNextItemWidth(-FLT_MIN);
                if (i >= 5)
                    ImGui::InputFloat("##value", &placeholder_members[i], 1.0f);
                else
                    ImGui::DragFloat("##value", &placeholder_members[i], 0.01f);
                ImGui::NextColumn();
            }
            ImGui::PopID();
        }
        ImGui::TreePop();
    }
    ImGui::PopID();
}

int main()
{

    int window_width = 800;
    int window_height = 800;




    vector<GObject*> objects;
    vector<Light> lights;
    vector<GObject*> gLights; //GObjects that have emission.

    Camera cam;
    Config config;
    int orw = 20;

    //initial call to deserialize just so I can define ImageArray...
    cout<<"Loading scene from scene.xml..." << endl;
    auto load_start = chrono::steady_clock::now();
    deserialize("scene.xml", lights, gLights, objects, cam, config);
    string last_write_time = get_write_time("scene.xml");
    auto load_end = chrono::steady_clock::now();
    cout<<"Loading completed in: " << (load_end-load_start)/chrono::milliseconds(1)<< " (ms)" << endl;

    //Creation of CImg display buffer and window.
    ImageArray img(cam.H_RES, cam.V_RES);
    //CImg<float> display_image(cam.H_RES, cam.V_RES,1,3,0);
    //CImgDisplay display(display_image, "Oke's Path Tracer!");
    //thread gui_thread(gui, img);

    auto gui_future = async(launch::async, [=, &img]
    {
        gui(img);
    });
    //creating filename....
    auto t = std::time(nullptr);
    auto tm = *std::localtime(&t);
    ostringstream filename;
    filename << "render-"  << std::put_time(&tm, "%d-%m-%Y %H-%M-%S");

    bool looping = true;

    while(looping)
    {

        //Creation of BoundVolume Hierarchy
        auto bvh_start = chrono::steady_clock::now();
        BoundVolume* scene_bv = BoundVolume::compute_bound_volume(objects);
        Vector center = Vector(0,0,0);
        for(unsigned int k = 0; k < objects.size(); k++)
        {
            center  = center + objects[k]->position;
        }

        center = center / objects.size();
        BoundVolumeHierarchy* bvh = new BoundVolumeHierarchy(scene_bv, center);

        for (auto obj: objects)
        {
            bvh->insert_object(obj,0);
        }
        auto top_node_bv = bvh->build_BVH();  // the obj this pointer points to self deletes.
        auto bvh_end = chrono::steady_clock::now();
        cout<<"BVH Constructed in: " << (bvh_end-bvh_start)/chrono::milliseconds(1)<< " (ms)" << endl;


        //Creation of samplers used for montecarlo integration.
        Sampler* sampler1 = new RandomSampler();//HaltonSampler(7, rand()%5000 + 1503);//
        Sampler* sampler2 = new RandomSampler();//HaltonSampler(3, rand()%5000 + 5000); //


    /////////////////////////////////////// CAST & DISPLAY  CODE /////////////////////////////
        auto cast_start = chrono::steady_clock::now();
        int s;
        for(s=0;s<config.spp; s++)
        {
            cast_rays_multithread(config, cam, img, sampler1, sampler2, bvh, objects, lights, gLights);


            img.floatArrayUpdate();
            auto gui_status = gui_future.wait_for(chrono::milliseconds(0));


            if (gui_status == future_status::ready)
            {

                looping = false;
                break;
            }
        }

        delete sampler1;
        delete sampler2;
        sampler1 = nullptr;
        sampler2 = nullptr;

        lights.clear();
        for (auto p : objects)
        {
            delete p;
            p = nullptr;
        }
        delete bvh;
        bvh = nullptr;
        objects.clear();
        auto cast_end = chrono::steady_clock::now();

        cout << "Casting completed in: "<< setw(orw) << (cast_end - cast_start)/chrono::milliseconds(1)<< " (ms)"<<endl;
        cout << "Number of primary rays: " << setw(orw+1) << numPrimaryRays << endl;
        cout << "Number of Triangle Tests: " << setw(orw) << numRayTrianglesTests << endl;
        cout << "Number of Triangle Intersections: " <<setw(orw-11) << numRayTrianglesIsect << endl;
        cout << "Percentage of sucesful triangle tests: " << setw(orw-12) << 100*(float) numRayTrianglesIsect/numRayTrianglesTests<< "%" << endl;
        cout << "----------------------------------------------------------\n\n\n\n"<<endl;
        cout<< "Waiting for modification of scene.xml or close window to save" << endl;
        while(last_write_time == get_write_time("scene.xml"))
        {
            auto gui_status = gui_future.wait_for(chrono::milliseconds(0));
            if (gui_status == future_status::ready)
            {
                cout << "closed window" << endl;
                looping = false;
                break;
            }
        }
        last_write_time = get_write_time("scene.xml");
        if(looping)
        {
            img.clearArray();
            cout<<"Loading scene from scene.xml..." << endl;
            auto load_start = chrono::steady_clock::now();
            deserialize("scene.xml", lights, gLights, objects, cam, config);

            auto load_end = chrono::steady_clock::now();
            cout<<"Loading completed in: " << (load_end-load_start)/chrono::milliseconds(1)<< " (ms)" << endl;
        }else
        {
            //Sample scaling, do not touch this as this ensures each image has same relative brightness regardless of no. samples.
            for (int i = 0; i < img.PIXEL_COUNT; ++i)
            {
                img.pixelMatrix[i] = img.pixelMatrix[i]/s;
            }

            filename << "_spp-" << s <<"_cast-"<<(cast_end-cast_start)/chrono::seconds(1)<<".png";
        }
    }

    ///// Draw/Save code
    auto save_start = chrono::steady_clock::now();
    if(config.stretch == "norm")
    {
        img.normalise(img.MAX_VAL);
    }else if(config.stretch == "gamma")
    {
        img.gammaCorrection(1.0/2.2);
        img.normalise(img.MAX_VAL);
    }else if(config.stretch == "rein")
    {
        img.normalise(1.0);
        img.reinhardToneMap();
        img.normalise(img.MAX_VAL);
    }
    draw(img, filename.str());
    img.clearArray();
    auto save_end = chrono::steady_clock::now();
    cout << "Image Save completed in: "<< setw(orw-7) <<(save_end - save_start)/chrono::milliseconds(1)<< " (ms)"<<endl;
    cout << "----------------------------------------\n\n\n\n" << endl;


    //getch();
    return 0;

    /* Animation codes
    ostringstream filename;
    filename << "render" << setfill('0') <<setw(3)<< i << std::put_time(&tm, "%d-%m-%Y %H-%M-%S") <<".png";
      //system("D:\Programming\Raytracer\ffmpeg -f image2 -framerate 24 -i D:\Programming\Raytracer\renders\test%03d.png -pix_fmt yuv420p -b:v 0 -crf 30 -s 1000x1000 render2.webm");
    */
}

static void HelpMarker(const char* desc)
{
    ImGui::TextDisabled("(?)");
    if (ImGui::IsItemHovered())
    {
        ImGui::BeginTooltip();
        ImGui::PushTextWrapPos(ImGui::GetFontSize() * 35.0f);
        ImGui::TextUnformatted(desc);
        ImGui::PopTextWrapPos();
        ImGui::EndTooltip();
    }
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
        glDrawElements(GL_TRIANGLES, 6, GL_UNSIGNED_INT, 0);
        glTexSubImage2D(GL_TEXTURE_2D, 0, 0, 0,800,800, GL_RGB, GL_FLOAT, img.float_array);
        glfwPollEvents();
        //glfwMakeContextCurrent(window2);
        static float f = 0.0f;
        static int counter = 0;

        ImGui_ImplOpenGL3_NewFrame();
        ImGui_ImplGlfw_NewFrame();
        ImGui::NewFrame();
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

        ImGui::Render();
        glfwSwapBuffers(window);

    }

    // Cleanup
    ImGui_ImplOpenGL3_Shutdown();

    ImGui_ImplGlfw_Shutdown();
    ImGui::DestroyContext();

    glfwDestroyWindow(window);
    glfwTerminate();
}

void cast_rays_multithread(const Config& config, const Camera& cam, const ImageArray& img, Sampler* sampler1, Sampler* sampler2, BoundVolumeHierarchy* bvh, const vector<GObject*>& objects, const vector<Light>& lights, const vector<GObject*>& gLights)
{
    int total_pixels = cam.V_RES*cam.H_RES;
    int cores_to_use = config.threads_to_use;//global
    volatile atomic<size_t> pixel_count(0);
    vector<future<void>> future_vector;

    for (int i = 0; i<cores_to_use; i++ )
    {
        future_vector.emplace_back(
            async(launch::async, [=,&cam, &img, &pixel_count]()
            {
                while(true)
                {
                    int x_index = pixel_count++;

                    if (x_index >=total_pixels)
                    {
                        break;
                    }

                    int y_index = x_index / cam.H_RES;
                    x_index = x_index%cam.H_RES;

                    //Box Muller Pixel Sampling
                    double x_offset;//= sampler1->next() - 0.5;
                    double y_offset;// = sampler2->next() - 0.5;

                    double u1 = sampler1->next()*0.9 + 0.1; //We scale the box muller distribution radial input to fit within a pixel
                    double u2 = sampler2->next();//*0.9 + 0.1;
                    double R  = sqrt(-2.0 * log(u1));
                    double angle = 2.0 * M_PI * u2;
                    x_offset = R * cos(angle);
                    y_offset = R * sin(angle);

                    Vector ray_dir = -cam.N*cam.n + cam.H*(((double)2*(x_index+x_offset)/(cam.H_RES-1)) -1)*cam.u + cam.V*(((double)2*(y_index+y_offset)/(cam.V_RES-1)) -1)*cam.v;
                    Vector ray_norm = normalise(ray_dir);
                    ray_dir = ray_norm*cam.focus_dist/(-1*ray_norm.dot(cam.n)); //this division ensures we get a planar focal plane, as opposed to spherical.
                    double aperture_radius = cam.aperture* sqrt(sampler1->next());
                    double aperture_angle = 2* M_PI * sampler2->next();
                    Vector aperture_u_offset = aperture_radius * cos(aperture_angle) * cam.u;
                    Vector aperture_v_offset = aperture_radius * sin(aperture_angle) * cam.v;
                    ray_dir = ray_dir -(aperture_u_offset + aperture_v_offset);

                    Color c = trace_rays_iterative(cam.pos+aperture_u_offset+aperture_v_offset, ray_dir, bvh, config, 0, sampler1, sampler2, objects, gLights);//shade(hit, 0, sampler1, sampler2, bvh, config, objects, lights);
                    //cout << img.pixelMatrix[0].r << endl;
                    img.pixelMatrix[img.index(x_index, y_index)] = (img.pixelMatrix[img.index(x_index, y_index)]) + c;
                }
            }));
    }
}

Hit intersect(const Vector& src, const Vector& ray_dir, BoundVolumeHierarchy* bvh)
//Takes a source point and ray direction, checks if it intersects any object
//returns hit struct which contains 'meta data' about the interection.
{
    Hit hit;
    hit.src = src;
    hit.ray_dir= normalise(ray_dir);
    hit.t=-1;
    hit.obj = nullptr;

    GObject::intersection inter = bvh->intersect(src+0.0001*hit.ray_dir, hit.ray_dir, 0);
    if(inter.t > 0.0001 && (hit.obj == nullptr || (inter.t) < hit.t))//if hit is viisible and new hit is closer than previous
    {
        //yes the below is pretty shit, why do two so simillar structs exist....
        hit.t = inter.t;// ray_dir.abs();
        hit.obj = inter.obj_ref;

        if (inter.color.r == -1)
        {
            hit.color = inter.obj_ref->color;
        }else
        {
            hit.color = inter.color;
        }
        hit.n = inter.n;
    }

    return hit;
}


Color trace_rays_iterative(const Vector& origin, const Vector& ray_dir, BoundVolumeHierarchy* bvh, const Config& config, int depth, Sampler* ha1, Sampler* ha2, const vector<GObject*>& objects, const vector<GObject*>& gLights)
{
    Vector o = origin; //copy
    Vector d = ray_dir; //copy
    Color c;
    Color weight = Color(1,1,1);
    bool ignore_direct = false;
    depth = 0;

    while (true)
    {
        if (depth> config.max_reflections)
        {
            break;
        }

        Hit hit = intersect(o, d, bvh); //see if current ray intersects something or not.

        if(hit.obj == nullptr || hit.t == -1)
        //nothing was hit;
        {
            break;
        }
        Vector hit_point = hit.src + hit.t*hit.ray_dir;
        double n_dot_ray = hit.n.dot(hit.ray_dir);
        if(is_light(hit.obj) && n_dot_ray < 0)
        {
            double divisor = 1;//max(1.0,hit.t*hit.t);

            if(!ignore_direct)
            {
                c = c + -1*weight*n_dot_ray*hit.obj->emission/(divisor *255.0);
            }else
            {
                c = c + -1*weight*n_dot_ray*hit.obj->emission/(divisor *255.0);
            }
            break;
        }

        //NEE
        /*
        if(hit.obj->brdf != 2)
        //not specular
        {
            auto light_i = int((gLights.size()-1) * ha2->next());
            auto light = gLights[light_i];
            auto prob_choosing_light = 1/gLights.size(); //uniform because idek how to IS this...
            Vector light_point = light->get_random_point(ha1->next(), ha2->next());
            Vector NEE_Ray = light_point - hit_point;
            Hit light_hit = intersect(hit_point, NEE_Ray, bvh);
            double NEE_Ray_length = NEE_Ray.abs();
            Vector out = normalise(NEE_Ray);
            double light_n_dot_NEE = light_hit.n.dot(out);
            if (NEE_Ray_length -0.0001 < light_hit.t < NEE_Ray_length +0.0001 && light_n_dot_NEE < 0)
            {
                Vector in = -1*d;

                auto length = NEE_Ray_length;
                Color brdf_coef = hit.color/255; //mat.eval(in, out)
                //c = c+ light->emission/255 * weight * brdf_coef  * hit.n.dot(out) * -1*light_n_dot_NEE  / ( length * length) / (prob_choosing_light);
            }
        }*/

        //auto survive_prob = 0.90;
       // if (ha1->next() > survive_prob) break;
        o = hit_point;
        if(hit.obj->brdf==2)
        {
            ignore_direct = false;
        }else
        {
            ignore_direct = true;
        }
        //ignore_direct = !mat.is_specular;
        Vector new_ray_dir;
        if(hit.obj->brdf==0)
        //diffuse
        {
            new_ray_dir = uniform_hemisphere(ha1->next(), ha2->next(), hit.n);
        }else if(hit.obj->brdf==2)
        //mirror
        {
           new_ray_dir = normalise(hit.ray_dir - hit.n * 2  *n_dot_ray);
        }else if(hit.obj->brdf==3)
        //constant density volume
        {
            double density =2;
            double scatter_prob = ha1->next() + 0.2;
            Hit volume_hit = intersect(hit_point+(d*0.0001), d, bvh); //we assume d is normalised
            if(hit.obj == volume_hit.obj && volume_hit.n.dot(d) >0 ) // we are inside the obj, not a tangent.
            {
                o = hit_point+(scatter_prob*density*d);
                if(volume_hit.t > scatter_prob*density) // We should scatter it.
                {

                    new_ray_dir = uniform_sphere(ha1->next(),ha2->next());
                }
                else
                {
                    new_ray_dir = d;
                }
            }
        }
        d= new_ray_dir;
        //o = hit_point;
        weight = weight* hit.color*hit.n.dot(new_ray_dir)/(255.0);
        if(weight.r <0)
        {
            weight= weight*-1; // to account for constant density volume doing back reflection
        }
        depth+=1;
    }


    return c;

    //old trace code

    /*
                Vector sample_dir = uniform_hemisphere(ha1->next(), ha2->next(), hit.n); ////can replace with halton series etc in the future.
                out_rays.push_back(normalise(sample_dir));

                //compute ray direct to light ray and in future to known points of light reflection (BDPT)
               /* for(unsigned int i = 0; i < objects.size(); i++)
                //in future may keep a separate vector for emissive objects (in cases of many objects)
                {
                    if(objects[i]->emission.r > 0 || objects[i]->emission.g >0 || objects[i]->emission.b > 0)
                    //if object is emissive
                    {
                        Vector light_point = objects[i]->get_random_point(ha1->next(), ha2->next());
                        Vector s =  normalise(light_point- hit_point);

                        //The below is only valid for opaque objects, ensure we have a valid reflection.
                        if(s.dot(hit.n) >0)
                        {
                            out_rays.push_back(s);
                        }
                    }
                }
        //mirror

                Vector next_ray = normalise(hit.ray_dir - hit.n * 2  *n_dot_ray); // n_dot_ray being the cos angle
                out_rays.push_back(next_ray);
                break;*/
}

Color shade(const Hit& hit, int reflection_count, Sampler* ha1, Sampler* ha2, BoundVolumeHierarchy* bvh, const Config& config, const vector<GObject*>& objects, const vector<Light>& lights)
{
    Color c = Color(0, 0, 0);

    if(hit.obj == nullptr || hit.t == -1)
    {
        return c;
    }

    if(hit.obj != nullptr && hit.obj->emission.r>0 && hit.n.dot(hit.ray_dir)< 0) //bad check to see if emissive. && hit.obj->emission.r >0 && hit.n.dot(hit.ray_dir)<0
    {
        return hit.obj->emission/255;
        //c = c+ (hit.obj->emission)/255;
    }

    if (reflection_count> config.max_reflections)
    {
        return c;
    }

    Vector p = hit.src + hit.t * hit.ray_dir; //hit point
    Vector n = hit.n; //hit.obj->normal(p);
    //Vector v = hit.src - p; //vector from point to viewer

    if(hit.obj->brdf == 0 || hit.obj->brdf == 1)
    //diffuse object
    {
        //indirect illumination
        Vector v1, v2;
        create_orthonormal_basis(n, v1, v2);

        Vector sample_dir = cosine_weighted_hemisphere(ha1->next(), ha2->next(), hit.n); ////can replace with halton series etc in the future.
        Vector transformed_dir;
        //I could use my matrix class here but this will save some time on construction/arithmetic maybe...
        transformed_dir.x = Vector(v1.x, v2.x, n.x).dot(sample_dir);
        transformed_dir.y = Vector(v1.y, v2.y, n.y).dot(sample_dir);
        transformed_dir.z = Vector(v1.z, v2.z, n.z).dot(sample_dir);
        double cos_t = transformed_dir.dot(n);
        Hit diffuse_relfec_hit = intersect(p, transformed_dir, bvh);
        if(diffuse_relfec_hit.t != -1 && diffuse_relfec_hit.n.dot(transformed_dir)< 0)//Inbound ray hits correct face (outbound normal vector)
        {
            Color diffuse_reflec_color = shade(diffuse_relfec_hit,reflection_count+1, ha1, ha2, bvh, config, objects, lights);
            double divisor =max(1.0,diffuse_relfec_hit.t*diffuse_relfec_hit.t);
            c = c + diffuse_reflec_color*hit.color/(divisor*255);//idk where 0.1 comes from.cos_t*
        }

        //direct illumination
        //for(unsigned int i = 0; i < lights.size(); i++)
        for(unsigned int i = 0; i < objects.size(); i++)
        {
            if(objects[i]->emission.r > 0 || objects[i]->emission.g !=0 || objects[i]->emission.b != 0)
            {
                Vector light_point = objects[i]->get_random_point(ha2->next(), ha1->next());//lights[i].position;//
                Vector s =  light_point- p;

                //Inbound ray hits correct face (outbound normal vector)

                double dist = s.abs();
                s = normalise(s);

                Hit shadow = intersect(p, s, bvh); //0.001 offset to avoid collision withself //+0.001*s

                //old if statement back in day of point light source.if(shadow.obj == nullptr || shadow.obj == objects[i] || shadow.t < 0.0001 || shadow.t > dist-0.0001)//
                //the object is not occluded from the light.
                double cosine_term  = shadow.n.dot(s) *-1; //term used to simulate limb darkening?
                if(shadow.obj == objects[i] && cosine_term>0)
                {
                    if(s.dot(n)> 0 ) // light is on right side of the face of obj normal
                    {
                        //diffuse
                        double divisor = max(1.0, shadow.t*shadow.t);
                        c = c + hit.color *cosine_term* objects[i]->emission * s.dot(n)/(divisor * 255 * 255); // lights[i].color

                        if(hit.obj->brdf == 1)
                        //diffuse object with specular...
                        {
                            Vector h = normalise(s + normalise(-1 * hit.t * hit.ray_dir));
                            double val = h.dot(n)/h.abs();
                            c = c + cosine_term*objects[i]->emission* pow(val, hit.obj->shininess)/(divisor*255);

                        }
                    }
                }
            }
        }
    }else if (hit.obj->brdf == 2)
    //glass
    {
        double n_1 = 1;
        double n_2 = 1.6;
        double cos_angle = hit.n.dot(hit.ray_dir);
        Vector next_ray;
        double PR;
        double crit_angle = -1;
        if (cos_angle>0)
        //ray incident on the obj
        {
            std::swap(n_1, n_2);
            crit_angle = asin(n_1/n_2);
        }
        double angle = acos(abs(cos_angle));
        if(crit_angle != -1 && angle > crit_angle)
        {
             next_ray = normalise(hit.ray_dir - hit.n * 2  *cos_angle);
        }else
        {
             PR =schlick_fresnel(abs(cos_angle), n_1, n_2);


            if(ha1->next() < PR)
            //reflection
            {
                next_ray = normalise(hit.ray_dir - hit.n * 2  *cos_angle);
            }else
            //refraction
            {
                next_ray = snells_law(hit.ray_dir, hit.n, cos_angle, n_1, n_2);
            }
        }


        Hit trace_hit = intersect(p, next_ray, bvh);
        if(trace_hit.t != -1)
        {
            Color color = shade(trace_hit, reflection_count+1, ha1, ha2, bvh, config, objects, lights);
            c = c + 0.9*color*hit.color/255; //factor of 0.9 for attenuation
        }
        //copy paste for direct illumination...
        for(unsigned int i = 0; i < objects.size(); i++)
        {
            if(objects[i]->emission.r > 0 || objects[i]->emission.g !=0 || objects[i]->emission.b != 0)
            {
                Vector light_point = objects[i]->get_random_point(ha2->next(), ha1->next());//lights[i].position;//
                Vector s =  light_point- p;
                double dist = s.abs();
                s = normalise(s);

                Hit shadow = intersect(p, s, bvh); //0.001 offset to avoid collision withself //+0.001*s

                if(shadow.obj == nullptr || shadow.obj == objects[i] || shadow.t < 0.0001 || shadow.t > dist-0.0001)//
                //the object is not occluded from the light.
                {
                    if(s.dot(n)>= 0 ) // light is on right side of the face of obj normal
                    {
                        //diffuse
                        //c = c + hit.color * objects[i]->emission * s.dot(n)/(255*255); // lights[i].color


                        //diffuse object with specular...
                        {
                            //Vector h = normalise(s + normalise(-1*hit.t * hit.ray_dir));
                            //double val = h.dot(n)/h.abs();
                            //c = c + objects[i]->emission* pow(val, hit.obj->shininess)/(255);

                        }
                    }
                }
            }
        }
    }

    return c;
}

void draw(ImageArray& img, string filename)
{
    png::image< png::rgb_pixel > image(img.WIDTH, img.HEIGHT);

    for (int y = 0; y < img.HEIGHT; ++y)
    {
        for (int x = 0; x < img.WIDTH; ++x)
        {
            Color c = img.pixelMatrix[img.index(x,y)];
            image[y][x] = png::rgb_pixel((int)c.r, (int)c.g,(int) c.b);
        }
    }

    image.write("renders/"+filename);
}

void deserialize(string filename, vector<Light>& lights, vector<GObject*>& gLights, vector<GObject*>& objects, Camera& cam, Config& config)
//Deserialises the scene/config.xml, modifies global structures and vectors
{
    CMarkup xml;
    xml.Load(filename);

    xml.FindElem(); //config
    config.threads_to_use = stoi(xml.GetAttrib("threads"));
    if (config.threads_to_use < 1){config.threads_to_use=1;}
    config.max_reflections = stoi(xml.GetAttrib("max_reflections"));
    config.spp = stoi(xml.GetAttrib("samplesPP"));
    config.stretch = xml.GetAttrib("stretch");

    xml.FindElem(); //camera
    Camera::deserialize(xml.GetSubDoc(),cam);

    while(xml.FindElem())
    {
        string element = xml.GetTagName();
        if(element == "Sphere")
        {
            Sphere* sp = new Sphere();
            sp->deserialize(xml.GetSubDoc());
            objects.push_back(sp);
        }
        else if(element == "Plane")
        {
            Plane* pl = new Plane();
            pl->deserialize(xml.GetSubDoc());
            objects.push_back(pl);
        }
        else if(element == "Light")
        {
            Light l = Light();
            l.deserialize(xml.GetSubDoc());
            lights.push_back(l);
            continue;
        }
        else if(element == "Mesh")
        {
            Mesh* m = new Mesh();
            m->deserialize(xml.GetSubDoc());
            objects.push_back(m);
        }
    }
    for(int i =0;i<objects.size();i++)
    {
        if(is_light(objects[i]))
        {
            gLights.push_back(objects[i]);
        }
    }
}

#include <windows.h>

#include "imgui/imgui.h"
#include "imgui/imgui_impl_glfw.h"
#include "imgui/imgui_impl_opengl3.h"
#include "imageArray.h"
#include <GL/glew.h>
#include <GLFW/glfw3.h>


static void glfw_error_callback(int error, const char* description);

void gui(ImageArray& img);

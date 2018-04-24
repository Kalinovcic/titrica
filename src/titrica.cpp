#include <windows.h>

#include "GL/glew.h"
#include "GL/gl.h"
#include "GL/glu.h"
#include "glm/glm.hpp"
#include "glm/gtx/transform.hpp"
using namespace glm;

#include "SDL2/SDL.h"
#include "SDL2/SDL_mixer.h"
#undef main

#include "imgui/imgui.h"
#include "imgui/imgui.cpp"
#include "imgui/imgui_demo.cpp"
#include "imgui/imgui_draw.cpp"
using namespace ImGui;

#include <algorithm>
#include <vector>

namespace kutikula
{

#define WINDOW_TITLE    "Titrica Titrić"
#define WINDOW_WIDTH    1200
#define WINDOW_HEIGHT   675

SDL_Window* the_window = NULL;
SDL_GLContext the_context = NULL;
bool window_should_close = false;
int window_width;
int window_height;

float camera_angle_vertical = 0.15 * M_PI;
float camera_angle_horizontal = 0.25 * M_PI;
float camera_target_zoom = 15.0;
float camera_zoom = camera_target_zoom;
vec3 camera_position;

float m1 = 1;
float m2 = 1;
float l1 = 2;
float l2 = 3;
float A = 1;

bool do_graph1 = true;
bool do_graph2 = true;
std::vector<std::pair<float, float>> graph1;
std::vector<std::pair<float, float>> graph2;

bool show_grid = true;

void report(const char* error)
{
    MessageBoxA(NULL, error, "Error", MB_OK);
    ExitProcess(0);
}



#include "gui.cpp"
#include "3d.cpp"

void init_graphics()
{
    if (SDL_Init(SDL_INIT_VIDEO | SDL_INIT_AUDIO))
    {
        report("SDL2 failed to initialize!");
    }
    atexit(SDL_Quit);

    SDL_GL_SetAttribute(SDL_GL_CONTEXT_FLAGS, SDL_GL_CONTEXT_DEBUG_FLAG);
    SDL_GL_SetAttribute(SDL_GL_CONTEXT_MAJOR_VERSION, 2);
    SDL_GL_SetAttribute(SDL_GL_CONTEXT_MINOR_VERSION, 0);
    SDL_GL_SetAttribute(SDL_GL_RED_SIZE, 8);
    SDL_GL_SetAttribute(SDL_GL_GREEN_SIZE, 8);
    SDL_GL_SetAttribute(SDL_GL_BLUE_SIZE, 8);
    SDL_GL_SetAttribute(SDL_GL_DEPTH_SIZE, 16);
    SDL_GL_SetAttribute(SDL_GL_DOUBLEBUFFER, 1);

    auto window_flags = SDL_WINDOW_OPENGL | SDL_WINDOW_RESIZABLE;
    the_window = SDL_CreateWindow(WINDOW_TITLE, SDL_WINDOWPOS_CENTERED, SDL_WINDOWPOS_CENTERED, WINDOW_WIDTH, WINDOW_HEIGHT, window_flags);
    if (!the_window)
    {
        report("SDL2 failed to create a window!");
    }
    SDL_ShowWindow(the_window);

    the_context = SDL_GL_CreateContext(the_window);
    if (!the_context)
    {
        report("Failed to create a OpenGL context!");
    }

    SDL_GL_SetSwapInterval(1);
 
    glewInit();

    ImGui_ImplSdl_Init(the_window);
    ImGui_ImplSdl_CreateDeviceObjects();
}

void handle_events()
{
    SDL_GetWindowSize(the_window, &window_width, &window_height);
    glViewport(0, 0, window_width, window_height);

    SDL_Event event;
    while (SDL_PollEvent(&event))
    {
        switch (event.type)
        {
        case SDL_QUIT:
        {
            window_should_close = true;
        } break;
        /*case SDL_MOUSEBUTTONDOWN: case SDL_MOUSEBUTTONUP:
        {
            if (event.button.button == SDL_BUTTON_LEFT)
            {
                if (event.button.state == SDL_PRESSED)
                    start_rotation(event.button.x, event.button.y);
                else
                    stop_rotation();
            }
        } break;
        case SDL_MOUSEMOTION:
        {
            update_rotation(event.motion.x, event.motion.y);
        } break;*/
        default:
        {
            ImGui_ImplSdl_ProcessEvent(&event);
        } break;
        }
    }
}

void entry()
{
    init_graphics();
    while (!window_should_close)
    {
        handle_events();

        glClearColor(0.2, 0.2, 0.2, 1.0);
        glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);

        render_3d_test();
        render_gui();

        SDL_GL_SwapWindow(the_window);
    }

    ImGui_ImplSdl_Shutdown();
}

}

int main()
{
    kutikula::entry();
}

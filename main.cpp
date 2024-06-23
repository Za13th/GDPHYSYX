#include "glad/glad.h"
#include <GLFW/glfw3.h>
#define TINYOBJLOADER_IMPLEMENTATION
#include "tiny_obj_loader.h"
#include <string>
#include <iostream>
#include <iomanip>
#include <chrono>
using namespace std::chrono_literals;
constexpr std::chrono::nanoseconds timestep(16ms);
//16 ms = 60FPS, 64 ms = 30FPS (supposedly because this laptop is dying)
#include <glm/glm.hpp>
#include <glm/gtc/matrix_transform.hpp>
#include <glm/gtc/type_ptr.hpp>

#define STB_IMAGE_IMPLEMENTATION
#include "stb_image.h"
//#include <vector>

#include "GDPHYSX.cpp"
#include "GDGRAP1.cpp"

//window width and height
#define WIDTH 800.f
#define HEIGHT 800.f
// callbacks below are for getting the keyboard and mouse inputs
void Key_Callback(GLFWwindow* window, int key, int scancode, int action, int mods)
//window pointer, keycode of the press, physical position of the press, press/hold, modifier keys
{}

void Mouse_Callback(GLFWwindow* window, double xpos, double ypos)
{}

void Mouse_Click_Callback(GLFWwindow* window, int button, int action, int mods)
{}

int main(void)
{
    int sparks;
    std::cout << "Input number of sparks: ";
    std::cin >> sparks;

    GLFWwindow* window;

    /* Initialize the library */
    if (!glfwInit())
        return -1;

    /* Create a windowed mode window and its OpenGL context */
    window = glfwCreateWindow(WIDTH, HEIGHT, "Project13", NULL, NULL);
    if (!window)
    {
        glfwTerminate();
        return -1;
    }

    /* Make the window's context current */
    glfwMakeContextCurrent(window);
    gladLoadGL();

    //sets up the viewport
    glViewport(0, 0, WIDTH, HEIGHT);

    Shader p("Shaders/particle.vert", "Shaders/particle.frag");
    Model Sphere("3D/sphere.obj", false, false);

    physics::DragForceGenerator dfg = physics::DragForceGenerator(0.14, 0.1);
    physics::PhysicsWorld World(&Sphere,&dfg);// pass model and drag force generator for internal generation of particles
   
    srand(time(NULL));

    for (int i = 0; i < sparks; i++) //add the spark particles to the physics world
    {
        physics::Particle* p = new physics::Particle(&Sphere);
        World.forceRegistry.Add(p, &dfg);
        World.AddParticle(p);
    }

    OrthoCamera BirdsEye;
    PerspectiveCamera Perspective(WIDTH,HEIGHT);
    
    p.useProgram();
    
    bool ortho = true, paused = false, releasedPause = true;
    //ortho: if true, ortho, else perspective
    //paused: if true, game is paused by bypassing the update cuntion
    //releasedPause: used for controlling the input for pausing the game
    using clock = std::chrono::high_resolution_clock;
    auto curr_time = clock::now();
    auto prev_time = curr_time;
    std::chrono::nanoseconds curr_ns(0);

    /* Loop until the user closes the window */
    while (!glfwWindowShouldClose(window))
    {
        //get current time
        curr_time = clock::now();
        //check duration between now and last iteration
        auto dur = std::chrono::duration_cast<std::chrono::nanoseconds> (curr_time - prev_time);
        //set the prev with current for next iteration
        prev_time = curr_time;

        //add the duration since the last iteration to the time since last frame
        curr_ns += dur;

        if (glfwGetKey(window, GLFW_KEY_1)) //change camera to ortho via the '1' key
        {
            ortho = true;
        }
        else if (glfwGetKey(window, GLFW_KEY_2)) //change camera to perspective via the '2' key
        {
            ortho = false;
        }

        if (glfwGetKey(window, GLFW_KEY_SPACE) == GLFW_RELEASE) 
            releasedPause = true;
        if (glfwGetKey(window, GLFW_KEY_SPACE) && (releasedPause == true)) //toggles the game's pause if the space bar is pressed
        {
            if (paused)
                paused = false;
            else
                paused = true;

            releasedPause = false; //disable the space bar until released via line 115-116
        }
        else if (glfwGetKey(window, GLFW_KEY_SPACE) == GLFW_RELEASE) //reenables the space bar
            releasedPause = true;


        if (curr_ns >= timestep)
        {
            //ns -> ms
            auto ms = std::chrono::duration_cast<std::chrono::milliseconds>(curr_ns);
            //std::cout << "MS: " << (float)ms.count() << "\n";
            //reset
            curr_ns -= curr_ns;
            
            //updates below

            if(!paused)//if the game isn't paused, upddate
            World.Update((float)ms.count() / 1000);
\
            //std::cout << "P6 Update\n";
        }
        //std::cout << "Normal Update\n";

        /* Render here */
        glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);

        //simultaneously controls both cameras
        BirdsEye.PlayerControl(window);
        Perspective.PlayerControl(window);

        if (ortho) //draws the world using either the ortho or perspective camera depending on the chosen camera
            World.Draw(p.getProgram(),&BirdsEye);
        else
            World.Draw(p.getProgram(),&Perspective);

        /* Swap front and back buffers */
        glfwSwapBuffers(window);

        /* Poll for and process events */
        glfwPollEvents();
    }
    //deletes the VAO and VBOs of the models used
    Sphere.cleanUp();

    glfwTerminate();
    return 0;
}
/*******************************************************************
** This code is part of Breakout.
**
** Breakout is free software: you can redistribute it and/or modify
** it under the terms of the CC BY 4.0 license as published by
** Creative Commons, either version 4 of the License, or (at your
** option) any later version.
**
** This version was put together by JTAG for the platform Nintendo Switch, requires devkitPro SDK
******************************************************************/

// Include the main libnx system header, for Switch development
#include <switch.h>

#define ENABLE_NXLINK

#define GLFW_INCLUDE_NONE
#include <glad/glad.h>
#include <GLFW/glfw3.h>

#include "game.h"
#include "resource_manager.h"

#include <sstream>
#include <iostream>

#ifndef __SWITCH__
#define ASSET(_str) "./resources/" _str
#else
#define ASSET(_str) "romfs:/" _str
#endif

#include "imgui.h"
//
#include "imgui_impl_glfw.h"
#include "imgui_impl_opengl3.h"

void framebuffer_size_callback(GLFWwindow* window, int width, int height);
void key_callback(GLFWwindow* window, int key, int scancode, int action, int mode);
void joystick_callback(int jid, int event);

// settings
const unsigned int SCREEN_WIDTH = 1280; // on switch this will always get forced to 1280 * 720 no matter what
const unsigned int SCREEN_HEIGHT = 720;

Game Breakout(SCREEN_WIDTH, SCREEN_HEIGHT);

// Set up variables for FPS calculation
float m_DeltaTime = 0.0f;
float m_LastFrameTime = 0.0f;
float m_Fps = 0.0f; // cur fps
float m_AvgFps = 0.0f; // average fps
float m_AvgCounter = 0.0f; // used to get the m_AvgFps value
float m_AvgIndex = 0.0f; // also used for the m_AvgFps value

GLFWwindow* initGL()
{
    // GLFW / GLAD / OpenGL/EGL
    glfwInitHint(GLFW_JOYSTICK_HAT_BUTTONS, GLFW_FALSE); // set joysticks to be directional and not buttons
    glfwInit();
    glfwWindowHint(GLFW_CONTEXT_VERSION_MAJOR, 4);
    glfwWindowHint(GLFW_CONTEXT_VERSION_MINOR, 3);
    glfwWindowHint(GLFW_OPENGL_PROFILE, GLFW_OPENGL_CORE_PROFILE);

#ifdef __APPLE__
    glfwWindowHint(GLFW_OPENGL_FORWARD_COMPAT, GL_TRUE);
#endif

    GLFWwindow* window = glfwCreateWindow(SCREEN_WIDTH, SCREEN_HEIGHT, "LearnOpenGL", NULL, NULL);
    if (window == NULL)
    {
        std::cout << "Failed to create GLFW window" << std::endl;
        glfwTerminate();
        return nullptr;
    }

    glfwMakeContextCurrent(window);
        
    if (!gladLoadGLLoader((GLADloadproc)glfwGetProcAddress))
    {
        std::cout << "Failed to initialize GLAD" << std::endl;
        return nullptr;
    }

	printf("OpenGL Vendor: %s\n", glGetString(GL_VENDOR));
	printf("OpenGL Renderer: %s\n", glGetString(GL_RENDERER));
	printf("OpenGL Version: %s\n", glGetString(GL_VERSION));

    glfwSetFramebufferSizeCallback(window, framebuffer_size_callback);
    glfwSetKeyCallback(window, key_callback);
    glfwSetJoystickCallback(joystick_callback);

    // tell GLFW to capture our mouse
    glfwSetInputMode(window, GLFW_CURSOR, GLFW_CURSOR_DISABLED);

	if (glfwJoystickIsGamepad(GLFW_JOYSTICK_1))
	{
		GLFWgamepadstate state;
		printf("Gamepad detected: %s\n", glfwGetGamepadName(GLFW_JOYSTICK_1));
		glfwGetGamepadState(GLFW_JOYSTICK_1, &state);
	}

    // OpenGL configuration
    // --------------------
    glViewport(0, 0, SCREEN_WIDTH, SCREEN_HEIGHT);
    //glEnable(GL_DEPTH_TEST);
    glEnable(GL_BLEND);
    glBlendFunc(GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA);
	glfwSwapInterval(1); // vsync on

    // IMGUI TEMP
    // Setup ImGui
    IMGUI_CHECKVERSION();
    ImGui::CreateContext();
    ImGuiIO& io = ImGui::GetIO(); (void)io;
    ImGui::StyleColorsDark();

    // Setup ImGui for GLFW
    ImGui_ImplGlfw_InitForOpenGL(window, true);
    ImGui_ImplOpenGL3_Init("#version 430 core");
    // IMGUI END

    Breakout.Init();

    return window;
}

void ImBegin()
{
    // Start ImGui frame
    ImGui_ImplOpenGL3_NewFrame();
    ImGui_ImplGlfw_NewFrame();
    ImGui::NewFrame();
}

void ImEnd()
{
	// Rendering
	ImGui::Render();
	ImGui_ImplOpenGL3_RenderDrawData(ImGui::GetDrawData());
}

int main(int argc, char* argv[])
{
    GLFWwindow* window = initGL();

    // Other initialization goes here. As a demonstration, we print hello world.
    printf("Starting Breakout!\n");

    // Main loop
    while (appletMainLoop() && !glfwWindowShouldClose(window))
    {
        // Measure time between frames (delta time)
        float time = glfwGetTime();
        m_DeltaTime = time - m_LastFrameTime;
        m_LastFrameTime = time;

        m_Fps = 1000.0f / (m_DeltaTime * 1000.0f);
        m_AvgCounter += m_Fps;
        m_AvgIndex += 1.0f;
        m_AvgFps = m_AvgCounter / m_AvgIndex;

        // Input
        Breakout.ProcessInput(m_DeltaTime);

        // Update
        Breakout.Update(m_DeltaTime);

        // Rendering
        // Clear the screen
        glClearColor(0.0f, 0.0f, 0.0f, 1.0f);
        glClear(GL_COLOR_BUFFER_BIT);

        // Render the game
        Breakout.Render();

        // Render ImGui ontop
        std::stringstream fpsSS, dtSS;
        fpsSS << "FPS: " << m_Fps << ", AVG: " << m_AvgFps;
        dtSS << "Frame Time: " << m_DeltaTime << "s, " << (m_DeltaTime * 1000.0f) << "ms";

        ImBegin();

        // ImGui content
        ImGui::Begin("Debug Info");
        ImGui::Text(fpsSS.str().c_str());
        ImGui::Text(dtSS.str().c_str());
        ImGui::End();
        
        ImEnd();
        
        // Events and buffer swap
        glfwSwapBuffers(window);
        glfwPollEvents();  
    }

    ResourceManager::Clear();

    // Cleanup
    ImGui_ImplOpenGL3_Shutdown();
    ImGui_ImplGlfw_Shutdown();
    ImGui::DestroyContext();

    glfwDestroyWindow(window);
    glfwTerminate();
    return 0;
}

// GLFW Callbacks

// glfw: whenever the window size changed (by OS or user resize) this callback function executes
// ---------------------------------------------------------------------------------------------
void framebuffer_size_callback(GLFWwindow* window, int width, int height)
{
    glViewport(0, 0, width, height);

    printf("Framebuffer Callback, Width: %d, Height: %d\n", width, height);
}

void key_callback(GLFWwindow* window, int key, int scancode, int action, int mode)
{
    GLFWgamepadstate gamepad = {};

    glfwGetGamepadState(GLFW_JOYSTICK_1, &gamepad);

	// Exit by pressing Start (aka Plus)
	if (gamepad.buttons[GLFW_GAMEPAD_BUTTON_START] == GLFW_PRESS || gamepad.buttons[GLFW_GAMEPAD_BUTTON_BACK] == GLFW_PRESS) // + or - on switch
	{
		glfwSetWindowShouldClose(window, GLFW_TRUE);
	}

    //printf("Key: %d\n", key);

    if (key >= 0 && key < 1024)
    {
        if (action == GLFW_PRESS)
            Breakout.Keys[key] = true;
        else if (action == GLFW_RELEASE)
            Breakout.Keys[key] = false;
    }
}

void joystick_callback(int jid, int event)
{
	if (event == GLFW_CONNECTED)
	{
		printf("Joystick %d connected\n", jid);
		if (glfwJoystickIsGamepad(jid))
			printf("Joystick %d is gamepad: \"%s\"\n", jid, glfwGetGamepadName(jid));
	}
	else if (event == GLFW_DISCONNECTED)
		printf("Joystick %d disconnected\n", jid);
}

//-----------------------------------------------------------------------------
// nxlink support
//-----------------------------------------------------------------------------

#ifndef ENABLE_NXLINK
#define TRACE(fmt,...) ((void)0)
#else
#include <unistd.h>
#define TRACE(fmt,...) printf("%s: " fmt "\n", __PRETTY_FUNCTION__, ## __VA_ARGS__)

static int s_nxlinkSock = -1;

static void initNxLink()
{
    if (R_FAILED(socketInitializeDefault()))
        return;

    s_nxlinkSock = nxlinkStdio();
    if (s_nxlinkSock >= 0)
        TRACE("printf output now goes to nxlink server");
    else
        socketExit();
}

static void deinitNxLink()
{
    if (s_nxlinkSock >= 0)
    {
        close(s_nxlinkSock);
        socketExit();
        s_nxlinkSock = -1;
    }
}

extern "C" void userAppInit()
{
    romfsInit();
    initNxLink();
}

extern "C" void userAppExit()
{
    deinitNxLink();
    romfsExit();
}

#endif

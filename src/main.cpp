#include <iostream>
#include <glad/glad.h>
#include <SDL2/SDL.h>
#include "Engine/Renderer.h"
#include "Engine/Shader.h"
#include "Utils/Logger.h"

// Screen dimensions
const int SCREEN_WIDTH = 1280;
const int SCREEN_HEIGHT = 720;

int main(int argc, char* argv[]) {
    // Initialize Logger
    LOG_INFO("Starting VibeReaper...");

    // Initialize SDL
    if (SDL_Init(SDL_INIT_VIDEO | SDL_INIT_AUDIO | SDL_INIT_GAMECONTROLLER) < 0) {
        LOG_ERROR("SDL could not initialize! SDL_Error: " + std::string(SDL_GetError()));
        return -1;
    }

    // Set OpenGL attributes
    SDL_GL_SetAttribute(SDL_GL_CONTEXT_MAJOR_VERSION, 3);
    SDL_GL_SetAttribute(SDL_GL_CONTEXT_MINOR_VERSION, 3);
    SDL_GL_SetAttribute(SDL_GL_CONTEXT_PROFILE_MASK, SDL_GL_CONTEXT_PROFILE_CORE);

    // Create window
    SDL_Window* window = SDL_CreateWindow("VibeReaper",
        SDL_WINDOWPOS_CENTERED, SDL_WINDOWPOS_CENTERED,
        SCREEN_WIDTH, SCREEN_HEIGHT,
        SDL_WINDOW_OPENGL | SDL_WINDOW_SHOWN | SDL_WINDOW_RESIZABLE);

    if (window == nullptr) {
        LOG_ERROR("Window could not be created! SDL_Error: " + std::string(SDL_GetError()));
        SDL_Quit();
        return -1;
    }

    // Create OpenGL context
    SDL_GLContext glContext = SDL_GL_CreateContext(window);
    if (glContext == nullptr) {
        LOG_ERROR("OpenGL context could not be created! SDL_Error: " + std::string(SDL_GetError()));
        SDL_DestroyWindow(window);
        SDL_Quit();
        return -1;
    }

    // Initialize GLAD
    if (!gladLoadGLLoader((GLADloadproc)SDL_GL_GetProcAddress)) {
        LOG_ERROR("Failed to initialize GLAD");
        SDL_GL_DeleteContext(glContext);
        SDL_DestroyWindow(window);
        SDL_Quit();
        return -1;
    }

    LOG_INFO("OpenGL Version: " + std::string((char*)glGetString(GL_VERSION)));
    LOG_INFO("Renderer: " + std::string((char*)glGetString(GL_RENDERER)));

    // Initialize Engine Systems
    VibeReaper::Renderer renderer;
    if (!renderer.Initialize()) {
        LOG_ERROR("Failed to initialize Renderer");
        return -1;
    }
    renderer.SetViewport(0, 0, SCREEN_WIDTH, SCREEN_HEIGHT);

    VibeReaper::Shader shader;
    // Assuming assets are relative to the executable or working directory
    // We will ensure assets are copied to the bin folder
    if (!shader.LoadFromFiles("assets/shaders/basic.vert", "assets/shaders/basic.frag")) {
        LOG_WARNING("Failed to load basic shaders. Rendering might be incorrect.");
    }

    // Main loop flag
    bool quit = false;
    SDL_Event e;

    // Time management
    Uint64 lastTime = SDL_GetPerformanceCounter();
    double deltaTime = 0.0;

    // Game Loop
    while (!quit) {
        // Calculate Delta Time
        Uint64 currentTime = SDL_GetPerformanceCounter();
        deltaTime = (double)((currentTime - lastTime) * 1000 / (double)SDL_GetPerformanceFrequency());
        deltaTime /= 1000.0; // Convert to seconds
        lastTime = currentTime;

        // FPS Counter (display every second)
        static float fpsTimer = 0.0f;
        static int frameCount = 0;
        fpsTimer += deltaTime;
        frameCount++;
        
        if (fpsTimer >= 1.0f) {
            float fps = frameCount / fpsTimer;
            LOG_INFO("FPS: " + std::to_string((int)fps));
            fpsTimer = 0.0f;
            frameCount = 0;
        }

        // Handle Events
        while (SDL_PollEvent(&e) != 0) {
            if (e.type == SDL_QUIT) {
                quit = true;
            } else if (e.type == SDL_WINDOWEVENT) {
                if (e.window.event == SDL_WINDOWEVENT_RESIZED) {
                    renderer.SetViewport(0, 0, e.window.data1, e.window.data2);
                }
            } else if (e.type == SDL_KEYDOWN) {
                if (e.key.keysym.sym == SDLK_ESCAPE) {
                    quit = true;
                }
            }
        }

        // Update
        // TODO: Game logic update here

        // Render
        renderer.Clear();

        // Use shader (even if we don't have geometry yet, just to test)
        if (shader.GetProgramID() != 0) {
            shader.Use();
            
            // Just for fun/testing, let's pulse the color if we were drawing something
            // float timeValue = SDL_GetTicks() / 1000.0f;
            // float greenValue = (sin(timeValue) / 2.0f) + 0.5f;
            // shader.SetVec3("uColor", glm::vec3(0.0f, greenValue, 0.0f));
        }

        // Swap buffers
        renderer.SwapBuffers(window);
    }

    // Cleanup
    SDL_GL_DeleteContext(glContext);
    SDL_DestroyWindow(window);
    SDL_Quit();

    LOG_INFO("VibeReaper shutdown successfully");
    return 0;
}

#include <iostream>
#include <glad/glad.h>
#include <SDL2/SDL.h>
#include <glm/glm.hpp>
#include <glm/gtc/matrix_transform.hpp>
#include <glm/gtc/type_ptr.hpp>
#include "Engine/Renderer.h"
#include "Engine/Shader.h"
#include "Engine/Mesh.h"
#include "Engine/Texture.h"
#include "Engine/Camera.h"
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
    SDL_Window* window = SDL_CreateWindow("VibeReaper - Phase 2",
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

    // Initialize Renderer
    VibeReaper::Renderer renderer;
    if (!renderer.Initialize()) {
        LOG_ERROR("Failed to initialize Renderer");
        return -1;
    }
    renderer.SetViewport(0, 0, SCREEN_WIDTH, SCREEN_HEIGHT);

    // Load lighting shader
    VibeReaper::Shader shader;
    if (!shader.LoadFromFiles("assets/shaders/lighting.vert", "assets/shaders/lighting.frag")) {
        LOG_ERROR("Failed to load lighting shaders");
        return -1;
    }

    // Create mesh (procedural cube)
    VibeReaper::Mesh cube = VibeReaper::Mesh::GenerateCube();

    // Load texture
    VibeReaper::Texture texture;
    if (!texture.LoadFromFile("assets/textures/test_texture.png")) {
        LOG_WARNING("Failed to load test texture");
    }

    // Create camera
    float aspectRatio = (float)SCREEN_WIDTH / (float)SCREEN_HEIGHT;
    VibeReaper::Camera camera(45.0f, aspectRatio);
    camera.SetTarget(glm::vec3(0.0f, 0.0f, 0.0f));
    camera.SetDistance(3.5f);

    // Mouse control state
    bool mousePressed = false;
    int lastMouseX = 0, lastMouseY = 0;

    // Lighting parameters
    glm::vec3 lightPos(2.0f, 2.0f, 2.0f);
    glm::vec3 lightColor(1.0f, 1.0f, 1.0f);

    // Main loop
    bool quit = false;
    SDL_Event e;

    // Time management
    Uint64 lastTime = SDL_GetPerformanceCounter();
    double deltaTime = 0.0;
    float rotation = 0.0f;

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
            } 
            else if (e.type == SDL_WINDOWEVENT) {
                if (e.window.event == SDL_WINDOWEVENT_RESIZED) {
                    int width = e.window.data1;
                    int height = e.window.data2;
                    renderer.SetViewport(0, 0, width, height);
                    camera.SetAspectRatio((float)width / (float)height);
                }
            } 
            else if (e.type == SDL_KEYDOWN) {
                if (e.key.keysym.sym == SDLK_ESCAPE) {
                    quit = true;
                }
            }
            else if (e.type == SDL_MOUSEBUTTONDOWN) {
                if (e.button.button == SDL_BUTTON_LEFT) {
                    mousePressed = true;
                    SDL_GetMouseState(&lastMouseX, &lastMouseY);
                }
            }
            else if (e.type ==SDL_MOUSEBUTTONUP) {
                if (e.button.button == SDL_BUTTON_LEFT) {
                    mousePressed = false;
                }
            }
            else if (e.type == SDL_MOUSEMOTION) {
                if (mousePressed) {
                    int mouseX, mouseY;
                    SDL_GetMouseState(&mouseX, &mouseY);
                    
                    float deltaX = (float)(mouseX - lastMouseX);
                    float deltaY = (float)(mouseY - lastMouseY);
                    
                    camera.Orbit(deltaX * 0.25f, -deltaY * 0.25f);
                    
                    lastMouseX = mouseX;
                    lastMouseY = mouseY;
                }
            }
            else if (e.type == SDL_MOUSEWHEEL) {
                camera.Zoom((float)e.wheel.y * 0.5f);
            }
        }

        // Update
        rotation += deltaTime * 45.0f; // 45 degrees per second
        if (rotation >= 360.0f) rotation -= 360.0f;

        camera.Update(deltaTime);

        // Render
        renderer.Clear();

        // Use lighting shader
        shader.Use();

        // Set uniforms
        // Model matrix (rotate cube)
        glm::mat4 model = glm::mat4(1.0f);
        model = glm::rotate(model, glm::radians(rotation), glm::vec3(0.0f, 1.0f, 0.0f));
        shader.SetMat4("uModel", model);

        // View and projection matrices
        shader.SetMat4("uView", camera.GetViewMatrix());
        shader.SetMat4("uProjection", camera.GetProjectionMatrix());

        // Lighting uniforms
        shader.SetVec3("uLightPos", lightPos);
        shader.SetVec3("uLightColor", lightColor);
        shader.SetVec3("uViewPos", camera.GetPosition());
        shader.SetFloat("uAmbientStrength", 0.2f);
        shader.SetFloat("uSpecularStrength", 0.5f);
        shader.SetFloat("uShininess", 32.0f);

        // Material color
        shader.SetVec3("uColor", glm::vec3(1.0f, 1.0f, 1.0f));

        // Bind texture
        shader.SetInt("uTexture", 0);
        texture.Bind(0);

        // Draw cube
        cube.Draw(shader);

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

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
#include "Game/World.h"

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
        LOG_WARNING("Failed to load test texture, creating fallback white texture");
        texture.CreateWhiteTexture();
    }

    // Load world from MAP file
    VibeReaper::World world;
    if (!world.LoadMap("assets/maps/debug_test.map")) {
        LOG_ERROR("Failed to load map, exiting");
        return -1;
    }

    // Get player spawn position
    glm::vec3 playerSpawn = world.GetPlayerSpawnPosition();
    // Transform spawn position from Z-up to Y-up: (x, y, z) -> (x, z, -y)
    glm::vec3 engineSpawn(playerSpawn.x, playerSpawn.z, -playerSpawn.y);
    
    LOG_INFO("Player spawn (Engine): " + std::to_string(engineSpawn.x) + ", " + 
             std::to_string(engineSpawn.y) + ", " + std::to_string(engineSpawn.z));

    // Get light position from map (assuming first light found)
    glm::vec3 lightPos(0.0f, 500.0f, 0.0f); // Default high above
    std::vector<const VibeReaper::Entity*> lights = world.GetEntitiesByClass("light");
    if (!lights.empty()) {
        glm::vec3 mapLightPos = lights[0]->GetOrigin();
        // Transform light position from Z-up to Y-up
        lightPos = glm::vec3(mapLightPos.x, mapLightPos.z, -mapLightPos.y);
        LOG_INFO("Light spawn (Quake): " + std::to_string(mapLightPos.x) + ", " + 
                 std::to_string(mapLightPos.y) + ", " + std::to_string(mapLightPos.z));
        LOG_INFO("Light spawn (Engine): " + std::to_string(lightPos.x) + ", " + 
                 std::to_string(lightPos.y) + ", " + std::to_string(lightPos.z));
    } else {
        LOG_INFO("No light found in map, using default at (0, 500, 0)");
    }

    // Create camera
    float aspectRatio = (float)SCREEN_WIDTH / (float)SCREEN_HEIGHT;
    VibeReaper::Camera camera(45.0f, aspectRatio, 0.1f, 4000.0f);
    
    // Set camera to look +X
    camera.SetTarget(engineSpawn); 
    camera.SetDistance(200.0f);
    camera.SetRotation(-90.0f, 20.0f); // Look +X, slightly down 
    
    // Mouse control state
    bool mousePressed = false;
    int lastMouseX = 0, lastMouseY = 0;

    // Lighting parameters
    glm::vec3 lightColor(1.0f, 1.0f, 1.0f);

    // Main loop
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

        // FPS Counter
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
        camera.Update(deltaTime);

        // Render
        renderer.Clear();

        // Use lighting shader
        shader.Use();

        // Set uniforms
        shader.SetMat4("uView", camera.GetViewMatrix());
        shader.SetMat4("uProjection", camera.GetProjectionMatrix());

        shader.SetVec3("uLightPos", lightPos);
        shader.SetVec3("uLightColor", lightColor);
        shader.SetVec3("uViewPos", camera.GetPosition());
        shader.SetFloat("uAmbientStrength", 0.5f);
        shader.SetFloat("uSpecularStrength", 0.5f);
        shader.SetFloat("uShininess", 32.0f);

        // Set texture sampler to use texture unit 0
        shader.SetInt("uTexture", 0);
        
        shader.SetVec3("uColor", glm::vec3(1.0f, 1.0f, 1.0f));

        // Render world with rotation (Quake Z-up to Engine Y-up)
        glm::mat4 worldModel = glm::mat4(1.0f);
        worldModel = glm::rotate(worldModel, glm::radians(-90.0f), glm::vec3(1.0f, 0.0f, 0.0f));
        shader.SetMat4("uModel", worldModel);
        
        // World handles texture binding now
        world.Render(shader);

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
